#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef enum { KT, IT, SCT, OT, DT } TokenType;


typedef struct {
    char key[50];
    TokenType value;
} Pair;

const char *keywords[] = {
    "calculate", "check", "track", "create", "adjust", "suggest", "generate",
    "recommend", "find", "search", "assign", "return", "for", "if", "else"
};
const int keywordCount = sizeof(keywords) / sizeof(keywords[0]);


const char *knownKeywords[] = {
    "int", "double", "float", "bool", "char", "void", "const", "struct"
};
const int knownKeywordCount = sizeof(knownKeywords) / sizeof(knownKeywords[0]);


const char delimiters[] = ";(),[]{}";


int isKeyword(const char *word) {
    for (int i = 0; i < keywordCount; i++) {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}


int isKnownKeyword(const char *word) {
    for (int i = 0; i < knownKeywordCount; i++) {
        if (strcmp(word, knownKeywords[i]) == 0)
            return 1;
    }
    return 0;
}

Pair* tokenizeFunction(const char *functionDeclaration, int *tokenCount) {
    Pair *tokens = malloc(200 * sizeof(Pair));
    char buffer[100];
    int index = 0, bufferIndex = 0;
    int inParameter = 0;

    while (functionDeclaration[index] != '\0') {
        char ch = functionDeclaration[index];

        if (strchr(delimiters, ch)) {
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';
                if (isKnownKeyword(buffer)) {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = KT;
                } else if (inParameter) {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = IT;
                } else if (isKeyword(buffer)) {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = KT;
                } else {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = IT;
                }
                (*tokenCount)++;
                bufferIndex = 0;
            }
            char delimiterStr[2] = {ch, '\0'};
            strcpy(tokens[*tokenCount].key, delimiterStr);
            tokens[*tokenCount].value = DT;
            (*tokenCount)++;
            inParameter = (ch == '(') ? 1 : (ch == ')') ? 0 : inParameter;
        } else if (isspace(ch)) {
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';
                if (isKnownKeyword(buffer)) {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = KT;
                } else if (inParameter) {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = IT;
                } else if (isKeyword(buffer)) {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = KT;
                } else {
                    strcpy(tokens[*tokenCount].key, buffer);
                    tokens[*tokenCount].value = IT;
                }
                (*tokenCount)++;
                bufferIndex = 0;
            }
        } else {
            buffer[bufferIndex++] = ch;
        }
        index++;
    }

    if (bufferIndex > 0) {
        buffer[bufferIndex] = '\0';
        if (isKnownKeyword(buffer)) {
            strcpy(tokens[*tokenCount].key, buffer);
            tokens[*tokenCount].value = KT;
        } else if (inParameter) {
            strcpy(tokens[*tokenCount].key, buffer);
            tokens[*tokenCount].value = IT;
        } else if (isKeyword(buffer)) {
            strcpy(tokens[*tokenCount].key, buffer);
            tokens[*tokenCount].value = KT;
        } else {
            strcpy(tokens[*tokenCount].key, buffer);
            tokens[*tokenCount].value = IT;
        }
        (*tokenCount)++;
    }

    return tokens;
}


void printTokens(Pair *tokens, int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        printf("Token: %s -> ", tokens[i].key);
        switch (tokens[i].value) {
            case KT: printf("KT\n"); break;
            case IT: printf("IT\n"); break;
            case SCT: printf("SCT\n"); break;
            case OT: printf("OT\n"); break;
            case DT: printf("DT\n"); break;
        }
    }
}


int main() {
    const char *functionDeclarations[] = {
        "double calculateDeterminant(double** matrix, int size);",
        "double** transposeMatrix(double** matrix, int rows, int cols);",
        "void printMatrix(double** matrix, int rows, int cols);",
        "double** matrixMultiplication(double** A, double** B, int rowsA, int colsA, int colsB);",
        "double traceMatrix(double** matrix, int size);",
        "double calculateNorm(double** matrix, int rows, int cols);",
        "double** inverseMatrix(double** matrix, int size);",
        "double determinantOfSubMatrix(double** matrix, int size, int excludeRow, int excludeCol);",
        "double** cofactorMatrix(double** matrix, int size);",
        "double** adjugateMatrix(double** matrix, int size);",
        "double calculateFrobeniusNorm(double** matrix, int rows, int cols);",
        "int isSymmetric(double** matrix, int size);",
        "int isSkewSymmetric(double** matrix, int size);",
        "int isOrthogonal(double** matrix, int size);",
        "double** scalarMultiply(double** matrix, int rows, int cols, double scalar);",
        "double** addMatrices(double** A, double** B, int rows, int cols);",
        "double** subtractMatrices(double** A, double** B, int rows, int cols);",
        "double maxElement(double** matrix, int rows, int cols);",
        "double minElement(double** matrix, int rows, int cols);",
        "double** rotateMatrix(double** matrix, int size);"
    };

    int functionCount = sizeof(functionDeclarations) / sizeof(functionDeclarations[0]);

    for (int i = 0; i < functionCount; i++) {
        printf("Function Declaration: %s\n", functionDeclarations[i]);
        int tokenCount = 0;
        Pair *tokens = tokenizeFunction(functionDeclarations[i], &tokenCount);
        printTokens(tokens, tokenCount);
        printf("-----------------------\n");
        free(tokens);
    }

    return 0;
}
