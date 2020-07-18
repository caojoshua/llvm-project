add_llvm_library( LLVMAliasAnalysisSanitizer
  AasanOfflineDetection.cpp
  AasanYaml.cpp
  AasanIDAnalysis.cpp

  DEPENDS
  LLVMAnalysis
  LLVMCore
  LLVMSupport
  LLVMObjectYAML

  LINK_LIBS
  LLVMAnalysis
  LLVMCore
  LLVMSupport
  LLVMObjectYAML
  )


