#include "../src/lexer.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    char* path;
    if(argc>1)
        path = argv[1];
    FILE* fp = fopen(path, "r");
    char doc[8192];
    int i = 0;
    do
    {
        doc[i] = fgetc(fp);
    } while (doc[i++] != EOF);
    doc[i - 1] = 0;
    Token* token;
    DocStream* stream = createDocStream(createDocument(doc));
    while(token = readToken(stream))
    {
        printf("<%s %s>\n", token->name, token->attribute);
    }
    return 0;
}
