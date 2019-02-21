#include "../src/syntax.h"
#include <stdio.h>

void printBNF(Map* set)
{
    ProductionGroup** list =(ProductionGroup**) mapValues(set);
    for (int i = 0; i < set->length;i++)
    {
        ProductionGroup* group = list[i];
        printf("<%s> ::= ", group->name);
        for (int j = 0; j < group->count;j++)
        {
            Production* production = group->productions[j];
            for (int k = 0; k < production->count;k++)
            {
                Terminal* term = production->sequence[k];
                if(term->type==TERMINAL)
                    printf("\"%s\" ", term->tokenName);
                else if(term->type==NON_TERMINAL)
                    printf("<%s> ", term->tokenName);
            }
            if(j!=group->count-1)
                printf("| ");
        }
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    char* path;
    if (argc > 1)
        path = argv[1];
    FILE* fp = fopen(path, "r");
    char doc[8192];
    int i = 0;
    do
    {
        doc[i] = fgetc(fp);
    } while (doc[i++] != EOF);
    doc[i - 1] = 0;
    Map* productionSet = analyseBNF(doc);
    printBNF(productionSet);
    return 0;
}
