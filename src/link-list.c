#include "link-list.h"
#include <stdlib.h>

ListNode* createNode(void* element)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->next = NULL;
    node->element = element;
    return node;
}

void add(LinkList* list, void* element)
{
    if(list->length==0)
    {
        list->header = createNode(element);
        list->tail = list->header;
        list->length = 1;
    }
    else 
    {
        list->tail->next = createNode(element);
        list->tail = list->tail->next;
        list->length++;
    }
}

int indexOf(LinkList* list, void* element)
{
    int idx = 0;
    for (ListNode* p = list->header; p;p=p->next ,idx++)
    {
        if(p->element==element)
            return idx;
    }
    return -1;
}

ListNode* nodeOf(LinkList* list, void* element)
{
    for (ListNode* p = list->header; p; p = p->next)
    {
        if (p->element == element)
            return p;
    }
    return NULL;
}

ListNode* getPrior(LinkList* list, ListNode* node)
{
    if(node==NULL)
        return NULL;
    for (ListNode* p = list->header; p->next; p = p->next)
    {
        if (p->next == node)
            return p;
    }
}

ListNode* removeNode(LinkList* list, ListNode* node)
{
    ListNode* prior = getPrior(list, node);
    prior->next = node->next;
}

void* remove(LinkList* list, void* element)
{
    for (ListNode* p = list->header; p->next;p=p->next)
    {
        if(p->next->element==element)
        {
            ListNode* node = p->next;
            p->next = node->next;
            if(!node->next)
                list->tail = p;
            free(node);
            list->length--;
            return element;
        }
    }
    return NULL;
}

LinkList* createLinkList()
{
    LinkList* list = (LinkList*)calloc(1, sizeof(LinkList));
    list->add = add;
    list->length = 0;
    list->remove = remove;
}