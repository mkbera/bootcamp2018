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
StringRef FuncEntryName = "__func_entry";
StringRef FuncExitName = "__func_exit";

// std::string mtx_lock("pthread_mutex_lock");
// std::string mtx_unlock("pthread_mutex_unlock");
std::string skip_this("printf");


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

    if (DEBUG) {
      errs() << "This is a function called: " << F.getName() << "\n"
             << "This is the function body: " << F << "\n";
    }

    Module &M = *F.getParent();
    bool flag = false;

    bool main_start = true;
    bool main_end = false;

    StringRef prev_name;
    for (auto &BB : F) {
      for (auto &I : BB) {
        const char* the_main_man = "main";
std::string skip_this_2(the_main_man);
        if(F.getName().str() == skip_this_2) {
          if (main_start) {
            main_start = false;

            Function *FuncEntry = M.getFunction(FuncEntryName);
            if (!FuncEntry) {
              errs() << "Unknown function referenced\n";
            }

            // Insert call before I
            IRBuilder<> IRB(&I);
            CallInst *call =
                IRB.CreateCall(FuncEntry, IRB.CreateGlobalStringPtr(F.getName()));

          }

          // ReturnInst ri(&I);
          // if (ri.getReturnValue()) {
          //   Function *FuncExit = M.getFunction(FuncExitName);
          //   if (!FuncExit) {
          //     errs() << "Unknown function referenced\n";
          //   }

          //   // Insert call before I
          //   IRBuilder<> IRB(&I);
          //   CallInst *call =
          //       IRB.CreateCall(FuncExit, IRB.CreateGlobalStringPtr(F.getName()));            

          // }

          auto *op = (&I)->getOpcodeName();
          if ((std::string)op == "ret") {
            // errs() << "marks" << "\n";
              Function *FuncExit = M.getFunction(FuncExitName);
            if (!FuncExit) {
              errs() << "Unknown function referenced\n";
            }

            // Insert call before I
            IRBuilder<> IRB(&I);
            CallInst *call =
                IRB.CreateCall(FuncExit, IRB.CreateGlobalStringPtr(F.getName()));            

          }

        }



        if (flag == true) {
          flag = false;
          Function *FuncExit = M.getFunction(FuncExitName);
          if (!FuncExit) {
            errs() << "Unknown function referenced\n";
          }
          IRBuilder<> IRB(&I);
          CallInst *call =
              IRB.CreateCall(FuncExit, IRB.CreateGlobalStringPtr(prev_name));
        }
        if (isa<CallInst>(I)) { // Function call inst
          
          StringRef name = cast<CallInst>(I).getCalledFunction()->getName();
          prev_name = cast<CallInst>(I).getCalledFunction()->getName();
            // errs() << "Function being called: " << name << "\n";

          if (name.str() == skip_this) {
            if (DEBUG) {
              errs() << "Mutex lock instruction: " << I << "\n";
            }
            continue;
          }

          Function *FuncEntry = M.getFunction(FuncEntryName);
          if (!FuncEntry) {
            errs() << "Unknown function referenced\n";
          }

          // Insert call before I
          IRBuilder<> IRB(&I);
          CallInst *call =
              IRB.CreateCall(FuncEntry, IRB.CreateGlobalStringPtr(name));

          flag = true;


        }
      }
    }


  return true; // Do not change this
}

// Register our pass with the pass manager in opt. For more information, see:
// http://llvm.org/docs/WritingAnLLVMPass.html
static RegisterPass<InstrumentFunctions>
    X("instrfuncs", "Instrument function calls",
      false /* true if only looks at CFG */, false /* true if analysis pass */);
