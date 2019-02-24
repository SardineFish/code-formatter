#include "../src/formatter.h"
#include <stdlib.h>

char* readFile(char* path)
{
    FILE* fp = fopen(path, "r");
    char* doc = (char*)calloc(8192, sizeof(char));
    int i = 0;
    do
    {
        doc[i] = fgetc(fp);
    } while (doc[i++] != EOF);
    doc[i - 1] = 0;
    return doc;
}

int main()
{
    char* bnf = readFile("./test/c-like.bnf");
    char* code = readFile("./test/code-test.c");
    char formattedBuffer[8192];
    SyntaxDef* syntax = compileBNF(bnf, "syntax");
    SyntaxTree* ast = topDownAnalyse(syntax, code);
    formatedCode(ast, formattedBuffer);
}