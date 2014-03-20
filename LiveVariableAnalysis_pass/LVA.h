#ifndef LVA_H
#define LVA_H

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/ValueMap.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"

#include <map>

using namespace llvm;

namespace LLVMPractice {
  class LiveVariableAnalysis : public FunctionPass {
    public:
      static char ID;

      LiveVariableAnalysis() : FunctionPass(ID) 
      {
        ;
      }
      virtual bool doInitialization(Module &M);
      virtual bool runOnFunction(Function &F);
      virtual bool doFinalization(Module &M);

      struct BasicBlockLivenessInfo {
        BitVector *use;
        BitVector *def;
        BitVector *in;
        BitVector *out;
        BasicBlock *block;

        BasicBlockLivenessInfo(BasicBlock *block, unsigned defcount)
        {
          this->block = block;
          use = new BitVector(defcount);
          def = new BitVector(defcount);
          in = new BitVector(defcount);
          out = new BitVector(defcount);
        }
        
        ~BasicBlockLivenessInfo()
        {
          delete use;
          delete def;
          delete in;
          delete out;
        }
      };

      typedef ValueMap<Value*, unsigned> LatticeEncoding;
      LatticeEncoding instToLatticeBit;

      unsigned getLatticeBit(Value *i)
      {
        assert(instToLatticeBit.count(i));
        return instToLatticeBit[i];
      }

      typedef ValueMap<BasicBlock*, BasicBlockLivenessInfo*> BlockInfoMapping;
      BlockInfoMapping blockToInfo;

      typedef std::map<std::pair<BasicBlock*, BasicBlock*>, BitVector*> FlowMask;

    private:
      void initInstrToLatticeBit(Function &F);
      void initBlocksInfo(unsigned defcount, Function &F);
      void initBlockDef(BasicBlockLivenessInfo *blockInfo);
      void initEntryArgDef(BasicBlockLivenessInfo *blockInfo, unsigned argCount);
      void markUses(Value::use_iterator use_begin, Value::use_iterator use_end, Instruction *defInst, unsigned defBit);
      void initBlocksUse(Function &F);
      void initMask(FlowMask &mask, Function &F);
      void printBitVector(BitVector *bv);
      void printBitVectorDetailed(BitVector *bv);
      BitVector getMaskPhiValues(BasicBlock *phiBlock);

      Module *Mod;

  };
}

#endif
