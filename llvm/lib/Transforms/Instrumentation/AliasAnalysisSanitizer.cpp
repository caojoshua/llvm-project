
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/TypeSize.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Instrumentation.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

#include <stdio.h>
#include <string>

using namespace llvm;

namespace {
struct AliasAnalysisSanitizerPass : public ModulePass {
  static char ID;

  static const std::string MemAllocHookName;
  static const std::string PointerDefHookName;
  static const std::string StoreHookName;

  Function *MemAllocHook;
  Function *PointerDefHook;
  Function *StoreHook;

  PointerType *PointerTy;
  Type *VoidTy;
  IntegerType *LongTy;

  AliasAnalysisSanitizerPass() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M);
  virtual bool doInitialization(Module &M);

  void instrumentStore(StoreInst *SI);
  void instrumentPointer(Module &M, BasicBlock::iterator &I);
  void instrumentAlloca(Module &M, AllocaInst *AI, BasicBlock::iterator &Loc);
};
} // namespace

char AliasAnalysisSanitizerPass::ID = 0;
static RegisterPass<AliasAnalysisSanitizerPass>
    X("AasanPass", "Alias Analysis Sanitizer Pass", false, false);

const std::string AliasAnalysisSanitizerPass::MemAllocHookName = "MemAllocHook";
const std::string AliasAnalysisSanitizerPass::PointerDefHookName =
    "PointerDefHook";
const std::string AliasAnalysisSanitizerPass::StoreHookName = "StoreHook";

bool AliasAnalysisSanitizerPass::doInitialization(Module &M) {
  LLVMContext &Context = M.getContext();

  PointerTy = PointerType::getUnqual(Type::getInt8Ty(Context));
  VoidTy = Type::getVoidTy(Context);
  LongTy = Type::getInt64Ty(Context);

  // MemAllocHook definition
  std::vector<Type *> Params;
  Params.push_back(PointerTy);
  Params.push_back(LongTy);
  FunctionType *MemAllocHookType = FunctionType::get(VoidTy, Params, false);
  MemAllocHook = dyn_cast<Function>(
      M.getOrInsertFunction(MemAllocHookName, MemAllocHookType));

  // PointerDefHook definition
  Params.clear();
  Params.push_back(PointerTy);
  Params.push_back(PointerTy);
  FunctionType *PointerDefHookType = FunctionType::get(VoidTy, Params, false);
  PointerDefHook = dyn_cast<Function>(
      M.getOrInsertFunction(PointerDefHookName, PointerDefHookType));

  // StoreHook definition
  FunctionType *StoreHookType = PointerDefHookType;
  StoreHook =
      dyn_cast<Function>(M.getOrInsertFunction(StoreHookName, StoreHookType));

  return true;
}

bool AliasAnalysisSanitizerPass::runOnModule(Module &M) {
  TargetLibraryInfoImpl TLII;
  TargetLibraryInfo TLI(TLII);

  for (Function &F : M) {
    if (F.getName().equals(MemAllocHookName) ||
        F.getName().equals(PointerDefHookName) ||
        F.getName().equals(StoreHookName)) {
      continue;
    }
    for (Function::iterator B = F.begin(), Fend = F.end(); B != Fend; ++B) {
      for (BasicBlock::iterator I = B->begin(), Bend = B->end(); I != Bend;
           ++I) {
        if (isAllocationFn(&*I, &TLI)) {
          // errs() << *I << "\n";
          // errs() << "isAllocationFn\n";
        }

        if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
          instrumentStore(SI);
        }

        if (I->getType()->isPointerTy()) {
          instrumentPointer(M, I);
        }
      }
    }
  }

  return true;
}

void AliasAnalysisSanitizerPass::instrumentStore(StoreInst *SI) {
  Value *ValueStored = SI->getValueOperand();
  if (ValueStored->getType()->isPointerTy()) {
    std::vector<Value *> Args;
    Args.push_back(new BitCastInst(ValueStored, PointerTy, "", SI));
    Args.push_back(new BitCastInst(SI->getPointerOperand(), PointerTy, "", SI));
    CallInst::Create(StoreHook, Args, "", SI);
  }
}

void AliasAnalysisSanitizerPass::instrumentPointer(Module &M,
                                                   BasicBlock::iterator &I) {
  BasicBlock::iterator Loc = I;
  // phis need to be grouped together, so we insert before the first
  // non-phi
  if (isa<PHINode>(I)) {
    do {
      ++Loc;
    } while (isa<PHINode>(I));
  }
  // insert instructions after the pointer definition
  else if (!I->isTerminator()) {
    ++Loc;
  } else if (isa<InvokeInst>(I)) {
    // InvokeInst is the only terminator that produce a value. We cannot
    // insert instructions after a terminator, because a terminator
    // needs to be at the end of a basic block. Probably need to create
    // a block right after the current block and insert the instructions
    // there.
    return;
  }

  if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
    instrumentAlloca(M, AI, Loc);
  }

  std::vector<Value *> Args;
  Args.push_back(new BitCastInst(&*I, PointerTy, "", &*Loc));
  if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    Args.push_back(
        new BitCastInst(LI->getPointerOperand(), PointerTy, "", &*Loc));
  } else {
    Args.push_back(ConstantPointerNull::get(PointerTy));
  }
  CallInst::Create(PointerDefHook, Args, "", &*Loc);
  I = Loc;
}

void AliasAnalysisSanitizerPass::instrumentAlloca(Module &M, AllocaInst *AI,
                                                  BasicBlock::iterator &Loc) {
  Type *type = AI->getType()->getElementType();
  ConstantInt *TypeSize =
      ConstantInt::get(LongTy, M.getDataLayout().getTypeAllocSize(type));
  Value *ArrSize = new ZExtInst(AI->getArraySize(), LongTy, "", &*Loc);
  Value *v =
      BinaryOperator::Create(Instruction::Mul, ArrSize, TypeSize, "", &*Loc);
  std::vector<Value *> Args;
  Args.push_back(new BitCastInst(&*AI, PointerTy, "", &*Loc));
  Args.push_back(v);
  CallInst::Create(MemAllocHook, Args, "", &*Loc);
}

ModulePass *llvm::createAliasAnalysisSanitizerPass() {
  return new AliasAnalysisSanitizerPass();
}
