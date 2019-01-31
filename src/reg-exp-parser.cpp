#include "reg-exp-parser.h"
#include <stdexcept>
#include <typeinfo>

#define TEMPLATE_UNEXPECT_TOKEN(token) string("Unexpect token '") + token + string("'.")

using namespace std;

CharUnit::CharUnit(char ch)
{
    this->chr = ch;
}

RegExpUnit* parsePattern(string pattern, int& startIdx, int length, RegExpUnit* header);
RegExpUnit* parseUnit(string pattern, int& startIdx, int length);
GroupUnit* parseGroup(string pattern, int& startIdx, int length);
OptionalUnit* parseOptional(string pattern, int& startIdx, int length, RegExpUnit* lastUnit);
RepeatUnit* parseRepeat(string pattern, int& startIdx, int length, RegExpUnit* lastUnit);
OrUnit* parseOr(string pattern, int& startIdx, int length, RegExpUnit* lastUnit);

RegExpHeader* parseRegExp(string pattern, int startIdx, int length)
{
    auto header = new RegExpHeader();
    parsePattern(pattern, startIdx, length, header);
    return header;
}

RegExpUnit* parseUnit(string pattern, int& idx, int length)
{
    char chr = pattern[idx];
    if (('A' <= chr && chr <= 'Z') || ('a' <= chr && chr <= 'z') || ('0' <= chr && chr <= '9'))
    {
        return new CharUnit(chr);
    }
    else if (chr == '(')
    {
        return parseGroup(pattern, idx, length);
    }
    return NULL;
}

RegExpUnit* parsePattern(string pattern, int& i, int length, RegExpUnit* header)
{
    auto p = header;
    for (; i < length; i++)
    {
        auto unit = parseUnit(pattern, i, length);
        if(unit)
        {
            p->next = unit;
            unit->prior = p;
            p = p->next;
        }
        else
        {
            char chr = pattern[i];
            switch(chr)
            {
                case '?':
                    p = parseOptional(pattern, i, length, p);
                    break;
                case '+':
                case '*':
                    p = parseRepeat(pattern, i, length, p);
                    break;
                case '|':
                    p = parseOr(pattern, i, length, p);
                    break;
                default:
                    i--;
                    return header->next;
            }
            if (!p)
                throw runtime_error("Syntax error.");
        }
    }
    return header->next;
}

GroupUnit* parseGroup(string pattern, int& idx, int length)
{
    auto group = new GroupUnit();
    RegExpUnit* p = group;
    idx++;
    parsePattern(pattern, idx, length, group);
    group->header = group->next;
    group->next = NULL;
    if(pattern[++idx] != ')')
        throw runtime_error(TEMPLATE_UNEXPECT_TOKEN(pattern[idx]));
    return group;
}

OptionalUnit* parseOptional(string pattern, int& idx, int length, RegExpUnit* priorUnit)
{
    auto unit = new OptionalUnit();
    unit->prior = priorUnit->prior;
    unit->content = priorUnit;
    priorUnit->prior->next = unit;
    priorUnit->prior = unit;
    return unit;
}

RepeatUnit* parseRepeat(string pattern, int& idx, int length, RegExpUnit* priorUnit)
{
    auto repeat = new RepeatUnit();
    if (pattern[idx] == '*')
    {
        repeat->content = priorUnit;
        repeat->prior = priorUnit->prior;
        priorUnit->prior->next = repeat;
        priorUnit->prior = repeat;
        return repeat;
    }
    else if (pattern[idx] == '+')
    {
        if(dynamic_cast<GroupUnit*>(priorUnit))
        {
            auto group = new GroupUnit();
            group->header = ((GroupUnit*)priorUnit)->header;
            repeat->content = group;
        }
        else if (dynamic_cast<CharUnit*>(priorUnit))
        {
            repeat->content = new CharUnit(((CharUnit*)priorUnit)->chr);
        }
        repeat->prior = priorUnit->prior;
        priorUnit->next = repeat;
        return repeat;
    }
    else
        throw runtime_error(TEMPLATE_UNEXPECT_TOKEN(pattern[idx]));
}

OrUnit* parseOr(string pattern, int& idx, int length, RegExpUnit* priorUnit)
{
    auto orUnit = new OrUnit();
    orUnit->current = priorUnit;
    priorUnit->prior->next = orUnit;
    priorUnit->prior = orUnit;
    orUnit->other = parseUnit(pattern, ++idx, length);
    return orUnit;
}

string RegExpUnit::toString()
{
    if(this->next==NULL)
        return "";
    return this->next->toString();
}
string CharUnit::toString()
{
    auto ch = this->chr;
    return chr + RegExpUnit::toString();
}
string GroupUnit::toString()
{
    auto str = this->header->toString();
    return string("(") + str + string(")") + RegExpUnit::toString();
}
string OptionalUnit::toString()
{
    auto str = this->content->toString();
    return str + string("?") + RegExpUnit::toString();
}
string OrUnit::toString()
{
    auto current = this->current->toString();
    auto other = this->other->toString();
    return current + string("|") + other + RegExpUnit::toString();
}
string RepeatUnit::toString()
{
    auto str = this->content->toString();
    return str + string("*") + RegExpUnit::toString();
}