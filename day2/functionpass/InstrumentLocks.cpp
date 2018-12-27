// PROBLEM: Extend the LLVM pass to instrument "pthread_mutex_unlock" calls. You
// should insert a call to your own analysis method that prints "Lock released:
// pthread_mutex_unlock" from your instrumentation function. Check that mutex
// lock and unlock calls are properly paired.

// INFO:

// opt -load ./functionpass/libFunctionPass.so -instrlocks ../progs/prog11.bc
// > prog11.bc

// clang prog11.bc -o prog11 -lpthread
// ./prog11

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <string>

using namespace llvm;

StringRef LockAcqName = "__lock_acq";

std::string mtx_lock("pthread_mutex_lock");

namespace {

struct InstrumentLocks : public FunctionPass {
  static char ID;

  InstrumentLocks() : FunctionPass(ID) {}

  StringRef getPassName() const override { return "instrumentLocks"; }

  bool runOnFunction(Function &F) override {
    // TODO: Extend this function to instrument "pthread_mutex_unlock"

    if (DEBUG) {
      errs() << "This is a function called: " << F.getName() << "\n"
             << "This is the function body: " << F << "\n";
    }

    Module &M = *F.getParent();

    for (auto &BB : F) {
      for (auto &I : BB) {
        if (isa<CallInst>(I)) { // Function call inst
          StringRef name = cast<CallInst>(I).getCalledFunction()->getName();
          if (DEBUG) {
            errs() << "Function being called: " << name << "\n";
          }

          if (name.str() == mtx_lock) {
            if (DEBUG) {
              errs() << "Mutex lock instruction: " << I << "\n";
            }

            Function *LockAcq = M.getFunction(LockAcqName);
            if (!LockAcq) {
              errs() << "Unknown function referenced\n";
            }

            // Insert call before I
            IRBuilder<> IRB(&I);
            CallInst *call =
                IRB.CreateCall(LockAcq, IRB.CreateGlobalStringPtr(name));
          }
        }
      }
    }

    return false; // Do not change this
  }

  // We don't modify the program, so we preserve all analyses
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }

private:
  Function *LockAcq;

  const bool DEBUG = false;
};

char InstrumentLocks::ID = 0;

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<InstrumentLocks> X("instrlocks",
                                       "Example Lock Analysis Pass",
                                       false /* true if only looks at CFG */,
                                       false /* true if analysis pass */);

} // namespace
