#include "ast.h"
#include "type.h"
#include "basicTypes.h"


TypedValue *BinaryOpASTNode::Codegen(){
    TypedValue *left=Left->Codegen();
    TypedValue *right=Right->Codegen();
    return left->Op(Op,right);
}

TypedValue *CallASTNode::Codegen(){
    TypedValue *callee=Callee->Codegen();
    std::vector<TypedValue*> args;
    for (int i=0;i<Args.size();i++) {
        args.push_back(Args[i]->Codegen());
    }
    return callee->Call(args);
}

TypedValue *DotASTNode::Codegen(){
    TypedValue *callee=Callee->Codegen();
    return callee->Dot(Name);
}

TypedValue *StringASTNode::Codegen(){
    return NULL; // TODO StringConst(c,Val);
}

TypedValue *VariableASTNode::Codegen(){
    return NULL; //TODO
}

TypedValue *BlockASTNode::Codegen(){
    // TODO :
    // iterate over scope's ScopeEntries and collect variables
    
    // TODO :
    // generate code, and return a function that can be passed a frame for the parent
    
    // frame contents:
    // pointer to parent frame (may be NULL)
    // Array of variables, each in format:
    //      Type Pointer,Value Pointer
    //      inited to (NULL,NULL)
    // Declaration statements set Type pointer, and use Type to get size to allocate for value, then set Value pointer
    
    
    
    //TypedValue *type=Type->Codegen();
    //TypeValue *tv=new TypeValue(type);
    //TypedValue *v=new Allocate(tv); // TODO : maybe just save type here, and add value in pass at start of block ApplyScope?
    //e->Value=v;

    return NULL; //TODO
}

TypedValue *DeclarationASTNode::Codegen(){
    if (AssignNode!=NULL){
        return AssignNode->Codegen();
    } else {
        return NULL; // TODO : no-op
    }
}

TypedValue *NumberASTNode::Codegen(){
    llvm::Value *value=llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(Val));
    TypeValue *type=new TypeValue(PointerConstant(runtime::Type_Double));
    return new TypedValueRaw(type,value);
}

TypedValue *ExpressionStatementASTNode::Codegen(){
    return Expression->Codegen();
}