// TODO: this is just some skeleton code

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
struct AasanOfflineDetectionPass : public FunctionPass {
  static char ID;

  AasanOfflineDetectionPass() : FunctionPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  // virtual bool runOnModule(Module &M);
  virtual bool runOnFunction(Function &F);
};
} // namespace

char AasanOfflineDetectionPass::ID = 0;
static RegisterPass<AasanOfflineDetectionPass>
    X("AasanOfflineDetectionPass",
      "Alias Analysis Sanitizer Offline Detection Pass", true, true);

void AasanOfflineDetectionPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<AAResultsWrapperPass>();
  AU.addRequired<LoopInfoWrapperPass>();
}

/*bool AasanOfflineDetectionPass::runOnModule(Module &M) {
  AAResultsWrapperPass &AAwrapper = getAnalysis<AAResultsWrapperPass>();
  AAResults &AA = AAwrapper.getAAResults(); 

  return false;
}*/

bool AasanOfflineDetectionPass::runOnFunction(Function &F) {
  AAResultsWrapperPass &AAwrapper = getAnalysis<AAResultsWrapperPass>();
  AAResults &AA = AAwrapper.getAAResults(); 
  for (BasicBlock &B : F) {
    for (Instruction &I1 : B) {
      for (Instruction &I2 : B) {
        if (&I1 != &I2) {
          if (AA.alias(&I1, &I2) == MayAlias) {
            errs() << "may alias found\n";
          }
        }
      }
    }
  }
  return false;
}
