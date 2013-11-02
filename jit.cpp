#include "jit.h"

#include "runtime/runtime.h"

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
using namespace llvm;


Value *ErrorV(const char *Str) { std::cout << Str << std::endl; return 0; }



/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" 
double printd(double X) {
  printf("%f\n", X);
  return 0;
}

namespace jit
{


Context::Context() : Builder(getGlobalContext()){
    InitializeNativeTarget();
    LLVMContext &Context = getGlobalContext();
    
    // Make the module, which holds all the code.
    TheModule = new Module("my cool jit", Context);

    // Create the JIT.  This takes ownership of the module.
    std::string ErrStr;
    TheExecutionEngine = EngineBuilder(TheModule).setErrorStr(&ErrStr).create();
    if (!TheExecutionEngine) {
        fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
        exit(1);
    }

    FunctionPassManager OurFPM(TheModule);

    // Set up the optimizer pipeline.  Start with registering info about how the
    // target lays out data structures.
    OurFPM.add(new DataLayout(*TheExecutionEngine->getDataLayout()));
    // Provide basic AliasAnalysis support for GVN.
    OurFPM.add(createBasicAliasAnalysisPass());
    // Promote allocas to registers.
    OurFPM.add(createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    OurFPM.add(createInstructionCombiningPass());
    // Reassociate expressions.
    OurFPM.add(createReassociatePass());
    // Eliminate Common SubExpressions.
    OurFPM.add(createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    OurFPM.add(createCFGSimplificationPass());

    OurFPM.doInitialization();

    // Set the global so the code gen can use this.
    TheFPM = &OurFPM;
}



Value *makeCall(Context &c) {
  // Look up the name in the global module table.
  Function *CalleeF = c.TheModule->getFunction("printd");
  if (CalleeF == 0)
    return ErrorV("Unknown function referenced");
  
  // If argument mismatch error.
  if (CalleeF->arg_size() != 1)
    return ErrorV("Incorrect # arguments passed");

  std::vector<Value*> ArgsV;
  for (unsigned i = 0, e = 1; i != e; ++i) {
    ArgsV.push_back(ConstantFP::get(getGlobalContext(), APFloat(42.0)));
    if (ArgsV.back() == 0) return 0;
  }
  
  return c.Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

Value *makeCall(std::string name, std::vector<Value*> args, Context &c) {
    // Look up the name in the global module table.
    Function *CalleeF = c.TheModule->getFunction(name);
    if (CalleeF == 0)
        return ErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != args.size())
        return ErrorV("Incorrect # arguments passed");

    return c.Builder.CreateCall(CalleeF, args, "calltmp");
}

Function *getF(Context &c){
  // Make the function type:  double(double,double) etc.
  std::vector<Type*> Doubles(1, 
                             Type::getDoubleTy(getGlobalContext()));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                       Doubles, false);
  
  std::string Name = "printd";
  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, c.TheModule);
  
  // If F conflicted, there was already something named 'Name'.  If it has a
  // body, don't allow redefinition or reextern.
  if (F->getName() != Name) {
    // Delete the one we just made and get the existing one.
    F->eraseFromParent();
    F = c.TheModule->getFunction(Name);
    
    // If F already has a body, reject this.
    if (!F->empty()) {
      //ErrorF("redefinition of function");
      return 0;
    }
    
    // If F took a different number of args, reject.
    if (F->arg_size() != 1) {
      //ErrorF("redefinition of function with different # args");
      return 0;
    }
  }
  
  // Set names for all arguments.
  unsigned Idx = 0;
  for (Function::arg_iterator AI = F->arg_begin(); Idx != 1;
       ++AI, ++Idx)
    AI->setName("X");
    
  return F;
}

/*
Function *MakeFunction(Value *Value) {
  NamedValues.clear();
  
  Function *TheFunction = Proto->Codegen();
  if (TheFunction == 0)
    return 0;
  
  // If this is an operator, install it.
  if (Proto->isBinaryOp())
    BinopPrecedence[Proto->getOperatorName()] = Proto->getBinaryPrecedence();
  
  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
  Builder.SetInsertPoint(BB);
  
  // Add all arguments to the symbol table and create their allocas.
  Proto->CreateArgumentAllocas(TheFunction);

  if (Value *RetVal = Body->Codegen()) {
    // Finish off the function.
    Builder.CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);

    // Optimize the function.
    TheFPM->run(*TheFunction);
    
    return TheFunction;
  }
  
  // Error reading body, remove function.
  TheFunction->eraseFromParent();

  if (Proto->isBinaryOp())
    BinopPrecedence.erase(Proto->getOperatorName());
  return 0;
}*/


void run(BlockASTNode *root) {
    std::cout << "jit test..." << std::endl;
    //root->Statements[0]->Codegen();
    
    Context c;

    // Run the main "interpreter loop" now.
    //MainLoop();
    Function *F = getF(c);
    Value *V = makeCall(c);
    
    
    // JIT the function, returning a function pointer.
    void *FPtr = c.TheExecutionEngine->getPointerToFunction(F);

    // Cast it to the right type (takes no arguments, returns a double) so we
    // can call it as a native function.
    double (*FP)(double) = (double (*)(double))(intptr_t)FPtr;
    fprintf(stderr, "Evaluated to %f\n", FP(33.0));

    

    c.TheFPM = 0;

    // Print out all of the generated code.
    c.TheModule->dump();
    
    std::cout << "jit test done." << std::endl;
}

}