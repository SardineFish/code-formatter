#include "syntax-bnf.c"
#include "syntax.h"

inline char* strClone(const char* str)
{
    int length = strlen(str);
    char* clone = (char*)calloc(length + 1, sizeof(char));
    memcpy(clone, str, length + 1);
    return clone;
}

Production* cloneProduction(Production* src)
{
    Production* dst = createProduction();
    dst->count = src->count;
    dst->sequenceList = listClone(src->sequenceList);
    return dst;
}

void escapeInstantLeftRecursion(const SyntaxDef* syntax, ProductionGroup* productionGroup)
{
    LinkList* leftRecursiveProductions = createLinkList();
    LinkList* nonLeftRecursiveProductions = createLinkList();
    // Seperate <A> ::= <A><a1> | <A><a2> ... | <b1> | <b2> ... into <A><a1> | <A><a2> ... & <b1> | <b2> ...
    for (ListNode* ptr = productionGroup->productionList->header; ptr;ptr=ptr->next)
    {
        Production* p = ptr->element;
        Terminal* headerTerminal = p->sequenceList->header->element;
        if (headerTerminal->type == NON_TERMINAL && strcmp(headerTerminal->tokenName, productionGroup->name) == 0)
        {
            listAdd(leftRecursiveProductions, p);
        }
        else
        {
            listAdd(nonLeftRecursiveProductions, p);
        }
    }
    if (leftRecursiveProductions->length > 0)
    {
        // Generate new production
        int i = 1;
        char* name = createStr(MAX_LEN);
        do
        {
            sprintf(name, "%s-%d", productionGroup->name, i++);
        } while (mapHasKey(syntax->patternSet, name));
        ProductionGroup* newProduction = createProductionGroup(name);

        // <b1> | <b2> ... ---> <b1><A'> | <b2><A'> ...
        for (ListNode* p = nonLeftRecursiveProductions->header; p; p = p->next)
        {
            Production* production = p->element;
            Terminal* nonTerminal = createTerminal(NON_TERMINAL);
            nonTerminal->tokenName = strClone(newProduction->name);
            nonTerminal->nonTerminal = newProduction;
            listAdd(production->sequenceList, nonTerminal);
        }

        // <A><a1> | <A><a2> ... ---> <a1><A'> | <a2><A'> ... | ""
        for (ListNode* p = leftRecursiveProductions->header; p; p = p->next)
        {
            Production* production = p->element;
            // <A> <a> ---> <A> <a> <A'>
            Terminal* nonTerminal = createTerminal(NON_TERMINAL);
            nonTerminal->tokenName = strClone(newProduction->name);
            nonTerminal->nonTerminal = newProduction;
            listAdd(production->sequenceList, nonTerminal);
            // <A> <a> <A'> ---> <a> <A'>
            destroyTerminal((Terminal*)production->sequenceList->header->element);
            listRemove(production->sequenceList, production->sequenceList->header);
        }
        Terminal* empty = createTerminal(TERMINAL);
        empty->tokenName = strClone("");
        empty->empty = 1;
        listAdd(leftRecursiveProductions, empty);

        destroyList(productionGroup->productionList);
        productionGroup->productionList = nonLeftRecursiveProductions;
        mapSetValue(syntax->patternSet, name, newProduction);
    }
}

void escapeLeftRecursion(const SyntaxDef* syntax)
{
    ProductionGroup** groups = (ProductionGroup**)mapValues(syntax->patternSet);
    for (int i = 0; i < syntax->patternSet->length;i++)
    {
        ProductionGroup* groupI = groups[i];
        for (int j = 0; j < i;j++)
        {
            ProductionGroup* groupJ = groups[j];
            for (ListNode* p=groupI->productionList->header; p; p=p->next)
            {
                Production* production;
            Loop:
                production = p->element;
                // Exist <Ai> ::= <Aj><a1><a2>...
                // When <Aj> ::= <b1> | <b2> | ...
                // Change <Ai> ::= <Aj><a1><a2>... into <Ai> ::= <b1><a1><a2>... | <b2><a1><a2>... | ...
                if(((Terminal*)production->sequenceList->header->element)->nonTerminal == groupJ)
                {
                    // <Ai> ::= <Aj><a1><a2>... ---> <Ai> ::= <a1><a2>...
                    listRemove(production->sequenceList ,production->sequenceList->header->element);
                    // <b1> | <b2> | ... ---> <b1><a1><a2>... | <b2><a1><a2>... | ...
                    LinkList* prdctJ = listClone(groupJ->productionList);
                    for (ListNode* prdctPtr = prdctJ->header; prdctPtr; prdctPtr = prdctPtr->next)
                    {
                        prdctPtr->element = cloneProduction(prdctPtr->element);
                        Production* subProduction = prdctPtr->element;
                        listAddRange(subProduction->sequenceList, production->sequenceList);
                    }
                    listAddRange(groupI->productionList, prdctJ);
                    ListNode* nextP = p->next;
                    listRemoveNode(groupI->productionList, p);
                    p = nextP;
                    if(!p)
                        break;
                    goto Loop;
                }
            }
        }
        // Escape instant left recursion
        escapeInstantLeftRecursion(syntax, groupI);
    }
}

SyntaxDef* compileBNF(const char* input, const char* entry)
{
    Map* patternSet = analyseBNF(input);
    SyntaxDef* syntax = (SyntaxDef*)malloc(sizeof(SyntaxDef));
    syntax->patternSet = patternSet;
    syntax->entry = mapGetValue(patternSet, entry);
    //escapeLeftRecursion(syntax);
    //bnfPostProcess(syntax->patternSet);
    return syntax;
}

SyntaxTree* parseSyntax(const SyntaxDef* syntax, const char* source)
{
}