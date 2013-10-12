#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <assert.h>

#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IRBuilder.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/DataLayout.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/TargetSelect.h"


class TypedValue;
class TypedRef;


llvm::Value *PointerConstant(void *ptr);

// A helper class for using, and creating TypeInfo structs.
class TypeInfo {
public:
    TypeInfo(llvm::Value *typeInfoStruct);
    TypeInfo(llvm::Value *size, llvm::Function *Dot, llvm::Function *Call, llvm::Value unique);
    TypeInfo(TypedValue *size, TypedValue *Dot, TypedValue *Call, bool unique); // Handy helper
    
    // Gen code to write out the contents to a struct value
    llvm::Value *toStruct();
    
    // For comparing types for equality.
    // unique == use nominative type equality (types equal iff they point to the same TypeInfo instance)
    // not unique == use structural type equality (types equal iff the contents of the TypeInso are equal, see below)
    llvm::Value unique;
    
    llvm::Value *Size; // An int TODO: decide what kind of int
    
    llvm::Function *DotFunc; // Actual struct stores a pointer to this, not the function itself
    llvm::Function *CallFunc; // Actual struct stores a pointer to this, not the function itself
    

    // Paramaters:
    // These are paramaters that this Type is parameterized by.
    // Ex: all pointer classes are identical, except for the TypeParams[0] points to the corresponding type
    // The same goes for a C++ style vector.
    // A Map would have 2 entries (both types), a math style vector would have a size and a type.
    // Maybe this size+array should be replaced with a struct (and thus the type info for each entry can be stored elsewhere in the struct definition)
    // These values need to be accessible to (passed to) DotFunc and CallFunc, so pass them in as arg 1.
    TypedRef *Paramaters; // Some data structure that holds the paramaters, maybe a struct, maybe a instance of a dummy 0 sized type, it depends on the type.
    
    
    // Equality:
    // Equality for Types a and b is define as follows (first match)
    // 1. Equal if a and b point to the same TypeInfo
    // 2. Unequal if a.unique or b.unique
    // 3. Unequal if DotFunc,CallFunc, or Size differ
    // 4. Equal iff Paramaters are equal (Equal types, and equal values. For values that are Types, recurse)
    //      This means pointers to 2 equal types are equal, unless you set unique (as you would for a named pointer type)
    //      Also, 2 separately defined vectors of the same type are equal, etc.
    
    
    // Maybe some way to indicate mutability (constant, mutable, immutable)
    
};


// A TypedValue with a Type that is dynamic checked to be TypeInfo*
class TypeValue final{
public:
    TypeValue(llvm::Value *typePtr); // Make this a pointer to the passed info value
    
    // Generate runtime code checkes that src.Type is Type
    // Since this may fail, the code this represents may throw an error!
    TypeValue(TypedValue *src);
    
    // The pointer itself
    llvm::Value *Value;
};

// A value that has a Type (such as the result of an expression, a variable, etc)
class TypedValue{
public:
    virtual ~TypedValue();
    TypedValue(TypeValue *type);
    
    // The Type (Pointer to a TypeInfo). It is known to have Type Type, so it is Value instead of TypedValue
    TypeValue * const Type;
    
    // Get the actual data (not a pointer) for this value
    // If a pointer is needed, the value must be allocated somewhere!
    virtual llvm::Value * getValue() = 0;
    
    
    // Below are some helpers for generating code.
    
    // Used for "this.name".
    TypedValue *Dot(const std::string &name);
    // Used for "this(agrs...)".
    TypedValue *Call(const std::vector<TypedValue*>&);
    // Used for: "this[operand]", "this+operand", "this=operand" etc
    TypedValue *Op(std::string op, TypedValue *operand);  // TODO replace string with enum
};




// A TypedValue that is not written out to memory, and thus not mutable or addressable
class TypedValueRaw : public TypedValue {
public:
    TypedValueRaw(class TypeValue *type, llvm::Value *value);
    
    // get the value:
    // return Val
    llvm::Value * getValue();
    
    // The value itself
    llvm::Value * const Val;

};

// A TypedValue that is written out to memory, and thus is mutable and addressable
class TypedRef : public TypedValue{
public:
    TypedRef(TypeValue *type, llvm::Value *ptr);
    
    // deref Ptr:
    // return *Ptr
    llvm::Value * getValue();
    
    // store to Ptr:
    // assert EquivalentTypes(TypeValue,v->TypeValue)
    //*Ptr=v.getValue()
    llvm::Value * storeValue(TypedValue *v);
    
    // The value represented here is a pointer to data to this.Type
    //  This allows the value to have a static type (well, a void* basically), which at least has a fixed size, so we can pass it.
    llvm::Value * const Ptr;
};
