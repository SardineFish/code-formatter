#include <stdio.h>
#include <stdlib.h>
#include "reg-exp.h"
#include <errno.h>

int main()
{
    regExp("[_A-Z0-9]+ab(df*|g+(h?))");
    return 0;
}