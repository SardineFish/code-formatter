#pragma once
typedef int Boolean;

typedef struct 
{
    Boolean (*test)(const char* string);
} RegExpType;

RegExpType* regExp(const char* pattern);