#include "reg-exp.h"
#include "reg-exp-parser.h"
#include "stdlib.h"

RegExp* regExp(const char* pattern)
{
    RegExpNode* header = parse(pattern);
    return compile(header);
}