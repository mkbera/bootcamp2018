// PROBLEM: Write an LLVM pass to instrument the entry and exit of every
// function in a translation unit. This exercise is similar to the
// InstrumentLocks.cpp example.

// INFO:

// Example: Suppose an example.c file contain the following snippet:
// int fun() { printf("I am fun()\n"); }
// int main() { fun(); }
// Your pass should instrument example.c such that we get the following output
// when example.c is compiled and executed.

// "Function entry: fun"
// I am fun()
// "Function exit: fun"

// The way to do this is to instrument before and after the calls to fun() at
// runtime.

// You can use the following instructions to test your pass:

// opt -load ./functionpass/libFunctionPass.so -mergereturn -instrfuncs
// ../progs/prog9.bc > prog9.bc
// clang prog9.bc -o prog9
// ./prog9

// opt -load ./functionpass/libFunctionPass.so -mergereturn -instrfuncs
// ../progs/prog10.bc > prog10.bc
// clang prog10.bc -o prog10
// ./prog10

#define DEBUG_TYPE "llvm-hacking" // This goes before any #includes

#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/EscapeEnumerator.h"

using namespace llvm;

namespace {

// NOTE: Your modifications should be limited to the InstrumentFunctions struct.
struct InstrumentFunctions : public FunctionPass {

  InstrumentFunctions() : FunctionPass(ID) {}
  StringRef getPassName() const override;
  bool runOnFunction(Function &F) override;

  static char ID; // Pass identification, replacement for typeid

private:
  const bool DEBUG = false;
};
} // namespace

char InstrumentFunctions::ID = 0;

StringRef InstrumentFunctions::getPassName() const {
  return "InstrumentFunctions";
}

bool InstrumentFunctions::runOnFunction(Function &F) {
  // TODO: Write your code here

  return true; // Do not change this
}

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<InstrumentFunctions>
    X("instrfuncs", "Instrument function calls",
      false /* true if only looks at CFG */, false /* true if analysis pass */);
