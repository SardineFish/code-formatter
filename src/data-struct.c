#include "data-struct.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

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

void destroyList(LinkList* list)
{
    ListNode* p = list->header;
    ListNode* next;
    while(p)
    {
        next = p->next;
        free(p);
        p = next;
    }
    free(list);
}

int listToArray(LinkList* list, void*** array)
{
    void** arr = (void**)calloc(list->length, sizeof(void*));
    int i = 0;
    for (ListNode* p = list->header; p;p=p->next)
    {
        arr[i++] = p->element;
    }
    *array = arr;
    return list->length;
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
void arrayListAdd(ArrayList* list, void* element)
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

// Algorithm djb2
unsigned long hash(unsigned char* str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

Map* createMap(int maxSize)
{
    maxSize *= 2;
    Map* map = (Map*)malloc(sizeof(Map));
    map->_size = maxSize;
    map->items = (void*)calloc(maxSize, sizeof(KeyValuePair*));
    map->length = 0;
    return map;
}
void* getMapValue(Map* map, char* key)
{
    unsigned long hashCode = hash(key);
    int base = hashCode % map->_size;
    for (int i = 0; i < map->_size; i++)
    {
        int idx = (base + i) % map->_size;
        if(!map->items[idx])
            return NULL;
        if (strcmp(map->items[idx]->key, key) == 0)
        {
            return map->items[idx]->value;
        }
    }
    return NULL;
}
void setMapValue(Map* map, char* key, void* value)
{
    unsigned long hashCode = hash(key);
    int base = hashCode % map->_size;
    for (int i = 0; i < map->_size; i++)
    {
        int idx = (base + i) % map->_size;
        if(!map->items[idx])
        {
            map->items[idx] = (KeyValuePair*)malloc(sizeof(KeyValuePair));
            map->items[idx]->key = key;
            map->items[idx]->value = value;
            map->length++;
            return;
        }
        else if (strcmp(map->items[idx]->key, key) == 0)
        {
            map->items[idx]->value = value;
            map->length++;
            return;
        }
    }
    throwError("Hash table over flow.");
}
char** mapKeys(Map* map)
{
    ArrayList* list = createArrayList(map->length);
    for (int i = 0; i < map->_size; i++)
    {
        KeyValuePair* pair = map->items[i];
        if (pair)
        {
            arrayListAdd(list, pair->key);
        }
    }
    char** keys = (char**)list->list;
    free(list);
    return keys;
}
void** mapValues(Map* map)
{
    ArrayList* list = createArrayList(map->length);
    for (int i = 0; i < map->_size; i++)
    {
        KeyValuePair* pair = map->items[i];
        if (pair)
        {
            arrayListAdd(list, pair->value);
        }
    }
    void** values = list->list;
    free(list);
    return values;
}