#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "basicTypes.h"

//===----------------------------------------------------------------------===//
// "Library" functions that can be "extern'd" from user code.
//===----------------------------------------------------------------------===//

namespace runtime
{


bool typeEquivalent(Value_Type a, Value_Type b);

void *heapAllocate(int size);

void *heapAllocateValue(Value_Type type);

// TODO : don't use a string for op (maybe p string?)
IndirectFunc *getOperatorFunc(const char *op, Value_Type ta, void *a, Value_Type tb, void *b, Value_Type *tOut);


void invokeFunc(Value_Type tf, IndirectFunc *f, void *out, void *args);

void invokeFuncWrapper(Value_Type tf, IndirectFunc *f, void *out,  ...);

Value_Type getFuncOutputType(Value_Type tf);

extern "C"
bool valuesEqual(Value_Type t, void *a, void *b);

void check(bool b);

//void assert(bool b);

Value_Type makeFunctionType(FunctionInfo &info);

ArgInfo makeArg(Value_Type t);
ArgInfo makeArg(ArgTypeSource t,int i);
bool isFunction(Value_Type tf);

void test();

}