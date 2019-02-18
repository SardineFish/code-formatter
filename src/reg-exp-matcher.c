#include "reg-exp-matcher.h"
#include "reg-exp.h"
#include <stdlib.h>
#include <string.h>

typedef char byte;

void emptyClosure(RegExpNFANode* node, ArrayList* set, byte* stateExist, int stateCount)
{
    if (stateExist[node->id])
        return;
    arrayListAdd(set, node);
    stateExist[node->id] = TRUE;
    for (ListNode* p = node->edges->header; p; p = p->next)
    {
        RegExpNFAEdge* edge = p->element;
        if (edge->chrLow == EMPTY_CHAR)
            emptyClosure(edge->next, set, stateExist, stateCount);
    }
}

ArrayList* moveTo(ArrayList* set, char chr, int stateCount)
{
    ArrayList* out = createArrayList(stateCount);
    byte* stateExist = (byte*)calloc(stateCount, sizeof(char));
    for (int i = 0; i < set->length; i++)
    {
        RegExpNFANode* node = set->list[i];
        for (ListNode* pEdge = node->edges->header; pEdge; pEdge = pEdge->next)
        {
            RegExpNFAEdge* edge = pEdge->element;
            if (stateExist[edge->next->id])
                continue;
            if (edge->chrLow <= chr && chr <= edge->chrHigh)
            {
                stateExist[edge->next->id] = TRUE;
                arrayListAdd(out, edge->next);
            }
        }
    }
    free(stateExist);
    return out;
}

ArrayList* getNextStates(ArrayList* currentStates, char chr, int stateCount)
{
    ArrayList* moveSet = moveTo(currentStates, chr, stateCount);
    ArrayList* nextStates = createArrayList(stateCount);
    byte* stateExist = (byte*)calloc(stateCount, sizeof(char));
    for (int i = 0; i < moveSet->length; i++)
    {
        emptyClosure((RegExpNFANode*)moveSet->list[i], nextStates, stateExist, stateCount);
    }
    destroyArrayList(currentStates);
    destroyArrayList(moveSet);
    free(stateExist);
    return nextStates;
}

char* regExpMatch(RegExp* regexp, const char* text, Boolean greedy)
{
    Boolean found = FALSE;
    ArrayList* currentStates = createArrayList(regexp->totalStates);
    byte* stateExist = (byte*)calloc(regexp->totalStates, sizeof(char));
    emptyClosure(regexp->NFA, currentStates, stateExist, regexp->totalStates);
    int length = strlen(text);
    char* result = (char*)calloc(length + 1, sizeof(char));
    int lastMatchLength = 0;
    for (int i = 0; i < length; i++)
    {
        char chr = text[i];
        currentStates = getNextStates(currentStates, chr, regexp->totalStates);
        if (currentStates->length > 0)
        {
            result[i] = chr;
            if (listIndexOf(currentStates, regexp->finalState) >= 0)
            {
                lastMatchLength = i + 1;
                if (!greedy)
                {
                    found = TRUE;
                    goto Exit;
                }
            }
        }
        else
        {
            if (greedy)
            {
                result[lastMatchLength] = 0;
                found = TRUE;
                goto Exit;
            }
            found = FALSE;
            goto Exit;
        }
    }
Exit:
    if (greedy)
    {
        result[lastMatchLength] = 0;
        found = TRUE;
    }
    if(lastMatchLength==0)
    {
        ArrayList* initialState = createArrayList(regexp->totalStates);
        emptyClosure(regexp->NFA, initialState, stateExist, regexp->totalStates);
        if (listIndexOf(initialState,regexp->finalState)>=0)
        {
            found = TRUE;
        }
        else
            found = FALSE;
        destroyArrayList(initialState);
    }
    destroyArrayList(currentStates);
    free(stateExist);
    if(found)
        return result;
    return NULL;
}