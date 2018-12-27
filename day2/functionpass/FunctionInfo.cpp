// opt -view-cfg ./prog8.bc > /dev/null
// opt -load ./functionpass/libFunctionPass.so -loopinfuncs ../progs/prog8.bc
// > /dev/null

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Instructions.h>

using namespace llvm;

namespace {

struct LoopsInFuncs : public FunctionPass {
  const bool DEBUG = false;
  static char ID; // Pass identification, replacement for typeid

  LoopsInFuncs() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  bool runOnFunction(Function &F) override {
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    errs() << "Function: " << F.getName() << "\n";
    for (Function::iterator b = F.begin(), be = F.end(); b != be; ++b) {
      if (DEBUG) {
        errs() << "BB : \t\t" << (*b) << "\n";
      }

      bool isLoop = LI.getLoopFor(&*b);
      if (isLoop) {
        errs() << "BB " << (*b).getName() << " is part of a loop\t";

        for (BasicBlock::iterator i = b->begin(), ie = b->end(); i != ie; ++i) {
          if (isa<CallInst>(&(*i)) || isa<InvokeInst>(&(*i))) {
            errs() << "Function "
                   << cast<CallInst>(&(*i))->getCalledFunction()->getName()
                   << " is part of the loop\t";
          }
        }
      }
      errs() << "\n";
    }
    errs() << '\n';
    return false;
  }
};

char LoopsInFuncs::ID = 0;

static RegisterPass<LoopsInFuncs> X("loopinfuncs", "Analyze loops", true, true);

} // namespace
