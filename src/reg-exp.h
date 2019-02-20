#pragma once

typedef struct RegExpNFANodeType RegExpNFANode;
typedef struct RegExpNFAEdgeType RegExpNFAEdge;
typedef struct RegExpType RegExp;

typedef int RegExpFlag;
enum
{
    REG_F_NONE = 0,
    REG_F_MULTILINE = 1,
    REG_F_GREEDY = 2,
};

#include "reg-exp-parser.h"
#include "data-struct.h"
#include "reg-exp-matcher.h"
typedef int Boolean;
struct RegExpNFANodeType
{
    int id;
    LinkList* edges;
    LinkList* edgesIn;
};
struct RegExpNFAEdgeType
{
    unsigned char chrLow;
    unsigned char chrHigh;
    RegExpNFANode* prior;
    RegExpNFANode* next;
};


struct RegExpType 
{
    RegExpNFANode* NFA;
    int totalStates;
    RegExpFlag flag;
    RegExpNFANode* finalState;
    Boolean (*test)(const char* string);
};

RegExp* regExp(const char* pattern, RegExpFlag flag);