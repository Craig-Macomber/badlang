#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <assert.h>

#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"

#include "lex.h"
#include "scope.h"


/// ASTNode - Base class for all expression nodes.
class ASTNode {
public:
    virtual ~ASTNode();
    virtual void Print(std::ostream&, int indent) const = 0;
    virtual void ApplyScope(Scope *scope);
};


class StatementASTNode : public ASTNode {
public:
    virtual void Print(std::ostream &out, int indentAmmount) const;
    virtual void EditScope(Scope *scope) const;
protected:
    virtual void PrintContent(std::ostream &out, int indent)  const= 0;
};

class ExpressionASTNode : public ASTNode {
};


class ExpressionStatementASTNode : public StatementASTNode {
    ExpressionASTNode *Expression;
public:
    ExpressionStatementASTNode(ExpressionASTNode *expression);
    virtual void ApplyScope(Scope *scope);
protected:
    virtual void PrintContent(std::ostream &out, int indent) const;
};

// TODO : Make Type an expression (here and in parser) not a String
class DeclarationASTNode : public StatementASTNode {
    std::string Name;
    ExpressionASTNode *Type;
    ExpressionASTNode *ValueNode;
public:
    DeclarationASTNode(std::string name, ExpressionASTNode *type, ExpressionASTNode *value=NULL);
    virtual void PrintContent(std::ostream &out, int indent) const;
    virtual void EditScope(Scope *scope) const;
    virtual void ApplyScope(Scope *scope);
};

class BlockASTNode : public ExpressionASTNode {  
public:
    BlockASTNode();
    std::vector<StatementASTNode*> Statements;
    virtual void Print(std::ostream &out, int indentAmmount) const;
    virtual void ApplyScope(Scope *scope);
};

class VariableASTNode : public ExpressionASTNode {  
    std::string Name;
public:
    VariableASTNode(std::string name);
    const std::string &getName() const;
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
};


/// NumberASTNode - Expression class for numeric literals like "1.0".
class NumberASTNode : public ExpressionASTNode {
    double Val;
public:
    NumberASTNode(double val);
    virtual void Print(std::ostream &out, int indent) const;
};


/// StringASTNode - Expression class for string literals like "abc".
class StringASTNode : public ExpressionASTNode {
    std::string Val;
public:
    StringASTNode(std::string val);
    virtual void Print(std::ostream &out, int indent) const;
};


/// CallASTNode - Expression class for function calls.
class CallASTNode : public ExpressionASTNode {
    ExpressionASTNode *Callee;
    std::vector<ExpressionASTNode*> Args;
public:
    CallASTNode(ExpressionASTNode *callee, std::vector<ExpressionASTNode *> &args);
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
};

class DotASTNode : public ExpressionASTNode {
    ExpressionASTNode *Callee;
    std::string Name;
public:
    DotASTNode(ExpressionASTNode *callee, std::string name);
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
};


class BinaryOpASTNode : public ExpressionASTNode {
    ExpressionASTNode *Left;
    ExpressionASTNode *Right;
    std::string Op;
public:
    BinaryOpASTNode(ExpressionASTNode *left,const std::string &op, ExpressionASTNode *right);
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
};
