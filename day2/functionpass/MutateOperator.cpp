// cd ../progs; clang -o3 -emit-llvm -c prog2.c -o prog2.bc;
// clang prog2.bc -o prog2

// opt -load ./functionpass/libFunctionPass.so -mutateoperator ../progs/prog2.bc
// > ./prog2.bc
// clang ./prog2.bc - o prog2

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

struct MutateOperator : public FunctionPass {

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;

  MutateOperator() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *op = dyn_cast<BinaryOperator>(&I)) {
          // Insert at the point where the instruction `op` appears.
          IRBuilder<> builder(op);

          // Make a multiply with the same operands as `op`.
          Value *lhs = op->getOperand(0);
          Value *rhs = op->getOperand(1);
          Value *mul = builder.CreateMul(lhs, rhs);

          // Everywhere the old instruction was used as an operand, use our
          // new multiply instruction instead.
          for (auto &U : op->uses()) {
            User *user = U.getUser(); // A User is anything with operands.
            user->setOperand(U.getOperandNo(), mul);
          }

          // We modified the code.
          return true;
        }
      }
    }

    return false;
  }

  // We don't modify the program, so we preserve all analyses
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};

char MutateOperator::ID = 0;

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<MutateOperator> X("mutateoperator", "Mutate an operator",
                                      false /* true if only looks at CFG */,
                                      false /* true if analysis pass */);

} // namespace
