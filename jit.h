#pragma once

#include "ast.h"

#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/DataLayout.h"
#include "llvm/DerivedTypes.h"
#include "llvm/IRBuilder.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>


namespace jit
{

void run(BlockASTNode *root);

class Context{
public:
    llvm::Module *TheModule;
    llvm::IRBuilder<> Builder;
    std::map<std::string, llvm::AllocaInst*> NamedValues;
    llvm::FunctionPassManager *TheFPM;
    llvm::ExecutionEngine *TheExecutionEngine;
    Context();
};

}