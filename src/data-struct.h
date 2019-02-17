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

void listAdd(LinkList* list, void* element);
void* listRemove(LinkList* list, void* element);
int listExist(LinkList* list, void* element);
void listCat(LinkList* dst, LinkList* src);

LinkList* createLinkList();


typedef struct
{
    void** list;
    int length;
    int maxSize;
}ArrayList;
ArrayList* createArrayList(int maxLength);
void arrayListAdd(ArrayList* list, void* element);
void destroyArrayList(ArrayList* list);
int listIndexOf(ArrayList* list, void* element);