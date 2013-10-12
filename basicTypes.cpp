#include <stddef.h>
#include <stdlib.h>

#include "basicTypes.h"

namespace runtime {




TypeInfo _Type_Type={sizeof(Value_Type),true,NULL,NULL,NULL,NULL};
TypeInfo _Type_Bool={sizeof(bool),true,NULL,NULL,NULL,NULL};
TypeInfo _Type_Double={sizeof(double),true,NULL,NULL,NULL,NULL};
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