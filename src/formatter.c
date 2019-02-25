#include "formatter.h"
#include "data-struct.h"
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef ASTNode** FormatStruct;
typedef struct
{
    char* name;
    void (*process)(ASTNode* node, int);
} ASTFormatDef;

void formatASTNode(ASTNode* node, int indent);
void formatAstNodeWithOutSpace(ASTNode* node, int indent);

inline void format(int indent, int count, ...)
{
    va_list params;
    va_start(params, count);
    for (int i = 0; i < count; i++)
    {
        ASTNode* node = va_arg(params, ASTNode*);
        formatASTNode(node, indent);
    }
}

/*inline ASTNode* indent(int count)
{
    char* indentText = (char*)calloc(count * INDENT_SPACE + 1, sizeof(char));
    memset(indentText, ' ', count * INDENT_SPACE);
    ASTNode* node = createASTNode(AST_TERMINAL, "indent");
    node->token = createToken("indent", indentText, -1);
    return node;
}

inline ASTNode* inlineText(char* text)
{
    ASTNode* node = createASTNode(AST_TERMINAL, "indent");
    node->token = createToken("text", text, -1);
    return node;
}*/

void printIndent(int count)
{
    char* indentText = (char*)calloc(count * INDENT_SPACE + 1, sizeof(char));
    memset(indentText, ' ', count * INDENT_SPACE);
    printf("%s", indentText);
    free(indentText);
}

void formatExternStatement(ASTNode* node, int indent)
{
    if (node->count == 0)
        return;
    for (int i = 0; i < node->count; i++)
    {
        formatASTNode(node->children[i], indent);
    }
    printf("\n");
}

void formatVarDef(ASTNode* node, int indent)
{
    formatASTNode(node->children[0], indent);
    printf(" ");
    formatASTNode(node->children[1], indent);
}

void formatFuncDef(ASTNode* node, int indent)
{
    formatASTNode(node->children[0], indent); // type
    printf(" ");
    for (int i = 1; i < node->count - 1;i++)
    {
        formatASTNode(node->children[i], indent);
    }
    formatASTNode(node->children[node->count - 1], indent);
}

void formatBlock(ASTNode* node, int indent)
{
    printf("\n");
    printIndent(indent);
    printf("{");
    formatASTNode(node->children[1], indent + 1);
    printf("\n");
    printIndent(indent);
    printf("}");
}

void formatBlockBody(ASTNode* node, int indent)
{
    if(node->count==3)
    {
        printf("\n");
        printIndent(indent);
        printf("{");
        formatASTNode(node->children[1], indent + 1);
        printf("\n");
        printIndent(indent);
        printf("}");
    }
    else
    {
        formatASTNode(node->children[0], indent + 1);
    }
}

void formatBlockEnd(ASTNode* node, int indent)
{
    for (int i = 0; i < node->count - 1;i++)
    {
        formatASTNode(node->children[i], indent);
    }
    //printf("\n");
    if(strcmp(node->children[node->count - 1]->name, "body")==0)
        formatASTNode(node->children[node->count - 1], indent + 1);
    else
        formatASTNode(node->children[node->count - 1], indent);
}

void formatBlockEndIndent(ASTNode* node, int indent)
{
    printf("\n");
    printIndent(indent);
    formatBlockEnd(node, indent);
}

void formatIf(ASTNode* node, int indent)
{
    printf("if(");
    formatASTNode(node->children[2], indent);
    printf(")");
    formatASTNode(node->children[4], indent);
    /*if(node->count>5)
        printf("\n");*/
    for (int i = 5; i < node->count;i++)
        formatASTNode(node->children[i], indent);
}

void formatStatement(ASTNode* node, int indent)
{
    printf("\n");
    printIndent(indent);
    for (int i = 0; i < node->count; i++)
        formatASTNode(node->children[i], indent);
    //printf("\n");
}

void formatParam(ASTNode* node, int indent)
{
    formatASTNode(node->children[0], indent);
    printf(" ");
    formatASTNode(node->children[1], indent);
}

void formatFix(ASTNode* node, int indent)
{
    for (int i = 0; i < node->count;i++)
        formatAstNodeWithOutSpace(node->children[i], indent);
}

ASTFormatDef formatDef[] = {
    {"extern-statement", formatExternStatement},
    {"var-def", formatVarDef},
    {"function-def", formatFuncDef},
    {"param",formatParam},
    {"block", formatBlock},
    {"statement", formatStatement},
    {"if", formatIf},
    {"else-if", formatStatement},
    {"else", formatStatement},
    {"block-body", formatBlockBody},
    {"switch-block", formatBlockBody},
    {"switch-case", formatBlockEndIndent},
    {"switch-default", formatBlockEndIndent},
    {"prefix", formatFix},
    {"postfix", formatFix},
    {"type-ptr", formatFix},
};
char* spaceArroundTokens[] = {
    "+",  "-",  "*",  "/",  "%",  "&",  "^",  "|", "<", ">", "?", "=", "&&",  "||", "++", "--",
    "+=", "-=", "*=", "/=", "&=", "^=", "|=", "<=", ">=", "<<=", ">>=", "<<", ">>",
};
char* spaceRightTokens[] = {
    ",",
    ";",
    "case",
    "unsigned",
};
char* forceInsertSpace[] = {"expr"};

Map* formatMap;
Map* spaceArroundTokensMap;
Map* spaceRightTokensMap;
Map* forceInsertSpaceMap;

void formatASTNode(ASTNode* node, int indent)
{
    if(node->type == AST_TERMINAL)
    {
        if(mapHasKey(spaceArroundTokensMap,node->token->attribute))
        {
            printf(" %s ", node->token->attribute);
        }
        else if (mapHasKey(spaceRightTokensMap, node->token->attribute))
        {
            printf("%s ", node->token->attribute);
        }
        else
            printf("%s", node->token->attribute);
    }
    else if (node->type == AST_NON_TERMINAL)
    {
        void (*process)(ASTNode*, int) = mapGetValue(formatMap, node->name);
        if(process)
        {
            process(node, indent);
        }
        else
        {
            for (int i = 0; i < node->count;i++)
            {
                formatASTNode(node->children[i], indent);
            }
        }
        
    }
    
}

void formatAstNodeWithOutSpace(ASTNode* node, int indent)
{
    if (mapHasKey(forceInsertSpaceMap, node->name))
    {
        formatASTNode(node, indent);
        return;
    }
    if (node->type == AST_TERMINAL)
    {
        printf("%s", node->token->attribute);
    }
    else if (node->type == AST_NON_TERMINAL)
    {
        void (*process)(ASTNode*, int) = mapGetValue(formatMap, node->name);
        if (process)
        {
            process(node, indent);
        }
        else
        {
            for (int i = 0; i < node->count; i++)
            {
                formatAstNodeWithOutSpace(node->children[i], indent);
            }
        }
    }
}

void formatedCode(SyntaxTree* ast, char* buffer)
{
    formatMap = createMap(512);
    spaceArroundTokensMap = createMap(128);
    spaceRightTokensMap = createMap(128);
    forceInsertSpaceMap = createMap(128);
    for (int i = 0; i < sizeof(formatDef) / sizeof(ASTFormatDef);i++)
        mapSetValue(formatMap, formatDef[i].name, formatDef[i].process);
    for (int i = 0; i < sizeof(spaceArroundTokens) / sizeof(char*);i++)
        mapSetValue(spaceArroundTokensMap, spaceArroundTokens[i], spaceArroundTokens[i]);
    for (int i = 0; i < sizeof(spaceRightTokens) / sizeof(char*);i++)
        mapSetValue(spaceRightTokensMap, spaceRightTokens[i], spaceRightTokens[i]);
    for (int i = 0; i < sizeof(forceInsertSpace) / sizeof(char*); i++)
        mapSetValue(forceInsertSpaceMap, forceInsertSpace[i], forceInsertSpace[i]);
    formatASTNode(ast->root, 0);
}