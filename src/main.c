#include <stdio.h>
#include <stdlib.h>
#include "reg-exp.h"
#include <errno.h>
#include <time.h>

int main()
{
    system("chcp 65001");
    //regExp("a|b");
    char regexp[1024];
    char source[1024];
    printf("RegExp> ");
    scanf("%s", regexp);
    printf("Source> ");
    scanf("%s", source);
    clock_t end, start = clock();
    RegExp* regex = regExp(regexp);
    end = clock();
    printf("Compiled in %fms\n", (double)(end - start) / CLOCKS_PER_SEC / 1000.0);
    start = clock();
    char* result = regExpMatch(regex, source, TRUE);
    end = clock();
    printf("Matched in %fms\n", (double)(end - start) / CLOCKS_PER_SEC / 1000.0);
    if(result)
        printf("Match: %s\n", result);
    else
        printf("Match: null\n");
    return 0;
}