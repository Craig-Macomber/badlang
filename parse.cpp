#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <assert.h>

#include "lex.h"
#include "ast.h"

using namespace lex;

static StatementASTNode *readStatement(Lexer *lex);

static void readBlock(Lexer *lex, BlockASTNode * block){
    while (lex->CurTok!='}' && lex->CurTok!=tok_eof) {
        StatementASTNode * s=readStatement(lex);
        assert(s!=NULL);
        block->Statements.push_back(s);
    }
}

static void expect(int expected, int got){
    if (expected!=got){
        std::string e,g;
        e=expected;
        g=got;
        std::cout << "ERROR Expected token: '" << e << "' but got: '" << g << "'" <<std::endl;
        std::cout << "      Expected token: " << expected << " but got: " << got <<std::endl;
        throw 1;
    }
}

static ExpressionASTNode *readExpression(Lexer *lex){
    ExpressionASTNode * e;
    switch (lex->CurTok) {
    case '(':
        lex->getNextToken();
        e=readExpression(lex);
        expect(')',lex->CurTok);
        break;
    case tok_identifier:
        e=new VariableASTNode(lex->StringValue);
        break;
    case tok_number:
        e=new NumberASTNode(lex->NumVal);
        break;
    case tok_string:
        e=new StringASTNode(lex->StringValue);
        break;
    case '{':
        {   
            lex->getNextToken();
            BlockASTNode *b=new BlockASTNode();
            e=b;
            readBlock(lex,b);
            expect('}',lex->CurTok);
        }
        break;
    default:
        std::string s;
        s=lex->CurTok;
        std::cout << "readExpression default: " << s << std::endl;
    }
    lex->getNextToken();
    
    while (1) { // Apply post fixes
        switch (lex->CurTok) {
        case '(': // call, read args
            {
                std::vector<ExpressionASTNode *> args;
                lex->getNextToken();
                if (lex->CurTok!=')') {
                    while (1) {
                        args.push_back(readExpression(lex));
                        if (lex->CurTok==')'){
                            break;
                        }
                        expect(',',lex->CurTok);
                        lex->getNextToken();
                    }
                }
                lex->getNextToken();
                e = new CallASTNode(e,args);
            }
            break;
        case '.':
            lex->getNextToken();
            expect(tok_identifier,lex->CurTok);
            e = new DotASTNode(e,lex->StringValue);
            lex->getNextToken();
            break;
        
        case '=':
        // TODO : Add more binary operators here, add binding priority to readExpression
            {
                std::string op;
                op=lex->CurTok;
                lex->getNextToken();
                e=new BinaryOpASTNode(e,op,readExpression(lex));
            }
            break;
        // TODO cases for operators here
        
        default:
            return e;
        }
    }
}

static StatementASTNode *readStatement(Lexer *lex){
  switch (lex->CurTok) {
    case tok_eof:
        return NULL;
    case tok_var:
        {
            lex->getNextToken();
            expect(tok_identifier,lex->CurTok);
            std::string name=lex->StringValue;
            lex->getNextToken();
            ExpressionASTNode * type=readExpression(lex);
            ExpressionASTNode *value=NULL;
            if (lex->CurTok=='=') {
                lex->getNextToken();
                value=readExpression(lex);
            }
            expect(';',lex->CurTok);
            lex->getNextToken();
            return new DeclarationASTNode(name,type,value);
        }
        break;
    case '(':
    case '{':
    case tok_number:
    case tok_string:
    case tok_identifier:
        {
            StatementASTNode *n=new ExpressionStatementASTNode(readExpression(lex));
            expect(';',lex->CurTok);
            lex->getNextToken();
            return n;
        }
    default:
        std::string s;
        s=lex->CurTok;
        std::cout << "default: '" << s << "'" << std::endl;
        lex->getNextToken();
        return NULL;
    }
}

BlockASTNode * parse(Lexer *lex){
    lex->getNextToken();
    BlockASTNode *b=new BlockASTNode();
    readBlock(lex,b);
    return b;
}
