#pragma once
#include <stdio.h>

typedef int Boolean;

typedef struct 
{
    char* name;
    char* attribute;
    int position;
    int line;
    int column;
    Boolean eof;
} Token;

typedef struct 
{
    char* text;
    int length;
    int lineCount;
    int* endlPos;
} Document;

typedef struct 
{
    Document* doc;
    int pos;
    int line;
    int column;
    int length;
} DocStream;

typedef struct
{
    Token** tokens;
    int count;
} TokenDoc;

typedef struct
{
    int line;
    int column;
    int index;
} DocPosition;

Token* createToken(char* name, char* attribute, DocPosition pos);

Document* createDocument(char* text);

Boolean getText(Document* doc, char* buffer, int pos, int len);

DocStream* createDocStream(Document* doc);

Token* readToken(DocStream* doc);

TokenDoc* getTokens(Document* doc);