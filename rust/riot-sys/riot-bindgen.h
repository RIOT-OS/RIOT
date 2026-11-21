#ifdef RIOT_SYS_BINDGEN
// Workarounds for https://github.com/rust-lang/rust-bindgen/issues/1636
// (only needed when building for cortex using toolchain=llvm)
//
// Only applied while bindgen parses the headers, not when the generated static
// function wrappers are built.
#undef UINT16_MAX
#undef UINT32_MAX
#define UINT16_MAX 0xffff
#define UINT32_MAX 0xffffffff
#endif

#include "riot-headers.h"
