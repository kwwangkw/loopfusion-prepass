add_llvm_library( LLVMHW2 MODULE
  LoopFusePrePass.cpp
  LoopFuse.cpp
  PLUGIN_TOOL
  opt
  )
