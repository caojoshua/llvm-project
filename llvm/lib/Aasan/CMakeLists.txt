add_llvm_library( LLVMAliasAnalysisSanitizer 
  AasanOfflineDetection.cpp
  AasanYaml.cpp
  AasanIDAnalysis.cpp

  PLUGIN_TOOL
  opt
  )
