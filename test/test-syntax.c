#include "../src/syntax.h"
#include <stdio.h>
int main(int argc, char* argv[])
{
    char* path;
    if (argc > 1)
        path = argv[1];
    FILE* fp = fopen("./test/c-like.bnf", "r");
    char doc[8192];
    char code[8192];
    int i = 0;
    do
    {
        doc[i] = fgetc(fp);
    } while (doc[i++] != EOF);
    doc[i - 1] = 0;
    fclose(fp);
    fp = fopen("./test/simple-code.c", "r");
    i = 0;
    do
    {
        code[i] = fgetc(fp);
    } while (code[i++] != EOF);
    code[i - 1] = 0;

    SyntaxDef* syntax = compileBNF(doc, "syntax");
    topDownAnalyse(syntax, code);

    return 0;
}
