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
    node->id = -1;
    return node;
}
RegExpNFAEdge* createNFAEdge(char chr, RegExpNFANode* prior, RegExpNFANode* next)
{
    RegExpNFAEdge* edge = (RegExpNFAEdge*)malloc(sizeof(RegExpNFAEdge));
    edge->chr = chr;
    edge->next = next;
    edge->prior = prior;
    return edge;
}

int identifyNFAStates(RegExpNFANode* node, int id, LinkList* edgeSet)
{
    if(node->id >= 0)
        return id;
    node->id = id;
    for (ListNode* p = node->edges->header; p;p=p->next)
    {
        RegExpNFAEdge* edge = ((RegExpNFAEdge*)p->element);
        edgeSet->add(edgeSet, edge);
        id = identifyNFAStates(edge->next, id + 1, edgeSet);
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
        if (edge->chr == EMPTY_CHAR)
        {
            sprintf(subStr, "Labeled[%d->%d,\"ε\"],", edge->prior->id, edge->next->id, edge->chr);
        }
        else
            sprintf(subStr, "Labeled[%d->%d,\"%c\"],", edge->prior->id, edge->next->id, edge->chr);
        strcat(str, subStr);
    }
    strcat(str, "\b}, VertexLabels -> \"Name\"]");
    puts(str);
    puts("\n");
}

#endif


RegExpNFANode* compileToNFA(const RegExpNode* root)
{
    RegExpNFANode* initialState = createNFANode();
    RegExpNFANode* finalState = compileSequence(root->header, initialState);
    LinkList* edgeSet = createLinkList();
    identifyNFAStates(initialState, 0, edgeSet);
#ifdef DEBUG
    printNFA(edgeSet);
#endif
}

void compile(const RegExpNode* root)
{
    RegExpNFANode* rootNFA = compileToNFA(root);
}

RegExpNFANode* compileSequence(RegExpNode* header, RegExpNFANode* headerState)
{
    RegExpNFANode* tailState = headerState;
    for (RegExpNode* p = header; p; p = p->next)
    {
        RegExpNFANode* initialState = tailState;
        RegExpNFANode* finalState = tailState;
        if (p->type == REGEXP_CHAR)
        {
            finalState = createNFANode();
            RegExpNFAEdge* edge = createNFAEdge(p->chr, initialState, finalState);
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
            initialState->edges->add(initialState->edges, createNFAEdge(EMPTY_CHAR, initialState, finalState));
            if(p->repeat)
            {
                // Return to the initial state to repeat these sequence.
                finalState->edges->add(finalState->edges, createNFAEdge(EMPTY_CHAR, finalState, initialState));
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
        header->edges->add(header->edges, createNFAEdge(EMPTY_CHAR, header, initialState));
        finalState->edges->add(finalState->edges, createNFAEdge(EMPTY_CHAR, finalState, tail));
    }
    return tail;
}