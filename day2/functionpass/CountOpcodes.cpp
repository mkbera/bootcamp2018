// PROBLEM: Write an LLVM pass to identify the different types of operands in
// the program and their counts.

// INFO:

// The following APIs might be useful to you.

// Iterate over basic blocks
// for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {}

// Get opcode name: i->getOpcodeName()

// Iterate over operands in an instruction
// for (User::op_iterator O = I.op_begin(), E = I.op_end(); O != E; ++O) {}

#define DEBUG_TYPE "opCounter" // This goes before any #includes

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

// NOTE: Your modifications should be limited to the CountOpCodes struct
struct CountOpCodes : public FunctionPass {

  static char ID;

  CountOpCodes() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    // TODO: Write your code here

    return false; // Do not change this
  }
};

char CountOpCodes::ID = 0;

static RegisterPass<CountOpCodes> X("opcodeCounter",
                                    "Counts opcodes per functions");

} // namespace
