#include "reg-exp-parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

RegExpNFANode* compileGroup(RegExpNode* group, RegExpNFANode* header);
RegExpNFANode* compileSequence(RegExpNode* header, RegExpNFANode* headerState);
RegExpNFANode* compileSelectable(RegExpNode* node, RegExpNFANode* header);

RegExpNFANode* createNFANode()
{
    RegExpNFANode* node = (RegExpNFANode*)malloc(sizeof(RegExpNFANode));
    node->edges = createLinkList();
    node->edgesIn = createLinkList();
    node->id = -1;
    return node;
}
RegExpNFAEdge* createNFAEdge(char chrLow, char chrHigh, RegExpNFANode* prior, RegExpNFANode* next)
{
    RegExpNFAEdge* edge = (RegExpNFAEdge*)malloc(sizeof(RegExpNFAEdge));
    edge->chrLow = chrLow;
    edge->chrHigh = chrHigh;
    edge->next = next;
    edge->prior = prior;
    if(next)
        listAdd(next->edgesIn, edge);
    return edge;
}

void collapseNFANode(RegExpNFANode* dst, RegExpNFANode* src, RegExpNFAEdge* edge)
{
    listRemove(dst->edges, edge);
    for (ListNode* p=src->edgesIn->header; p;p=p->next)
    {
        RegExpNFAEdge* edge = p->element;
        edge->next = dst;
    }
    for (ListNode* p = src->edges->header; p; p = p->next)
    {
        RegExpNFAEdge* edge = p->element;
        edge->prior = dst;
    }
    listCat(dst->edges, src->edges);
    free(src->edges);
    free(src->edgesIn);
    free(src);
    free(edge);
}

void collapseNFA(RegExpNFANode* node)
{
    if(node->id==-2)
        return;
    node->id = -2;
    for (ListNode* p = node->edges->header; p;p=p->next)
    {
        RegExpNFAEdge* edge;
    Next:
        if(!p)
            break;
        edge = (RegExpNFAEdge*)p->element;
        if(edge->chrLow==EMPTY_CHAR)
        {
            p = p->next;
            if(node == edge->next)
            {
                listRemove(node->edges, edge);
            }
            else
                collapseNFANode(node, edge->next, edge);
            goto Next;
        }
    }

    for (ListNode* p = node->edges->header; p; p = p->next)
    {
        RegExpNFAEdge* edge = p->element;
        collapseNFA(edge->next);
    }
}

int identifyNFAStates(RegExpNFANode* node, int id, LinkList* edgeSet, LinkList* nodeSet)
{
    if(node->id >= 0)
        return id-1;
    node->id = id;
    nodeSet->add(nodeSet, node);
    for (ListNode* p = node->edges->header; p;p=p->next)
    {
        RegExpNFAEdge* edge = ((RegExpNFAEdge*)p->element);
        edgeSet->add(edgeSet, edge);
        id = identifyNFAStates(edge->next, id + 1, edgeSet, nodeSet);
    }
    return id;
}

#ifdef DEBUG

void printNFARecursive(LinkList* edgeSet, char* str)
{
    
}
void printNFA(LinkList* edgeSet)
{
    char str[8192] = "Graph[{";

    char subStr[1024];
    for (ListNode* p = edgeSet->header; p; p = p->next)
    {
        RegExpNFAEdge* edge = ((RegExpNFAEdge*)p->element);
        if (edge->chrLow == EMPTY_CHAR)
        {
            sprintf(subStr, "Labeled[%d->%d,\"ε\"],", edge->prior->id, edge->next->id);
        }
        else if(edge->chrHigh - edge->chrLow>0)
            sprintf(subStr, "Labeled[%d->%d,\"[%c-%c]\"],", edge->prior->id, edge->next->id, edge->chrLow, edge->chrHigh);
        else
            sprintf(subStr, "Labeled[%d->%d,\"%c\"],", edge->prior->id, edge->next->id, edge->chrLow);
        strcat(str, subStr);
    }
    strcat(str, "\b}, VertexLabels -> \"Name\"]");
    puts(str);
    puts("\n");
}

#endif


RegExp* compile(const RegExpNode* root, RegExpFlag flag)
{
    // Compile to NFA
    RegExpNFANode* initialState = createNFANode();
    RegExpNFANode* finalState = compileSequence(root->header, initialState);
    // collapseNFA(initialState);
    LinkList* edgeSet = createLinkList();
    LinkList* nodeSet = createLinkList();
    identifyNFAStates(initialState, 0, edgeSet, nodeSet);
#ifdef DEBUG
    printNFA(edgeSet);
#endif

    RegExp* regexp = (RegExp*)malloc(sizeof(RegExp));
    regexp->NFA = initialState;
    regexp->flag = flag;
    regexp->finalState = finalState;
    regexp->totalStates = nodeSet->length;
    return regexp;
}

RegExpNFANode* compileSequence(RegExpNode* header, RegExpNFANode* headerState)
{
    RegExpNFANode* tailState = headerState;
    for (RegExpNode* p = header; p; p = p->next)
    {
        RegExpNFANode* initialState = tailState;
        RegExpNFANode* finalState = tailState;
        if (p->type == REGEXP_CHAR || p->type == REGEXP_CHAR_SET)
        {
            finalState = createNFANode();
            RegExpNFAEdge* edge = createNFAEdge(p->charFrom, p->charTo, initialState, finalState);
            initialState->edges->add(initialState->edges, edge);
        }
        else if (p->type == REGEXP_GROUP)
        {
            finalState =  compileGroup(p, initialState);
        }
        else if (p->type == REGEXP_SELECTABLE)
        {
            finalState = compileSelectable(p, initialState);
        }

        if(p->optional)
        {
            // Create empty edge to skip these sequence.
            initialState->edges->add(initialState->edges, createNFAEdge(EMPTY_CHAR, EMPTY_CHAR, initialState, finalState));
            if(p->repeat)
            {
                // Return to the initial state to repeat these sequence.
                finalState->edges->add(finalState->edges, createNFAEdge(EMPTY_CHAR, EMPTY_CHAR, finalState, initialState));
            }
        }

        // Skip all selectable sequence which has been handled.
        while (p->next && p->next->selectable)
            p = p->next;

        tailState = finalState;
    }
    return tailState;
}

RegExpNFANode* compileGroup(RegExpNode* group, RegExpNFANode* header)
{
    return compileSequence(group->header, header);
}

/**
 * @param header - Initial state of these reg-exp combination
 */
RegExpNFANode* compileSelectable(RegExpNode* node, RegExpNFANode* header)
{
    // Final state of these reg-exp combination
    RegExpNFANode* tail = createNFANode();
    for (RegExpNode* p = node; p && p->selectable; p = p->next)
    {
        // Initial state of sub reg-exp
        RegExpNFANode* initialState = createNFANode();
        // Final state of sub reg-exp
        RegExpNFANode* finalState = compileSequence(p->header, initialState);
        // Combine the sub reg-exp with other combination;
        header->edges->add(header->edges, createNFAEdge(EMPTY_CHAR, EMPTY_CHAR, header, initialState));
        finalState->edges->add(finalState->edges, createNFAEdge(EMPTY_CHAR, EMPTY_CHAR, finalState, tail));
    }
    return tail;
}