#include "../src/reg-exp.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char *pattern, *source;
    int greedy = 0;
    if(strlen(argv[1])==strlen("--greedy") && strcmp(argv[1],"--greedy")==0)
    {
        greedy = 1;
        pattern = argv[2];
        source = argv[3];
    }
    else 
    {
        pattern = argv[1];
        source = argv[2];
    }
    RegExp* regex = regExp(pattern);
    char* result = regExpMatch(regex, source, greedy);
    if(!result)
        printf("null\n");
    else
        printf("%s\n", result);
    return 0;
}