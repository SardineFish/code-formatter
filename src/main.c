#include <stdio.h>
#include <stdlib.h>
#include "reg-exp.h"
#include <errno.h>

int main()
{
    system("chcp 65001");
    char regexp[1024];
    printf("RegExp> ");
    scanf("%s", regexp);
    regExp(regexp);
    return 0;
}