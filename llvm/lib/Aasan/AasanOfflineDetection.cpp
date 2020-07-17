// TODO: this is just some skeleton code
#include "llvm/Aasan/Aasan.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"

#include <fstream>

using namespace llvm;

namespace {
struct AasanOfflineDetectionPass : public ModulePass {
  static char ID;

  AasanOfflineDetectionPass() : ModulePass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual bool runOnModule(Module &M);
  virtual bool doInitialization(Module &M);
  //virtual bool runOnFunction(Function &F);
};
} // namespace

char AasanOfflineDetectionPass::ID = 0;
static RegisterPass<AasanOfflineDetectionPass>
    X("AasanOfflineDetectionPass",
      "Alias Analysis Sanitizer Offline Detection Pass", true, true);

void AasanOfflineDetectionPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<AAResultsWrapperPass>();
}

bool AasanOfflineDetectionPass::doInitialization(Module &M) {
  std::string s;
  std::ifstream infile("/home/josh/src/sslab/link_test/with_aasan/aasan.log");
  errs() << "data\n";
  while (getline(infile, s)) {
    /* errs() << s << "\n"; */
  }
  return false;
}

bool AasanOfflineDetectionPass::runOnModule(Module &M) {
  /*
  AAResultsWrapperPass &AAwrapper = getAnalysis<AAResultsWrapperPass>();
  AAResults &AA = AAwrapper.getAAResults(); 
  */
  /* AAResultsWrapperPass &AAwrapper = getAnalysis<AAResultsWrapperPass>(); */
  for (Function &F : M) {
    errs() << F.getName() << "\n";
    for (BasicBlock &B : F) {
      for (Instruction &I1 : B) {
        /*
        for (Instruction &I2 : B) {
          if (&I1 != &I2) {
            if (AA.alias(&I1, &I2) == MayAlias) {
              errs() << "may alias found\n";
            }
          }
        }
        */
      }
    }
  }
  return false;
}

ModulePass *llvm::createAasanOfflineDetectionPass() {
  return new AasanOfflineDetectionPass();
}
