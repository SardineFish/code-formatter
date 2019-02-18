#include "../src/reg-exp.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
#ifdef __BORLANDC__
#define _setmode setmode
#endif

void readString(char* buffer)
{
    for (int i = 0;;i++)
    {
        buffer[i] = getchar();
        if(!buffer[i])
            return;
    }
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    //_setmode(_fileno(stdout), _O_BINARY);
    freopen(0, "wb", stdout);
#endif
    char pattern[8192], source[8192];
    int greedy = 0;
    if (argc>1 && strlen(argv[1]) == strlen("--greedy") && strcmp(argv[1], "--greedy") == 0)
    {
        greedy = 1;
    }
    readString(pattern);
    //printf("%s\n", pattern);
    readString(source);
    //printf("%s\n", source);
    RegExp* regex = regExp(pattern);
    char* result = regExpMatch(regex, source, greedy);
    /*printf("%s\n", pattern);
    printf("%s\n", source);*/
    /*
    for (int i = 0; i < strlen(result)+1;i++)
        printf("%c: %d\n", result[i], result[i]);*/
        if (!result)
            printf("null");
        else
        {
            printf("%s", result);
            if(strlen(result)==0)
                putchar(0);
        }
    return 0;
}