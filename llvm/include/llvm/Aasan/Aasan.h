#include "llvm/Pass.h"
#include "llvm/Transforms/Instrumentation.h"

namespace llvm {
FunctionPass *createAasanIDAnalysisPass();
}
