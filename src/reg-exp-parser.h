#pragma once
#include "reg-exp.h"

enum
{
    REG_EXP_UNIT = 0,
    REG_EXP_CHAR = 1,
    REG_EXP_GROUP = 2,
    REG_EXP_OR = 3,
    REG_EXP_OPTIONAL = 4,
    REG_EXP_REPEAT = 5
};

typedef int UnitType;

template <int T>
class UnitTypeClass
{
  public:
    int type()
    {
        return T;
    }
};

class RegExpUnit
{
  public:
    RegExpUnit* next = NULL;
    RegExpUnit* prior = NULL;
    virtual string toString();
};

class RegExpHeader : public RegExpUnit
{
};

class CharUnit : public RegExpUnit
{
  public:
    char chr = 0;
    CharUnit(char ch);
    virtual string toString();
};

class GroupUnit : public RegExpUnit
{
  public:
    int groupID = 0;
    RegExpUnit* header = NULL;
    virtual string toString();
};

class OrUnit : public RegExpUnit
{
  public:
    RegExpUnit* current = NULL;
    RegExpUnit* other = NULL;
    virtual string toString();
};

class OptionalUnit : public RegExpUnit
{
  public:
    RegExpUnit* content = NULL;
    virtual string toString();
};

class RepeatUnit : public RegExpUnit
{
  public:
    RegExpUnit* content = NULL;
    virtual string toString();
};

RegExpHeader* parseRegExp(string pattern, int startIdx, int length);