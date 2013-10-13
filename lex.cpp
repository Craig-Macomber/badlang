#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>


#include "lex.h"

namespace lex
{

int Lexer::readchar() {
    pos++;
    return getchar();
}

SourceRange::SourceRange(int start, int end): start(start), end(end){}

/// gettok - Return the next token from standard input.
int Lexer::gettok() {
    static int LastChar = ' ';
    
    // Skip any whitespace.
    while (isspace(LastChar)){
        LastChar = readchar();
    }
    
    range = SourceRange(pos,pos);
    if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        StringValue = LastChar;
        while (isalnum((LastChar = readchar()))) {
            StringValue += LastChar;
        }
        
        if (StringValue == "var") return tok_var;
        if (StringValue == "extern") return tok_extern;
        return tok_identifier;
    }
    
    if (isdigit(LastChar)) {   // Number: [0-9.]+
        std::string NumStr;
        do {
			NumStr += LastChar;
			LastChar = readchar();
        } while (isdigit(LastChar) || LastChar == '.');
        
        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }
    
    if (LastChar == '#') {
		// Comment until end of line.
		do LastChar = readchar();
		while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
		
		if (LastChar != EOF) {
			return gettok();
		}
    }
    
    // string literals
    if (LastChar == '"') {
		StringValue = "";
		while ((LastChar = readchar()) != EOF && LastChar != '"'){
			StringValue += LastChar;
		} 
		if (LastChar != EOF){
			LastChar = readchar();
			return tok_string;
		} else {
			return tok_unexpectedEOFInString;
		}
    }
    
    // Check for end of file.  Don't eat the EOF.
    if (LastChar == EOF)
		return tok_eof;
    
    // Otherwise, just return the character as its ascii value.
    int ThisChar = LastChar;
    LastChar = readchar();
    return ThisChar;
}



//===----------------------------------------------------------------------===//
// MISC
//===----------------------------------------------------------------------===//


/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results.
int Lexer::getNextToken() {
    range = SourceRange(range.start,pos);
	return CurTok = gettok();
}



/// top ::= definition | external | expression | ';'
// void MainLoop() {
//   while (1) {
//     getNextToken();
//     switch (CurTok) {
//     case tok_eof:
//         return;
//     case tok_var:
//         std::cout << "var" << std::endl;
//         break;
//     case tok_extern:
//         std::cout << "extern" << std::endl;
//         break;
//     case tok_identifier:
//         std::cout << "identifier: " << StringValue << std::endl;
//         break;
//     case tok_number:
//         std::cout << "number: " << NumVal << std::endl;
//         break;
//     case tok_string:
//         std::cout << "string: " << StringValue << std::endl;
//         break;
//     default:
//         std::string s;
//         s=CurTok;
//         std::cout << "default: " << s << std::endl;
//     }
//   }
// }

}
