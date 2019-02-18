#pragma once

typedef int RegExpNodeType;
typedef int Boolean;
typedef struct RegExpNodeType RegExpNode;

#include "reg-exp.h"
#include "data-struct.h"

//#define DEBUG

#define EMPTY_CHAR 0

enum
{
    REGEXP_CHAR = 1,
    REGEXP_GROUP = 2,
    REGEXP_SELECTABLE = 4,
    REGEXP_CHAR_SET = 8
};
enum
{
    TRUE = 1,
    FALSE = 0
};

struct RegExpNodeType
{
    RegExpNodeType type;
    unsigned char chr;
    unsigned char charFrom;
    unsigned char charTo;
    Boolean optional;
    Boolean repeat;
    Boolean selectable;
    RegExpNode* header;
    RegExpNode* next;
};

RegExpNode* parse(const char* pattern);
RegExp* compile(const RegExpNode* ast);