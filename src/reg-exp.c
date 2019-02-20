#include "reg-exp.h"
#include "reg-exp-parser.h"
#include "stdlib.h"

RegExp* regExp(const char* pattern, RegExpFlag flag)
{
    RegExpNode* header = parse(pattern, flag);
    return compile(header, flag);
}