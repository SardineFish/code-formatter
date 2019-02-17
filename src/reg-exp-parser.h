#pragma once
#include "reg-exp.h"
#include "link-list.h"

#define DEBUG

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

typedef int RegExpNodeType;
typedef int Boolean;
typedef struct RegExpNodeType RegExpNode;

struct RegExpNodeType
{
    RegExpNodeType type;
    char chr;
    char charFrom;
    char charTo;
    Boolean optional;
    Boolean repeat;
    Boolean selectable;
    RegExpNode* header;
    RegExpNode* next;
};

typedef struct RegExpNFANodeType RegExpNFANode;
typedef struct RegExpNFAEdgeType RegExpNFAEdge;
struct RegExpNFANodeType
{
    int id;
    LinkList* edges;
};
struct RegExpNFAEdgeType
{
    char chrLow;
    char chrHigh;
    RegExpNFANode* prior;
    RegExpNFANode* next;
};

RegExpNode* parse(const char* pattern);
void compile(const RegExpNode* ast);