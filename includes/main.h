//
// Created by Dan Percic on 01.11.2020.
//

#ifndef RAINBOW_MAIN_H
#define RAINBOW_MAIN_H

#include <stdbool.h>
#include <errno.h>

#define STRING_TERMINATOR '\0'
#define ALLOC(var, Type) if((var = malloc(sizeof(Type))) == NULL) err(-1, "Not enough memory");
#define ALLOC_LEN(var, Type, length) if((var = malloc(sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define REALLOC(ptr, Type, length) if((ptr = realloc(ptr, sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define TYPE_OFFSET 99
#define ever (;;)
#define or ||
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

Atom** atoms;
Atom* atom;
int idx = 0;

bool addExpression();

bool instruction();

bool error(const char* msg)
{
    err(1, "\n%s\nLine: %d", msg, atom->line);
}

bool incrementIndex()
{
    printf("--- consumed: %s\n", TYPES[atom->type - TYPE_OFFSET]);
    idx++;
    return true;
}

bool consume(const enum Type type)
{
    atom = atoms[idx];
    return atom->type == type ? incrementIndex() : false;
}

bool complexExpression()
{
    if (not addExpression())
    {
        return false;
    }

    if (consume(LESS))
    {
        if (addExpression())
        {
            return true;
        }

        error("missing ADD expression after LESS");
    }

    if (consume(EQUAL))
    {
        if (addExpression())
        {
            return true;
        }

        error("missing ADD expression after EQUAL");
    }

    return true;
}

bool baseType()
{
    return consume(TYPE_INT) or consume(TYPE_REAL) or consume(TYPE_STR);
}

bool assignExpression()
{
    if (not consume(ID))
    {
        return complexExpression();
    }

    if (not consume(ASSIGN))
    {
        idx--;
        return complexExpression();
    }

    if (not complexExpression())
    {
        error("expression is missing after ASSIGN operator!");
    }

    return true;
}

bool block()
{
    if (not instruction())
    {
        return false;
    }

    for ever
    {
        if (not instruction())
        {
            return true;
        }
    }
}

bool logicExpression()
{
    if (not assignExpression())
    {
        return false;
    }

    for ever
    {
        if (consume(AND))
        {
            if (not assignExpression())
            {
                error("missing ASSIGN expression after AND");
            }

            continue;
        }

        if (consume(OR))
        {
            if (not assignExpression())
            {
                error("missing ASSIGN expression after OR");
            }

            continue;
        }

        return true;
    }
}

bool functionArguments()
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

        if (not functionArguments())
        {
            error("there's an unexpected comma in the function arguments!");
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
        if (not logicExpression())
        {
            error("wrong expression between parentheses");
        }

        if (not consume(RPAR))
        {
            error("missing right parenthesis in factor");
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

    if (logicExpression())
    {
        for ever
        {
            if (not consume(COMMA))
            {
                break;
            }

            if (not logicExpression())
            {
                error("missing expression after comma");
            }
        }
    }

    return consume(RPAR) ? true : error("missing right parenthesis at factor final");
}

bool prefixedExpression()
{
    if (consume(SUB))
    {
        return factor() ? true : error("missing factor after SUB");
    }

    if (consume(NOT))
    {
        return factor() ? true : error("missing factor after NOT");
    }

    return factor();
}

bool multiplyExpression()
{
    if (not prefixedExpression())
    {
        return false;
    }

    for ever
    {
        if (consume(MUL))
        {
            if (not prefixedExpression())
            {
                error("missing prefixed expression after MUL");
            }

            continue;
        }

        if (consume(DIV))
        {
            if (not prefixedExpression())
            {
                error("missing prefixed expression after DIV");
            }

            continue;
        }

        return true;
    }
}

bool addExpression()
{
    if (not multiplyExpression())
    {
        return false;
    }

    for ever
    {
        if (consume(ADD))
        {
            if (not multiplyExpression())
            {
                error("missing multiply expression after ADD");
            }

            continue;
        }

        if (consume(SUB))
        {
            if (not multiplyExpression())
            {
                error("missing multiply expression after SUB");
            }

            continue;
        }

        return true;
    }
}

bool instruction()
{
    if (logicExpression())
    {
        return consume(SEMICOLON) ? true : error("expression has no ; at the end");
    }

    if (consume(IF))
    {
        if (not consume(LPAR))
        {
            error("missing left parenthesis");
        }

        if (not logicExpression())
        {
            error("missing expression in if");
        }

        if (not consume(RPAR))
        {
            error("missing right parenthesis");
        }

        if (not block())
        {
            error("missing block in if");
        }

        if (not consume(ELSE))
        {
            return consume(END) ? true : error("missing END at the IF final");
        }

        if (not block())
        {
            error("missing block in ELSE branch");
        }

        return consume(END) ? true : error("missing END after ELSE branch");
    }

    if (consume(RETURN))
    {
        if (not logicExpression())
        {
            error("expression is missing");
        }

        return consume(SEMICOLON) ? true : error("; is missing");
    }

    if (not consume(WHILE))
    {
        return false;
    }

    if (not consume(LPAR))
    {
        error("missing left parenthesis in WHILE");
    }

    if (not logicExpression())
    {
        error("missing expression in WHILE");
    }

    if (not consume(RPAR))
    {
        error("missing right parenthesis in WHILE");
    }

    if (not block())
    {
        error("missing block in WHILE");
    }

    return consume(END) ? true : error("missing END at the end of WHILE");
}

bool variableDeclaration()
{
    if (not consume(VAR))
    {
        return false;
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

bool functionDeclaration()
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

    if (not functionArguments())
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

    for ever if (not variableDeclaration()) break;

    if (not block())
    {
        error("block statement is missing!");
    }

    return consume(END) ? true : error("END is missing!");
}

bool program()
{
    atom = atoms[idx];

    for ever
    {
        if (variableDeclaration() or functionDeclaration() or block())
        {
            continue;
        }

        break;
    }

    return consume(FINISH) ? printf("Syntactic analysis successfully done!") : error("FINISH is missing!");
}

#endif //RAINBOW_MAIN_H
