#define DEBUG_TYPE "mxpa_lva"

#include "llvm/Support/Debug.h"

// MXPA
#include "LVA.h"

// std
#include <utility>

using namespace llvm;

namespace LLVMPractice {

  // 
  void LiveVariableAnalysis::initInstrToLatticeBit(Function &F) 
  {
    unsigned InstrIdx = 0;

    for (Function::arg_iterator I = F.arg_begin(), E = F.arg_end(); I != E; ++I)
    {
      instToLatticeBit.insert(std::make_pair(I, InstrIdx));
      InstrIdx++;
    }

    for (Function::iterator ib = F.begin(), eb = F.end(); ib != eb; ++ib)
    {
      BasicBlock *BB = ib;
      for (BasicBlock::iterator iI = BB->begin(), iE = BB->end(); iI != iE; ++iI) 
      {
        Instruction *I = iI;
        instToLatticeBit.insert(std::make_pair(I, InstrIdx));
        InstrIdx++;
      }
    }
  }
  
  void LiveVariableAnalysis::initBlocksInfo(unsigned defcount, Function &F)
  {
    for (Function::iterator ib = F.begin(), ie = F.end(); ib != ie; ++ib)
    {
      BasicBlock *BB = ib;
      struct BasicBlockLivenessInfo *BBLI = new BasicBlockLivenessInfo(BB, defcount);
      blockToInfo.insert(std::make_pair(ib, BBLI));
    }
  }
  
  void LiveVariableAnalysis::initBlockDef(BasicBlockLivenessInfo *blockInfo)
  {
    BasicBlock *BB = blockInfo->block;

    for (BasicBlock::iterator iI = BB->begin(), iE = BB->end(); iI != iE; ++iI)
    {
      blockInfo->def->set(getLatticeBit(iI));  
    }
  }
  
  void LiveVariableAnalysis::initEntryArgDef(BasicBlockLivenessInfo *blockInfo, unsigned argCount)
  {
    for (unsigned i = 0; i < argCount; ++i)
    {
      blockInfo->def->set(i);
    }
  }
  
  void LiveVariableAnalysis::markUses(Value::use_iterator use_begin, Value::use_iterator use_end, Instruction *defInst, unsigned defBit) 
  {
    for (Value::use_iterator u = use_begin, e = use_end; u != e; ++u)
    {
      Instruction *useInst = dyn_cast<Instruction>(*u);
      if (!useInst) continue;
      BasicBlock *useBlock = useInst->getParent();

      if (!defInst) {
        if (defInst->getParent() == useBlock && !isa<PHINode>(useInst)) {
          continue;
        }
      }

      struct BasicBlockLivenessInfo *useBBLI = blockToInfo[useBlock];
      useBBLI->use->set(getLatticeBit(defInst));
    }
  
  }
  
  void LiveVariableAnalysis::initBlocksUse(Function &F)
  {
    for (Function::iterator ib = F.begin(), ie = F.end(); ib != ie; ++ib)
    {
      BasicBlock *BB = ib;
      for (BasicBlock::iterator iI = BB->begin(), iE = BB->end(); iI != iE; ++iI)
      {
        Instruction *I = iI;
        markUses(I->use_begin(), I->use_end(), I, getLatticeBit(I));
      }
    }

    for (Function::arg_iterator ia = F.arg_begin(), ie = F.arg_end(); ia != ie; ++ia)
    {
      markUses(ia->use_begin(), ia->use_end(), NULL, getLatticeBit(ia));
    }
  }
  
  BitVector LiveVariableAnalysis::getMaskPhiValues(BasicBlock *phiBlock)
  {
    
  
  }
  
  void LiveVariableAnalysis::initMask(FlowMask &mask, Function &F)
  {
  
  }
  
  void LiveVariableAnalysis::printBitVector(BitVector *bv)
  {
  
  }
  
  void LiveVariableAnalysis::printBitVectorDetailed(BitVector *bv)
  {
  
  }
  
  static RegisterPass<LiveVariableAnalysis> X("mxpa_lva", "Live Variable Analysis");
  
  char LiveVariableAnalysis::ID = 0;
  
  bool LiveVariableAnalysis::doInitialization(Module &M) 
  {
  
  }

  bool LiveVariableAnalysis::runOnFunction(Function &F)
  {
    //1. Initialize instToLatticeBit
    instToLatticeBit.clear();
    initInstrToLatticeBit(F);
    unsigned defcount = instToLatticeBit.size();
  
    DEBUG(errs() << "total defcount is " << defcount << "\n");

    //2. Initialize blockToInfo
    for (BlockInfoMapping::iterator ib = blockToInfo.begin(), ie = blockToInfo.end(); ib != ie; ++ib) 
    {
      delete ib->second;
    }
    blockToInfo.clear();
    initBlocksInfo(defcount, F);

    //3. Initialize def and use sets
    initEntryArgDef(blockToInfo[F.begin()], F.arg_size());
    for (BlockInfoMapping::iterator BIMit = blockToInfo.begin(), BIMie = blockToInfo.end(); BIMit != BIMie; ++BIMit)
    {
      initBlockDef(BIMit->second);
    }
    initBlocksUse(F);


    
  }

  bool LiveVariableAnalysis::doFinalization(Module &M)
  {
  
  }
}
