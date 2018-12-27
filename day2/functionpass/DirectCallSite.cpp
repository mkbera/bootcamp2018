// opt -load ./functionpass/libFunctionPass.so -directcallsite ../progs/prog3.bc
// > /dev/null

#include "llvm/IR/CallSite.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

struct DirectCallSite : public FunctionPass {

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;

  DirectCallSite() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    for (BasicBlock &bb : F) {
      for (Instruction &i : bb) {
        // Find the call site in our instruction (if there is one)
        CallSite cs(&i);
        // Skip if the instruction is not a function call
        if (!cs.getInstruction()) {
          continue;
        }

        Value *called = cs.getCalledValue()->stripPointerCasts();
        if (Function *f = dyn_cast<Function>(called)) {
          errs() << "Direct call to function: " << f->getName()
                 << " from function: " << F.getName() << "\n";
        }
      }
    }
    return false;
  }
};

char DirectCallSite::ID = 0;

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<DirectCallSite> X("directcallsite",
                                      "Find direct call sites (callees)",
                                      false /* true if only looks at CFG */,
                                      false /* true if analysis pass */);

} // namespace
