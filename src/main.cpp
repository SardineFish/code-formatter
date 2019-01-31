#include <iostream>
#include "reg-exp.h"

using namespace std;

int main()
{
    auto reg = new RegExp("abc(de*|f(ghi)|(test))?");
    return 0;
}