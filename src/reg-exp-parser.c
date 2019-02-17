#include "reg-exp-parser.h"
#include "error.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_UNEXPECT_TOKEN "Unexpect token."

RegExpNode* cloneNode(RegExpNode* origin);
RegExpNode* cloneNodeWithouNext(RegExpNode* origin);

int parsePattern(const char* pattern, int idx, int length, RegExpNode* header);
int parseGroup(const char* pattern, int idx, int length, RegExpNode* header);
int parseCharSet(const char* pattern, int idx, int length, RegExpNode* header);
int parseSelectable(const char* pattern, int idx, int length, RegExpNode* header);
int parseEscapeChar(const char* pattern, int idx, int length, RegExpNode* header);

RegExpNode* createRegExpNode(RegExpNodeType type, char chr)
{
    RegExpNode* node = (RegExpNode*)calloc(1, sizeof(RegExpNode));
    node->chr = chr;
    node->charFrom = chr;
    node->charTo = chr;
    node->header = NULL;
    node->next = NULL;
    node->optional = FALSE;
    node->repeat = FALSE;
    node->selectable = FALSE;
    node->type = type;
    return node;
}

void errorUnexpectChar(char chr, int pos)
{
    char msg[1024];
    sprintf(msg, "Syntax Error: Invalid regular expression: Unexpect token '%c' at 1:%d", chr, pos);
    throwError(msg);
}

#ifdef DEBUG
void testAST(const RegExpNode* node)
{
    if (!node)
        return;
    switch (node->type)
    {
    case REGEXP_GROUP:
        putchar('(');
        break;
    case REGEXP_CHAR:
        putchar(node->chr);
        break;
    case REGEXP_CHAR_SET:
        node->charTo - node->charFrom > 0 
            ? printf("[%c-%c]", node->charFrom, node->charTo)
            : printf("%c", node->charFrom);
        break;
    }
    testAST(node->header);
    if (node->selectable && node->next)
        putchar('|');
    if (node->type == REGEXP_GROUP)
        putchar(')');

    if (node->optional)
    {
        if (node->repeat)
            putchar('*');
        else
            putchar('?');
    }
    testAST(node->next);
}
#endif

RegExpNode* parse(const char* pattern)
{
    RegExpNode* header = createRegExpNode(REGEXP_GROUP, 0);
    int idx = parsePattern(pattern, 0, strlen(pattern), header);
    if (idx != strlen(pattern) - 1)
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    header->header = header->next;
    header->next = NULL;
#ifdef DEBUG
    printf("Parsed: ");
    testAST(header);
    printf("\n");
#endif
    return header;
}
char NormalCharSet[] = "!\"#%&'";
int parsePattern(const char* pattern, int idx, int length, RegExpNode* header)
{
    RegExpNode* p = header;
    for (idx; idx < length; idx++)
    {
        char chr = pattern[idx];
        RegExpNode* node;
        switch(chr)
        {
            case '(':
                node = createRegExpNode(REGEXP_GROUP, 0);
                idx = parseGroup(pattern, idx, length, node);
                break;
            case '[':
                node = createRegExpNode(REGEXP_CHAR_SET, 0);
                idx = parseCharSet(pattern, idx, length, node);
                break;
            case '\\':
                node = createRegExpNode(REGEXP_CHAR_SET, 0);
                idx = parseEscapeChar(pattern, idx, length, node);
                break;
            case '.':
                node = createRegExpNode(REGEXP_CHAR_SET, 0);
                parseCharSet("[^\r\n]", 0, 5, node);
                break;
            case ')':
            case ']':
            case '?':
            case '+':
            case '*':
            case '^':
            case '$':
                return idx - 1;
                break;
            default:
                node = createRegExpNode(REGEXP_CHAR, chr);
                break;
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
    if (header->type == REGEXP_GROUP)
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

    char* charSet = (char*)calloc(256, sizeof(char));
    int negated = FALSE;
    if (pattern[idx] == '^')
    {
        negated = TRUE;
        idx++;
    }
    for (idx; idx < length; idx++)
    {
        char chr = pattern[idx];
        if (chr == ']')
            break;
        charSet[chr] = 1;
        if (pattern[idx + 1] == '-')
        {
            for (char i = chr; i <= pattern[idx + 2]; i++)
            {
                charSet[i] = 1;
            }
            idx += 2;
        }
    }
    if (negated)
    {
        for (int i = 1; i < 256; i++)
            charSet[i] = !charSet[i];
    }

    LinkList* list = createLinkList();
    for (int i = 1; i < 256;i++)
    {
        if(charSet[i])
        {
            char from = i;
            char to = i;
            for (; i < 256 && charSet[i];i++)
                to = i;
            i--;
            RegExpNode* setNode = createRegExpNode(REGEXP_CHAR_SET, from);
            setNode->charTo = to;
            list->add(list, setNode);
        }
    }
    if(list->length>1)
    {
        header->type = REGEXP_GROUP;
        RegExpNode* node = header;
        for (ListNode* p = list->header; p; p = p->next)
        {
            node->next = createRegExpNode(REGEXP_SELECTABLE, 0);
            node->next->selectable = TRUE;
            node->next->header = (RegExpNode*)p->element;
            node = node->next;
        }
        header->header = header->next;
        header->next = NULL;
    }
    else{
        header->type = REGEXP_CHAR_SET;
        header->charFrom = ((RegExpNode*)list->header->element)->charFrom;
        header->charTo = ((RegExpNode*)list->header->element)->charTo;
    }

    if (pattern[idx] != ']')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    return idx;
}

int parseEscapeChar(const char* pattern, int idx, int length, RegExpNode* header)
{
    if (pattern[idx] != '\\')
        throwError("Parse regexp failed: " ERROR_UNEXPECT_TOKEN);
    idx++;
    header->type = REGEXP_CHAR_SET;
    switch(pattern[idx])
    {
        case 'd':
            header->type = REGEXP_CHAR_SET;
            header->charFrom = '0';
            header->charTo = '9';
            break;
        case 'D':
            parseCharSet("[^0-9]", 0, 6, header);
            break;
        case 'f':
            header->charFrom = header->charTo = '\f';
            break;
        case 'n':
            header->charFrom = header->charTo = '\n';
            break;
        case 'r':
            header->charFrom = header->charTo = '\r';
            break;
        case 't':
            header->charFrom = header->charTo = '\t';
            break;
        case 'v':
            header->charFrom = header->charTo = '\v';
            break;
        case 's':
            parseCharSet("[ \f\n\r\t\v]", 0, 8, header);
            break;
        case 'S':
            parseCharSet("[^ \f\n\r\t\v]", 0, 9, header);
            break;
        case 'w':
            parseCharSet("[A-Za-z0-9_]", 0, 12, header);
            break;
        case 'W':
            parseCharSet("[^A-Za-z0-9_]", 0, 13, header);
            break;
        default:
            header->type = REGEXP_CHAR;
            header->charFrom = header->charTo = header->chr = pattern[idx];
            break;
    }
    return idx;
}

RegExpNode* cloneNodeWithouNext(RegExpNode* origin)
{
    if (!origin)
        return NULL;
    RegExpNode* node = createRegExpNode(origin->type, origin->chr);
    node->header = cloneNode(origin->header);
    node->optional = origin->optional;
    node->selectable = origin->selectable;
    node->repeat = origin->repeat;
    node->charFrom = origin->charFrom;
    node->charTo = origin->charTo;
    return node;
}

RegExpNode* cloneNode(RegExpNode* origin)
{
    if (!origin)
        return NULL;
    RegExpNode* node = createRegExpNode(origin->type, origin->chr);
    node->header = cloneNode(origin->header);
    node->optional = origin->optional;
    node->selectable = origin->selectable;
    node->repeat = origin->repeat;
    node->charFrom = origin->charFrom;
    node->charTo = origin->charTo;
    for (RegExpNode* p = node; origin->next; p = p->next, origin = origin->next)
    {
        p->next = cloneNodeWithouNext(origin->next);
    }
    return node;
}