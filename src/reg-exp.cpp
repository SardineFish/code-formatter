#include "reg-exp.h"
#include "reg-exp-parser.h"
#include <iostream>

RegExp::RegExp(string pattern)
{
    auto group = parseRegExp(pattern, 0, pattern.length());
    cout << "Input: " << pattern << endl;
    cout << "Parse: " << group->toString() << endl;
}