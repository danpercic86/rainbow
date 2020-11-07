#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"

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
    REALLOC(buffer, char, *length + 1)
    buffer[*length] = *value;
    (*length)++;
    return buffer;
}

Atom* getNextAtom() {
    int state = 0, bufferLength = 0;
    char* buffer;
    char ch = *pch;
    Atom* atom = NULL;
    ALLOC(buffer, char)

    while (true) {
        printf("# State: %d | Char: '%c' (%d)\n", state, ch, ch);

        switch (state) {
        case 0:
            if (ch == '#') {
                state = 1;
                break;
            }

            if (isalpha(ch) || ch == '_') {
                state = 2;
                break;
            }

            if (ch == '\"') {
                state = 3;
                ch = *++pch;
                break;
            }

            if (isdigit(ch)) {
                state = 5;
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            if (ch == ':')
                state = 7;
            else if (ch == ';')
                state = 8;
            else if (ch == '(')
                state = 9;
            else if (ch == ')')
                state = 10;
            else if (ch == ',')
                state = 11;
            else if (ch == '|')
                state = 12;
            else if (ch == '&')
                state = 13;
            else if (ch == '=')
                state = 14;
            else if (ch == '!')
                state = 16;
            else if (ch == '<')
                state = 18;
            else if (ch == '+')
                state = 19;
            else if (ch == '-')
                state = 20;
            else if (ch == '*')
                state = 21;
            else if (ch == '/')
                state = 22;
            else if (ch == STRING_TERMINATOR)
                state = 23;
            else if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
                if (ch == '\n')
                    line++;
                ch = *++pch;
            }
            else err(-1, "Unknown character: %c", ch);

            break;
        case 1:
            // case COMMENT: read next characters until new line encountered
            if (ch == '\n') {
                line++;
                state = 0;
            }

            ch = *++pch;
            break;
        case 2:
            // case ID
            if (isalpha(ch) || ch == '_' || isdigit(ch)) {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            pch--;
            state = ADD_ATOM_STATE;
            break;
        case 3:
            // case STRING
            if (ch != '\"') {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            state = 4;
            break;
        case 4:
            buffer[bufferLength] = STRING_TERMINATOR;
            atom = addAtom(STR);
            ALLOC_LEN(atom->string, char, bufferLength)
            strcpy(atom->string, buffer);
            return atom;
        case 5:
            if (isdigit(ch)) {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            if (ch == '.') {
                state = 6;
                break;
            }

            pch--;
            atom = addAtom(INT);
            buffer[bufferLength] = STRING_TERMINATOR;
            atom->integer = strtol(buffer, NULL, 10);
            return atom;
        case 6:
            if (ch == '.') {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                if (ch == '.') {
                    err(-1, "Too many dots!");
                }
            }

            if (isdigit(ch)) {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            pch--;
            buffer[bufferLength] = STRING_TERMINATOR;
            atom = addAtom(REAL);
            atom->real = strtod(buffer, NULL);
            return atom;
        case 7:
            return addAtom(COLON);
        case 8:
            return addAtom(SEMICOLON);
        case 9:
            return addAtom(LPAR);
        case 10:
            return addAtom(RPAR);
        case 11:
            return addAtom(COMMA);
        case 12:
            ch = *++pch;
            if (ch == '|') {
                return addAtom(OR);
            }

            err(-1, "Unknown character after '|'!");
        case 13:
            ch = *++pch;
            if (ch == '&') {
                return addAtom(AND);
            }

            err(-1, "Unknown character after '&'!");
        case 14:
            ch = *++pch;

            if (ch == '=') {
                state = 15;
                break;
            }

            pch--;
            return addAtom(ASSIGN);
        case 15:
            return addAtom(EQUAL);
        case 16:
            ch = *++pch;

            if (ch == '=') {
                state = 17;
                break;
            }

            pch--;
            return addAtom(NOT);
        case 17:
            return addAtom(NOTEQUAL);
        case 18:
            return addAtom(LESS);
        case 19:
            return addAtom(ADD);
        case 20:
            return addAtom(SUB);
        case 21:
            return addAtom(MUL);
        case 22:
            return addAtom(DIV);
        case 23:
            return addAtom(FINISH);
        case ADD_ATOM_STATE:
            buffer[bufferLength] = STRING_TERMINATOR;
            if (strcmp(buffer, "var") == 0) {
                return addAtom(VAR);
            }
            if (strcmp(buffer, "function") == 0) {
                return addAtom(FUNCTION);
            }
            if (strcmp(buffer, "if") == 0) {
                return addAtom(IF);
            }
            if (strcmp(buffer, "else") == 0) {
                return addAtom(ELSE);
            }
            if (strcmp(buffer, "while") == 0) {
                return addAtom(WHILE);
            }
            if (strcmp(buffer, "end") == 0) {
                return addAtom(END);
            }
            if (strcmp(buffer, "return") == 0) {
                return addAtom(RETURN);
            }
            if (strcmp(buffer, "int") == 0) {
                return addAtom(TYPE_INT);
            }
            if (strcmp(buffer, "real") == 0) {
                return addAtom(TYPE_REAL);
            }
            if (strcmp(buffer, "str") == 0) {
                return addAtom(TYPE_STR);
            }

            atom = addAtom(ID);
            ALLOC_LEN(atom->string, char, bufferLength)
            strcpy(atom->string, buffer);
            return atom;
        default:
            err(0, "Unknown state!");
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

    printf("\n");
    int currentLine = 1;
    for (int i = 0; i < atomsNumber; i++) {
        if (currentLine != atoms[i]->line) {
            printf("\n");
            currentLine = atoms[i]->line;
        }

        switch (atoms[i]->type) {
        case ID:
            printf("ID:%s ", atoms[i]->string);
            break;
        case STR:
            printf("STR:%s ", atoms[i]->string);
            break;
        case REAL:
            printf("REAL:%f ", atoms[i]->real);
            break;
        case INT:
            printf("INT:%ld ", atoms[i]->integer);
            break;
        default:
            printf("%s ", TYPES[atoms[i]->type - TYPE_OFFSET]);
        }
    }

    free(fileContent);
    printf("\n\nDone!");
}