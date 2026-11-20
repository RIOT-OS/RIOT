// Workarounds for https://github.com/rust-lang/rust-bindgen/issues/1636
// (only needed when building for cortex using toolchain=llvm)
#undef UINT16_MAX
#undef UINT32_MAX
#define UINT16_MAX 0xffff
#define UINT32_MAX 0xffffffff

#include "riot-headers.h"
