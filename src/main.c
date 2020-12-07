#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

int atomsNumber = 0;
char* fileContent;
char* pch;
int line = 1;

Atom* addAtom(const enum Type type)
{
    Atom* currentAtom;
    ALLOC(currentAtom, Atom)
    currentAtom->type = type;
    currentAtom->line = line;
    REALLOC(atoms, Atom, (atomsNumber + 1))
    atoms[atomsNumber++] = currentAtom;
    return currentAtom;
}

char* addCharToBuffer(char* buffer, int* length, const char* value)
{
    REALLOC(buffer, char, *length + 1)
    buffer[*length] = *value;
    (*length)++;
    return buffer;
}

Atom* getNextAtom()
{
    int state = 0, bufferLength = 0;
    char* buffer;
    char ch = *pch;
    Atom* currentAtom = NULL;
    ALLOC(buffer, char)

    for ever
    {
        printf("# State: %d | Char: '%c' (%d)\n", state, ch, ch);

        switch (state)
        {
        case 0:
            if (ch == '#')
            {
                state = 1;
                break;
            }

            if (isalpha(ch) || ch == '_')
            {
                state = 2;
                break;
            }

            if (ch == '\"')
            {
                state = 4;
                ch = *++pch;
                break;
            }

            if (isdigit(ch))
            {
                state = 6;
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            if (ch == ':')
            {
                state = 11;
            }
            else if (ch == ';')
            {
                state = 12;
            }
            else if (ch == '(')
            {
                state = 13;
            }
            else if (ch == ')')
            {
                state = 14;
            }
            else if (ch == ',')
            {
                state = 15;
            }
            else if (ch == '|')
            {
                state = 16;
            }
            else if (ch == '&')
            {
                state = 18;
            }
            else if (ch == '=')
            {
                state = 20;
            }
            else if (ch == '!')
            {
                state = 23;
            }
            else if (ch == '<')
            {
                state = 26;
            }
            else if (ch == '+')
            {
                state = 27;
            }
            else if (ch == '-')
            {
                state = 28;
            }
            else if (ch == '*')
            {
                state = 29;
            }
            else if (ch == '/')
            {
                state = 30;
            }
            else if (ch == STRING_TERMINATOR)
            {
                state = 31;
            }
            else if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
            {
                if (ch == '\n')
                {
                    line++;
                }

                ch = *++pch;
            }
            else
            {
                err(-1, "Unknown character: %c", ch);
            }

            break;
        case 1:
            // case COMMENT: read next characters until new line encountered
            if (ch == '\n' || ch == '\r' || ch == '\0')
            {
                state = 0;

                if (ch == '\n')
                {
                    line++;
                }
            }

            ch = *++pch;
            break;
        case 2:
            // case ID
            if (isalpha(ch) || ch == '_' || isdigit(ch))
            {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            pch--;
            state = 3;
            break;
        case 3:
            // case ADD ID
            buffer[bufferLength] = STRING_TERMINATOR;
            if (strcmp(buffer, "var") == 0)
            {
                return addAtom(VAR);
            }
            if (strcmp(buffer, "function") == 0)
            {
                return addAtom(FUNCTION);
            }
            if (strcmp(buffer, "if") == 0)
            {
                return addAtom(IF);
            }
            if (strcmp(buffer, "else") == 0)
            {
                return addAtom(ELSE);
            }
            if (strcmp(buffer, "while") == 0)
            {
                return addAtom(WHILE);
            }
            if (strcmp(buffer, "end") == 0)
            {
                return addAtom(END);
            }
            if (strcmp(buffer, "return") == 0)
            {
                return addAtom(RETURN);
            }
            if (strcmp(buffer, "int") == 0)
            {
                return addAtom(TYPE_INT);
            }
            if (strcmp(buffer, "real") == 0)
            {
                return addAtom(TYPE_REAL);
            }
            if (strcmp(buffer, "str") == 0)
            {
                return addAtom(TYPE_STR);
            }

            currentAtom = addAtom(ID);
            ALLOC_LEN(currentAtom->string, char, bufferLength)
            strcpy(currentAtom->string, buffer);
            return currentAtom;
        case 4:
            // case STRING
            if (ch != '\"')
            {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            state = 5;
            break;
        case 5:
            buffer[bufferLength] = STRING_TERMINATOR;
            currentAtom = addAtom(STR);
            ALLOC_LEN(currentAtom->string, char, bufferLength)
            strcpy(currentAtom->string, buffer);
            return currentAtom;
        case 6:
            if (isdigit(ch))
            {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            if (ch == '.')
            {
                state = 7;
                break;
            }

            pch--;
            currentAtom = addAtom(INT);
            buffer[bufferLength] = STRING_TERMINATOR;
            currentAtom->integer = strtol(buffer, NULL, 10);
            return currentAtom;
        case 7:
            if (ch == '.')
            {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                if (ch == '.')
                {
                    err(-1, "Too many dots!");
                }
            }

            if (isdigit(ch))
            {
                state = 8;
                break;
            }

            err(-1, "Error on real type declaration");
        case 8:
            if (isdigit(ch))
            {
                buffer = addCharToBuffer(buffer, &bufferLength, &ch);
                ch = *++pch;
                break;
            }

            pch--;
            state = 9;
            break;
        case 9:
            buffer[bufferLength] = STRING_TERMINATOR;
            currentAtom = addAtom(REAL);
            currentAtom->real = strtod(buffer, NULL);
            return currentAtom;
        case 11:
            return addAtom(COLON);
        case 12:
            return addAtom(SEMICOLON);
        case 13:
            return addAtom(LPAR);
        case 14:
            return addAtom(RPAR);
        case 15:
            return addAtom(COMMA);
        case 16:
            ch = *++pch;
            if (ch == '|')
            {
                state = 17;
                break;
            }

            err(-1, "Unknown character after '|'!");
        case 17:
            return addAtom(OR);
        case 18:
            ch = *++pch;
            if (ch == '&')
            {
                state = 19;
                break;
            }

            err(-1, "Unknown character after '&'!");
        case 19:
            return addAtom(AND);
        case 20:
            ch = *++pch;

            if (ch == '=')
            {
                state = 21;
                break;
            }

            pch--;
            state = 22;
            break;
        case 21:
            return addAtom(EQUAL);
        case 22:
            return addAtom(ASSIGN);
        case 23:
            ch = *++pch;

            if (ch == '=')
            {
                state = 24;
                break;
            }

            pch--;
            state = 25;
            break;
        case 24:
            return addAtom(NOTEQUAL);
        case 25:
            return addAtom(NOT);
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
            err(0, "Unknown state!");
            break;
        }
    }
}

int main()
{
    FILE* file;
    file = fopen("../main.q", "r");

    if (file == NULL)
    {
        err(-1, "Fişierul de intrare nu poate fi deschis!");
    }

    fseek(file, 0L, SEEK_END);
    const long fileSize = ftell(file);
    rewind(file);

    fileContent = calloc(sizeof(char), fileSize + 1);
    if (not fileContent)
    {
        free(fileContent);
        fclose(file);
        err(-1, "Memory allocation fail!");
    }

    const uint CHUNKS_TO_READ = 1;
    if (CHUNKS_TO_READ != fread(fileContent, fileSize, CHUNKS_TO_READ, file))
    {
        free(fileContent);
        fclose(file);
        err(-1, "File read fails!");
    }

    fclose(file);
    fileContent[fileSize] = STRING_TERMINATOR;
    pch = fileContent;

    while (getNextAtom()->type != FINISH)
    {
        ++pch;
    }

    printf("\n");
    int currentLine = 1;
    printf("%d ", currentLine);

    for (int i = 0; i < atomsNumber; i++)
    {
        if (currentLine != atoms[i]->line)
        {
            currentLine = atoms[i]->line;
            printf("\n");
            printf("%d ", currentLine);
        }

        switch (atoms[i]->type)
        {
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
    printf("\n");

    program();

    printf("\n\nDone!");
}