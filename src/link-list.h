#pragma once

typedef struct ListNodeType ListNode;
typedef struct LinkListType LinkList;

struct LinkListType
{
    ListNode* header;
    ListNode* tail;
    int length;
    void (*add)(LinkList* list, void* element);
    void* (*remove)(LinkList* list, void* element);
};

struct ListNodeType
{
    void* element;
    ListNode* next;
};

LinkList* createLinkList();