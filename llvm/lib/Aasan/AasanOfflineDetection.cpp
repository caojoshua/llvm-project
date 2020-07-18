//===- AasanOfflineDetection.cpp - Offline Detection for Alias Analysis Sanitizer -===//
//
// This file defines the offline detection pass for Alias Analysis Sanitizer.
//
// Note: The pass must be a function pass because Alias Analyses in LLVM are function 
// passes and cannot be queried by a ModulePass
//
//===----------------------------------------------------------------------===//
//
#include "llvm/Aasan/Aasan.h"
#include "llvm/Aasan/AasanYaml.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"
#include "llvm/PassSupport.h"

#include <fstream>

using namespace llvm;

namespace {
struct AasanOfflineDetectionPass : public FunctionPass {
  static char ID;
  std::vector<AasanRecord *> LogRecords;

  AasanOfflineDetectionPass() : FunctionPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  /* virtual bool runOnModule(Function &M); */
  virtual bool doInitialization(Module &M);
  virtual bool runOnFunction(Function &F);
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
  std::ifstream infile("/home/josh/src/sslab/link_test/with_aasan/aasan.log");
  std::string str((std::istreambuf_iterator<char>(infile)),
                  std::istreambuf_iterator<char>());
  yaml::Input yin(str);
  yin >> LogRecords;
  return false;
}

bool AasanOfflineDetectionPass::runOnFunction(Function &F) {
  AAResultsWrapperPass *AAwrapper =
      getAnalysisIfAvailable<AAResultsWrapperPass>();
  if (AAwrapper) {
    AAResults &AA = AAwrapper->getAAResults();
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
  }
  return false;
}

FunctionPass *llvm::createAasanOfflineDetectionPass() {
  return new AasanOfflineDetectionPass();
}
