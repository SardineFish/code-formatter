#include "syntax.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 128

#define READ_STRING(BUFFER, END)                                                                                       \
    for (int j = 0; i < length && input[i] != END; j++, i++)                                                           \
    {                                                                                                                  \
        BUFFER[j] = input[i];                                                                                          \
    }

char* createStr(int size)
{
    return (char*)calloc(size, sizeof(char));
}

ProductionGroup* createProductionGroup(char* name)
{
    ProductionGroup* group = (ProductionGroup*)malloc(sizeof(ProductionGroup));
    group->productionList = NULL;
    group->name = name;
    group->count = 0;
    group->productions = NULL;
    return group;
}

Production* createProduction()
{
    Production* production = (Production*)malloc(sizeof(Production));
    production->sequenceList = NULL;
    production->sequence = NULL;
    production->count = 0;
    return production;
}

Terminal* createTerminal(TerminalType type)
{
    Terminal* terminal = (Terminal*)malloc(sizeof(Terminal));
    terminal->nonTerminal = NULL;
    terminal->tokenName = NULL;
    terminal->type = type;
    terminal->empty = 0;
    return terminal;
}

void destroyTerminal(Terminal* terminal)
{
    free(terminal->tokenName);
    free(terminal);
}

void bnfPostProcess(Map* set)
{
    ProductionGroup** groups = (ProductionGroup**)mapValues(set);
    for (int i = 0; i < set->length; i++)
    {
        ProductionGroup* group = groups[i];
        if(group->productions)
            free(group->productions);
        group->count = listToArray(group->productionList, (void***)&group->productions);

        for (int j = 0; j < group->count; j++)
        {
            Production* production = group->productions[j];
            if(production->sequence)
                free(production->sequence);
            production->count = listToArray(production->sequenceList, (void***)&production->sequence);
            for (int k = 0; k < production->count; k++)
            {
                Terminal* term = production->sequence[k];
                if (term->type == NON_TERMINAL)
                {
                    term->nonTerminal = (ProductionGroup*)getMapValue(set, term->tokenName);
                    int x = 0;
                }
            }
        }
    }
}

Map* analyseBNF(const char* input)
{
    int length = strlen(input);
    Map* productionSet = createMap(256);
    char chr;
    // Loop to read productions line by line
    for (int i = 0; i < length; i++)
    {
        char* name;
        ProductionGroup* productionGroup;
        LinkList* productionList = createLinkList();

        // Loop to read name
        for (; i < length; i++)
        {
            if (input[i] == '<')
            {
                name = createStr(MAX_LEN);
                i++;
                READ_STRING(name, '>');
                productionGroup = createProductionGroup(name);
            }
            // The symbol "::=" determine the begin of a production
            else if (input[i] == ':' && input[i + 1] == ':' && input[i + 2] == '=')
            {
                i += 3;
                break;
            }
        }

        LinkList* unitSet = createLinkList();
        // Loop to read terminal & non-terminal
        for (; i <= length; i++)
        {
            // non-terminal
            if (input[i] == '<')
            {
                char* nonTerminal = createStr(MAX_LEN);
                i++;
                READ_STRING(nonTerminal, '>');
                Terminal* terminal = createTerminal(NON_TERMINAL);
                terminal->tokenName = nonTerminal;
                unitSet->add(unitSet, terminal);
                continue;
            }
            // terminal
            else if (input[i] == '"')
            {
                char* terminalName = createStr(MAX_LEN);
                i++;
                READ_STRING(terminalName, '"');
                Terminal* terminal = createTerminal(TERMINAL);
                terminal->tokenName = terminalName;
                if (strlen(terminalName) == 0)
                    terminal->empty = 1;
                unitSet->add(unitSet, terminal);
                continue;
            }
            else if (input[i] == '\'')
            {
                char* terminalName = createStr(MAX_LEN);
                i++;
                READ_STRING(terminalName, '\'');
                Terminal* terminal = createTerminal(TERMINAL);
                terminal->tokenName = terminalName;
                if (strlen(terminalName) == 0)
                    terminal->empty = 1;
                unitSet->add(unitSet, terminal);
                continue;
            }
            // production seperator
            else if (input[i] == '|')
            {
                Production* production = createProduction();
                production->count = listToArray(unitSet, (void***)(&production->sequence));
                production->sequenceList = unitSet;
                listAdd(productionList, production);
                //destroyList(unitSet);
                unitSet = createLinkList();
            }
            // complete production
            else if (input[i] == '\n' || input[i] == '\0')
            {
                Production* production = createProduction();
                production->count = listToArray(unitSet, (void***)(&production->sequence));
                production->sequenceList = unitSet;
                listAdd(productionList, production);
                //destroyList(unitSet);
                unitSet = NULL;
                productionGroup->count = listToArray(productionList, (void***)(&productionGroup->productions));
                productionGroup->productionList = productionList;
                //destroyList(productionList);
                productionList = NULL;
                setMapValue(productionSet, productionGroup->name, productionGroup);
                break;
            }
        }
    }
    bnfPostProcess(productionSet);
    return productionSet;
}