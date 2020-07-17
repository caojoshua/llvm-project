add_llvm_library( LLVMAliasAnalysisSanitizer 
  AasanOfflineDetection.cpp
  AasanYaml.cpp
  AasanIDAnalysis.cpp

  DEPENDS
  LLVMCore
  LLVMSupport
  LLVMObjectYAML

  LINK_LIBS
  LLVMCore
  LLVMSupport
  LLVMObjectYAML
  )


