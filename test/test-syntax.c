#include "../src/syntax.h"
#include <stdio.h>
#define INDENT 2
void printASTNode(ASTNode* node, int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf(" ");
    }
    if (node->type == AST_TERMINAL)
        printf("\"%s\"\n", node->token->attribute);
    else if (node->type == AST_NON_TERMINAL)
    {
        printf("<%s>\n", node->production->name);
        for (int i = 0; i < node->count; i++)
        {
            printASTNode(node->children[i], indent + INDENT);
        }
    }
}
void printAST(SyntaxTree* ast)
{
    printASTNode(ast->root, 0);
}
int main(int argc, char* argv[])
{
    char* pathBNF;
    char* pathCode;
    if (argc > 2)
    {
        pathBNF = argv[1];
        pathCode = argv[2];
    }
    FILE* fp = fopen(pathBNF, "r");
    char doc[8192];
    char code[8192];
    int i = 0;
    do
    {
        doc[i] = fgetc(fp);
    } while (doc[i++] != EOF);
    doc[i - 1] = 0;
    fclose(fp);
    fp = fopen(pathCode, "r");
    i = 0;
    do
    {
        code[i] = fgetc(fp);
    } while (code[i++] != EOF);
    code[i - 1] = 0;

    SyntaxDef* syntax = compileBNF(doc, "syntax");
    SyntaxTree* ast = topDownAnalyse(syntax, code);
    printAST(ast);

    return 0;
}
