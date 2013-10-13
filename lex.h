#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace lex
{

// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.
enum Token {
  tok_eof = -1,

  // commands
  tok_var = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5,
  tok_string = -6,
  
  //errors
  tok_unexpectedEOFInString=-101
};


class Lexer {
public:
    std::string StringValue;  // Filled in if tok_identifier or tok_string
    double NumVal;              // Filled in if tok_number

    /// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
    /// token the parser is looking at.  getNextToken reads another token from the
    /// lexer and updates CurTok with its results.
    int CurTok;
    int getNextToken();
private:
    int gettok();
};

}