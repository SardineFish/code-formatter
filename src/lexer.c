#include "lexer.h"
#include "reg-exp.h"
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
    RegExp* regSpace = regExp("\\s+");
    RegExp* regComment = regExp("(//.*\r\n)");
    RegExp* regID = regExp("[_A-Za-z][_A-Za-z0-9]*");
    RegExp* regString = regExp("\"([^\\\"]|\\\\S)*\"");
    RegExp* regNumber = regExp("((\\d)+)((\\.((\\d)+))?)((e(\\+|-)?((\\d)+))?)");
    RegExp* regOperator = regExp("(->)|(\\+\\+|--)|(\\|\\||&&)|((\\+|-|\\*|/|%|=|&|\\||\\^|<<|>>|<|>|=|!)=?)|(\\?|:|,|\\.)");
    RegExp* regComment = regExp("//.*\r\n|/\\*.*\\*/");

    char* keywords[] = {
        "#include",
        "#define"
        "void",
        "char",
        "short",
        "int",
        "long",
        "unsigned",
        "double"
        "float",
        "if",
        "else if",
        "else",
        "for",
        "while",
        "do",
        "break",
        "continue",
        "return",
        "switch",
        "case",
    };
    char brackets[] = "()[]{}<>";
}