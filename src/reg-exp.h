#pragma once
#include <string>

using namespace std;

class RegExp;
class RegExpMatchResult;

class RegExp
{
  public:
    RegExp(string pattern);
    RegExpMatchResult match(string input);
    bool test(string input);
};

class RegExpMathResult
{
};