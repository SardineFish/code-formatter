#pragma once

typedef struct RegExpNFANodeType RegExpNFANode;
typedef struct RegExpNFAEdgeType RegExpNFAEdge;
typedef struct RegExpType RegExp;

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
    char chrLow;
    char chrHigh;
    RegExpNFANode* prior;
    RegExpNFANode* next;
};


struct RegExpType 
{
    RegExpNFANode* NFA;
    int totalStates;
    RegExpNFANode* finalState;
    Boolean (*test)(const char* string);
};

RegExp* regExp(const char* pattern);