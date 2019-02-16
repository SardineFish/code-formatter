#pragma once
#include "reg-exp.h"

enum
{
    REGEXP_CHAR = 1,
    REGEXP_GROUP = 2,
    REGEXP_SELECTABLE = 4,
};
enum
{
    TRUE = 1,
    FALSE = 0
};

typedef int RegExpNodeType;
typedef int Boolean;
typedef struct RegExpNodeType RegExpNode;

struct RegExpNodeType
{
    RegExpNodeType type;
    char chr;
    Boolean optional;
    Boolean repeat;
    Boolean selectable;
    RegExpNode* header;
    RegExpNode* next;
};
//typedef struct RegExpNodeType RegExpNode;

RegExpNode* parse(const char* pattern);