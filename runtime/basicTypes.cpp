#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include "runtime.h"

namespace runtime {

template <class C>
Value_Type GetValueType(){
    static TypeInfo _Type_C={sizeof(C),true,NULL,NULL,NULL,NULL};
    return &_Type_C;
}

template <int size>
void BasicCompare(void* out, void* args, void *context){
    *((bool*)(out))= 0==memcmp(args,(char*)args+size,size);
}

FunctionInfo _makeCompareInfo(Value_Type Type_C){
    std::vector<ArgInfo> infoArgs;
    infoArgs.push_back(makeArg(Type_C));
    infoArgs.push_back(makeArg(Type_C));
    std::vector<ArgInfo> returnArgs;
    returnArgs.push_back(makeArg(Type_Bool));
    return FunctionInfo(infoArgs, returnArgs);
}

FunctionInfo _makeAtributeFetcherInfo(Value_Type type, Value_Type returnType){
    std::vector<ArgInfo> infoArgs;
    infoArgs.push_back(makeArg(type));
    std::vector<ArgInfo> returnArgs;
    returnArgs.push_back(makeArg(returnType));
    return FunctionInfo(infoArgs, returnArgs);
}

template <int size>
void _equalsFetcher(void* out, void* args, void *context){
    static IndirectFunc DefaultCompare={&BasicCompare<size>,NULL};
    *(Value_Func*)out=&DefaultCompare;
}

template <int size>
TypeValuePair _basicDot(Value_Type type, std::string &name, Value_Type Type_C){
    assert(name == "==");
    TypeValuePair p;
    
    static FunctionInfo compareInfo = _makeCompareInfo(Type_C);
    static Value_Type compareType = makeFunctionType(compareInfo);
    static FunctionInfo equalsFetcherInfo = _makeAtributeFetcherInfo(Type_C, compareType);
    static Value_Type equalsFetcherType = makeFunctionType(equalsFetcherInfo);
    
    p.t=equalsFetcherType;
    check(isFunction(p.t));
    Value_Func *v=(Value_Func *)heapAllocate(sizeof(Value_Func));
    
    static IndirectFunc EqualsFetcher={&_equalsFetcher<size>,NULL}; 
    
    *v=&EqualsFetcher;
    p.Box=v;
    return p;
}

template <class C>
void BasicDot(void* out, void* args, void *context){
    Value_Type type=*((Value_Type*)args);
    std::string &name=*((std::string*)((char*)args+sizeof(Value_Type)));
    *((TypeValuePair*)(out))=_basicDot<sizeof(C)>(type,name,GetValueType<C>());
}

template <class C>
Value_Type MakeValueType(){
    Value_Type Type_C = GetValueType<C>();
    static IndirectFunc DefaultDot={&BasicDot<C>,NULL};
    
    Type_C->Dot = &DefaultDot;
    return Type_C;
}

template <class C>
Value_Type WrapType(){
    static Value_Type t = MakeValueType<C>();
    return t;
}

Value_Type Type_Type=WrapType<Value_Type>();
Value_Type Type_Bool=WrapType<bool>();
Value_Type Type_Double=WrapType<double>();
Value_Type Type_TypeInfo=WrapType<TypeInfo>();
Value_Type Type_String=WrapType<std::string>();
Value_Type Type_Int=WrapType<int>();

// Don't use this type to user code. If used as the ParamType for another type, that type is considered a function
// Causes Call to be special cased to avoid infinite recursion when trying to call something
Value_Type Type_FunctionType=WrapType<FunctionInfo>();
// TypeInfo Type_ExampleFunction={sizeof(IndirectFunc *),false,NULL,NULL,Type_FunctionType,NULL};

}