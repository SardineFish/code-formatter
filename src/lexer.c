#include "lexer.h"
#include "reg-exp.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LENGTH 8192

#define min(a, b) (((a) < (b)) ? (a) : (b))

enum
{
    TRUE = 1,
    FALSE = 0
};

Token* createToken(char* name, char* attribute, DocPosition pos)
{
    Token* token = (Token*)malloc(sizeof(Token));
    token->name = name;
    token->attribute = attribute;
    token->position = pos.index;
    token->line = pos.line;
    token->column = pos.column;
    token->eof = FALSE;
}

Document* createDocument(char* text)
{
    Document* doc = (Document*)malloc(sizeof(Document));
    doc->text = text;
    doc->length = strlen(text);
    LinkList* list = createLinkList();
    listAdd(list, 0);
    for (int i = 0; i < doc->length;i++)
        if(doc->text[i]=='\n')
            listAdd(list, (void*)i);
    doc->lineCount = listToArray(list, (void***)&doc->endlPos) + 1;
    return doc;
}

DocPosition getDocPos(Document* doc, int idx)
{
    DocPosition pos = {1, 1, idx};
    for (int i = 0; i < doc->lineCount;i++)
    {
        if(doc->endlPos[i]>idx)
        {
            pos.line = i;
            pos.column = idx - doc->endlPos[i - 1] ;
            return pos;
        }
    }
    return pos;
}

Boolean getText(Document* doc, char* buffer, int pos, int len)
{
    int limit = min(pos + len, doc->length) - pos;
    for (int i = 0; i < limit;i++)
        buffer[i] = doc->text[i + len];
    buffer[limit] = 0;
    return TRUE;
}

DocStream* createDocStream(Document* doc)
{
    DocStream* stream = (DocStream*)malloc(sizeof(DocStream));
    stream->doc = doc;
    stream->pos = 0;
    stream->length = doc->length;
    return stream;
}

char* subStr(Document* doc, int pos)
{
    return doc->text + pos;
}

char* strClone(const char* str)
{
    int length = strlen(str);
    char* clone = (char*)calloc(length + 1, sizeof(char));
    memcpy(clone, str, length + 1);
    return clone;
}

char* toStr(char chr)
{
    char* str = (char*)calloc(2, sizeof(char));
    str[0] = chr;
    str[1] = 0;
    return str;
}

Token* readToken(DocStream* stream)
{
    RegExp* regSpace = regExp("\\s+", REG_F_GREEDY);
    RegExp* regID = regExp("[_A-Za-z][_A-Za-z0-9]*", REG_F_GREEDY);
    RegExp* regString = regExp("\"([^\\\"]|\\\\\\S)*\"", REG_F_NONE);
    RegExp* regChar = regExp("'(\\\\(\\S|\\d+)|.)'", REG_F_GREEDY);
    RegExp* regNumber = regExp("(0x[0-9A-Fa-f]+)|((\\d)+)((\\.((\\d)+))?)((e(\\+|-)?((\\d)+))?)", REG_F_GREEDY);
    RegExp* regOperator = regExp("(->)|(\\+\\+|--)|(\\|\\||&&)|((\\+|-|\\*|/|%|=|&|\\||\\^|<<|>>|<|>|=|!)=?)|(\\?|:|,|\\.|;)",REG_F_GREEDY);
    RegExp* regComment = regExp("//.*\n|/\\*.*\\*/", REG_F_MULTILINE);
    RegExp* regKeywords = regExp("#include|#define|void|char|short|int|long|unsigned|double|float|if|else if|else|for|while|do|break|continue|return|switch|case|default", REG_F_GREEDY);
    char brackets[] = "()[]{}";
    char matchResult[MAX_STR_LENGTH];

    // Skip white spaces & comments
    int idx = stream->pos;
    for (idx = stream->pos; idx < stream->length;idx++)
    {
        if (idx == stream->length)
            return NULL;
        switch(stream->doc->text[idx])
        {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            case '\v':
            case '\f':
                break;
            case '/':
                if (regExpMatchNonAlloc(regComment, subStr(stream->doc, idx), FALSE, matchResult))
                {
                    idx += strlen(matchResult) - 1;
                    break;
                }
            default:
                goto NextToken;
        }
    }
NextToken:

    if (idx == stream->length)
        return NULL;
    // Get brackets
    switch (stream->doc->text[idx])
    {
        case '(':
        case '[':
        case '{':
        case ')':
        case ']':
        case '}':
            stream->pos = idx + 1;
            return createToken(toStr(stream->doc->text[idx]), toStr(stream->doc->text[idx]), getDocPos(stream->doc, idx));
    }

    // Get key words
    if(regExpMatchNonAlloc(regKeywords, subStr(stream->doc, idx),TRUE, matchResult) && strlen(matchResult)>0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken(strClone(matchResult), strClone(matchResult), getDocPos(stream->doc, idx));
    }

    // Get operators
    if (regExpMatchNonAlloc(regOperator, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken(strClone(matchResult), strClone(matchResult), getDocPos(stream->doc, idx));
    }

    // Get identifier
    if (regExpMatchNonAlloc(regID, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken("id", strClone(matchResult), getDocPos(stream->doc, idx));
    }

    // Get number
    if (regExpMatchNonAlloc(regNumber, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        int len = strlen(matchResult);
        stream->pos = idx + len;
        if(stream->doc->text[stream->pos]=='F'||stream->doc->text[stream->pos]=='f')
        {
            matchResult[len] = 'f';
            matchResult[len + 1] = '\0';
            stream->pos++;
            return createToken("number-float", strClone(matchResult), getDocPos(stream->doc, idx));
        }
        if (stream->doc->text[stream->pos] == 'L' || stream->doc->text[stream->pos] == 'l')
        {
            matchResult[len] = 'L';
            matchResult[len + 1] = '\0';
            stream->pos++;
            return createToken("number-long", strClone(matchResult), getDocPos(stream->doc, idx));
        }
        return createToken("number", strClone(matchResult), getDocPos(stream->doc, idx));
    }

    // Get string
    if (regExpMatchNonAlloc(regChar, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken("char", strClone(matchResult), getDocPos(stream->doc, idx));
    }

    // Get string
    if (regExpMatchNonAlloc(regString, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken("string", strClone(matchResult), getDocPos(stream->doc, idx));
    }

    char msg[MAX_STR_LENGTH];
    sprintf(msg, "Unexpect token '%c' at line %d:%d", stream->doc->text[idx], getDocPos(stream->doc,idx).line,getDocPos(stream->doc,idx).column);
    throwError(msg);
}

TokenDoc* getTokens(Document* doc)
{
    DocStream* stream = createDocStream(doc);
    LinkList* list = createLinkList();
    Token* token;
    while(token=readToken(stream))
        listAdd(list, token);
    TokenDoc* tokens = (TokenDoc*)malloc(sizeof(TokenDoc));
    tokens->count = listToArray(list, (void***)&tokens->tokens);
    return tokens;
}