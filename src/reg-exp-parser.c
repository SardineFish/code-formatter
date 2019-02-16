#include "reg-exp-parser.h"
#include "error.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ERROR_UNEXPECT_TOKEN "Unexpect token."

RegExpNode* cloneNode(RegExpNode* origin);
RegExpNode* cloneNodeWithouNext(RegExpNode* origin);

int parsePattern(const char* pattern, int idx, int length, RegExpNode* header);
int parseGroup(const char* pattern, int idx, int length, RegExpNode* header);
int parseCharSet(const char* pattern, int idx, int length, RegExpNode* header);
int parseSelectable(const char* pattern, int idx, int length, RegExpNode* header);

RegExpNode* createRegExpNode(RegExpNodeType type, char chr)
{
    RegExpNode* node = (RegExpNode*)calloc(1, sizeof(RegExpNode));
    node->chr = chr;
    node->header = NULL;
    node->next = NULL;
    node->optional = FALSE;
    node->repeat = FALSE;
    node->selectable = FALSE;
    node->type = type;
    return node;
}

void testAST(const RegExpNode* node)
{
    if(!node)
        return;
    switch(node->type)
    {
        case REGEXP_GROUP:
            putchar('(');
            break;
        case REGEXP_CHAR:
            putchar(node->chr);
            break;
        case REGEXP_CHAR_SET:
        {
            putchar('[');
            for (int i = 0; i < 256;i++)
                if(node->charSet[i])
                    putchar(i);
            putchar(']');
            break;
        }
    }

    if(node->optional)
    {
        if(node->repeat)
            putchar('*');
        else
            putchar('?');
    }
    testAST(node->header);
    if (node->selectable && node->next)
        putchar('|');
    if (node->type == REGEXP_GROUP)
        putchar(')');
    testAST(node->next);
}

RegExpNode* parse(const char* pattern)
{
    RegExpNode* header = createRegExpNode(REGEXP_GROUP, 0);
    int idx = parsePattern(pattern, 0, strlen(pattern), header);
    if(idx!=strlen(pattern) - 1)
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    header->header = header->next;
    header->next = NULL;
    testAST(header);
    return header;
}

int parsePattern(const char* pattern, int idx, int length, RegExpNode* header)
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
            idx = parseGroup(pattern, idx, length, node);
        }
        else if (chr == '[')
        {
            node = createRegExpNode(REGEXP_CHAR_SET, 0);
            idx = parseCharSet(pattern, idx, length, node);
        }
        else
        {
            return idx - 1;
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
        case '?':
        {
            node->optional = TRUE;
            node->repeat = FALSE;
            break;
        }
        default:
            idx--;
            break;
        }

        p->next = node;
        p = p->next;

        idx++;
        switch (pattern[idx])
        {
        case '|':
            idx = parseSelectable(pattern, idx, length, header);
            break;
        default:
            idx--;
            break;
        }
    }
    return length - 1;
}

int parseSelectable(const char* pattern, int idx, int length, RegExpNode* header)
{
    if (pattern[idx] != '|')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    RegExpNode* wrapper = header;
    if(header->type == REGEXP_GROUP)
    {
        wrapper = createRegExpNode(REGEXP_SELECTABLE, 0);
        wrapper->header = header->next;
        header->next = wrapper;
    }
    else
    {
        wrapper->header = header->next;
        header->next = NULL;
    }
    wrapper->selectable = TRUE;
    wrapper->next = createRegExpNode(REGEXP_SELECTABLE, 0);
    idx = parsePattern(pattern, idx + 1, length, wrapper->next);
    if (!wrapper->next->selectable)
    {
        wrapper->next->header = wrapper->next->next;
        wrapper->next->next = NULL;
        wrapper->next->selectable = TRUE;
    }
    return idx;
}

int parseGroup(const char* pattern, int idx, int length, RegExpNode* header)
{
    if (pattern[idx] != '(')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);

    idx = parsePattern(pattern, idx + 1, length, header);
    header->header = header->next;
    header->next = NULL;

    if (pattern[++idx] != ')')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    return idx;
}

int parseCharSet(const char* pattern, int idx, int length, RegExpNode* header)
{
    if (pattern[idx++] != '[')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);

    int negated = FALSE;
    if(pattern[idx]=='^')
    {
        negated = TRUE;
        idx++;
    }
    for (idx; idx < length;idx++)
    {
        char chr = pattern[idx];
        if(chr == ']')
            break;
        header->charSet[chr] = 1;
        if(pattern[idx+1]=='-')
        {
            for (char i = chr; i <= pattern[idx + 2];i++)
            {
                header->charSet[i] = 1;
            }
            idx += 2;
        }
    }
    if(negated)
    {
        for (int i = 0; i < 256;i++)
            header->charSet[i] = !(header->charSet[i]);
    }

    if (pattern[idx] != ']')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    return idx;
}

RegExpNode* cloneNodeWithouNext(RegExpNode* origin)
{
    if (!origin)
        return NULL;
    RegExpNode* node = createRegExpNode(origin->type, origin->chr);
    node->header = cloneNode(origin->header);
    node->optional = origin->optional;
    node->repeat = origin->repeat;
    return node;
}

RegExpNode* cloneNode(RegExpNode* origin)
{
    if (!origin)
        return NULL;
    RegExpNode* node = createRegExpNode(origin->type, origin->chr);
    node->header = cloneNode(origin->header);
    node->optional = origin->optional;
    node->repeat = origin->repeat;
    memcpy(node->charSet, origin->charSet, 256);
    for (RegExpNode* p = node; origin->next; p = p->next, origin = origin->next)
    {
        p->next = cloneNodeWithouNext(origin->next);
    }
    return node;
}