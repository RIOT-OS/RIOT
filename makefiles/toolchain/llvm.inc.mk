export LLVMPREFIX ?= llvm-
# Apple XCode doesn't prefix its tools with llvm-, but manually installed LLVM
# on OSX might have the llvm- prefix, we can't simply test against uname -s.
# Test if llvm-ar exists
ifeq (,$(shell command -v $(LLVMPREFIX)ar 2>/dev/null))
# fall back to system tools
export LLVMPREFIX :=
endif
export CC          = clang
export CXX         = clang++
export CCAS       ?= $(CC)
export AS          = $(LLVMPREFIX)as
export AR          = $(LLVMPREFIX)ar
export NM          = $(LLVMPREFIX)nm
# LLVM does have a linker, however, it is not entirely
# compatible with GCC. For instance spec files as used in
# `makefiles/libc/newlib.mk` are not supported. Therefore
# we just use GCC for now.
export LINK        = $(PREFIX)gcc
export LINKXX      = $(PREFIX)g++
# objcopy does not have a clear substitute in LLVM, use GNU binutils
#export OBJCOPY     = $(LLVMPREFIX)objcopy
export OBJCOPY    ?= $(shell command -v $(PREFIX)objcopy || command -v gobjcopy || command -v objcopy)
ifeq ($(OBJCOPY),)
$(warning objcopy not found. Hex file will not be created.)
export OBJCOPY     = true
endif
export OBJDUMP     = $(LLVMPREFIX)objdump
export SIZE        = $(LLVMPREFIX)size
# LLVM lacks a binutils strip tool as well...
#export STRIP      = $(LLVMPREFIX)strip
# We use GDB for debugging for now, maybe LLDB will be supported in the future.
include $(RIOTMAKE)/tools/gdb.inc.mk

# Include directories from gcc
#   $1: language <c|cpp>
#
#   `realpath` is used instead of `abspath` to support Mingw32 which has issues
#   with windows formatted gcc directories
#
# CFLAGS_CPU is used to get the correct multilib include header.
gcc_include_dirs = $(realpath \
    $(shell $(PREFIX)gcc $(CFLAGS_CPU) -v -x $1 -E /dev/null 2>&1 | \
        sed \
        -e '1,/\#include <...> search starts here:/d' \
        -e '/End of search list./,$$d' \
        -e 's/^ *//')\
)

ifneq (,$(TARGET_ARCH))
  ifeq (,$(CFLAGS_CPU))
    $(error CFLAGS_CPU must have been defined to use `llvm`.)
  endif

  # Tell clang to cross compile
  CFLAGS     += -target $(TARGET_ARCH)
  CXXFLAGS   += -target $(TARGET_ARCH)
  # We currently don't use LLVM for linking (see comment above).
  # LINKFLAGS  += -target $(TARGET_ARCH)

  # Clang on Linux uses GCC's C and C++ headers and libstdc++ (installed with GCC)

  # Extract include directories from GCC
  GCC_C_INCLUDE_DIRS   = $(call gcc_include_dirs,c)
  GCC_CXX_INCLUDE_DIRS = $(call gcc_include_dirs,c++)

  GCC_C_INCLUDES   = $(addprefix -isystem ,$(GCC_C_INCLUDE_DIRS))
  GCC_CXX_INCLUDES = $(addprefix -isystem ,$(GCC_CXX_INCLUDE_DIRS))

  INCLUDES    += $(GCC_C_INCLUDES)
  CXXINCLUDES += $(GCC_CXX_INCLUDES)
endif
