#include "llvm/ADT/DenseMap.h"
#include "llvm/Aasan/Aasan.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
struct AasanIDAnalysisPass : public FunctionPass {
  static char ID;

  AasanIDAnalysisPass() : FunctionPass(ID), ValID(0) {}

  virtual bool runOnFunction(Function &F);

private:
  unsigned int ValID;
  DenseMap<unsigned int, Instruction *> IDToValue;

  void addValue(LLVMContext &Context, Instruction &I);
  Value *getValueByID(unsigned int ID);
  unsigned int getIDByValue(Value *V);
};
} // namespace

char AasanIDAnalysisPass::ID = 0;
static RegisterPass<AasanIDAnalysisPass>
    X("AasanIDAnalysisPass", "Alias Analysis Sanitizer ID analysis pass", true,
      true);

bool AasanIDAnalysisPass::runOnFunction(Function &F) {
  LLVMContext &Context = F.getContext();
  for (BasicBlock &B : F) {
    for (Instruction &I : B) {
      if (I.getType()->isPointerTy()) {
        addValue(Context, I);
      }
    }
  }
  return false;
}

void AasanIDAnalysisPass::addValue(LLVMContext &Context, Instruction &I) {
  int ID = this->ValID++;
  IDToValue[ID] = &I;
  MDNode *MD = MDNode::get(Context, ConstantAsMetadata::get(ConstantInt::get(
                                        Type::getInt64Ty(Context), ID)));
  I.setMetadata(StringRef("PointerID"), MD);
}

Value *AasanIDAnalysisPass::getValueByID(unsigned int ID) {
  if (IDToValue.find(ID) != IDToValue.end()) {
    return IDToValue[ID];
  }
  return nullptr;
}

FunctionPass *llvm::createAasanIDAnalysisPass() {
  return new AasanIDAnalysisPass();
}
