#include "lex.h"
#include "ast.h"
#include "parse.h"
#include "scope.h"
#include "jit.h"
#include "runtime/runtime.h"

int main() {
    runtime::test();

    lex::Lexer lex=lex::Lexer();
    BlockASTNode *root=parse(&lex);
    root->Print(std::cout,0);
    std::cout << std::endl;
    root->ApplyScope(&emptyScope);
    jit::run(root);
    
    return 0;
}