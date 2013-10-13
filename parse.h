#pragma once

#include "ast.h"
#include "lex.h"

BlockASTNode * parse(lex::Lexer *lex);