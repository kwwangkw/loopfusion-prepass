add_llvm_library( LLVMHW2 MODULE
  LoopFusePrePass.cpp

  PLUGIN_TOOL
  opt
  )
