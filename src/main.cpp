#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

extern "C"
{
#include "formatter.h"
#include "lexer.h"
#include "reg-exp.h"
#include "syntax.h"
}

#define INDENT 4

char* readFile(const char* path)
{
    FILE* fp = fopen(path, "r");
    char* doc = (char*)calloc(8192, sizeof(char));
    int i = 0;
    do
    {
        doc[i] = fgetc(fp);
    } while (doc[i++] != EOF);
    doc[i - 1] = 0;
    return doc;
}

void printASTNode(ASTNode* node, int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf(" ");
    }
    if (node->type == AST_TERMINAL)
        printf("\"%s\"\n", node->token->attribute);
    else if (node->type == AST_NON_TERMINAL)
    {
        printf("<%s>\n", node->production->name);
        for (int i = 0; i < node->count; i++)
        {
            printASTNode(node->children[i], indent + INDENT);
        }
    }
}
void printAST(SyntaxTree* ast)
{
    if (!ast)
        return;
    printASTNode(ast->root, 0);
}

void help()
{
    cout << "Usage: code-formatter [--version] [--help] <command> [<args>] [options]" << endl
         << "Options:" << endl
         << "    --version           Display version information." << endl
         << "    --help              Display helps." << endl
         << "    --bnf <bnf-file>    Specify a BNF file to define the syntax." << endl
         << "    --greedy            Run regular expression in greedy mode." << endl
         << "    --multiline         Run regular expression in multiline mode." << endl
         << endl
         << "Available commands:" << endl
         << "    regex [--greedy] [--multiline]" << endl
         << "                        Run regular expression matcher."
         << "    lexer <filename>    Run lexer for a specific file & display tokens." << endl
         << "    syntax <filename>   Run syntax analyse for a specific file & display AST struct." << endl
         << "    format <filename>   Run code formatter for a specific file & display formatted code." << endl;
}

void version()
{
    cout << "CodeFormatter v1.0.0" << endl;
}

void regex(bool greedy, bool multiline)
{
    char regexp[1024];
    char source[1024];
    RegExpFlag flag = REG_F_NONE;
    if(greedy)
        flag |= REG_F_GREEDY;
    if(multiline)
        flag |= REG_F_MULTILINE;
    cout << "RegExp> ";
    cin >> regexp;
    cout << "Source> ";
    cin >> source;
    clock_t end, start = clock();
    RegExp* regex = regExp(regexp, flag);
    end = clock();
    cout << "Compiled in " << (double)(end - start) / CLOCKS_PER_SEC / 1000.0 << "ms" << endl;
    start = clock();
    char* result = regExpMatch(regex, source, greedy);
    end = clock();
    cout << "Matched in " << (double)(end - start) / CLOCKS_PER_SEC / 1000.0 << "ms" << endl;
    if (result)
        cout << "Match: " << result << endl;
    else
        cout << "Match: null" << endl;
}

void lexer(const char* path)
{
    char* source = readFile(path);
    Token* token;
    DocStream* stream = createDocStream(createDocument(source));
    while (token = readToken(stream))
    {
        printf("<%s %s>\n", token->name, token->attribute);
    }
}

void syntax(const char* path,const char* bnfPath)
{
    char* source = readFile(path);
    char* bnfCode = readFile(bnfPath);
    SyntaxDef* syntax = compileBNF(bnfCode, "syntax");
    SyntaxTree* ast = topDownAnalyse(syntax, source);
    printAST(ast);
}

void format(const char* path, const char* bnfPath)
{
    char* bnf = readFile(bnfPath);
    char* code = readFile(path);
    char formattedBuffer[8192];
    SyntaxDef* syntax = compileBNF(bnf, "syntax");
    SyntaxTree* ast = topDownAnalyse(syntax, code);
    formatedCode(ast, formattedBuffer);
}

class ArgsReader
{
  private:
    int count;
    vector<string> args;
    int pos;

  public:
    ArgsReader(int argc, char** argv)
    {
        this->pos = 0;
        this->count = argc - 1;
        for (int i = 1; i < argc;i++)
        {
            this->args.push_back(string(argv[i]));
        }
    }
    string get()
    {
        if(this->pos >=this->count)
            return "";
        return this->args[this->pos++];
    }
};

int main(int argc, char* argv[])
{
    ArgsReader* reader = new ArgsReader(argc, argv);
    string arg = "";
    string bnfPath = "";
    string command = "";
    string filename = "";
    bool multiline = false;
    bool greedy = false;
    while((arg=reader->get()) != "")
    {
        if(arg == "--help")
        {
            help();
            break;
        }
        else if (arg == "--version")
        {
            version();
            break;
        }
        else if (arg == "--bnf")
        {
            bnfPath = reader->get();
        }
        else if (arg == "--greedy")
            greedy  =true;
        else if (arg=="--multiline")
            multiline = true;
        else if(arg=="regex")
        {
            command = arg;
        }
        else if (arg == "lexer" || arg=="syntax" || arg == "format")
        {
            command = arg;
            filename = reader->get();
        }
    }
    if(command=="")
        help();
    if(command == "regex")
        regex(greedy, multiline);
    else if (command == "lexer")
        lexer(filename.c_str());
    else if (command=="syntax")
        syntax(filename.c_str(), bnfPath.c_str());
    else if (command == "format")
        format(filename.c_str(), bnfPath.c_str());
    return 0;
}