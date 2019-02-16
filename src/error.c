#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void throwError(const char* message)
{
    printf("%s\n", message);
    exit(1);
}
