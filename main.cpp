#include "lex.h"
#include "ast.h"
#include "parse.h"
#include "scope.h"

int main() {
    
    Lexer lex=Lexer();
    BlockASTNode *root=parse(&lex);
    root->Print(std::cout,0);
    std::cout << std::endl;
    root->ApplyScope(&emptyScope);
    return 0;
}