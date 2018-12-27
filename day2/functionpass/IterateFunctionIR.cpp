// opt -load ./functionpass/libFunctionPass.so -iteratefuncir ../progs/prog2.bc
// > /dev/null

#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

struct IterateFuncIR : public FunctionPass {

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;

  IterateFuncIR() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    errs() << "This is a function called: " << F.getName()
           << "\n"
           // operator <<(ostream&,...) is overloaded
           << "This is the function body: " << F << "\n";

    for (auto &BB : F) {
      // Print out the name of the basic block if it has one, and then the
      // number of instructions that it contains
      errs() << "Basic block name=" << BB.getName() << " has " << BB.size()
             << " instructions.\n";

      errs() << "Basic block body:" << BB << "\n";

      for (auto &I : BB) {
        errs() << "Instruction: " << I << "\n";
      }
      errs() << "\n\n";
    }

    return false;
  }

  // We don't modify the program, so we preserve all analyses
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};

char IterateFuncIR::ID = 0;

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<IterateFuncIR> X("iteratefuncir",
                                     "Iterate over Function IR",
                                     false /* true if only looks at CFG */,
                                     false /* true if analysis pass */);

} // namespace
