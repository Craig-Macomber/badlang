#include "type.h"
#include "basicTypes.h"

#include <assert.h>


llvm::Value *PointerConstant(void *ptr){
    llvm::Type *t=llvm::Type::getInt64Ty(llvm::getGlobalContext());
    llvm::Constant* constInt = llvm::ConstantInt::get(t, (int64_t)ptr); 
    llvm::Value* constPtr = llvm::ConstantExpr::getIntToPtr(constInt, llvm::PointerType::getUnqual(t));
    return constPtr;
}


static TypedValue *StringCost(const std::string &data){
    llvm::Value *value=NULL;// TODO
    TypeValue *type=new TypeValue(PointerConstant(runtime::Type_String));
    return new TypedValueRaw(type,value);
}

static TypedValue *IntCost(int v){
    llvm::Value *value=NULL;// TODO
    TypeValue *type=new TypeValue(PointerConstant(runtime::Type_String));
    return new TypedValueRaw(type,value);
}



llvm::Value *Equivalent(TypeValue *a, TypeValue *b){
    // gen code for:
    // return runtime::typeEquivalent(a.Value,b.Value);
    // TODO
    return NULL;
}


TypeValue::TypeValue(llvm::Value* typePtr) : Value(typePtr) {}

TypeValue::TypeValue(TypedValue *src) {
    // TODO
}


TypedValueRaw::TypedValueRaw(class TypeValue *type, llvm::Value *value)
    : TypedValue(type), Val(value)
{}

llvm::Value * TypedValueRaw::getValue(){
    // TODO
    return NULL;
}


TypedValue::TypedValue(TypeValue *type) : Type(type) {}

TypedValue *TypedValue::Dot(const std::string &name){
    llvm::Value *type=Type->Value;
    int length=name.size();
    llvm::Value *value=getValue();
    // Make call code for:
    // char *namePtr=&name // Allocate name as a global constant
    // f:=info.Dot(TypeInfo *Type.Value,int length,char *namePtr)
    // return f(value)
    return NULL; // TODO
}

// Used for "this(agrs...)".
TypedValue *TypedValue::Call(const std::vector<TypedValue*> &args){
    llvm::Value *type=Type->Value;
    int count=args.size();
    // TODO: llvm::Value *countValue=llvm::ConstantInt::get(GetContext(Type->Value)->LLVM, count);
    llvm::Value *typeArrayPtr=NULL; // TODO : alloc and fill
    // return runtime::
    return NULL; // TODO
}


TypedValue *TypedValue::Op(std::string op, TypedValue *operand){
    TypedValue *opFunc=Dot(op);
    std::vector<TypedValue*> args;
    args.push_back(operand);
    return opFunc->Call(args);
}

TypedValue::~TypedValue() {
    // TODO
}


TypedValue* Allocate(TypeValue *type){
    return new TypedRef(type,NULL); // TODO
}

TypedRef::TypedRef(TypeValue *type, llvm::Value *ptr) : TypedValue(type), Ptr(ptr){
    // TODO
}

llvm::Value * TypedRef::getValue() {
    // TODO
    return NULL;
}

llvm::Value * TypedRef::storeValue(TypedValue *v) {
    // TODO
    return NULL;
}
