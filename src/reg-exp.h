#pragma once


typedef int RegExpFlag;
enum
{
    REG_F_NONE = 0,
    REG_F_MULTILINE = 1,
    REG_F_GREEDY = 2,
};
#ifndef __cplusplus
typedef struct RegExpNFANodeType RegExpNFANode;
typedef struct RegExpNFAEdgeType RegExpNFAEdge;
typedef struct RegExpType RegExp;
#endif

#include "data-struct.h"
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
    struct RegExpNFANodeType* prior;
    struct RegExpNFANodeType* next;
};


struct RegExpType 
{
    struct RegExpNFANodeType* NFA;
    int totalStates;
    RegExpFlag flag;
    struct RegExpNFANodeType* finalState;
    Boolean (*test)(const char* string);
};

#ifdef __cplusplus
typedef RegExpNFANodeType RegExpNFANode;
typedef RegExpNFAEdgeType RegExpNFAEdge;
typedef RegExpType RegExp;
#endif

RegExp* regExp(const char* pattern, RegExpFlag flag);

char* regExpMatch(RegExp* regexp, const char* text, Boolean greedy);
Boolean regExpMatchNonAlloc(RegExp* regexp, const char* text, Boolean greedy, char* buffer);