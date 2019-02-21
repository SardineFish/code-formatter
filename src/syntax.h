#pragma once

#include "lexer.h"
#include "data-struct.h"

typedef struct ASTNodeStruct ASTNode;
enum
{
    AST_TERMINAL = 1,
    AST_NON_TERMINAL = 0
};
enum
{
    TERMINAL = 1,
    NON_TERMINAL = 0
};
typedef int ASTNodeType;
typedef int TerminalType;
typedef struct ProductionGroupType ProductionGroup;

typedef struct
{
    TerminalType type;
    char* tokenName;
    ProductionGroup* nonTerminal;
    Boolean empty;
} Terminal;

typedef struct
{
    Terminal** sequence;
    int count;
} Production;

struct ProductionGroupType
{
    char* name;
    Production** productions;
    int count;
};

struct ASTNodeStruct
{
    ProductionGroup* production;
    Token* token;
    ASTNode* children;
    ASTNodeType type;
    int count;
};

typedef struct
{
    ASTNode* root;
} SyntaxTree;

typedef struct
{
    Map* patternSet;
    ProductionGroup* entry;
} SyntaxDef;

Map* analyseBNF(const char* input);
SyntaxDef* compileBNF(const char* input, const char* entry);