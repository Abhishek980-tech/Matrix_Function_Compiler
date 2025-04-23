// Ensure this is compiled with /clr enabled (C++/CLI)


#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <msclr/marshal_cppstd.h>

#using <Microsoft.VisualBasic.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace msclr::interop;
using namespace System::Text::RegularExpressions;
using namespace System::Drawing;
using namespace std;

std::map<std::string, std::vector<std::vector<int>>> matrixVars;

// Input Dialog 
public ref class InputDialog : public Form {
public:
    InputDialog(String^ prompt) {
        InitializeComponent();
        this->labelPrompt->Text = prompt;
        this->StartPosition = FormStartPosition::CenterParent;
        this->textBoxInput->Focus();
    }

    String^ GetInput() {
        return this->textBoxInput->Text;
    }

protected:
    ~InputDialog() {
        if (components) delete components;
    }

private:
    Label^ labelPrompt;
    TextBox^ textBoxInput;
    Button^ buttonOK;
    System::ComponentModel::Container^ components;

    void InitializeComponent(void) {
        this->labelPrompt = gcnew Label();
        this->textBoxInput = gcnew TextBox();
        this->buttonOK = gcnew Button();
        this->SuspendLayout();

        this->labelPrompt->AutoSize = true;
        this->labelPrompt->Location = Point(12, 9);

        this->textBoxInput->Location = Point(15, 30);
        this->textBoxInput->Size = Drawing::Size(250, 20);
        this->textBoxInput->TabIndex = 0;

        this->buttonOK->DialogResult = System::Windows::Forms::DialogResult::OK;
        this->buttonOK->Location = Point(100, 65);
        this->buttonOK->Text = "OK";
        this->buttonOK->Click += gcnew EventHandler(this, &InputDialog::buttonOk_Click);

        this->Controls->Add(this->labelPrompt);
        this->Controls->Add(this->textBoxInput);
        this->Controls->Add(this->buttonOK);
        this->Size = Drawing::Size(300, 140);
        this->AcceptButton = this->buttonOK;
        this->ResumeLayout(false);
        this->PerformLayout();
    }

    void buttonOk_Click(System::Object^ sender, System::EventArgs^ e) {
        this->DialogResult = System::Windows::Forms::DialogResult::OK;
        this->Close();
    }
};

//  Display Matrix 
String^ DisplayMatrix(const std::vector<std::vector<int>>& mat) {
    if (mat.empty()) return "Matrix is empty!";
    std::ostringstream os;
    for (const auto& row : mat) {
        for (int val : row)
            os << val << "\t";
        os << "\n";
    }
    return marshal_as<String^>(os.str());
}

// Input Matrix 
void InputMatrix(std::vector<std::vector<int>>& mat, int size) {
    mat.resize(size, vector<int>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            String^ prompt = String::Format("Enter value for matrix[{0}][{1}]", i, j);
            InputDialog^ inputDialog = gcnew InputDialog(prompt);
            if (inputDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                String^ input = inputDialog->GetInput();
                int value;
                if (!Int32::TryParse(input, value)) {
                    MessageBox::Show("Invalid input! Defaulting to 0.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    value = 0;
                }
                mat[i][j] = value;
            }
        }
    }
}

namespace MatrixCompilerUI {
    public ref class MyForm : public Form {
    public:
        MyForm() {
            InitializeComponent();
        }

    private:
        Label^ labelCode;
        Label^ labelOutput;
        Label^ labelint;
        Label^ labelstr;
        RichTextBox^ textBoxCode;
        RichTextBox^ textBoxOutput;
        Button^ buttonRun;
        GroupBox^ tutorialBox;
        ComboBox^ tutorialDropdown;
        RichTextBox^ tutorialContent;
        System::ComponentModel::Container^ components;
        ListBox^ listBoxSuggestions;
        cli::array<String^>^ keywords;

        void InitializeComponent(void) {
            this->Text = "Matrix Function Compiler ";
            this->Size = Drawing::Size(1000, 700);
            this->BackColor = Color::FromArgb(15, 23, 42);
            this->Font = gcnew System::Drawing::Font("Segoe UI", 13);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->AutoScroll = true;

            this->labelint = gcnew Label();
            this->labelint->Text = "Matrix Function Compiler [Editor]";
            this->labelint->ForeColor = Color::Khaki;
            this->labelint->Location = Point(510, 10);
            this->labelint->AutoSize = true;
            this->labelint->Font = gcnew System::Drawing::Font("Segoe UI", 18, FontStyle::Bold);

            this->labelCode = gcnew Label();
            this->labelCode->Text = "CODE:";
            this->labelCode->ForeColor = Color::White;
            this->labelCode->Location = Point(40, 60);

            this->textBoxCode = gcnew RichTextBox();
            this->textBoxCode->Size = Drawing::Size(760, 350);
            this->textBoxCode->Location = Point(30, 90);
            this->textBoxCode->BackColor = Color::Black;
            this->textBoxCode->ForeColor = Color::WhiteSmoke;
            this->textBoxCode->Font = gcnew System::Drawing::Font("Consolas", 10);
            this->textBoxCode->TextChanged += gcnew EventHandler(this, &MyForm::textBoxCode_TextChanged);

            this->labelOutput = gcnew Label();
            this->labelOutput->Text = "OUTPUT:";
            this->labelOutput->ForeColor = Color::White;
            this->labelOutput->Location = Point(820, 60);

            this->textBoxOutput = gcnew RichTextBox();
            this->textBoxOutput->Size = Drawing::Size(530, 350);
            this->textBoxOutput->Location = Point(800, 90);
            this->textBoxOutput->BackColor = Color::Black;
            this->textBoxOutput->ForeColor = Color::IndianRed;
            this->textBoxOutput->ReadOnly = true;

            this->buttonRun = gcnew Button();
            this->buttonRun->Text = "RUN";
            this->buttonRun->Size = Drawing::Size(120, 40);
            this->buttonRun->Location = Point(670, 443);
            this->buttonRun->BackColor = Color::FromArgb(10, 155, 100);
            this->buttonRun->ForeColor = Color::White;
            this->buttonRun->FlatStyle = FlatStyle::Flat;
            this->buttonRun->Click += gcnew EventHandler(this, &MyForm::buttonRun_Click);

            this->labelstr = gcnew Label();
            this->labelstr->Text = "Help Your-Self[Manual]";
            this->labelstr->ForeColor = Color::Khaki;
            this->labelstr->Location = Point(510, 520);
            this->labelstr->AutoSize = true;
            this->labelstr->Font = gcnew System::Drawing::Font("Segoe UI", 18, FontStyle::Bold);


            this->tutorialBox = gcnew GroupBox();
            this->tutorialBox->Text = "Tutorial Box";
            this->tutorialBox->ForeColor = Color::White;
            this->tutorialBox->Size = Drawing::Size(880, 500);
            this->tutorialBox->Location = Point(280, 540);

            this->tutorialDropdown = gcnew ComboBox();
            this->tutorialDropdown->Size = Drawing::Size(840, 500);
            this->tutorialDropdown->Location = Point(20, 30);
            this->tutorialDropdown->DropDownStyle = ComboBoxStyle::DropDownList;
            this->tutorialDropdown->Items->AddRange(gcnew cli::array<String^> { "Concept Note", "Matrix Function", "Grammar of Function", "Tokenization", "Three Address Code" });
            this->tutorialDropdown->SelectedIndexChanged += gcnew EventHandler(this, &MyForm::tutorialDropdown_SelectedIndexChanged);

            this->tutorialContent = gcnew RichTextBox();
            this->tutorialContent->Size = Drawing::Size(840, 400);
            this->tutorialContent->Location = Point(20, 70);
            this->tutorialContent->ReadOnly = true;
            this->tutorialContent->BackColor = Color::White;
            this->tutorialContent->ForeColor = Color::Blue;

            this->tutorialBox->Controls->Add(this->tutorialDropdown);
            this->tutorialBox->Controls->Add(this->tutorialContent);

            this->listBoxSuggestions = gcnew ListBox();
            this->listBoxSuggestions->Visible = false;
            this->listBoxSuggestions->Font = gcnew System::Drawing::Font("Consolas", 10);
            this->listBoxSuggestions->Location = Point(30, 380);
            this->listBoxSuggestions->Size = Drawing::Size(200, 100);
            this->listBoxSuggestions->BackColor = Color::White;
            this->listBoxSuggestions->ForeColor = Color::Blue;
            this->listBoxSuggestions->Click += gcnew EventHandler(this, &MyForm::Suggestion_Click);

            this->Controls->Add(this->listBoxSuggestions);
            this->textBoxCode->KeyUp += gcnew KeyEventHandler(this, &MyForm::textBoxCode_KeyUp);
            this->textBoxCode->KeyDown += gcnew KeyEventHandler(this, &MyForm::textBoxCode_KeyDown);

            this->Controls->Add(this->labelCode);
            this->Controls->Add(this->labelint);
            this->Controls->Add(this->labelstr);
            this->Controls->Add(this->textBoxCode);
            this->Controls->Add(this->labelOutput);
            this->Controls->Add(this->textBoxOutput);
            this->Controls->Add(this->buttonRun);
            this->Controls->Add(this->tutorialBox);
        }
        void tutorialDropdown_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
            if (tutorialContent == nullptr) return;
            String^ selectedItem = tutorialDropdown->SelectedItem->ToString();
            if (selectedItem == "Concept Note") {
                tutorialContent->Text = "Concept Note\r\n\n"
                    "Matrix Compiler : Simplifying & Optimizing Matrix Operations\r\n\n"
                    "1. Introduction\r\n\n"
                    "   - Essential in scientific computing, ML, and data analysis.\r\n"
                    "   - Reduces complexity and minimizes errors.\r\n\n"
                    "2. Objective\r\n\n"
                    "   - Parse high-level matrix operations.\r\n"
                    "   - Optimize execution.\r\n"
                    "   - Generate efficient C++ code.\r\n\n"
                    "3. Why a Matrix Compiler?\r\n\n"
                    "   - Eliminates boilerplate code.\r\n"
                    "   - Improves efficiency and usability.\r\n\n"
                    "4. How It Works\r\n\n"
                    "   - Lexer & Parser -> AST -> Optimization -> C++ Code.\r\n"
                    "   - Generates optimized compiled output.\r\n\n";
            }
            else if (selectedItem == "Matrix Function") {
                tutorialContent->Text = "Common Functions for Matrix Operations\r\n\n"
                    "<< Here are 50 functions that can be included in the matrix operation library :>>\r\n\n"

                    "1.create_matrix(rows, cols)\r\n\n"

                    "2.delete_matrix(matrix)\r\n\n"

                    "3.print_matrix(matrix)\r\n\n"

                    "4.copy_matrix(matrix)\r\n\n"

                    "5.identity_matrix(size)\r\n\n"

                    "6.zero_matrix(rows, cols)\r\n\n"

                    "7.random_matrix(rows, cols, min, max)\r\n\n"

                    "8.add_matrices(matrix1, matrix2)\r\n\n"

                    "9.subtract_matrices(matrix1, matrix2)\r\n\n"

                    "10.multiply_matrices(matrix1, matrix2)\r\n\n"

                    "11.scalar_multiply(matrix, scalar)\r\n\n"

                    "12.transpose_matrix(matrix)\r\n\n"

                    "13.invert_matrix(matrix)\r\n\n"

                    "14.matrix_determinant(matrix)\r\n\n"

                    "15.matrix_trace(matrix)\r\n\n"

                    "16.matrix_rank(matrix)\r\n\n"

                    "17.matrix_norm(matrix, type)\r\n\n"

                    "18.matrix_power(matrix, exponent)\r\n\n"

                    "19.matrix_diagonal(matrix)\r\n\n"

                    "20.matrix_is_symmetric(matrix)\r\n\n"

                    "21.matrix_is_identity(matrix)\r\n\n"

                    "22.matrix_is_square(matrix)\r\n\n"

                    "23.matrix_is_singular(matrix)\r\n\n"

                    "24.matrix_is_positive_definite(matrix)\r\n\n"

                    "25.row_swap(matrix, row1, row2)\r\n\n"

                    "26.row_scale(matrix, row, scalar)\r\n\n"

                    "27.row_addition(matrix, row1, row2, scalar)\r\n\n"

                    "28.column_swap(matrix, col1, col2)\r\n\n"

                    "29.column_scale(matrix, col, scalar)\r\n\n"

                    "30.column_addition(matrix, col1, col2, scalar)\r\n\n"

                    "31.get_submatrix(matrix, start_row, start_col, end_row, end_col)\r\n\n"

                    "32.concatenate_matrices_horizontally(matrix1, matrix2)\r\n\n"

                    "33.concatenate_matrices_vertically(matrix1, matrix2)\r\n\n"

                    "34.matrix_lu_decomposition(matrix)\r\n\n"

                    "35.matrix_qr_decomposition(matrix)\r\n\n"

                    "36.matrix_eigenvalues(matrix)\r\n\n"

                    "37.matrix_eigenvectors(matrix)\r\n\n"

                    "38.matrix_cholesky_decomposition(matrix)\r\n\n"

                    "39.matrix_svd(matrix)\r\n\n"

                    "40.matrix_pseudo_inverse(matrix)\r\n\n"

                    "41.matrix_projection(matrix1, matrix2)\r\n\n"

                    "42.kronecker_product(matrix1, matrix2)\r\n\n"

                    "43.hadamard_product(matrix1, matrix2)\r\n\n"

                    "44.outer_product(vector1, vector2)\r\n\n"

                    "45.matrix_dot_product(matrix1, matrix2)\r\n\n"

                    "46.matrix_cross_product(vector1, vector2)\r\n\n"

                    "47.reshape_matrix(matrix, new_rows, new_cols)\r\n\n"

                    "48.flatten_matrix(matrix)\r\n\n"

                    "49.is_equal(matrix1, matrix2, tolerance)\r\n\n"

                    "50.matrix_histogram(matrix);\r\n\n";
            }
            else if (selectedItem == "Grammar of Function") {
                tutorialContent->Text = "Context Free Grammar\r\n\n"
                    "1.Create Matrix:\r\n"
                    "   S->EA\r\n"
                    "   E->create_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->a, a\r\n\n"
                    "2.Delete Matrix\r\n"
                    "   S->EA\r\n"
                    "   E->delete_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"
                    "3.Print Matrix\r\n"
                    "   S->EA\r\n"
                    "   E->print_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"
                    "4.Copy Matrix\r\n"
                    "   S->EA\r\n"
                    "   E->copy_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"
                    "5.Identity Matrix\r\n"
                    "   S->EA\r\n"
                    "   E->identity_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "6.Zero Matrix\r\n"
                    "   S -> EA\r\n"
                    "   E->zero_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->a, a\r\n\n"

                    "7.Random Matrix\r\n"
                    "   S -> EA\r\n"
                    "   E->random_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->a, a, a, a\r\n\n"


                    "Matrix Arithmetic\r\n\n"

                    "8.Add Matrices\r\n"
                    "   S -> EA\r\n"
                    "   E->add_matrices\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "9.Subtract Matrices\r\n"
                    "   S -> EA\r\n"
                    "   E->subtract_matrices\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "10.Multiply Matrices\r\n"
                    "   S -> EA\r\n"
                    "   E->multiply_matrices\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "11.Scalar Multiplication\r\n"
                    "   S -> EA\r\n"
                    "   E->scalar_multiply\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a\r\n\n"


                    "Matrix Properties\r\n\n"

                    "12.Transpose Matrix\r\n"
                    "   S -> EA\r\n"
                    "   E->transpose_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "13.Invert Matrix\r\n"
                    "   S -> EA\r\n"
                    "   E->invert_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "14.Matrix Determinant\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_determinant\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "15.Matrix Trace\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_trace\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "16.Matrix Rank\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_rank\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "17.Matrix Norm\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_norm\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a\r\n\n"

                    "18.Matrix Power\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_power\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a\r\n\n"

                    "19.Matrix Diagonal\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_diagonal\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"


                    "Matrix Properties(Boolean Checks)\r\n\n"

                    "20.Is Symmetric?\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_is_symmetric\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "21.Is Identity?\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_is_identity\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "22.Is Square?\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_is_square\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "23.Is Singular?\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_is_singular\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "24.Is Positive Definite?\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_is_positive_definite\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"


                    "Row and Column Operations\r\n\n"

                    "25.Row Swap\r\n"
                    "   S -> EA\r\n"
                    "   E->row_swap\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a\r\n\n"

                    "26.Row Scale\r\n"
                    "   S -> EA\r\n"
                    "   E->row_scale\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a\r\n\n"

                    "27.Row Addition\r\n"
                    "   S -> EA\r\n"
                    "   E->row_addition\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a, a\r\n\n"

                    "28.Column Swap\r\n"
                    "   S -> EA\r\n"
                    "   E->column_swap\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a\r\n\n"

                    "29.Column Scale\r\n"
                    "   S -> EA\r\n"
                    "   E->column_scale\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a\r\n\n"

                    "30.Column Addition\r\n"
                    "   S -> EA\r\n"
                    "   E->column_addition\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a, a\r\n\n"


                    "Submatrices and Concatenation\r\n\n"

                    "31.Get Submatrix\r\n"
                    "   S -> EA\r\n"
                    "   E->get_submatrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a, a, a\r\n\n"

                    "32.Concatenate Horizontally\r\n"
                    "   S -> EA\r\n"
                    "   E->concatenate_matrices_horizontally\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "33.Concatenate Vertically\r\n"
                    "   S -> EA\r\n"
                    "   E->concatenate_matrices_vertically\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"


                    "Matrix Decompositions\r\n\n"

                    "34.LU Decomposition\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_lu_decomposition\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "35.QR Decomposition\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_qr_decomposition\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "36.Eigenvalues\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_eigenvalues\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "37.Eigenvectors\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_eigenvectors\r\n"
                    "   A -> (C)\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "38.Cholesky Decomposition\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_cholesky_decomposition\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "39.Singular Value Decomposition (SVD)\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_svd\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "40.Pseudo-Inverse\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_pseudo_inverse\r\n"
                    "   A -> (C)"
                    "   C->S\r\n\n"


                    "Matrix Products\r\n\n"

                    "41.Projection\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_projection\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "42.Kronecker Product\r\n"
                    "   S -> EA\r\n"
                    "   E->kronecker_product\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "43.Hadamard Product\r\n"
                    "   S -> EA\r\n"
                    "   E->hadamard_product\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "44.Outer Product\r\n"
                    "   S -> EA\r\n"
                    "   E->outer_product\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "45.Dot Product\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_dot_product\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S\r\n\n"
                    "46.Cross Product\r\n"
                    "   S -> EA\r\n"
                    "   E->matrix_cross_product\r\n"
                    "   A-> (C)\r\n"
                    "   C->S, S\r\n\n"

                    "Reshaping and Comparison\r\n\n"

                    "47.Reshape Matrix\r\n"
                    "   S->EA\r\n"
                    "   E -> reshape_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, a, a\r\n\n"
                    "48.Flatten Matrix\r\n"
                    "   S->EA\r\n"
                    "   E->flatten_matrix\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n"

                    "49.Is Equal\r\n"
                    "   S -> EA\r\n"
                    "   E->is_equal\r\n"
                    "   A -> (C)\r\n"
                    "   C->S, S, a\r\n\n"

                    "50.Matrix Histogram\r\n"
                    "   S->EA\r\n"
                    "   E->matrix_histogram\r\n"
                    "   A -> (C)\r\n"
                    "   C->S\r\n\n";
            }
            else if (selectedItem == "Tokenization") {
                tutorialContent->Text = "Tokenization of the matrix operations [Grammar]\r\n"
                    "Function Declaration : double calculateDeterminant(double** matrix, int size)\r\n"
                    "Token: double->KT\r\n"
                    "Token : calculateDeterminant->IT\r\n"
                    "Token : (->DT\r\n"
                    "Token : double** ->IT\r\n"
                    "Token : matrix->IT\r\n"
                    "Token : , -> DT\r\n"
                    "Token : int->KT\r\n"
                    "Token : size->IT\r\n"
                    "Token : )->DT\r\n"
                    "Token : ;->DT\r\n\n\n\n\n\n\n"

                    "Similarly it is mostly same for everone \r\n";
            }
            else if (selectedItem == "Three Address Code") {
                tutorialContent->Text = "Three - Address Code (TAC) for Matrix Operations\r\n\n"
                    "1. Create Matrix\r\n\n"
                    "L1 : t1 = create_matrix(a, a)\r\n"
                    "matrix = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "2. Delete Matrix\r\n\n"
                    "L1 : if matrix != NULL goto L2\r\n"
                    "delete_matrix(matrix)\r\n"
                    "matrix = NULL\r\n"
                    "goto L3\r\n"
                    "L2 :\r\n"
                    "goto L3\r\n"
                    "L3 :\r\n\n"

                    "3. Print Matrix \r\n\n"
                    "L1 : print_matrix(matrix)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "4. Copy Matrix\r\n\n"
                    "L1 : t1 = copy_matrix(matrix)\r\n"
                    "copy = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "5. Identity Matrix\r\n\n"
                    "L1 : t1 = identity_matrix(n)\r\n"
                    "matrix = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "6. Zero Matrix\r\n\n"
                    "L1 : t1 = zero_matrix(a, a)\r\n"
                    "matrix = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "7. Random Matrix\r\n\n"
                    "L1 : t1 = random_matrix(a, a, a, a)\r\n"
                    "matrix = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "8. Add Matrices\r\n\n"
                    "L1 : t1 = add_matrices(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "9. Subtract Matrices\r\n\n"
                    "L1 : t1 = subtract_matrices(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "10. Multiply Matrices\r\n\n"
                    "L1 : t1 = multiply_matrices(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "11. Scalar Multiplication\r\n\n"
                    "L1 : t1 = scalar_multiply(A, scalar)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "12. Transpose Matrix\r\n\n"
                    "L1 : t1 = transpose_matrix(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "13. Invert Matrix\r\n\n"
                    "L1 : t1 = invert_matrix(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "14. Matrix Determinant\r\n\n"
                    "L1 : t1 = matrix_determinant(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "15. Matrix Trace\r\n\n"
                    "L1 : t1 = matrix_trace(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "16. Matrix Rank\r\n\n"
                    "L1 : t1 = matrix_rank(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "17. Matrix Norm\r\n\n"
                    "L1 : t1 = matrix_norm(A, p)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "18. Matrix Power\r\n\n"
                    "L1 : t1 = matrix_power(A, n)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "19. Matrix Diagonal\r\n\n"
                    "L1 : t1 = matrix_diagonal(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "20. Is Symmetric ?\r\n\n"
                    "L1 : t1 = matrix_is_symmetric(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "21. Is Identity ?\r\n\n"
                    "L1 : t1 = matrix_is_identity(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "22. Is Square ?\r\n\n"
                    "L1 : t1 = matrix_is_square(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "23. Is Singular ?\r\n\n"
                    "L1 : t1 = matrix_is_singular(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "24. Is Positive Definite ?\r\n\n"
                    "L1 : t1 = matrix_is_positive_definite(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "25. Row Swap\r\n\n"
                    "L1 : row_swap(A, r1, r2)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "26. Row Scale\r\n\n"
                    "L1 : row_scale(A, r, scalar)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "27. Row Addition\r\n\n"
                    "L1 : row_addition(A, r1, r2, scalar)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "28. Column Swap\r\n\n"
                    "L1 : column_swap(A, c1, c2)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "29. Column Scale\r\n\n"
                    "L1 : column_scale(A, c, scalar)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "30. Column Addition\r\n\n"
                    "L1 : column_addition(A, c1, c2, scalar)\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "31. Get Submatrix\r\n\n"
                    "L1 : t1 = get_submatrix(A, r1, r2, c1, c2)\r\n"
                    "result = \r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "32. Concatenate Horizontally\r\n\n"
                    "L1 : t1 = concatenate_matrices_horizontally(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "33. Concatenate Vertically\r\n\n"
                    "L1 : t1 = concatenate_matrices_vertically(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "34. LU Decomposition\r\n\n"
                    "L1 : t1 = matrix_lu_decomposition(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "35. QR Decomposition\r\n\n"
                    "L1 : t1 = matrix_qr_decomposition(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "36. Eigenvalues\r\n\n"
                    "L1 : t1 = matrix_eigenvalues(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "37. Eigenvectors\r\n\n"
                    "L1 : t1 = matrix_eigenvectors(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "38. Cholesky Decomposition\r\n\n"
                    "L1 : t1 = matrix_cholesky_decomposition(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "39. Singular Value Decomposition\r\n\n"
                    "L1 : t1 = matrix_svd(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "40. Pseudo - Inverse\r\n\n"
                    "L1 : t1 = matrix_pseudo_inverse(A)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "41. Projection\r\n\n"
                    "L1 : t1 = matrix_projection(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "42. Kronecker Product\r\n\n"
                    "L1 : t1 = kronecker_product(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "43. Hadamard Product\r\n\n"
                    "L1 : t1 = hadamard_product(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "44. Outer Product\r\n\n"
                    "L1 : t1 = outer_product(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "45. Dot Product\r\n\n"
                    "L1 : t1 = matrix_dot_product(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n"

                    "46. Cross Product\r\n\n"
                    "L1 : t1 = matrix_cross_product(A, B)\r\n"
                    "result = t1\r\n"
                    "goto L2\r\n"
                    "L2 :\r\n\n";
            }
        }

        void textBoxCode_TextChanged(System::Object^ sender, System::EventArgs^ e) {
            HighlightSyntax();
        }

        void HighlightSyntax() {
            int pos = textBoxCode->SelectionStart;
            int len = textBoxCode->SelectionLength;

            textBoxCode->SuspendLayout();
            textBoxCode->SelectAll();
            textBoxCode->SelectionColor = Color::AntiqueWhite;

            keywords = gcnew cli::array<String^> {
                "create_matrix", "print_matrix", "delete_matrix", "copy_matrix",
                    "identity_matrix", "zero_matrix", "random_matrix",
                    "add_matrix", "subtract_matrix", "multiply_matrix",
                    "scalar_multiply", "transpose_matrix", "invert_matrix", "matrix_determinant",
                    "matrix_trace", "matrix_rank", "matrix_norm", "matrix_power", "matrix_diagonal",
                    "matrix_is_symmetric", "matrix_is_identity", "matrix_is_square", "matrix_is_singular",
                    "matrix_is_positive_definite", "row_swap", "row_scale", "row_addition",
                    "column_swap", "column_scale", "column_addition",
                    "get_submatrix", "concatenate_matrices_horizontally", "concatenate_matrices_vertically",
                    "matrix_lu_decomposition", "matrix_qr_decomposition", "matrix_eigenvalues",
                    "matrix_cholesky_decomposition", "matrix_svd", "matrix_pseudo_inverse",
                    "matrix_projection", "kronecker_product", "hadamard_product", "outer_product",
                    "matrix_dot_product", "matrix_cross_product", "reshape_matrix", "flatten_matrix",
                    "is_equal", "matrix_histogram"
            };

            for each (String ^ keyword in keywords) {
                MatchCollection^ matches = Regex::Matches(textBoxCode->Text, "\\b" + keyword + "\\b");
                for each (Match ^ match in matches) {
                    textBoxCode->Select(match->Index, match->Length);
                    textBoxCode->SelectionColor = Color::Blue;
                    textBoxCode->SelectionFont = gcnew System::Drawing::Font("Consolas", 10, FontStyle::Bold);
                }
            }

            MatchCollection^ numbers = Regex::Matches(textBoxCode->Text, "\\b\\d+\\b");
            for each (Match ^ match in numbers) {
                textBoxCode->Select(match->Index, match->Length);
                textBoxCode->SelectionColor = Color::DarkCyan;
            }

            MatchCollection^ vars = Regex::Matches(textBoxCode->Text, "\\b[a-zA-Z_]\\w*\\b");
            for each (Match ^ match in vars) {
                String^ word = match->Value;
                if (Array::IndexOf(keywords, word) == -1) {
                    textBoxCode->Select(match->Index, match->Length);
                    textBoxCode->SelectionColor = Color::White;
                }
            }

            // Restore selection
            textBoxCode->Select(pos, len);
            textBoxCode->SelectionColor = Color::Black;
            textBoxCode->ResumeLayout();
        }

        void textBoxCode_KeyUp(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
            if (e->KeyCode == Keys::Up || e->KeyCode == Keys::Down || e->KeyCode == Keys::Enter)
                return;


            int pos = textBoxCode->SelectionStart;
            String^ text = textBoxCode->Text->Substring(0, pos);
            int start = text->LastIndexOf(' ');
            if (start < 0) start = 0;
            String^ currentWord = text->Substring(start)->Trim();

            listBoxSuggestions->Items->Clear();
            for each (String ^ keyword in keywords) {
                if (keyword->StartsWith(currentWord) && currentWord != "") {
                    listBoxSuggestions->Items->Add(keyword);
                }
            }

            if (listBoxSuggestions->Items->Count > 0) {
                listBoxSuggestions->Visible = true;
                listBoxSuggestions->BringToFront();
            }
            else {
                listBoxSuggestions->Visible = false;
            }
        }

        void textBoxCode_KeyDown(System::Object^ sender, KeyEventArgs^ e) {
            if (listBoxSuggestions->Visible) {
                if (e->KeyCode == Keys::Down) {
                    if (listBoxSuggestions->SelectedIndex < listBoxSuggestions->Items->Count - 1)
                        listBoxSuggestions->SelectedIndex++;
                    e->SuppressKeyPress = true;
                }
                else if (e->KeyCode == Keys::Up) {
                    if (listBoxSuggestions->SelectedIndex > 0)
                        listBoxSuggestions->SelectedIndex--;
                    e->SuppressKeyPress = true;
                }
                else if (e->KeyCode == Keys::Enter && listBoxSuggestions->SelectedIndex >= 0) {
                    InsertSuggestion();
                    e->SuppressKeyPress = true;
                }
            }
        }

        void Suggestion_Click(System::Object^ sender, EventArgs^ e) {
            InsertSuggestion();
        }

        void InsertSuggestion() {
            if (listBoxSuggestions->SelectedItem != nullptr) {
                int pos = textBoxCode->SelectionStart;
                String^ text = textBoxCode->Text->Substring(0, pos);
                int start = text->LastIndexOf(' ');
                if (start < 0) start = 0;
                String^ before = textBoxCode->Text->Substring(0, start);
                String^ after = textBoxCode->Text->Substring(pos);

                String^ selected = listBoxSuggestions->SelectedItem->ToString();
                textBoxCode->Text = before + " " + selected + after;
                textBoxCode->SelectionStart = (before + " " + selected)->Length;
                listBoxSuggestions->Visible = false;
            }
        }
        int Determinant(std::vector<std::vector<int>> mat) {
            int n = mat.size();
            if (n == 1) return mat[0][0];
            if (n == 2) return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
            int det = 0;
            for (int p = 0; p < n; ++p) {
                std::vector<std::vector<int>> submat(n - 1, std::vector<int>(n - 1));
                for (int i = 1; i < n; ++i) {
                    int colIdx = 0;
                    for (int j = 0; j < n; ++j) {
                        if (j == p) continue;
                        submat[i - 1][colIdx++] = mat[i][j];
                    }
                }
                det += mat[0][p] * Determinant(submat) * ((p % 2 == 0) ? 1 : -1);
            }
            return det;
        }


        void buttonRun_Click(System::Object^ sender, EventArgs^ e) {
            matrixVars.clear();
            textBoxOutput->Clear();
            std::string code = marshal_as<std::string>(textBoxCode->Text);
            std::istringstream stream(code);
            std::string line;
            int lineNumber = 0;

            while (getline(stream, line)) {
                lineNumber++;
                std::string originalLine = line;

                // Remove all whitespace
                line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

                if (line.empty()) continue;

                // Syntax checking for known commands
                if (line.find("=") != std::string::npos && line.find("create_matrix(") != std::string::npos) {
                    if (line.back() != ';') {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": Missing semicolon.\n").c_str()));
                        continue;
                    }
                    if (line.find("create_matrix(") == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": Invalid function call.\n").c_str()));
                        continue;
                    }
                }
                else if (line.find("print_matrix(") == 0) {
                    if (line.back() != ';') {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": Missing semicolon.\n").c_str()));
                        continue;
                    }
                }
                else {
                    // For functions without assignment (like delete_matrix, identity_matrix, add_matrices)
                    if (line.back() != ';') {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": Missing semicolon.\n").c_str()));
                        continue;
                    }
                }

                
                if (line.find("=create_matrix(") != std::string::npos) {
                    size_t eq = line.find("=");
                    size_t open = line.find("(");
                    size_t comma = line.find(",", open);
                    size_t close = line.find(")", comma);

                    std::string varName = line.substr(0, eq);
                    int rows = std::stoi(line.substr(open + 1, comma - open - 1));
                    int cols = std::stoi(line.substr(comma + 1, close - comma - 1));

                    if (rows != cols) {
                        textBoxOutput->AppendText(gcnew String(("Error: Only square matrices supported. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> matrix;
                    InputMatrix(matrix, rows);
                    matrixVars[varName] = matrix;
                }
                else if (line.find("print_matrix(") == 0) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string varName = line.substr(open + 1, close - open - 1);

                    if (matrixVars.count(varName)) {
                        textBoxOutput->AppendText(DisplayMatrix(matrixVars[varName]) + "\n");
                    }
                    else {
                        textBoxOutput->AppendText(gcnew String(("Error: matrix '" + varName + "' not found.\n").c_str()));
                    }
                }
                else if (line.find("delete_matrix(") == 0) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string varName = line.substr(open + 1, close - open - 1);

                    if (matrixVars.erase(varName)) {
                        textBoxOutput->AppendText(gcnew String(("Matrix '" + varName + "' deleted.\n").c_str()));
                    }
                    else {
                        textBoxOutput->AppendText(gcnew String(("Error: matrix '" + varName + "' not found.\n").c_str()));
                    }
                }
                else if (line.find("copy_matrix(") != std::string::npos) {
                    size_t eq = line.find("=");
                    std::string newName = line.substr(0, eq);
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string sourceName = line.substr(open + 1, close - open - 1);

                    if (matrixVars.count(sourceName)) {
                        matrixVars[newName] = matrixVars[sourceName]; // Copy
                        textBoxOutput->AppendText(gcnew String(("Matrix copied to '" + newName + "'.\n").c_str()));
                    }
                    else {
                        textBoxOutput->AppendText(gcnew String(("Error: matrix '" + sourceName + "' not found.\n").c_str()));
                    }
                }
                else if (line.find("identity_matrix(") != std::string::npos) {
                    // Supports both assignment (e.g., a=identity_matrix(3);) and non-assignment (e.g., identity_matrix(3);)
                    size_t eq = line.find("=");
                    std::string varName;
                    if (eq != std::string::npos) {
                        varName = line.substr(0, eq);
                    }
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    int size = std::stoi(line.substr(open + 1, close - open - 1));

                    std::vector<std::vector<int>> identity(size, std::vector<int>(size, 0));
                    for (int i = 0; i < size; ++i)
                        identity[i][i] = 1;

                    if (eq != std::string::npos)
                        matrixVars[varName] = identity;
                    else {
                        varName = "I" + std::to_string(size);
                        matrixVars[varName] = identity;
                    }
                    textBoxOutput->AppendText(gcnew String(("Identity matrix created: '" + varName + "'.\n").c_str()));
                }
                else if (line.find("zero_matrix(") != std::string::npos) {
                    size_t eq = line.find("=");
                    std::string varName = line.substr(0, eq);
                    size_t open = line.find("(");
                    size_t comma = line.find(",", open);
                    size_t close = line.find(")", comma);

                    int rows = std::stoi(line.substr(open + 1, comma - open - 1));
                    int cols = std::stoi(line.substr(comma + 1, close - comma - 1));

                    std::vector<std::vector<int>> zeros(rows, std::vector<int>(cols, 0));
                    matrixVars[varName] = zeros;
                    textBoxOutput->AppendText(gcnew String(("Zero matrix created: '" + varName + "'.\n").c_str()));
                }
                else if (line.find("random_matrix(") != std::string::npos) {
                    size_t eq = line.find("=");
                    std::string varName = line.substr(0, eq);
                    size_t open = line.find("(");
                    size_t close = line.find(")");

                    std::string args = line.substr(open + 1, close - open - 1);
                    std::replace(args.begin(), args.end(), ',', ' ');
                    std::istringstream ss(args);

                    int rows, cols, minVal, maxVal;
                    ss >> rows >> cols >> minVal >> maxVal;

                    std::vector<std::vector<int>> mat(rows, std::vector<int>(cols));
                    for (int i = 0; i < rows; ++i)
                        for (int j = 0; j < cols; ++j)
                            mat[i][j] = rand() % (maxVal - minVal + 1) + minVal;

                    matrixVars[varName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Random matrix created: '" + varName + "'.\n").c_str()));
                }
                else if (line.find("=") != std::string::npos && line.find("add_matrix(") != std::string::npos) {
                    size_t eq = line.find("=");
                    std::string resultVar = line.substr(0, eq);
                    std::string funcCall = line.substr(eq + 1);

                    // Remove whitespace
                    resultVar.erase(remove_if(resultVar.begin(), resultVar.end(), ::isspace), resultVar.end());
                    funcCall.erase(remove_if(funcCall.begin(), funcCall.end(), ::isspace), funcCall.end());

                    size_t open = funcCall.find("(");
                    size_t close = funcCall.find(")");
                    std::string args = funcCall.substr(open + 1, close - open - 1);

                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": add_matrices requires two arguments.\n").c_str()));
                        continue;
                    }

                    std::string A = args.substr(0, comma);
                    std::string B = args.substr(comma + 1);

                    
                    if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: One or both matrices not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> matA = matrixVars[A];
                    std::vector<std::vector<int>> matB = matrixVars[B];
                    if (matA.size() != matB.size() || matA[0].size() != matB[0].size()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrices must be of same dimensions. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    // Perform addition
                    int rows = matA.size();
                    int cols = matA[0].size();
                    std::vector<std::vector<int>> result(rows, std::vector<int>(cols, 0));
                    for (int i = 0; i < rows; ++i)
                        for (int j = 0; j < cols; ++j)
                            result[i][j] = matA[i][j] + matB[i][j];

                    matrixVars[resultVar] = result;
                    textBoxOutput->AppendText(gcnew String(("Sum matrix stored as '" + resultVar + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }

                else if (line.find("subtract_matrix(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": subtract_matrices requires two arguments.\n").c_str()));
                        continue;
                    }
                    std::string A = args.substr(0, comma);
                    std::string B = args.substr(comma + 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                    B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                    if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: One or both matrices not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> matA = matrixVars[A];
                    std::vector<std::vector<int>> matB = matrixVars[B];
                    if (matA.size() != matB.size() || matA[0].size() != matB[0].size()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrices must be of same dimensions. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    int rows = matA.size();
                    int cols = matA[0].size();
                    std::vector<std::vector<int>> result(rows, std::vector<int>(cols, 0));
                    for (int i = 0; i < rows; ++i) {
                        for (int j = 0; j < cols; ++j)
                            result[i][j] = matA[i][j] - matB[i][j];
                    }
                    std::string diffName = A + "_minus_" + B;
                    matrixVars[diffName] = result;
                    textBoxOutput->AppendText(gcnew String(("Difference matrix stored as '" + diffName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }
                else if (line.find("multiply_matrix(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": multiply_matrices requires two arguments.\n").c_str()));
                        continue;
                    }
                    std::string A = args.substr(0, comma);
                    std::string B = args.substr(comma + 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                    B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                    if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: One or both matrices not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> matA = matrixVars[A];
                    std::vector<std::vector<int>> matB = matrixVars[B];
                    if (matA.size() != matB.size() || matA[0].size() != matB[0].size()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrices must be of same dimensions for element-wise multiplication. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    int rows = matA.size();
                    int cols = matA[0].size();
                    std::vector<std::vector<int>> result(rows, std::vector<int>(cols, 0));
                    for (int i = 0; i < rows; ++i) {
                        for (int j = 0; j < cols; ++j)
                            result[i][j] = matA[i][j] * matB[i][j];
                    }
                    std::string mulName = A + "_times_" + B;
                    matrixVars[mulName] = result;
                    textBoxOutput->AppendText(gcnew String(("Element-wise product matrix stored as '" + mulName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }
                else if (line.find("scalar_multiply(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": scalar_multiply requires matrix and scalar.\n").c_str()));
                        continue;
                    }
                    std::string A = args.substr(0, comma);
                    std::string scalarStr = args.substr(comma + 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                    scalarStr.erase(remove_if(scalarStr.begin(), scalarStr.end(), ::isspace), scalarStr.end());

                    if (matrixVars.find(A) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    int scalar = std::stoi(scalarStr);
                    std::vector<std::vector<int>> matA = matrixVars[A];
                    int rows = matA.size();
                    int cols = matA[0].size();
                    std::vector<std::vector<int>> result(rows, std::vector<int>(cols, 0));
                    for (int i = 0; i < rows; ++i) {
                        for (int j = 0; j < cols; ++j)
                            result[i][j] = matA[i][j] * scalar;
                    }
                    std::string name = A + "_times_" + scalarStr;
                    matrixVars[name] = result;
                    textBoxOutput->AppendText(gcnew String(("Scalar multiplied matrix stored as '" + name + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }
                else if (line.find("transpose_matrix(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string A = line.substr(open + 1, close - open - 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());

                    if (matrixVars.find(A) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> matA = matrixVars[A];
                    int rows = matA.size();
                    int cols = matA[0].size();
                    std::vector<std::vector<int>> result(cols, std::vector<int>(rows, 0));
                    for (int i = 0; i < rows; ++i) {
                        for (int j = 0; j < cols; ++j)
                            result[j][i] = matA[i][j];
                    }
                    std::string name = A + "_T";
                    matrixVars[name] = result;
                    textBoxOutput->AppendText(gcnew String(("Transposed matrix stored as '" + name + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }
                // Invert matrix
                else if (line.find("invert_matrix(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string arg = line.substr(open + 1, close - open - 1);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    int n = mat.size();
                    if (n != mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Only square matrices can be inverted. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<double>> identity(n, std::vector<double>(n, 0));
                    std::vector<std::vector<double>> matDouble(n, std::vector<double>(n));

                    for (int i = 0; i < n; ++i) {
                        identity[i][i] = 1;
                        for (int j = 0; j < n; ++j)
                            matDouble[i][j] = mat[i][j];
                    }

                    // Gaussian Elimination
                    for (int i = 0; i < n; ++i) {
                        if (matDouble[i][i] == 0) {
                            bool swapped = false;
                            for (int j = i + 1; j < n; ++j) {
                                if (matDouble[j][i] != 0) {
                                    std::swap(matDouble[i], matDouble[j]);
                                    std::swap(identity[i], identity[j]);
                                    swapped = true;
                                    break;
                                }
                            }
                            if (!swapped) {
                                textBoxOutput->AppendText(gcnew String(("Error: Matrix is singular and cannot be inverted. Line " + std::to_string(lineNumber) + "\n").c_str()));
                                goto skip_invert;
                            }
                        }

                        double diag = matDouble[i][i];
                        for (int j = 0; j < n; ++j) {
                            matDouble[i][j] /= diag;
                            identity[i][j] /= diag;
                        }

                        for (int k = 0; k < n; ++k) {
                            if (k != i) {
                                double factor = matDouble[k][i];
                                for (int j = 0; j < n; ++j) {
                                    matDouble[k][j] -= factor * matDouble[i][j];
                                    identity[k][j] -= factor * identity[i][j];
                                }
                            }
                        }
                    }

                    {
                        std::string invName = arg + "_inverse";
                        std::vector<std::vector<int>> intResult(n, std::vector<int>(n));
                        for (int i = 0; i < n; ++i)
                            for (int j = 0; j < n; ++j)
                                intResult[i][j] = static_cast<int>(round(identity[i][j]));

                        matrixVars[invName] = intResult;
                        textBoxOutput->AppendText(gcnew String(("Inverse matrix stored as '" + invName + "'.\n").c_str()));
                        textBoxOutput->AppendText(DisplayMatrix(intResult) + "\n");
                    }

                skip_invert:
                    continue;
                }

                // Matrix Determinant
                else if (line.find("matrix_determinant(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string arg = line.substr(open + 1, close - open - 1);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    int det = Determinant(mat);

                    textBoxOutput->AppendText(gcnew String(("Determinant of '" + arg + "' is: " + std::to_string(det) + "\n").c_str()));
                    continue;
                }


                // Matrix Trace
                else if (line.find("matrix_trace(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string arg = line.substr(open + 1, close - open - 1);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    int trace = 0;
                    for (int i = 0; i < mat.size(); ++i)
                        trace += mat[i][i];

                    textBoxOutput->AppendText(gcnew String(("Trace of '" + arg + "' is: " + std::to_string(trace) + "\n").c_str()));
                    continue;
                }

                // Matrix Rank (naive approach: number of non-zero rows after row-reduction)
                else if (line.find("matrix_rank(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string arg = line.substr(open + 1, close - open - 1);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    int m = mat.size(), n = mat[0].size();
                    int rank = 0;

                    for (int row = 0, col = 0; row < m && col < n; ++col) {
                        int sel = row;
                        for (int i = row; i < m; ++i)
                            if (abs(mat[i][col]) > abs(mat[sel][col])) sel = i;

                        if (mat[sel][col] == 0) continue;

                        std::swap(mat[row], mat[sel]);
                        for (int i = row + 1; i < m; ++i) {
                            int factor = mat[i][col] / mat[row][col];
                            for (int j = col; j < n; ++j)
                                mat[i][j] -= factor * mat[row][j];
                        }
                        ++rank;
                        ++row;
                    }

                    textBoxOutput->AppendText(gcnew String(("Rank of '" + arg + "' is: " + std::to_string(rank) + "\n").c_str()));
                    continue;
                }

                // Matrix Norm
                else if (line.find("matrix_norm(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error: matrix_norm requires two arguments. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::string name = args.substr(0, comma);
                    std::string type = args.substr(comma + 1);
                    name.erase(remove_if(name.begin(), name.end(), ::isspace), name.end());
                    type.erase(remove_if(type.begin(), type.end(), ::isspace), type.end());

                    if (matrixVars.find(name) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[name];
                    double norm = 0;

                    if (type == "\"frobenius\"") {
                        for (auto& row : mat)
                            for (int val : row)
                                norm += val * val;
                        norm = sqrt(norm);
                    }
                    else {
                        textBoxOutput->AppendText(gcnew String(("Unsupported norm type. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    textBoxOutput->AppendText(gcnew String(("Norm of '" + name + "' is: " + std::to_string(norm) + "\n").c_str()));
                    continue;
                }

                // Matrix Power
                else if (line.find("matrix_power(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error: matrix_power requires two arguments. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::string name = args.substr(0, comma);
                    std::string expStr = args.substr(comma + 1);
                    name.erase(remove_if(name.begin(), name.end(), ::isspace), name.end());
                    expStr.erase(remove_if(expStr.begin(), expStr.end(), ::isspace), expStr.end());
                    int power = std::stoi(expStr);

                    if (matrixVars.find(name) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> base = matrixVars[name];
                    if (base.size() != base[0].size()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Only square matrices can be exponentiated. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    int n = base.size();
                    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));
                    for (int i = 0; i < n; ++i)
                        result[i][i] = 1;

                    while (power > 0) {
                        if (power % 2 == 1) {
                            std::vector<std::vector<int>> temp(n, std::vector<int>(n, 0));
                            for (int i = 0; i < n; ++i)
                                for (int j = 0; j < n; ++j)
                                    for (int k = 0; k < n; ++k)
                                        temp[i][j] += result[i][k] * base[k][j];
                            result = temp;
                        }

                        std::vector<std::vector<int>> temp(n, std::vector<int>(n, 0));
                        for (int i = 0; i < n; ++i)
                            for (int j = 0; j < n; ++j)
                                for (int k = 0; k < n; ++k)
                                    temp[i][j] += base[i][k] * base[k][j];
                        base = temp;
                        power /= 2;
                    }

                    std::string powName = name + "_pow_" + expStr;
                    matrixVars[powName] = result;
                    textBoxOutput->AppendText(gcnew String(("Power matrix stored as '" + powName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                    continue;
                }
                else if (line.find("matrix_diagonal(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        String^ errorMsg = gcnew String(("Error: Matrix " + arg + " not found.\n").c_str());
                        textBoxOutput->AppendText(errorMsg);
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    int size = std::min<int>(mat.size(), mat[0].size());
                    std::string diag = "Diagonal of matrix " + arg + ": ";
                    for (int i = 0; i < size; ++i)
                        diag += std::to_string(mat[i][i]) + " ";

                    String^ diagMsg = gcnew String((diag + "\n").c_str());
                    textBoxOutput->AppendText(diagMsg);
                }


                else if (line.find("matrix_is_symmetric(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + arg + " not found.\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    if (mat.size() != mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Matrix is not square, hence not symmetric.\n"));
                        continue;
                    }

                    bool symmetric = true;
                    for (int i = 0; i < mat.size(); ++i)
                        for (int j = 0; j < mat[0].size(); ++j)
                            if (mat[i][j] != mat[j][i])
                                symmetric = false;

                    std::string message = "Matrix is symmetric.\n";
                    textBoxOutput->AppendText(gcnew String(message.c_str()));

                }
                else if (line.find("matrix_is_identity(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        std::string msg = "Error: Matrix " + arg + " not found.\n";
                        textBoxOutput->AppendText(gcnew String(msg.c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    bool is_identity = true;

                    if (mat.size() != mat[0].size()) {
                        std::string msg = "Matrix is not square, hence not identity.\n";
                        textBoxOutput->AppendText(gcnew String(msg.c_str()));
                        continue;
                    }

                    for (int i = 0; i < mat.size(); ++i)
                        for (int j = 0; j < mat[0].size(); ++j)
                            if ((i == j && mat[i][j] != 1) || (i != j && mat[i][j] != 0))
                                is_identity = false;

                    std::string result = is_identity ? "Matrix is identity.\n" : "Matrix is not identity.\n";
                    textBoxOutput->AppendText(gcnew String(result.c_str()));
                }

                else if (line.find("matrix_is_square(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + arg + " not found.\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    bool square = (mat.size() == mat[0].size());
                    textBoxOutput->AppendText("Matrix is not square, hence not identity.\n");


                }
                else if (line.find("matrix_is_singular(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + arg + " not found.\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    if (mat.size() != mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Matrix is not square, so singularity can't be determined.\n"));
                        continue;
                    }

                    int det = Determinant(mat);  // use previously defined function
                    textBoxOutput->AppendText(det == 0 ? gcnew String("Matrix is singular.\n") : gcnew String("Matrix is not singular.\n"));

                }
                else if (line.find("matrix_is_positive_definite(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + arg + " not found.\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    if (mat.size() != mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Matrix is not square.\n"));
                        continue;
                    }

                    bool isPositiveDefinite = true;

                    for (int i = 0; i < mat.size(); ++i) {
                        if (mat[i][i] <= 0) {
                            isPositiveDefinite = false;
                            break;
                        }
                    }

                    textBoxOutput->AppendText(isPositiveDefinite ? gcnew String("Matrix is positive definite.\n") : gcnew String("Matrix is not positive definite.\n"));

                }
                else if (line.find("row_swap(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int row1, row2;
                    getline(ss, matrixName, ',');
                    ss >> row1;
                    ss.ignore(); // skip comma
                    ss >> row2;

                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + matrixName + " not found.\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (row1 >= mat.size() || row2 >= mat.size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Row index out of range.\n"));
                        continue;
                    }

                    std::swap(mat[row1], mat[row2]);

                    std::string resultName = matrixName + "_row_swapped";
                    matrixVars[resultName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Row-swapped matrix stored as '" + resultName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(mat) + "\n");
                }
                else if (line.find("row_scale(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int row, scalar;
                    getline(ss, matrixName, ',');
                    ss >> row;
                    ss.ignore(); // skip comma
                    ss >> scalar;

                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + matrixName + " not found.\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (row >= mat.size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Row index out of range.\n"));
                        continue;
                    }

                    for (int& val : mat[row]) {
                        val *= scalar;
                    }

                    std::string resultName = matrixName + "_row_scaled";
                    matrixVars[resultName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Row-scaled matrix stored as '" + resultName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(mat) + "\n");
                }
                else if (line.find("row_addition(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int row1, row2, scalar;
                    getline(ss, matrixName, ',');
                    ss >> row1;
                    ss.ignore();
                    ss >> row2;
                    ss.ignore();
                    ss >> scalar;

                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + matrixName + " not found.\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (row1 >= mat.size() || row2 >= mat.size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Row index out of range.\n"));
                        continue;
                    }

                    for (int j = 0; j < mat[0].size(); ++j) {
                        mat[row1][j] += scalar * mat[row2][j];
                    }

                    std::string resultName = matrixName + "_row_added";
                    matrixVars[resultName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Row-added matrix stored as '" + resultName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(mat) + "\n");
                }
                else if (line.find("column_swap(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int col1, col2;
                    getline(ss, matrixName, ',');
                    ss >> col1;
                    ss.ignore();
                    ss >> col2;

                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + matrixName + " not found.\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (col1 >= mat[0].size() || col2 >= mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Column index out of range.\n"));
                        continue;
                    }

                    for (int i = 0; i < mat.size(); ++i) {
                        std::swap(mat[i][col1], mat[i][col2]);
                    }

                    std::string resultName = matrixName + "_col_swapped";
                    matrixVars[resultName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Column-swapped matrix stored as '" + resultName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(mat) + "\n");
                }
                else if (line.find("column_scale(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int col, scalar;
                    getline(ss, matrixName, ',');
                    ss >> col;
                    ss.ignore();
                    ss >> scalar;

                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + matrixName + " not found.\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (col >= mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Column index out of range.\n"));
                        continue;
                    }

                    for (int i = 0; i < mat.size(); ++i) {
                        mat[i][col] *= scalar;
                    }

                    std::string resultName = matrixName + "_col_scaled";
                    matrixVars[resultName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Column-scaled matrix stored as '" + resultName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(mat) + "\n");
                }
                else if (line.find("column_addition(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int col1, col2, scalar;
                    getline(ss, matrixName, ',');
                    ss >> col1;
                    ss.ignore();
                    ss >> col2;
                    ss.ignore();
                    ss >> scalar;

                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + matrixName + " not found.\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (col1 >= mat[0].size() || col2 >= mat[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Column index out of range.\n"));
                        continue;
                    }

                    for (int i = 0; i < mat.size(); ++i) {
                        mat[i][col1] += scalar * mat[i][col2];
                    }

                    std::string resultName = matrixName + "_col_added";
                    matrixVars[resultName] = mat;
                    textBoxOutput->AppendText(gcnew String(("Column-added matrix stored as '" + resultName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(mat) + "\n");
                }
                else if (line.find("get_submatrix(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    std::stringstream ss(args);
                    std::string matrixName;
                    int r1, c1, r2, c2;
                    char comma;
                    ss >> matrixName >> comma >> r1 >> comma >> c1 >> comma >> r2 >> comma >> c2;
                    matrixName.erase(remove_if(matrixName.begin(), matrixName.end(), ::isspace), matrixName.end());

                    if (matrixVars.find(matrixName) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Matrix not found: " + matrixName + "\n").c_str()));
                        continue;
                    }

                    auto mat = matrixVars[matrixName];
                    if (r1 < 0 || c1 < 0 || r2 >= mat.size() || c2 >= mat[0].size() || r1 > r2 || c1 > c2) {
                        textBoxOutput->AppendText(gcnew String("Invalid submatrix range.\n"));
                        continue;
                    }

                    std::vector<std::vector<int>> submatrix;
                    for (int i = r1; i <= r2; ++i) {
                        std::vector<int> row;
                        for (int j = c1; j <= c2; ++j)
                            row.push_back(mat[i][j]);
                        submatrix.push_back(row);
                    }

                    std::string subName = matrixName + "_sub";
                    matrixVars[subName] = submatrix;
                    textBoxOutput->AppendText(gcnew String(("Submatrix stored as '" + subName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(submatrix) + "\n");
                }
                else if (line.find("concatenate_matrices_horizontally(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    std::string A = args.substr(0, comma);
                    std::string B = args.substr(comma + 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                    B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                    if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String("Error: One or both matrices not found.\n"));

                        continue;
                    }

                    auto matA = matrixVars[A];
                    auto matB = matrixVars[B];

                    if (matA.size() != matB.size()) {
                        textBoxOutput->AppendText(gcnew String("Matrices must have the same number of rows.\n"));
                        continue;
                    }

                    std::vector<std::vector<int>> result(matA.size());
                    for (int i = 0; i < matA.size(); ++i) {
                        result[i] = matA[i];
                        result[i].insert(result[i].end(), matB[i].begin(), matB[i].end());
                    }

                    std::string name = A + "_hcat_" + B;
                    matrixVars[name] = result;
                    textBoxOutput->AppendText(gcnew String(("Horizontally concatenated matrix stored as '" + name + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }
                else if (line.find("concatenate_matrices_vertically(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    std::string A = args.substr(0, comma);
                    std::string B = args.substr(comma + 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                    B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                    if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String("Error: One or both matrices not found.\n"));

                        continue;
                    }

                    auto matA = matrixVars[A];
                    auto matB = matrixVars[B];

                    if (matA[0].size() != matB[0].size()) {
                        textBoxOutput->AppendText(gcnew String("Matrices must have the same number of columns.\n"));
                        continue;
                    }

                    std::vector<std::vector<int>> result = matA;
                    result.insert(result.end(), matB.begin(), matB.end());

                    std::string name = A + "_vcat_" + B;
                    matrixVars[name] = result;
                    textBoxOutput->AppendText(gcnew String(("Vertically concatenated matrix stored as '" + name + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                }

                else if (line.find("dot_product(") != std::string::npos) {
                    size_t open = line.find("(");
                    size_t close = line.find(")");
                    std::string args = line.substr(open + 1, close - open - 1);
                    size_t comma = args.find(",");
                    if (comma == std::string::npos) {
                        textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": dot_product requires two vectors.\n").c_str()));
                        continue;
                    }
                    std::string A = args.substr(0, comma);
                    std::string B = args.substr(comma + 1);
                    A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                    B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                    if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: One or both vectors not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> vecA = matrixVars[A];
                    std::vector<std::vector<int>> vecB = matrixVars[B];

                    if ((vecA.size() != 1 && vecA[0].size() != 1) || (vecB.size() != 1 && vecB[0].size() != 1)) {
                        textBoxOutput->AppendText(gcnew String("Error: Both arguments must be vectors.\n"));
                        continue;
                    }

                    std::vector<int> flatA = (vecA.size() == 1) ? vecA[0] : std::vector<int>(vecA.size());
                    std::vector<int> flatB = (vecB.size() == 1) ? vecB[0] : std::vector<int>(vecB.size());
                    if (vecA[0].size() == 1) for (int i = 0; i < vecA.size(); ++i) flatA[i] = vecA[i][0];
                    if (vecB[0].size() == 1) for (int i = 0; i < vecB.size(); ++i) flatB[i] = vecB[i][0];

                    if (flatA.size() != flatB.size()) {
                        textBoxOutput->AppendText(gcnew String("Error: Vectors must be of same length.\n"));
                        continue;
                    }

                    int dot = 0;
                    for (int i = 0; i < flatA.size(); ++i)
                        dot += flatA[i] * flatB[i];

                    textBoxOutput->AppendText(gcnew String(("Dot product of " + A + " and " + B + ": " + std::to_string(dot) + "\n").c_str()));
                    }
                else if (line.find("cross_product(") != std::string::npos) {
                        size_t open = line.find("(");
                        size_t close = line.find(")");
                        std::string args = line.substr(open + 1, close - open - 1);
                        size_t comma = args.find(",");
                        if (comma == std::string::npos) {
                            textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": cross_product requires two vectors.\n").c_str()));
                            continue;
                        }
                        std::string A = args.substr(0, comma);
                        std::string B = args.substr(comma + 1);
                        A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                        B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                        if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                            textBoxOutput->AppendText(gcnew String(("Error: One or both vectors not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                            continue;
                        }

                        std::vector<std::vector<int>> vecA = matrixVars[A];
                        std::vector<std::vector<int>> vecB = matrixVars[B];

                        std::vector<int> flatA = (vecA.size() == 1) ? vecA[0] : std::vector<int>(vecA.size());
                        std::vector<int> flatB = (vecB.size() == 1) ? vecB[0] : std::vector<int>(vecB.size());
                        if (vecA[0].size() == 1) for (int i = 0; i < vecA.size(); ++i) flatA[i] = vecA[i][0];
                        if (vecB[0].size() == 1) for (int i = 0; i < vecB.size(); ++i) flatB[i] = vecB[i][0];

                        if (flatA.size() != 3 || flatB.size() != 3) {
                            textBoxOutput->AppendText(gcnew String("Error: Cross product is only defined for 3D vectors.\n"));
                            continue;
                        }

                        std::vector<int> result(3);
                        result[0] = flatA[1] * flatB[2] - flatA[2] * flatB[1];
                        result[1] = flatA[2] * flatB[0] - flatA[0] * flatB[2];
                        result[2] = flatA[0] * flatB[1] - flatA[1] * flatB[0];

                        std::vector<std::vector<int>> cross = { result };
                        std::string resName = A + "_cross_" + B;
                        matrixVars[resName] = cross;

                        textBoxOutput->AppendText(gcnew String(("Cross product vector stored as '" + resName + "'.\n").c_str()));
                        textBoxOutput->AppendText(DisplayMatrix(cross) + "\n");
                        }
                else if (line.find("outer_product(") != std::string::npos) {
                            size_t open = line.find("(");
                            size_t close = line.find(")");
                            std::string args = line.substr(open + 1, close - open - 1);
                            size_t comma = args.find(",");
                            if (comma == std::string::npos) {
                                textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": outer_product requires two vectors.\n").c_str()));
                                continue;
                            }
                            std::string A = args.substr(0, comma);
                            std::string B = args.substr(comma + 1);
                            A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                            B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                            if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                                textBoxOutput->AppendText(gcnew String(("Error: One or both vectors not found. Line " + std::to_string(lineNumber) + "\n").c_str()));
                                continue;
                            }

                            std::vector<std::vector<int>> vecA = matrixVars[A];
                            std::vector<std::vector<int>> vecB = matrixVars[B];

                            std::vector<int> flatA = (vecA.size() == 1) ? vecA[0] : std::vector<int>(vecA.size());
                            std::vector<int> flatB = (vecB.size() == 1) ? vecB[0] : std::vector<int>(vecB.size());
                            if (vecA[0].size() == 1) for (int i = 0; i < vecA.size(); ++i) flatA[i] = vecA[i][0];
                            if (vecB[0].size() == 1) for (int i = 0; i < vecB.size(); ++i) flatB[i] = vecB[i][0];

                            std::vector<std::vector<int>> result(flatA.size(), std::vector<int>(flatB.size()));

                            for (int i = 0; i < flatA.size(); ++i)
                                for (int j = 0; j < flatB.size(); ++j)
                                    result[i][j] = flatA[i] * flatB[j];

                            std::string resName = A + "_outer_" + B;
                            matrixVars[resName] = result;

                            textBoxOutput->AppendText(gcnew String(("Outer product matrix stored as '" + resName + "'.\n").c_str()));
                            textBoxOutput->AppendText(DisplayMatrix(result) + "\n");
                            }
                else if (line.find("reshape_matrix(") != std::string::npos) {
                                size_t open = line.find("(");
                                size_t close = line.find(")");
                                std::string args = line.substr(open + 1, close - open - 1);
                                std::stringstream ss(args);
                                std::string var;
                                int new_rows, new_cols;
                                getline(ss, var, ',');
                                ss >> new_rows;
                                ss.ignore();
                                ss >> new_cols;

                                var.erase(remove_if(var.begin(), var.end(), ::isspace), var.end());

                                if (matrixVars.find(var) == matrixVars.end()) {
                                    textBoxOutput->AppendText(gcnew String(("Error: Matrix " + var + " not found.\n").c_str()));
                                    continue;
                                }

                                std::vector<std::vector<int>> mat = matrixVars[var];
                                int total_elements = mat.size() * mat[0].size();
                                if (new_rows * new_cols != total_elements) {
                                    textBoxOutput->AppendText(gcnew String("Error: Total elements do not match for reshape.\n"));
                                    continue;
                                }

                                std::vector<int> flat;
                                for (auto& row : mat)
                                    flat.insert(flat.end(), row.begin(), row.end());

                                std::vector<std::vector<int>> reshaped(new_rows, std::vector<int>(new_cols));
                                for (int i = 0; i < new_rows * new_cols; ++i)
                                    reshaped[i / new_cols][i % new_cols] = flat[i];

                                std::string newName = var + "_reshaped";
                                matrixVars[newName] = reshaped;
                                textBoxOutput->AppendText(gcnew String(("Matrix reshaped and stored as '" + newName + "'.\n").c_str()));
                                textBoxOutput->AppendText(DisplayMatrix(reshaped) + "\n");
                                }
                else if (line.find("flatten_matrix(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + arg + " not found.\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    std::vector<std::vector<int>> flat(1);

                    for (auto& row : mat)
                        flat[0].insert(flat[0].end(), row.begin(), row.end());

                    std::string newName = arg + "_flattened";
                    matrixVars[newName] = flat;
                    textBoxOutput->AppendText(gcnew String(("Matrix flattened and stored as '" + newName + "'.\n").c_str()));
                    textBoxOutput->AppendText(DisplayMatrix(flat) + "\n");
                    }
                else if (line.find("is_equal(") != std::string::npos) {
                        size_t open = line.find("(");
                        size_t close = line.find(")");
                        std::string args = line.substr(open + 1, close - open - 1);
                        std::stringstream ss(args);
                        std::string A, B;
                        float tolerance;
                        getline(ss, A, ',');
                        getline(ss, B, ',');
                        ss >> tolerance;

                        A.erase(remove_if(A.begin(), A.end(), ::isspace), A.end());
                        B.erase(remove_if(B.begin(), B.end(), ::isspace), B.end());

                        if (matrixVars.find(A) == matrixVars.end() || matrixVars.find(B) == matrixVars.end()) {
                            textBoxOutput->AppendText(gcnew String("Error: One or both matrices not found.\n"));

                            continue;
                        }

                        std::vector<std::vector<int>> matA = matrixVars[A];
                        std::vector<std::vector<int>> matB = matrixVars[B];
                        if (matA.size() != matB.size() || matA[0].size() != matB[0].size()) {
                            textBoxOutput->AppendText(gcnew String("Matrices have different dimensions.\n"));
                            continue;
                        }

                        bool equal = true;
                        for (int i = 0; i < matA.size(); ++i)
                            for (int j = 0; j < matA[0].size(); ++j)
                                if (abs(matA[i][j] - matB[i][j]) > tolerance)
                                    equal = false;

                        textBoxOutput->AppendText(equal ? gcnew String("Matrices are equal within tolerance.\n") : gcnew String("Matrices are not equal.\n"));

                        }

                else if (line.find("matrix_histogram(") != std::string::npos) {
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);
                    arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

                    if (matrixVars.find(arg) == matrixVars.end()) {
                        textBoxOutput->AppendText(gcnew String(("Error: Matrix " + arg + " not found.\n").c_str()));
                        continue;
                    }

                    std::vector<std::vector<int>> mat = matrixVars[arg];
                    std::map<int, int> histogram;

                    for (auto& row : mat)
                        for (int val : row)
                            histogram[val]++;

                    std::string hist = "Histogram of matrix " + arg + ":\n";
                    for (auto& pair : histogram)
                        hist += std::to_string(pair.first) + ": " + std::to_string(pair.second) + "\n";

                    textBoxOutput->AppendText(gcnew String(hist.c_str()));
                    }






                else {
                    textBoxOutput->AppendText(gcnew String(("Syntax Error on line " + std::to_string(lineNumber) + ": Unknown command.\n").c_str()));
                    continue;
                }
            }
        }
    };
}

[STAThread]
void main() {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MatrixCompilerUI::MyForm());
}


