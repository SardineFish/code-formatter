#include "lexer.h"
#include "reg-exp.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LENGTH 8192

Token* createToken(char* name, char* attribute, int position)
{
    Token* token = (Token*)malloc(sizeof(Token));
    token->name = name;
    token->attribute = attribute;
    token->position = position;
    token->eof = FALSE;
}

Document* createDocument(const char* text)
{
    Document* doc = (Document*)malloc(sizeof(Document));
    doc->text = text;
    doc->length = strlen(text);
    return doc;
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

inline char* subStr(Document* doc, int pos)
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
    RegExp* regSpace = regExp("\\s+");
    RegExp* regComment = regExp("(//.*\r\n)");
    RegExp* regID = regExp("[_A-Za-z][_A-Za-z0-9]*");
    RegExp* regString = regExp("\"([^\\\"]|\\\\S)*\"");
    RegExp* regNumber = regExp("((\\d)+)((\\.((\\d)+))?)((e(\\+|-)?((\\d)+))?)");
    RegExp* regOperator = regExp("(->)|(\\+\\+|--)|(\\|\\||&&)|((\\+|-|\\*|/|%|=|&|\\||\\^|<<|>>|<|>|=|!)=?)|(\\?|:|,|\\.|;)");
    RegExp* regComment = regExp("//.*\r\n|/\\*.*\\*/");
    RegExp* regKeywords = regExp("#include|#define|void|char|short|int|long|unsigned|double|float|if|else if|else|for|while|do|break|continue|return|switch|case");
    char brackets[] = "()[]{}";
    char matchResult[MAX_STR_LENGTH];

    // Skip white spaces & comments
    int idx = stream->pos;
    for (idx = stream->pos; idx < stream->length;idx++)
    {
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
            return createToken(toStr(stream->doc->text[idx]), toStr(stream->doc->text[idx]), idx);
    }

    // Get key words
    if(regExpMatchNonAlloc(regKeywords, subStr(stream->doc, idx),TRUE, matchResult) && strlen(matchResult)>0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken(strClone(matchResult), strClone(matchResult), idx);
    }

    // Get operators
    if (regExpMatchNonAlloc(regOperator, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken(strClone(matchResult), strClone(matchResult), idx);
    }

    // Get identifier
    if (regExpMatchNonAlloc(regID, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken("identifier", strClone(matchResult), idx);
    }

    // Get number
    if (regExpMatchNonAlloc(regNumber, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken("number", strClone(matchResult), idx);
    }

    // Get string
    if (regExpMatchNonAlloc(regString, subStr(stream->doc, idx), TRUE, matchResult) && strlen(matchResult) > 0)
    {
        stream->pos = idx + strlen(matchResult);
        return createToken("string", strClone(matchResult), idx);
    }

    char msg[MAX_STR_LENGTH];
    sprintf(msg, "Unexpect token '%c' at %d", stream->doc->text[idx], idx);
    throwError(msg);
}