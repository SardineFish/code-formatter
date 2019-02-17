#include <stdio.h>

typedef int Boolean;

enum
{
    TRUE = 1,
    FALSE = 0
};

typedef struct 
{
    char* name;
    char* attribute;
    int position;
    Boolean eof;
} Token;

typedef struct 
{
    FILE* fp;
    int position;
} LexStream;

Token* createToken(char* name, char* attribute, int position);

LexStream openLexStream(const char* filename);

Token* readToken(LexStream stream);