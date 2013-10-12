#include "ast.h"

ASTNode::~ASTNode(){

}

void ASTNode::ApplyScope(Scope *scope){

}


static void indent(std::ostream &out, int indent){
    for (int i=0;i<indent*4;i++){
        out << " ";
    }
}

void StatementASTNode::Print(std::ostream &out, int indentAmmount) const{
    indent(out,indentAmmount);
    PrintContent(out,indentAmmount);
    out << ";" << std::endl;
}


void StatementASTNode::EditScope(Scope *scope) const{
}

ExpressionStatementASTNode::ExpressionStatementASTNode(ExpressionASTNode *expression) {
    Expression=expression;
}
void ExpressionStatementASTNode::PrintContent(std::ostream &out, int indent)  const{
    Expression->Print(out,indent);
}
void ExpressionStatementASTNode::ApplyScope(Scope *scope){
    Expression->ApplyScope(scope);
}


DeclarationASTNode::DeclarationASTNode(std::string name, ExpressionASTNode *type, ExpressionASTNode *value):
Name(name) {
    Type=type;
    ValueNode=value;
    if (ValueNode!=NULL){
        // Make a helper BinaryOpASTNode for assignment
        VariableASTNode *v=new VariableASTNode(Name);
        AssignNode=new BinaryOpASTNode(v,"=",ValueNode);
    }
}   
void DeclarationASTNode::PrintContent(std::ostream &out, int indent)  const{
    out << "var " << Name << " ";
    Type->Print(out,indent);
    if (ValueNode!=NULL){
        out << " = ";
        ValueNode->Print(out,indent);
    }
}
void DeclarationASTNode::EditScope(Scope *scope) const{
    ScopeEntry *e=new ScopeEntry(this);
    scope->Add(Name,e);
}
void DeclarationASTNode::ApplyScope(Scope *scope){
    Type->ApplyScope(scope);
    if (AssignNode!=NULL){
        AssignNode->ApplyScope(scope);
    }
}


BlockASTNode::BlockASTNode(){};
void BlockASTNode::Print(std::ostream &out, int indentAmmount) const{
    out << "{" << std::endl;
    for (int i=0;i<Statements.size();i++) {
        Statements[i]->Print(out,indentAmmount+1);
    }
    indent(out,indentAmmount);
    out << "}";
}
void BlockASTNode::ApplyScope(Scope *inScope){
    Scope scope(inScope);
    int count=Statements.size();
    for (int i=0;i<count;i++) {
        Statements[i]->EditScope(&scope);
    }
    
    for (int i=0;i<count;i++) {
        Statements[i]->ApplyScope(&scope);
    }
    
}


VariableASTNode::VariableASTNode(std::string name): Name(name) {}
const std::string &VariableASTNode::getName() const { return Name; }
void VariableASTNode::Print(std::ostream &out, int indent) const{
    out << Name;
}
void VariableASTNode::ApplyScope(Scope *scope){
    const ScopeEntry *e=scope->Resolve(Name);
    if (e==NULL){
        std::cout << "Unknown Identifier: " << Name << std::endl;
        Print(std::cout, 0);
        throw 2;
    }
}



NumberASTNode::NumberASTNode(double val) : Val(val) {}
void NumberASTNode::Print(std::ostream &out, int indent) const{
    out << Val;
}


StringASTNode::StringASTNode(std::string val) : Val(val) {}
void StringASTNode::Print(std::ostream &out, int indent) const{
    out << "\"" << Val << "\"";
}



CallASTNode::CallASTNode(ExpressionASTNode *callee, std::vector<ExpressionASTNode *> &args) :
Args(args) {
    Callee=callee;
}
void CallASTNode::Print(std::ostream &out, int indent) const{
    Callee->Print(out,indent);
    out << "(";
    for (int i=0;i<Args.size();i++) {
        if (i!=0) {
            out << ", ";
        }
        Args[i]->Print(out,indent);
    }
    out << ")";
}
void CallASTNode::ApplyScope(Scope *scope){
    Callee->ApplyScope(scope);
    for (int i=0;i<Args.size();i++) {
        Args[i]->ApplyScope(scope);
    }
}



DotASTNode::DotASTNode(ExpressionASTNode *callee, std::string name) : Name(name) {
    Callee=callee;
}
void DotASTNode::Print(std::ostream &out, int indent) const{
    Callee->Print(out,indent);
    out << "." << Name;
}
void DotASTNode::ApplyScope(Scope *scope){
    Callee->ApplyScope(scope);
}



BinaryOpASTNode::BinaryOpASTNode(ExpressionASTNode *left,const std::string &op, ExpressionASTNode *right):Op(op){
    Left=left;
    Right=right;
}   
void BinaryOpASTNode::Print(std::ostream &out, int indent) const{
    out << "(";
    Left->Print(out,indent);
    out << Op;
    Right->Print(out,indent);
    out << ")";
}
void BinaryOpASTNode::ApplyScope(Scope *scope){
    Left->ApplyScope(scope);
    Right->ApplyScope(scope);
}


