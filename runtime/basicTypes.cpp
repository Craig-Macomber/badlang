#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include "runtime.h"

namespace runtime {

bool _BasicCompare(double a, double b){
    std::cout << "double compare:" << a << ", " << b << std::endl;
    return a==b;
}

void BasicCompare(void* out, void* args, void *context){
    *((bool*)(out))=_BasicCompare(((double*)args)[0],((double*)args)[1]);
}
IndirectFunc DefaultCompare={&BasicCompare,NULL};

FunctionInfo _makeCompareInfo(){
    std::vector<ArgInfo> infoArgs;
    infoArgs.push_back(makeArg(Type_Double));
    infoArgs.push_back(makeArg(Type_Double));
    std::vector<ArgInfo> returnArgs;
    returnArgs.push_back(makeArg(Type_Bool));
    return FunctionInfo(infoArgs, returnArgs);
}

FunctionInfo compareInfo = _makeCompareInfo();
const Value_Type compareType = makeFunctionType(compareInfo);

FunctionInfo _makeAtributeFetcherInfo(Value_Type type, Value_Type returnType){
    std::vector<ArgInfo> infoArgs;
    infoArgs.push_back(makeArg(type));
    std::vector<ArgInfo> returnArgs;
    returnArgs.push_back(makeArg(returnType));
    return FunctionInfo(infoArgs, returnArgs);
}
FunctionInfo equalsFetcherInfo = _makeAtributeFetcherInfo(Type_Double, compareType);
const Value_Type equalsFetcherType = makeFunctionType(equalsFetcherInfo);

void _DoubleEqualsFetcher(void* out, void* args, void *context){
    *(Value_Func*)out=&DefaultCompare;
}
IndirectFunc DoubleEqualsFetcher={&_DoubleEqualsFetcher,NULL}; 

TypeValuePair _BasicDot(Value_Type type, std::string &name, void* context){
    std::cout << "Dot:" << name << std::endl;
    assert(name == "==");
    TypeValuePair p;
    p.t=equalsFetcherType;
    check(isFunction(p.t));
    std::cout << "f:" << p.t << std::endl;
    Value_Func *v=(Value_Func *)heapAllocate(sizeof(Value_Func));
    *v=&DoubleEqualsFetcher;
    p.Box=v;
    return p;
}



void BasicDot(void* out, void* args, void *context){
    std::cout << "BasicDot:" << args << ", " << ((Value_Type*)args) << ", " << ((std::string*)((char*)args+sizeof(Value_Type)))<< std::endl;
    Value_Type type=*((Value_Type*)args);
    std::string &name=*((std::string*)((char*)args+sizeof(Value_Type)));
    *((TypeValuePair*)(out))=_BasicDot(type,name,context);
}
IndirectFunc DefaultDot={&BasicDot,NULL};


TypeInfo _Type_Type={sizeof(Value_Type),true,NULL,NULL,NULL,NULL};
TypeInfo _Type_Bool={sizeof(bool),true,NULL,NULL,NULL,NULL};
TypeInfo _Type_Double={sizeof(double),true,&DefaultDot,NULL,NULL,NULL};
TypeInfo _Type_TypeInfo={sizeof(TypeInfo),true,NULL,NULL,NULL,NULL};
TypeInfo _Type_String={sizeof(std::string),true,NULL,NULL,NULL,NULL};
TypeInfo _Type_Int={sizeof(int),true,NULL,NULL,NULL,NULL};

Value_Type Type_Type=&_Type_Type;
Value_Type Type_Bool=&_Type_Bool;
Value_Type Type_Double=&_Type_Double;
Value_Type Type_TypeInfo=&_Type_TypeInfo;
Value_Type Type_String=&_Type_String;
Value_Type Type_Int=&_Type_Int;

// Don't use this type to user code. If used as the ParamType for another type, that type is considered a function
// Causes Call to be special cased to avoid infinite recursion when trying to call something
TypeInfo _Type_FunctionType={sizeof(FunctionInfo),true,NULL,NULL,NULL,NULL};
Value_Type Type_FunctionType=&_Type_FunctionType;
// TypeInfo Type_ExampleFunction={sizeof(IndirectFunc *),false,NULL,NULL,Type_FunctionType,NULL};

}