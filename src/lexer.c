#include "lexer.h"
#include <stdlib.h>

Token* createToken(char* name, char* attribute, int position)
{
    Token* token = (Token*)malloc(sizeof(Token));
    token->name = name;
    token->attribute = attribute;
    token->position = position;
    token->eof = FALSE;
}

LexStream* openLexStream(const char* filename)
{
    LexStream* stream = (LexStream*)malloc(sizeof(LexStream));
    stream->fp = fopen(filename, "r");
    stream->position = 0;
    return stream;
}

inline char readChar(LexStream* stream)
{
    fseek(stream->fp, stream->position, SEEK_SET);
    stream->position++;
    return fgetc(stream->fp);
}

Token* readToken(LexStream* stream)
{
    char chr;
SkipWhiteSpace:
    chr = readChar(stream);
    if(chr==' '||chr=='\r'||chr=='\n'||chr=='\t')
        goto SkipWhiteSpace;
    if(chr == '/')
    {
        chr = readChar(stream);
        if(chr=='*')
        {
            
        }
    }
}