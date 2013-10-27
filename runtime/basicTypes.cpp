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
void _EqualsFetcher(void* out, void* args, void *context){
    static IndirectFunc DefaultCompare={&BasicCompare<size>,NULL};
    *(Value_Func*)out=&DefaultCompare;
}

template <int size>
TypeValuePair _BasicDot(Value_Type type, std::string &name, Value_Type Type_C){
    std::cout << "Dot:" << name << std::endl;
    assert(name == "==");
    TypeValuePair p;
    
    static FunctionInfo compareInfo = _makeCompareInfo(Type_C);
    static Value_Type compareType = makeFunctionType(compareInfo);
    static FunctionInfo equalsFetcherInfo = _makeAtributeFetcherInfo(Type_C, compareType);
    static Value_Type equalsFetcherType = makeFunctionType(equalsFetcherInfo);
    
    
    p.t=equalsFetcherType;
    check(isFunction(p.t));
    std::cout << "f:" << p.t << std::endl;
    Value_Func *v=(Value_Func *)heapAllocate(sizeof(Value_Func));
    
    static IndirectFunc EqualsFetcher={&_EqualsFetcher<size>,NULL}; 
    
    *v=&EqualsFetcher;
    p.Box=v;
    return p;
}

template <class C>
void BasicDot(void* out, void* args, void *context){
    std::cout << "BasicDot:" << args << ", " << ((Value_Type*)args) << ", " << ((std::string*)((char*)args+sizeof(Value_Type)))<< std::endl;
    Value_Type type=*((Value_Type*)args);
    std::string &name=*((std::string*)((char*)args+sizeof(Value_Type)));
    *((TypeValuePair*)(out))=_BasicDot<sizeof(C)>(type,name,GetValueType<C>());
}

template <class C>
Value_Type MakeValueType(){
    Value_Type Type_C = GetValueType<C>();
    static IndirectFunc DefaultDot={&BasicDot<C>,NULL};
    
    Type_C->Dot = &DefaultDot;
    return Type_C;
}

Value_Type Type_Type=MakeValueType<Value_Type>();
Value_Type Type_Bool=MakeValueType<bool>();
Value_Type Type_Double=MakeValueType<double>();
Value_Type Type_TypeInfo=MakeValueType<TypeInfo>();
Value_Type Type_String=MakeValueType<std::string>();
Value_Type Type_Int=MakeValueType<int>();

// Don't use this type to user code. If used as the ParamType for another type, that type is considered a function
// Causes Call to be special cased to avoid infinite recursion when trying to call something
TypeInfo _Type_FunctionType={sizeof(FunctionInfo),true,NULL,NULL,NULL,NULL};
Value_Type Type_FunctionType=&_Type_FunctionType;
// TypeInfo Type_ExampleFunction={sizeof(IndirectFunc *),false,NULL,NULL,Type_FunctionType,NULL};

}