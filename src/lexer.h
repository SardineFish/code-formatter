#include <stdio.h>

typedef int Boolean;

typedef struct 
{
    char* name;
    char* attribute;
    int position;
    Boolean eof;
} Token;

typedef struct 
{
    char* text;
    int length;
} Document;

typedef struct 
{
    Document* doc;
    int pos;
    int length;
} DocStream;

Token* createToken(char* name, char* attribute, int position);

Document* createDocument(char* text);

Boolean getText(Document* doc, char* buffer, int pos, int len);

DocStream* createDocStream(Document* doc);

Token* readToken(DocStream* doc);