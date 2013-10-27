#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include "runtime.h"

//===----------------------------------------------------------------------===//
// "Library" functions that can be "extern'd" from user code.
//===----------------------------------------------------------------------===//

namespace runtime {


ArgInfo makeArg(Value_Type t){
    ArgInfo a;
    a.srcType=ats_direct;
    a.type=t;
    return a;
}
ArgInfo makeArg(ArgTypeSource t,int i) {
    ArgInfo a;
    a.srcType=t;
    a.paramIndex=i;
    return a;
}


void check(bool b){
    if (!b){
        std::cout << "Check Failed" << std::endl;
        throw 2;
    }
}

void checkEqual(int a, int b){
    if (a != b){
        std::cout << "Check equal: " << a << ", " << b << std::endl;
        check(false);
    }
}


void *heapAllocate(int size) {
  return malloc(size);
}

void *heapAllocateValue(Value_Type type) {
    return heapAllocate(type->Size);
}



bool isFunction(Value_Type tf) {
    std::cout << "tf:" << tf << std::endl;
    return (tf->ParamType==Type_FunctionType);
}

const FunctionInfo &getFuncInfo(Value_Type tf) {
    assert(isFunction(tf));
    return *((FunctionInfo*)tf->Param);
}

// does not handel non ats_direct args
void checkFunctionAccepts(Value_Type tf, const std::vector<Value_Type> & argTypes) {
    check(isFunction(tf));
    const FunctionInfo &info=getFuncInfo(tf);
    checkEqual(argTypes.size(), info.argTypes.size());
    for (int i=0;i<argTypes.size();i++) {
        check(info.argTypes[i].srcType==ats_direct);
        check(typeEquivalent(argTypes[i],info.argTypes[i].type));
    }
}

// does not handel non ats_direct args
void checkFunctionReturns(Value_Type tf, const std::vector<Value_Type> & returnTypes) {
    check(isFunction(tf));
    FunctionInfo *info=(FunctionInfo *)(tf->Param);
    checkEqual(returnTypes.size(), info->returnTypes.size());
    for (int i=0;i<returnTypes.size();i++) {
        check(info->returnTypes[i].srcType==ats_direct);
        check(typeEquivalent(returnTypes[i],info->returnTypes[i].type));
    }
}

void checkFunctionValid(const FunctionInfo &f) {
    for (int i=0;i<f.argTypes.size();i++) {
        const ArgInfo & a=f.argTypes[i];
        if (a.srcType==ats_direct) {
            check(a.type!=0); // Check non null type
        } else if (a.srcType==ats_param) {
            int k=a.paramIndex;
            check(k<i);
            check(k>=0);
            check(f.argTypes[k].type==Type_Type);
        } else {
            check(false); // Invalid !
        }
    }
    for (int i=0;i<f.returnTypes.size();i++) {
        const ArgInfo & a=f.returnTypes[i];
        if (a.srcType==ats_direct) {
            check(a.type!=0); // Check non null type
        } else if (a.srcType==ats_returnValue) {
            int k=a.paramIndex;
            check(k<i);
            check(k>=0);
            check(f.returnTypes[k].type==Type_Type);
        } else if (a.srcType==ats_param) {
            int k=a.paramIndex;
            check(k<f.argTypes.size());
            check(k>=0);
            check(f.argTypes[k].type==Type_Type);
        } else {
            check(false); // Invalid !
        }
    }
}


int getArgsSize(const std::vector<ArgInfo> & args) {
    int total=0;
    for (int i=0;i<args.size();i++) {
        const ArgInfo & a=args[i];
        if (a.srcType==ats_direct) {
            total+=a.type->Size;
        } else {
            total+=sizeof(void*);
        }
    }
    return total;
}

int getFuncArgsSize(const FunctionInfo &f) {
    return getArgsSize(f.argTypes);
}

int getFuncReturnSize(const FunctionInfo &f) {
    return getArgsSize(f.returnTypes);
}


void invokeFunc(IndirectFunc *f, void *out, void *args) {
    f->Func(out,args,f->Context);
}


// ... args must be pointers to the args, which will be copied
// out will be filled with the return values. It better be big enough.
// TODO : this does not handel when some return values's types are other return values (or paramaters)
// This isn't the fast way to do this.
void invokeFuncWrapper(Value_Type tf, Value_Func f, void *out, ...) {
    const FunctionInfo & info=getFuncInfo(tf);
    int size=getFuncArgsSize(info);
    std::cout << "invokeFuncWrapper size:" << size << std::endl;
    int offset=0;
    char *args=(char *)alloca(size);
    
    va_list ap; //will point to each unnamed argument in turn
    va_start(ap,out); // point to first element after out
    std::vector<void*> argPtrs;
    for (int i=0;i<info.argTypes.size();i++) {
        const ArgInfo & a=info.argTypes[i];
        int argSize;
        void* argSrc;
        if (a.srcType==ats_direct) {
            argSize=a.type->Size;
            argSrc=va_arg(ap,void*);
        } else {
            assert(a.srcType==ats_param);
            
            // The type of this arg is a previous arg, so look up its size, and allocate a box
            Value_Type argType=*(Value_Type*)argPtrs[a.paramIndex];
            int boxSize=argType->Size;
            void* argBox=alloca(boxSize);
            void* boxSrc=va_arg(ap,void*);
            memcpy(argBox,boxSrc,boxSize);
            
            // The "real" arg is a pointer to the box
            argSize=sizeof(void*);
            argSrc=&argBox;
        }
        memcpy(args+offset,argSrc,argSize);
        std::cout << "invokeFuncWrapper arg:" << offset << ", " <<  argSize << "At:" << (void*)(args+offset) << std::endl;
        argPtrs.push_back(args+offset);
        offset+=argSize;
    }
    assert(offset==size);
    
    invokeFunc(f,out,args);
    
    va_end(ap); //cleanup
}

std::vector<ArgInfo> _makeTypeValueReturnArgs(){
    std::vector<ArgInfo> typeValueReturnArgs;
    typeValueReturnArgs.push_back(makeArg(Type_Type));
    typeValueReturnArgs.push_back(makeArg(ats_returnValue,0));
    return typeValueReturnArgs;
}

const std::vector<ArgInfo> typeValueReturnArgs = _makeTypeValueReturnArgs();

Value_Type makeFunctionType(FunctionInfo &info) {
    Value_Type tf=new TypeInfo();
    tf->ParamType=Type_FunctionType;
    tf->Param=&info;
    tf->Size=sizeof(Value_Func);
    tf->Unique=false;
    tf->Dot=0; // TODO?
    tf->Call=0; // Not used (Call for functions is special cased)
    return tf;
}



FunctionInfo::FunctionInfo(std::vector<ArgInfo> argTypes,std::vector<ArgInfo> returnTypes):
    argTypes(argTypes),returnTypes(returnTypes) {
    // TODO: checkFunctionValid(*this);
}

FunctionInfo _makeDotInfo(){
    std::vector<ArgInfo> dotInfoArgs;
    dotInfoArgs.push_back(makeArg(Type_Type));
    dotInfoArgs.push_back(makeArg(Type_String));
    return FunctionInfo(dotInfoArgs, typeValueReturnArgs);
}

FunctionInfo dotInfo = _makeDotInfo();
const Value_Type dotType = makeFunctionType(dotInfo);

// invokes the dot function for t, and returns the resulting atribute fetcher function
TypeValuePair dot(Value_Type t, const std::string &text) {
    TypeValuePair p;
    std::string s = text;
    std::cout << "dot:" << s << std::endl;
    invokeFuncWrapper(dotType,t->Dot,&p,&t,&s);
    
    // The return type[s] of the atributeFetcherFunction may vary
    // But it is required accept an instance of Type t, so check that:
    std::vector<Value_Type> args;
    args.push_back(t);
    checkFunctionAccepts(p.t,args);
    
    return p;
}



Value_Type pointerType(Value_Type t) {
    // TODO
    return NULL;
}


std::vector<ArgInfo> _makeCallArgs() {
    std::vector<ArgInfo> _callArgs;
    _callArgs.push_back(makeArg(Type_Type));
    _callArgs.push_back(makeArg(Type_Int));
    _callArgs.push_back(makeArg(pointerType(Type_Type)));
    return _callArgs;
}
const std::vector<ArgInfo> _callArgs = _makeCallArgs();


FunctionInfo callInfo = FunctionInfo(_callArgs,typeValueReturnArgs);

const Value_Type callType = makeFunctionType(callInfo);



Value_Func getCallFunc(Value_Type ta, void *a, const std::vector<Value_Type> & argTypes, Value_Type *funcTypeOut) {
    // Is a a function?
    if (isFunction(ta)) {
        // Type check the function to make sure args match
        checkFunctionAccepts(ta,argTypes);
        *funcTypeOut=ta;
        return *((Value_Func *)a);
    }

    // Not a function, invoke Call, return result
    // TODO
    return NULL;
}


Value_Func getOperatorFunc(const std::string & op, Value_Type ta, void *a, Value_Type tb, Value_Type *funcTypeOut) {
    TypeValuePair tv=dot(ta,op);
    const FunctionInfo &atributeFetcherFunctionInfo=getFuncInfo(tv.t);
    
    // atribute for an operator must be a single value, so check that it is:
    check(atributeFetcherFunctionInfo.returnTypes.size()==1);
    
    // TODO : support ats_param
    check(atributeFetcherFunctionInfo.returnTypes[0].srcType==ats_direct);
    Value_Type atributeType=atributeFetcherFunctionInfo.returnTypes[0].type;
    
    Value_Func atributeFetcherFunction=*(Value_Func*)tv.Box;
    free(tv.Box);
    
    int size=atributeType->Size;
    assert(size==getFuncReturnSize(atributeFetcherFunctionInfo));
    void *out=alloca(size);
    
    invokeFuncWrapper(tv.t,atributeFetcherFunction,out,a);
    std::vector<Value_Type> argTypes;
    argTypes.push_back(ta);
    argTypes.push_back(tb);
    return getCallFunc(atributeType,out,argTypes,funcTypeOut);
}


// apply an operator.
// Eventually the code generator won't use this, since it removes optimization options:
// - Lookups cannot be done eagerly
// - Return type cannot the computed eagerly
// - Cannot avoid allocations
// Caller must free result.
void *applyOperator(const std::string & op, Value_Type ta, void *a, Value_Type tb, void *b, Value_Type *funcTypeOut) {
    Value_Func f=getOperatorFunc(op,ta,a,tb,funcTypeOut);
    int size=getFuncReturnSize(getFuncInfo(*funcTypeOut));
    void *out=heapAllocate(size);
    invokeFuncWrapper(*funcTypeOut,f,out,a,b);
    return out;
}


// Like applyOperator, this is slow, and does not expose optimization opportunities 
bool valuesEqual(Value_Type t, void *a, void *b) {
    // Fast path for Types
    if (t==Type_Type) {
        return typeEquivalent(*(Value_Type*)(a),*(Value_Type*)(b));
    }
    // Invoke == operator
    Value_Type tf;
    void *result=applyOperator("==",t,a,t,b,&tf);
    
    // Type check function
    std::vector<Value_Type> r;
    r.push_back(Type_Bool);
    checkFunctionReturns(tf,r);
    bool out=*(bool*)result;
    free(result);
    return out;    
}


// Equality:
// Equality for Types a and b is define as follows (first match)
// 1. Equal if a and b point to the same TypeInfo
// 2. Unequal if a.Unique or b.Unique
// 3. Unequal if DotFunc,CallFunc, or Size differ
// 4. Equal iff Paramaters are equal (Equal types, and equal values.)
// Examples:
// This means pointers to 2 equal types are equal, unless you set Unique (as you would for a named pointer type)
// Also, 2 separately defined vectors of the same type are equal, etc.

bool typeEquivalent(Value_Type a, Value_Type b) {
    std::cout << "typeEquivalent" << a << " =? " << b << std::endl;
    if (a==b){
        return true;
    }
    if (a->Unique || b->Unique) {
        return false;
    }
    if (a->Size!=b->Size || a->Dot!=b->Dot || a->Call!=b->Call){
        // TODO: Assumes shallow equality tests is enough for Dot and Call (function equality, ick!)
        // This may be an issue if they are implemented as closures (Is anything not a closure?)
        return false; 
    }
    if (!typeEquivalent(a->ParamType,b->ParamType)) {
        return false;
    }
    return valuesEqual(a->ParamType,a->Param,b->Param);
}

void test(){
    double a=1,b=2;
    assert(valuesEqual(Type_Type,&Type_Type,&Type_Type));
    assert(!valuesEqual(Type_Type,&Type_Type,&Type_Double));
    assert(valuesEqual(Type_Type,&Type_Double,&Type_Double));
    assert(!valuesEqual(Type_Double,&a,&b));
    b=1;
    assert(valuesEqual(Type_Double,&a,&b));
}

}