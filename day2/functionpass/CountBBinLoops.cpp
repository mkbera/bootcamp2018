// PROBLEM: Write an LLVM pass to find the number of loops in a function, and
// the number of basic blocks in each loop.

// INFO:

// Visualize the CFG of functions. That allows you to see the number of
// basic blocks, loops, etc.

// clang -O0 -emit-llvm ./prog8.c -c -o prog8.bc
// clang prog8.bc -o prog8
// opt -view-cfg ./prog8.bc > /dev/null

// The following APIs might be useful to you.

// A loop consists of basic blocks (review the CFG of a function
// containing a for loop)
// for (Loop::block_iterator bb = L->block_begin(); bb != L->block_end(); ++bb)
// {}

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct CountBBinLoops : public FunctionPass {

  static char ID;

  CountBBinLoops() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.setPreservesAll();
  }

  virtual bool runOnFunction(Function &F) {
    // TODO: Write your code here. Extend the skeleton code to get the desired
    // result.
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    int loopCounter = 0;
    for (LoopInfo::iterator i = LI.begin(), e = LI.end(); i != e; i++) {
      Loop *L = *i; // This gives you a loop handle (e.g., for loop)
    }

    return false; // Do not change this
  }
};

char CountBBinLoops::ID = 0;

static RegisterPass<CountBBinLoops>
    X("countbbinloop", "Count the number of BBs inside each loop");

} // namespace
