//
// Created by danpercic on 01.11.2020.
//

#ifndef RAINBOW_MAIN_H
#define RAINBOW_MAIN_H

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

const char* TYPES[28] = {"id", "int", "real", "str", "var", "if", "else", "while", "end", "return", "function", ":",
                         ";", "(", ")", ",", "||", "&&", "!", "!=", "==", "=", "<", "+", "-", "*", "/", "FINISH"};

typedef struct {
    enum Type type;
    int line;
    union {
        char* string;
        double real;
        long integer;
    };
} Atom;

#endif //RAINBOW_MAIN_H