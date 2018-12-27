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
#include <map>

using namespace llvm;

namespace {

// NOTE: Your modifications should be limited to the CountOpCodes struct
struct CountOpCodes : public FunctionPass {

  static char ID;

  CountOpCodes() : FunctionPass(ID) {}

 bool runOnFunction(Function &F) override {
 	std::map<std::string, int> opcodeCounter;
 	std::map<std::string, int>::iterator itr;

    for (auto &BB : F) {
      for (auto &I : BB) {
      	auto *op = (&I)->getOpcodeName();
      	opcodeCounter[op] += 1;
        // if (auto *op = dyn_cast<BinaryOperator>(&I)) {
        //   // Insert at the point where the instruction `op` appears.
        //   IRBuilder<> builder(op);

        //   // Make a multiply with the same operands as `op`.
        //   Value *lhs = op->getOperand(0);
        //   Value *rhs = op->getOperand(1);
        //   Value *mul = builder.CreateMul(lhs, rhs);

        //   // Everywhere the old instruction was used as an operand, use our
        //   // new multiply instruction instead.
        //   for (auto &U : op->uses()) {
        //     User *user = U.getUser(); // A User is anything with operands.
        //     user->setOperand(U.getOperandNo(), mul);
        //   }

        //   // We modified the code.
        //   return true;
        // }
      }
    }

    errs() << "Function: " << F.getName() << "\n";
    for(itr = opcodeCounter.begin(); itr != opcodeCounter.end(); ++itr) {
    	errs() << itr->first << ": " << itr->second << "\n";
    }
    errs() << "\n";
    return false;
  }

};

char CountOpCodes::ID = 0;

static RegisterPass<CountOpCodes> X("opcodeCounter",
                                    "Counts opcodes per functions");

} // namespace
