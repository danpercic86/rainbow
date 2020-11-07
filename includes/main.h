//
// Created by danpercic on 01.11.2020.
//

#ifndef RAINBOW_MAIN_H
#define RAINBOW_MAIN_H

#define STRING_TERMINATOR '\0'
#define ADD_ATOM_STATE 420
#define ALLOC(var, Type) if((var = malloc(sizeof(Type))) == NULL) err(-1, "Not enough memory");
#define ALLOC_LEN(var, Type, length) if((var = malloc(sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define REALLOC(ptr, Type, length) if((ptr = realloc(ptr, sizeof(Type) * length)) == NULL) err(-1, "Not enough memory");
#define TYPE_OFFSET 99

enum Type {
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

const char* TYPES[32] = {"ID", "TYPE_INT", "TYPE_REAL", "TYPE_STR", "INT", "REAL", "STR", "VAR", "IF", "ELSE", "WHILE",
                         "END", "RETURN", "FUNCTION", "COLON", "SEMICOLON", "LPAR", "RPAR", "COMMA", "OR", "AND", "NOT",
                         "NOTEQUAL", "EQUAL", "ASSIGN", "LESS", "ADD", "SUB", "MUL", "DIV", "FINISH"};

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