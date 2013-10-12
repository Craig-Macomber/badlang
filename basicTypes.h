#pragma once


#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace runtime
{

// For now, all functions will use the this signature.
// This isn't a great calling convention, but its really easy.
typedef void (*_Func)(void* out, void* args, void *context);

// For now, all function pointers are indirected through one of these
typedef struct {
    _Func Func; // The function pointer
    void *Context; // The context to be provided to *func when calling it
} IndirectFunc, *Value_Func;


typedef struct TypeInfo{
    // The size (in bytes) an instance of this type takes up
    int Size;
    
    // For comparing types for equality.
    // unique == use nominative type equality (types equal iff they point to the same TypeInfo instance)
    // not unique == use structural type equality (types equal iff the contents of the TypeInso are equal, see below)
    bool Unique;
    
    // Used to implement "." operator on instances
    // myTypeInstance.someName
    // MyType.Dot(MyType,"someName") (Type T, T f)
    // Where T should be a function (MyType this) (someType)
    // which will get invoked at the call site.
    // The fact that the type itself is passed to Dot (and Call)
    // allows recursive types.
    IndirectFunc * Dot;
 
    // Used to make callable objects.
    // myTypeInstance(someInt, someBool)
    // MyType.Call(MyType,2,&[int,bool])
    // Which should return a func(MyType,int,bool) Type and instance,
    // which will get invoked at the call site.
    IndirectFunc * Call;// func(This Type, count int, argTypes *Type) (T Type, value *T)
 
    // Data that may be useful to Call and Dot, such as Type paramaters
    struct TypeInfo *ParamType;
    void *Param;
} TypeInfo, *Value_Type;

enum ArgTypeSource {
    ats_direct = 0,
    ats_param = 1,
    ats_returnValue = 2
};

typedef struct ArgInfo{
    ArgTypeSource srcType;
    union {
        int paramIndex;
        Value_Type type;
    };
} ArgInfo;

typedef struct FunctionInfo{
    FunctionInfo(std::vector<ArgInfo> argTypes,std::vector<ArgInfo> returnTypes);
    const std::vector<ArgInfo> argTypes;
    const std::vector<ArgInfo> returnTypes;
} FunctionInfo;
 

extern Value_Type Type_Type;
extern Value_Type Type_Bool;
extern Value_Type Type_Double;
extern Value_Type Type_TypeInfo;
extern Value_Type Type_String;
extern Value_Type Type_Int;

// Don't use this type to user code. If used as the ParamType for another type, that type is considered a function
// Causes Call to be special cased to avoid infinite recursion when trying to call something
extern Value_Type Type_FunctionType;
// TypeInfo Type_ExampleFunction={sizeof(IndirectFunc *),false,NULL,NULL,Type_FunctionType,NULL};

}