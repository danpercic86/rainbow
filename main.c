#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STRING_TERMINATOR '\0'
#define ALLOC(var, Type) if((var = malloc(sizeof(Type))) == NULL) err(-1, "Not enough memory");
#define ALLOC_LEN(var, Type, length) if((var = malloc(sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define REALLOC(ptr, Type, length) if((ptr = realloc(ptr, sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");

enum Type {
    ID,
    INT,
    REAL,
    STR,
    VAR,
    IF,
    ELSE,
    WHILE,
    END,
    RETURN,
    FUNCTION,
    COLON,
    SEMICOLON,
    LPAR,
    RPAR,
    COMMA,
    OR,
    AND,
    NOT,
    NOTEQUAL,
    EQUAL,
    ASSIGN,
    LESS,
    ADD,
    SUB,
    MUL,
    DIV,
    FINISH
};

typedef struct {
    enum Type type;
    int line;
    union {
        char* string;
        double real;
        long integer;
    };
} Atom;

Atom** atoms;
int atomsNumber = 0;

char* fileContent;
char* pch;
int line = 1;

Atom* addAtom(const enum Type type) {
    Atom* atom;
    ALLOC(atom, Atom)
    atom->type = type;
    atom->line = line;
    REALLOC(atoms, Atom, (atomsNumber + 1))
    atoms[atomsNumber++] = atom;
    return atom;
}

char* addCharToBuffer(char* buffer, int* length, const char* value) {
    (*length)++;
    REALLOC(buffer, char, *length)
    buffer[*length] = *value;
    return buffer;
}

Atom* getNextAtom() {
    int state = 0, bufferLength = 0;
    char* buffer;
    char ch = *pch;
    Atom* atom = NULL;
    ALLOC(buffer, char)

    while (true) {
        printf("#%d %c(%d)\n", state, ch, ch);

        switch (state) {
        case 0:
            if (isalpha(ch) || ch == '_') {
                state = 1;
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else if (isdigit(ch) || ch == '.') {
                state = 3;
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else if (ch == '\"')
                state = 8;
            else if (ch == ':')
                state = 10;
            else if (ch == ';')
                state = 11;
            else if (ch == '(')
                state = 12;
            else if (ch == ')')
                state = 13;
            else if (ch == ',')
                state = 14;
            else if (ch == '|')
                state = 15;
            else if (ch == '&')
                state = 17;
            else if (ch == '!')
                state = 19;
            else if (ch == '=')
                state = 23;
            else if (ch == '<')
                state = 26;
            else if (ch == '+')
                state = 27;
            else if (ch == '-')
                state = 28;
            else if (ch == '*')
                state = 29;
            else if (ch == '/')
                state = 30;
            else if (ch == STRING_TERMINATOR)
                state = 31;
            else if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
                if (ch == '\n')
                    line++;
                ch = *++pch;
            }
            else {
                err(-1, "Unknown character: %c", ch);
            }
            break;
        case 1:
            if (isalpha(ch) || ch == '_') {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else {
                pch--;
                state = 2;
            }
            break;
        case 2:
            buffer[bufferLength] = STRING_TERMINATOR;
            if (strcmp(buffer, "var") == 0) {
                return addAtom(VAR);
            }
            else if (strcmp(buffer, "function") == 0) {
                return addAtom(FUNCTION);
            }
            else if (strcmp(buffer, "if") == 0) {
                return addAtom(IF);
            }
            else if (strcmp(buffer, "else") == 0) {
                return addAtom(ELSE);
            }
            else if (strcmp(buffer, "while") == 0) {
                return addAtom(WHILE);
            }
            else if (strcmp(buffer, "end") == 0) {
                return addAtom(END);
            }
            else if (strcmp(buffer, "return") == 0) {
                return addAtom(RETURN);
            }
            else if (strcmp(buffer, "int") == 0) {
                return addAtom(INT);
            }
            else if (strcmp(buffer, "real") == 0) {
                return addAtom(REAL);
            }
            else if (strcmp(buffer, "str") == 0) {
                return addAtom(STR);
            }
            else {
                atom = addAtom(ID);
                ALLOC_LEN(atom->string, char, bufferLength)
                strcpy(atom->string, buffer);
                return atom;
            }
        case 3:
            if (isdigit(ch)) {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else if (ch == '.') {
                state = 5;
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else {
                pch--;
                state = 4;
            }
            break;
        case 4:
            atom = addAtom(INT);
            buffer[bufferLength] = STRING_TERMINATOR;
            atom->integer = strtol(buffer, NULL, 10);
            return atom;
        case 5:
            if (isdigit(ch)) {
                state = 6;
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else err(-1, "caracter necunoscut - lipsesc zecimale dintr-un numar real");
            break;
        case 6:
            if (isdigit(ch)) {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else {
                pch--;
                state = 7;
            }
            break;
        case 7:
            buffer[bufferLength] = STRING_TERMINATOR;
            atom = addAtom(REAL);
            atom->real = strtod(buffer, NULL);
            return atom;
        case 8:
            if (ch != '\"') {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
            }
            else {
                pch--;
                state = 9;
            }
            break;
        case 9:
            buffer[bufferLength] = STRING_TERMINATOR;
            atom = addAtom(STR);
            ALLOC_LEN(atom->string, char, bufferLength)
            strcpy(atom->string, buffer);
            return atom;
        case 10:
            return addAtom(COLON);
        case 11:
            return addAtom(SEMICOLON);
        case 12:
            return addAtom(LPAR);
        case 13:
            return addAtom(RPAR);
        case 14:
            return addAtom(COMMA);
        case 15:
            if (ch == '|') {
                pch--;
                state = 16;
            }
            else {
                err(-1, "caracter necunoscut - un singur |");
            }
            break;
        case 16:
            return addAtom(OR);
        case 17:
            if (ch == '&') {
                pch--;
                state = 18;
            }
            else {
                err(-1, "caracter necunoscut - un singur &");
            }
            break;
        case 18:
            return addAtom(AND);
        case 19:
            if (ch == '=')
                state = 21;
            else {
                state = 20;
            }
            pch--;
            break;
        case 20:
            return addAtom(NOT);
        case 21:
            return addAtom(NOTEQUAL);
        case 23:
            if (ch == '=')
                state = 25;
            else {
                state = 24;
            }
            pch--;
            break;
        case 24:
            return addAtom(ASSIGN);
        case 25:
            return addAtom(EQUAL);
        case 26:
            return addAtom(LESS);
        case 27:
            return addAtom(ADD);
        case 28:
            return addAtom(SUB);
        case 29:
            return addAtom(MUL);
        case 30:
            return addAtom(DIV);
        case 31:
            return addAtom(FINISH);
        default:
            err(0, "s-a ajuns intr-o stare neprevazuta");
            break;
        }
    }
}

int main() {
    FILE* file;
    file = fopen("../main.q", "r");

    if (file == NULL) {
        err(-1, "Fişierul de intrare nu poate fi deschis!");
    }

    fseek(file, 0L, SEEK_END);
    const long fileSize = ftell(file);
    rewind(file);

    fileContent = calloc(sizeof(char), fileSize + 1);
    if (!fileContent) {
        free(fileContent);
        fclose(file);
        err(-1, "Memory allocation fail!");
    }

    const uint CHUNKS_TO_READ = 1;
    if (CHUNKS_TO_READ != fread(fileContent, fileSize, CHUNKS_TO_READ, file)) {
        free(fileContent);
        fclose(file);
        err(-1, "File read fails!");
    }

    fclose(file);
    fileContent[fileSize] = STRING_TERMINATOR;
    pch = fileContent;

    while (getNextAtom()->type != FINISH) { ++pch; }

    for (int i = 0; i < atomsNumber; i++) {
        printf("%d ", atoms[i]->type);
    }

    free(fileContent);
    printf("Done!");
}