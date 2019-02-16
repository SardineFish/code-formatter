#include "reg-exp.h"
#include "reg-exp-parser.h"
#include "stdlib.h"

RegExpType* regExp(const char* pattern)
{
    RegExpNode* header = parse(pattern);
    compile(header);
    return NULL;
}