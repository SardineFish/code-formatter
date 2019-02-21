#include "syntax-bnf.c"

SyntaxDef* compileBNF(const char* input,const char* entry)
{
    Map* patternSet = analyseBNF(input);
    SyntaxDef* syntax = (SyntaxDef*)malloc(sizeof(SyntaxDef));
    syntax->patternSet = patternSet;
    syntax->entry = mapGetValue(patternSet, entry);
    
}