add_llvm_library( LLVMHW2 MODULE
  LoopFusePrePass.cpp
  LoopFuse583.cpp

  PLUGIN_TOOL
  opt
  )
