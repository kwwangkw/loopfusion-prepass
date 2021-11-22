//===-- Frequent Path Loop Invariant Code Motion Pass ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// EECS583 F21 - This pass can be used as a template for your Frequent Path LICM
//               homework assignment. The pass gets registered as "fplicm".
//
// This pass performs loop invariant code motion, attempting to remove as much
// code from the body of a loop as possible.  It does this by either hoisting
// code into the preheader block, or by sinking code to the exit blocks if it is
// safe. 
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

#define DEBUG_TYPE "fplicm"


namespace Correctness{
struct FPLICMPass : public LoopPass {
  static char ID;
  FPLICMPass() : LoopPass(ID) {}

  bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    bool Changed = false;

    /* *******Implementation Starts Here******* */

    // Create objects for each analysis pass
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
    
    // find all bb's in loop
    std::unordered_set<BasicBlock*> all_loop_bbset;
    for (Loop::block_iterator bb = L->block_begin(), e = L->block_end(); bb != e; ++bb) {
        BasicBlock* block = *bb;
        all_loop_bbset.insert(block);
        errs() << block << " " << block->getName() << "\n";
    }

    // set of bb's in most freq path
    std::unordered_set<BasicBlock*> bbset;
    // list of bb's in most freq path
    std::vector<BasicBlock*> bblist;
    Loop::block_iterator bb = L->block_begin();
    BasicBlock* currblock = *bb;
    errs() << "\n";
    errs() << currblock << " " << currblock->getName() << "\n";

    // find most freq path until reach a bb that has already been in the path
    // ie: go through bb's until reach a backedge
    while (bbset.find(currblock) == bbset.end()) {
      bbset.insert(currblock);
      bblist.push_back(currblock);

      for (succ_iterator it = succ_begin(currblock), end = succ_end(currblock); it != end; ++it) {
        BasicBlock* succblock = *it;
        auto prob = bpi.getEdgeProbability(currblock, succblock);
        // the successor with >=80% branch prob is the most freq successor
        if (prob >= BranchProbability(4, 5)) {
          currblock = succblock;
          errs() << currblock << " "  << currblock->getName() << "\n";
          break;
        }
      }
    } // while

    errs() << "\n";

    std::vector<Instruction*> loads_to_hoist;

    // go through bb's in freq path
    for (int i = 0; i < bblist.size(); ++i) {
      BasicBlock* bb = bblist[i];
      // for each instruction in bb
      for (BasicBlock::iterator it = bb->begin(), e = bb->end(); it != e; ++it) {
        // if instr is a load, get its operand op
        Instruction* instr = &(*it);
        if (strcmp(instr->getOpcodeName(), "load") == 0) {
          errs() << "load: " << instr << " " << instr->getName() << " " << instr->getParent()->getName() << "\n";

          bool store_in_freq_path = false;
          bool store_in_infreq_path = false;

          User::op_iterator opit = instr->op_begin();
          Value* op = *opit;
          errs() << "    op: " << op->getName() << "\n";

          // find all uses of operand op, and look for stores
          for (Value::user_iterator user = op->user_begin(), ue = op->user_end(); user != ue; ++user) {
            Instruction* user_instr = dyn_cast<Instruction>(*user);
            errs() << "        " << user_instr->getOpcodeName() << ": " << user_instr->getParent() << " " << user_instr->getName() << " " << user_instr->getParent()->getName() << "\n";
            // is store in the freq path or the infreq path?
            if (strcmp(user_instr->getOpcodeName(), "store") == 0) {
              if (bbset.find(user_instr->getParent()) != bbset.end()) {
                store_in_freq_path = true;
                errs() << "            (freq path)\n";
              }
              else if (all_loop_bbset.find(user_instr->getParent()) != all_loop_bbset.end()) {
                store_in_infreq_path = true;
                errs() << "            (infreq path)\n";
              }
            }
          } // for user

          if (!store_in_freq_path && store_in_infreq_path) {
            loads_to_hoist.push_back(instr);
          }
        } // if load
      } // for instr
    } // for i

    errs() << "\n";
    errs() << "loads to hoist:\n";
    for (auto load : loads_to_hoist) {
      load->dump();
      errs() << "    " << load << " " << load->getParent()->getName() << "\n";
    }

    // map of load operand to its corresp allocated variable
    std::unordered_map<std::string, AllocaInst*> op_to_var_map;
    BasicBlock* preheader = L->getLoopPreheader();

    errs() << "\n";
    errs() << "loop preheader: " << preheader << " " << preheader->getName() << "\n";
    for (int i = 0; i < loads_to_hoist.size(); ++i) {
      // load is the ptr to load instruction
      // op is the ptr to load's operand
      Instruction* load = loads_to_hoist[i];
      User::op_iterator opit = load->op_begin();
      Value* op = *opit;
      std::string opname = (op->getName()).str();
      errs() << load << " " << load->getParent()->getName() << "\n" << "    operator: " << opname << "\n";

      AllocaInst* Val;
      // if there's already a freq load on the same opcode, reuse alloc variable
      if (op_to_var_map.find(opname) == op_to_var_map.end()) {
        Val = new AllocaInst(load->getType(), 0, "", preheader->getTerminator());
        op_to_var_map[opname] = Val;
      }
      else {
        Val = op_to_var_map[opname];
      }
      LoadInst* LD = new LoadInst(load->getType(), op, "", preheader->getTerminator());
      StoreInst* ST = new StoreInst(LD, Val, preheader->getTerminator());

      std::vector<Instruction*> stores;
      // find all uses of operand op in infreq stores
      for (Value::user_iterator user = op->user_begin(), ue = op->user_end(); user != ue; ++user) {
        Instruction* user_instr = dyn_cast<Instruction>(*user);
        if (strcmp(user_instr->getOpcodeName(), "store") == 0) { // store
          if (all_loop_bbset.find(user_instr->getParent()) != all_loop_bbset.end()) { // in loop
            if (bbset.find(user_instr->getParent()) == bbset.end()) { // infrequent

              stores.push_back(user_instr);

            }
          }
        }
      } // for user

      for (int j = 0; j < stores.size(); ++j) {
        Instruction* user_instr = stores[j];
        errs() << "        before: ";
        user_instr->dump();

        // fixup store's operand with alloc var
        user_instr->setOperand(1, Val);

        errs() << "        after:  ";
        user_instr->dump();
      }

      // replace freq load's operand with alloc var
      load->setOperand(0, Val);
      errs() << "    newload: ";
      load->dump();

    } // for i
    

    /* *******Implementation Ends Here******* */
    
    return Changed;
  }


  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<BranchProbabilityInfoWrapperPass>();
    AU.addRequired<BlockFrequencyInfoWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

private:
  /// Little predicate that returns true if the specified basic block is in
  /// a subloop of the current one, not the current one itself.
  bool inSubLoop(BasicBlock *BB, Loop *CurLoop, LoopInfo *LI) {
    assert(CurLoop->contains(BB) && "Only valid if BB is IN the loop");
    return LI->getLoopFor(BB) != CurLoop;
  }

};
} // end of namespace Correctness

char Correctness::FPLICMPass::ID = 0;
static RegisterPass<Correctness::FPLICMPass> X("fplicm-correctness", "Frequent Loop Invariant Code Motion for correctness test", false, false);


namespace Performance{
struct FPLICMPass : public LoopPass {
  static char ID;
  FPLICMPass() : LoopPass(ID) {}

  bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    bool Changed = false;

    /* *******Implementation Starts Here******* */
    

    /* *******Implementation Ends Here******* */
    
    return Changed;
  }


  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<BranchProbabilityInfoWrapperPass>();
    AU.addRequired<BlockFrequencyInfoWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

private:
  /// Little predicate that returns true if the specified basic block is in
  /// a subloop of the current one, not the current one itself.
  bool inSubLoop(BasicBlock *BB, Loop *CurLoop, LoopInfo *LI) {
    assert(CurLoop->contains(BB) && "Only valid if BB is IN the loop");
    return LI->getLoopFor(BB) != CurLoop;
  }

}; 
} // end of namespace Performance

char Performance::FPLICMPass::ID = 0;
static RegisterPass<Performance::FPLICMPass> Y("fplicm-performance", "Frequent Loop Invariant Code Motion for performance test", false, false);
