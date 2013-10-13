#pragma once

#include <string>
#include <vector>

#include "lex.h"
#include "scope.h"

class TypedValue;

/// ASTNode - Base class for all AST nodes.
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
    virtual void Codegen() = 0;
protected:
    virtual void PrintContent(std::ostream &out, int indent)  const= 0;
};

class ExpressionASTNode : public ASTNode {
public:
    virtual TypedValue *Codegen() = 0;
};


class ExpressionStatementASTNode : public StatementASTNode {
    ExpressionASTNode *Expression;
public:
    ExpressionStatementASTNode(ExpressionASTNode *expression);
    virtual void ApplyScope(Scope *scope);
    virtual void Codegen();
protected:
    virtual void PrintContent(std::ostream &out, int indent) const;
};

class BlockASTNode : public ExpressionASTNode {  
public:
    BlockASTNode();
    std::vector<StatementASTNode*> Statements;
    virtual void Print(std::ostream &out, int indentAmmount) const;
    virtual void ApplyScope(Scope *scope);
    virtual TypedValue *Codegen();
};

class VariableASTNode : public ExpressionASTNode {  
    std::string Name;
public:
    VariableASTNode(std::string name);
    const std::string &getName() const;
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
    virtual TypedValue *Codegen();
};


/// NumberASTNode - Expression class for numeric literals like "1.0".
class NumberASTNode : public ExpressionASTNode {
    double Val;
public:
    NumberASTNode(double val);
    virtual void Print(std::ostream &out, int indent) const;
    virtual TypedValue *Codegen();
};


/// StringASTNode - Expression class for string literals like "abc".
class StringASTNode : public ExpressionASTNode {
    std::string Val;
public:
    StringASTNode(std::string val);
    virtual void Print(std::ostream &out, int indent) const;
    virtual TypedValue *Codegen();
};


/// CallASTNode - Expression class for function calls.
class CallASTNode : public ExpressionASTNode {
    ExpressionASTNode *Callee;
    std::vector<ExpressionASTNode*> Args;
public:
    CallASTNode(ExpressionASTNode *callee, std::vector<ExpressionASTNode *> &args);
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
    virtual TypedValue *Codegen();
};

class DotASTNode : public ExpressionASTNode {
    ExpressionASTNode *Callee;
    std::string Name;
public:
    DotASTNode(ExpressionASTNode *callee, std::string name);
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
    virtual TypedValue *Codegen();
};


class BinaryOpASTNode : public ExpressionASTNode {
    ExpressionASTNode *Left;
    ExpressionASTNode *Right;
    std::string Op;
public:
    BinaryOpASTNode(ExpressionASTNode *left,const std::string &op, ExpressionASTNode *right);
    virtual void Print(std::ostream &out, int indent) const;
    virtual void ApplyScope(Scope *scope);
    virtual TypedValue *Codegen();
};

class DeclarationASTNode : public StatementASTNode {
    std::string Name;
    ExpressionASTNode *Type;
    ExpressionASTNode *ValueNode;
    BinaryOpASTNode *AssignNode;
public:
    DeclarationASTNode(std::string name, ExpressionASTNode *type, ExpressionASTNode *value=NULL);
    virtual void PrintContent(std::ostream &out, int indent) const;
    virtual void EditScope(Scope *scope) const;
    virtual void ApplyScope(Scope *scope);
    virtual void Codegen();
};
