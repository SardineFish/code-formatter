#pragma once

#ifndef __cplusplus
typedef struct RegExpNodeStruct RegExpNode;
#endif
typedef int RegExpNodeType;
typedef int Boolean;

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

struct RegExpNodeStruct
{
    RegExpNodeType type;
    unsigned char chr;
    unsigned char charFrom;
    unsigned char charTo;
    Boolean optional;
    Boolean repeat;
    Boolean selectable;
    struct RegExpNodeStruct* header;
    struct RegExpNodeStruct* next;
};

#ifdef __cplusplus
typedef RegExpNodeStruct RegExpNode;
#endif

RegExpNode* parse(const char* pattern, RegExpFlag flag);
RegExp* compile(const RegExpNode* ast, RegExpFlag flag);