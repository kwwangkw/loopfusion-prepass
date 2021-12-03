//===-- Frequent Path Loop Invariant Code Motion Pass ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass performs constraint loosening on bodies of code to make loop fusion
// possible even when code does not meet the 4 requirements of loop fusion.
// To do this, we are loosening the requirements of conforming bounds of loops
// as well as code control equivalency.
//
////===----------------------------------------------------------------------===//
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Transforms/Scalar/LoopFuse.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/CodeMoverUtils.h"
#include "llvm/Transforms/Utils/LoopPeel.h"
#include "llvm/IR/PassManager.h"

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <string>
#include "llvm/IR/Value.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringRef.h"

/* *******Implementation Starts Here******* */
// include necessary header files
/* *******Implementation Ends Here******* */

using namespace llvm;

namespace {

struct mypass: public FunctionPass {
    static char ID;
    mypass() : FunctionPass(ID) {}

    // Specify the list of analysis passes that will be used inside your pass.
//    void getAnalysisUsage(AnalysisUsage &AU) const {
//        AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
//        AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
//    }

    bool runOnFunction(Function &F) override{
      bool Changed = false;
      FunctionAnalysisManager FAM;
      llvm::LoopFusePass lfp;
      lfp.run(F, FAM);
      /* *******Implementation Starts Here******* */

      // Go through code and find all BBs

      // FIRST - Control Equivalency loosening
      // Iterate through the code and find all for loops
      // For each for loop:
      // 1) Check if the follow command is an if statement
      // 2) Check if the next command is another for loop header
      //    - check for no intervening code
      //    - check for dependencies
      // 3) If the two conditions above are true:
      // 4) Move the inner for loop out of the if statement, nest it's components in a new if statement

      // SECOND - Conforming Bounds loosening
      // TBD
      // opt -loop-fusion < input.bc > output.bc
      return Changed;
    }
};

//void getAnalysisUsage(AnalysisUsage &AU) const override {
//  AU.addRequired<BranchProbabilityInfoWrapperPass>();
//  AU.addRequired<BlockFrequencyInfoWrapperPass>();
//  AU.addRequired<LoopInfoWrapperPass>();
//}

} // end of namespace

char mypass::ID = 0;
static RegisterPass<mypass> X("myfusionpass", "583 project fusion pass", false, false);
