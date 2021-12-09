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
    bool prepass(Function &F, LoopInfo &LI) {
        bool Changed = false;
        std::unordered_map<BasicBlock*, int> loopHeaders;
        
        // vec of all blocks in function
        std::vector<BasicBlock*> blocks;
        // map: for.end BB* -> pair<for.body BB*, loopDepth>
        std::unordered_map<BasicBlock*, std::pair<BasicBlock*, int>> loopEnds;

        // map: for.end BB* -> pair<for.body BB*, loopDepth>
        //     construct this map by iterating through loops and using getBlocks()
        // blocks vector (all blocks in function): 
        //     iterate through - if we see a for.body block, go two up to check if it's a if.then, if it's an if.then, go one up and see if it's an for.end
        //     if it's a for.end: search map for for.end key, and the value is the for.body block
        //     as a result, we now have for.body for two loops: we confirmed that there exists a for-if-for structure



        if (!LI.empty()) { // All for loops stored in LoopFuser LI, LDT
        for (auto CurrLoop : LI) { // For each for loop:
            loopHeaders[CurrLoop->getHeader()] = CurrLoop->getLoopDepth();
            errs() << "\n********************\nLOOP ENDS:\n";
            // this builds the loopEnds map
            for (auto block : CurrLoop->getBlocks()) {
            // note: getBlocks() does not contain the for.end block
            if (block->getName().find("for.inc") != -1) {
                for (auto it = succ_begin(block), end = succ_end(block); it != end; ++it) {
                BasicBlock* succblock = *it;
                // if the succ block to for.inc is not for.body (but instd the bb that leads to for.end)
                if (succblock->getName().find("for.body") == -1 && succblock->getSingleSuccessor() != nullptr) {
                    BasicBlock* endBlock = succblock->getSingleSuccessor();
                    errs() << "    this should be the end block: " << endBlock->getName() << '\n';
                    loopEnds[endBlock] = std::make_pair(CurrLoop->getHeader(),CurrLoop->getLoopDepth());
                }
                }
            }
            }
        } // for (auto CurrLoop : LI)
        errs() << "Loop Ends:\n";
        for (auto endBlock : loopEnds) { 
            errs() << endBlock.first->getName() << ", " << endBlock.second.first->getName() << "\n";
        }
        }    
        
        errs() << "\n********************\nALL BLOCKS IN FUNCTION:\n";
        for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
        BasicBlock &block_ref = *bb;
        BasicBlock *block = &block_ref;
        errs() << block->getName() <<"\n";
        blocks.push_back(block);
        }

        errs() << "\n********************\nFOR IF FOR RECOGNITION:\n";
        int counter = 0;
        bool hasForIfFor = false;
        std::vector<BasicBlock*> forIfFors;
        for (int i = 0; i < blocks.size(); i++) {
            // // we check predecessors 3 levels up, so it makes no sense to check pred until 3rd bb
            if (i > 2) {
                if (blocks[i]->getName().find("for.body") != -1 && 
                // blocks[i-1] is a block named like "for.body5.lr.ph" or "for.cond23.for.end29_crit_edge"
                blocks[i-2]->getName().find("if.then") != -1 && 
                blocks[i-3]->getName().find("for.end") != -1) {
                    hasForIfFor = true;
                    forIfFors.push_back(loopEnds[blocks[i-3]].first);

                    // Know that we can execute the swaps
                    
                    errs() << "     CAN SWAP\n";
                    errs() << "     if block:" << blocks[i-2]->getName() << "\n";
                    errs() << "     start of foriffor: " << loopEnds[blocks[i-3]].first->getName() << "\n\n";
                }
            }
        }
        
        errs() << "ForIfFors:\n";
        for(auto forBlock : forIfFors){
            errs() << "       " << forBlock->getName() << "\n";
        }

        return Changed;
    }

    bool runOnFunction(Function &F) override{
        bool Changed = false;
        FunctionAnalysisManager FAM;
        llvm::LoopFusePass lfp;
        lfp.run(F, FAM);
        /* *******Implementation Starts Here******* */
        auto &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    
        prepass(F, LI);
        
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
