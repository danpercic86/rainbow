//
// Created by Dan Percic on 01.11.2020.
//

#ifndef RAINBOW_MAIN_H
#define RAINBOW_MAIN_H

#include <stdbool.h>

#define STRING_TERMINATOR '\0'
#define ALLOC(var, Type) if((var = malloc(sizeof(Type))) == NULL) err(-1, "Not enough memory");
#define ALLOC_LEN(var, Type, length) if((var = malloc(sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define REALLOC(ptr, Type, length) if((ptr = realloc(ptr, sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define TYPE_OFFSET 99
#define ever (;;)
#define or ||
#define and &&
#define not !

enum Type
{
    ID = TYPE_OFFSET,
    TYPE_INT,
    TYPE_REAL,
    TYPE_STR,
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

const char* TYPES[32] = {
        "ID", "TYPE_INT", "TYPE_REAL", "TYPE_STR", "INT", "REAL", "STR", "VAR", "IF", "ELSE", "WHILE",
        "END", "RETURN", "FUNCTION", "COLON", "SEMICOLON", "LPAR", "RPAR", "COMMA", "OR", "AND", "NOT",
        "NOTEQUAL", "EQUAL", "ASSIGN", "LESS", "ADD", "SUB", "MUL", "DIV", "FINISH"
};

typedef struct
{
    enum Type type;
    int line;
    union
    {
        char* string;
        double real;
        long integer;
    };
} Atom;

Atom* pa;

bool exprAdunare();

bool instr();

bool error(const char* msg)
{
    err(-1, "%s", msg);
}

bool consume(int type)
{
    if (pa->type == type)
    {
        pa++;
        return true;
    }

    return false;
}

bool exprComp()
{
    if (not exprAdunare())
    {
        return false;
    }

    if (consume(LESS))
    {
        if (exprAdunare())
        {
            return true;
        }

        error("lipsa exprAdunare dupa LESS");
    }

    if (consume(EQUAL))
    {
        if (exprAdunare())
        {
            return true;
        }

        error("lipsa exprAdunare dupa EQUAL");
    }

    return true;
}

bool baseType()
{
    return consume(TYPE_INT) or consume(TYPE_REAL) or consume(TYPE_STR);
}

bool exprAtribuire()
{
    if (not consume(ID))
    {
        return exprComp();
    }

    if (not consume(ASSIGN))
    {
        pa--;
        return exprComp();
    }

    if (not exprComp())
    {
        error("expression is missing after ASSIGN operator!");
    }

    return true;
}

bool block()
{
    if (not instr())
    {
        return false;
    }

    for ever
    {
        if (not instr())
        { return true; }
    }
}

bool exprLogica()
{
    if (not exprAtribuire())
    {
        return false;
    }

    for ever
    {
        if (consume(AND))
        {
            if (not exprAtribuire())
            {
                error("lipseste exprAtribuire dupa AND");
            }

            continue;
        }

        if (consume(OR))
        {
            if (not exprAtribuire())
            {
                error("lipseste exprAtribuire dupa OR");
            }

            continue;
        }

        return true;
    }
}

bool funcArgs()
{
    if (not consume(ID))
    {
        return true;
    }

    if (not consume(COLON))
    {
        error(": is missing from function arguments!");
    }

    if (not baseType())
    {
        error("function arguments have no base type!");
    }

    for ever
    {
        if (not consume(COMMA))
        {
            return true;
        }

        if (not funcArgs())
        {
            error("There's an unexpected comma in the function arguments!");
        }
    }
}

bool factor()
{
    if (consume(INT) or consume(REAL) or consume(STR))
    {
        return true;
    }

    if (consume(LPAR))
    {
        if (not exprLogica())
        {
            error("expresie gresita intre paranteze");
        }

        if (not consume(RPAR))
        {
            error("lipsa paranteza dreapta");
        }

        return true;
    }

    if (not consume(ID))
    {
        return false;
    }

    if (not consume(LPAR))
    {
        return true;
    }

    if (exprLogica())
    {
        for ever
        {
            if (consume(COMMA))
            {
                break;
            }

            if (not exprLogica())
            {
                error("lipsa expresie dupa virgula");
            }
        }
    }

    return consume(RPAR) ? true : error("lipsa paranteza dreapta");
}

bool exprPrefix()
{
    if (consume(SUB))
    {
        return factor() ? true : error("lipsa factor dupa SUB");
    }

    if (consume(NOT))
    {
        return factor() ? true : error("lipsa factor dupa NOT");
    }

    return factor();
}

bool exprInmultire()
{
    if (not exprPrefix())
    {
        return false;
    }

    for ever
    {
        if (consume(MUL))
        {
            if (not exprPrefix())
            {
                error("lipsa exprPrefix dupa MUL");
            }

            continue;
        }

        if (consume(DIV))
        {
            if (not exprPrefix())
            {
                error("lipsa exprPrefix dupa DIV");
            }

            continue;
        }

        return true;
    }
}

bool exprAdunare()
{
    if (not exprInmultire())
    {
        return false;
    }

    for ever
    {
        if (consume(ADD))
        {
            if (not exprInmultire())
            {
                error("lipsa exprInmultire dupa ADD");
            }

            continue;
        }

        if (consume(SUB))
        {
            if (not exprInmultire())
            {
                error("lipsa exprInmultire dupa SUB");
            }

            continue;
        }

        return true;
    }
}

bool instr()
{
    if (exprLogica())
    {
        return consume(SEMICOLON) ? true : error("expresie neterminata de ;");
    }

    if (consume(IF))
    {
        if (not consume(LPAR))
        {
            error("lipsa paranteza stanga");
        }

        if (not exprLogica())
        {
            error("lipsa expresie in if");
        }

        if (not consume(RPAR))
        {
            error("lipsa paranteza dreapta");
        }

        if (not block())
        {
            error("lipsa block din if");
        }

        if (not consume(ELSE))
        {
            return consume(END) ? true : error("lipseste END la finalul if");
        }

        if (not block())
        {
            error("lipsa block la ramura ELSE");
        }

        return consume(END) ? true : error("lipseste END dupa ramura ELSE");
    }

    if (consume(RETURN))
    {
        if (not exprLogica())
        {
            error("lipsa expresie");
        }

        return consume(SEMICOLON) ? true : error("lipsa ;");
    }

    if (not consume(WHILE))
    {
        return false;
    }

    if (not consume(LPAR))
    {
        error("lipsa paranteza stanga la while");
    }

    if (not exprLogica())
    {
        error("lipsa expresie in while");
    }

    if (not consume(RPAR))
    {
        error("lipsa paranteza dreapta la while");
    }

    if (not block())
    {
        error("lipsa block din interiorul while");
    }

    return consume(END) ? true : error("lipsa END la finalul WHILE");
}

bool declVar()
{
    if (not consume(VAR))
    {
        error("var is missing!");
    }

    if (not consume(ID))
    {
        error("ID is missing!");
    }

    if (not consume(COLON))
    {
        error(": is missing!");
    }

    if (not baseType())
    {
        error("base type is missing!");
    }

    return consume(SEMICOLON) ? true : error("; is missing!");
}

bool declFunc()
{
    if (not consume(FUNCTION))
    {
        return false;
    }

    if (not consume(ID))
    {
        error("ID is missing!");
    }

    if (not consume(LPAR))
    {
        error("( is missing!");
    }

    if (not funcArgs())
    {
        error("function arguments are missing!");
    }

    if (not consume(RPAR))
    {
        error(") is missing!");
    }

    if (not consume(COLON))
    {
        error(": is missing!");
    }

    if (not baseType())
    {
        error("the base type is missing!");
    }

    for ever
    {
        if (not declVar())
        {
            break;
        }
    }

    if (not block())
    {
        error("block statement is missing!");
    }

    return consume(END) ? true : error("END is missing!");
}

bool program()
{
    for ever
    {
        if (declVar() or declFunc() or block())
        {
            continue;
        }

        break;
    }

    return consume(FINISH) ? printf("Syntactic analysis successfully done!") : error("FINISH is missing!");
}

#endif //RAINBOW_MAIN_H
