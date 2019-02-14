#include "reg-exp-parser.h"
#include <memory.h>
#include <stdlib.h>

enum
{
    ERROR_UNEXPECT_TOKEN = -1,
};

RegExpNode* cloneNode(RegExpNode* origin);
RegExpNode* cloneNodeWithouNext(RegExpNode* origin);
int parseGroup(const char* pattern, int idx, int length, RegExpNode* header);


RegExpNode* createRegExpNode(RegExpNodeType type, char chr)
{
    RegExpNode* node = (RegExpNode*)malloc(sizeof(RegExpNode));
    node->chr = chr;
    node->header = NULL;
    node->next = NULL;
    node->optional = FALSE;
    node->repeat = FALSE;
    node->type = type;
    return node;
}

RegExpNode* parse(const char* pattern)
{
    
}

int getPattern(const char* pattern, int idx, int length, RegExpNode* header)
{
    RegExpNode* p = header;
    for (idx; idx < length; idx++)
    {
        char chr = pattern[idx];
        RegExpNode* node;
        if (('A' <= chr && chr <= 'Z') || ('a' <= chr && chr <= 'z') || ('0' <= chr && chr <= '9'))
        {
            node = createRegExpNode(REGEXP_CHAR, chr);
        }
        else if (chr == '(')
        {
            node = createRegExpNode(REGEXP_GROUP, 0);
            idx = getPattern(pattern, idx, length, node);
        }

        idx++;
        switch (pattern[idx])
        {
            case '*':
            {
                node->repeat = TRUE;
                node->optional = TRUE;
                break;
            }
            case '+':
            {
                RegExpNode* wrapper = createRegExpNode(REGEXP_GROUP, 0);
                wrapper->header = node;
                node->next = cloneNode(node);
                node->next->repeat = TRUE;
                node->next->optional = TRUE;
                node = wrapper;
                break;
            }
            default:
                idx--;
                break;
        }

        /*idx++;
        switch (pattern[idx])
        {
            case:
                
                break;
        
            default:
                break;
        }*/
    }
}

int getGroup(const char* pattern, int idx, int length, RegExpNode* header)
{
    if(pattern[idx] != '(')
        return ERROR_UNEXPECT_TOKEN;
    idx = getPattern(pattern, idx, length, header);
    if (pattern[++idx] != ')')
        return ERROR_UNEXPECT_TOKEN;
    return idx;
}

RegExpNode* cloneNodeWithouNext(RegExpNode* origin)
{
    if(!origin)
        return NULL;
    RegExpNode* node = createRegExpNode(origin->type, origin->chr);
    node->header = cloneNode(origin->header);
    node->optional = origin->optional;
    node->repeat = origin->repeat;
    return node;
}

RegExpNode* cloneNode(RegExpNode* origin)
{
    if(!origin)
        return NULL;
    RegExpNode* node = createRegExpNode(origin->type, origin->chr);
    node->header = cloneNode(origin->header);
    node->optional = origin->optional;
    node->repeat = origin->repeat;
    for (RegExpNode* p = node; origin->next; p = p->next, origin = origin->next)
    {
        p->next = cloneNodeWithouNext(origin->next);
    }
    return node;
}