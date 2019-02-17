#include "data-struct.h"
#include <stdlib.h>

ListNode* createNode(void* element)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->next = NULL;
    node->element = element;
    return node;
}

void listAdd(LinkList* list, void* element)
{
    if (list->length == 0)
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
    for (ListNode* p = list->header; p; p = p->next, idx++)
    {
        if (p->element == element)
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
    if (node == NULL)
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

void* listRemove(LinkList* list, void* element)
{
    if (list->header->element == element)
    {
        ListNode* node = list->header;
        if (list->tail == list->header)
            list->tail = list->header->next;
        list->header = list->header->next;
        free(node);
        list->length--;
        return element;
    }
    else
    {
        for (ListNode* p = list->header; p->next; p = p->next)
        {
            if (p->next->element == element)
            {
                ListNode* node = p->next;
                p->next = node->next;
                if (!node->next)
                    list->tail = p;
                free(node);
                list->length--;
                return element;
            }
        }
    }
    return NULL;
}

int listExist(LinkList* list, void* element)
{
    for (ListNode* p = list->header; p->next; p = p->next)
    {
        if (p->element == element)
            return 1;
    }
    return 0;
}

void listCat(LinkList* dst, LinkList* src)
{
    if (dst->length == 0)
    {
        dst->header = dst->tail = src->header;
    }
    else
    {
        dst->tail->next = src->header;
        dst->length += src->length;
    }
    
    
}

LinkList* createLinkList()
{
    LinkList* list = (LinkList*)calloc(1, sizeof(LinkList));
    list->add = listAdd;
    list->length = 0;
    list->remove = listRemove;
}

ArrayList* createArrayList(int maxLength)
{
    ArrayList* list = (ArrayList*)malloc(sizeof(ArrayList));
    list->list = (void**)calloc(maxLength, sizeof(void*));
    list->length = 0;
    list->maxSize = maxLength;
    return list;
}
void arrayListAdd(ArrayList* list,void* element)
{
    list->list[list->length++] = element;
}
void destroyArrayList(ArrayList* list)
{
    free(list->list);
    free(list);
}
int listIndexOf(ArrayList* list, void* element)
{
    for (int i = 0; i < list->length; i++)
        if (list->list[i] == element)
            return i;

    return -1;
}