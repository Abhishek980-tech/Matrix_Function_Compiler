
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
            // Handle tutorial dropdown selection changes
        }

        void textBoxCode_TextChanged(System::Object^ sender, System::EventArgs^ e) {
            HighlightSyntax();
        }

        void HighlightSyntax() {
            // Syntax highlighting logic
        }

        void textBoxCode_KeyUp(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
            // Handle key up events for code suggestions
        }

        void textBoxCode_KeyDown(System::Object^ sender, KeyEventArgs^ e) {
            // Handle key down events for code suggestions
        }

        void Suggestion_Click(System::Object^ sender, EventArgs^ e) {
            InsertSuggestion();
        }

        void InsertSuggestion() {
            // Insert selected suggestion into the code editor
        }

        void buttonRun_Click(System::Object^ sender, System::EventArgs^ e) {
            // Handle the RUN button click event
        }
    };
}

[STAThread]
void main() {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MatrixCompilerUI::MyForm());
}
