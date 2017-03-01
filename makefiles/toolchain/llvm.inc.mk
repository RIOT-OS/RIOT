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
export LINK        = $(CC)
export AS          = $(LLVMPREFIX)as
export AR          = $(LLVMPREFIX)ar
export NM          = $(LLVMPREFIX)nm
# There is no LLVM linker yet, use GNU binutils.
#export LINKER      = $(LLVMPREFIX)ld
# objcopy does not have a clear substitute in LLVM, use GNU binutils
#export OBJCOPY     = $(LLVMPREFIX)objcopy
export OBJCOPY    ?= $(shell command -v $(PREFIX)objcopy gobjcopy objcopy | head -n 1)
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

ifneq (,$(TARGET_ARCH))
  # Clang on Linux uses GCC's C++ headers and libstdc++ (installed with GCC)
  # Ubuntu and Debian use /etc/alternatives/gcc-$(TARGET_ARCH)-include/c++/$(GCC_VERSION)
  # Arch uses /usr/$(TARGET_ARCH)/include/c++/$(GCC_VERSION)
  # Gentoo uses /usr/lib/gcc/$(TARGET_ARCH)/$(GCC_VERSION)/include/g++-v5 (or
  # other number depending on GCC version)
  GCC_CXX_INCLUDE_PATTERNS ?= \
  /etc/alternatives/gcc-$(TARGET_ARCH)-include/c++/*/ \
  /usr/$(TARGET_ARCH)/include/c++/*/ \
  /usr/lib/gcc/$(TARGET_ARCH)/*/include/g++-v8 \
  /usr/lib/gcc/$(TARGET_ARCH)/*/include/g++-v7 \
  /usr/lib/gcc/$(TARGET_ARCH)/*/include/g++-v6 \
  /usr/lib/gcc/$(TARGET_ARCH)/*/include/g++-v5 \
  #

  # Try to find the proper multilib directory using GCC, this may fail if a cross-
  # GCC is not installed.
  ifeq ($(GCC_MULTI_DIR),)
    GCC_MULTI_DIR := $(shell $(PREFIX)gcc $(CFLAGS) -print-multi-directory 2>/dev/null)
  endif

  # Try to find the sysroot of the cross-GCC
  ifeq ($(GCC_SYSROOT),)
    GCC_SYSROOT := $(shell $(PREFIX)gcc $(CFLAGS) -print-sysroot 2>/dev/null)
  endif

  ifeq ($(GCC_SYSROOT),)
    # Fall back to a sensible default
    GCC_SYSROOT := /usr/$(TARGET_ARCH)
  endif

  # Add the sysroot top level include dir to include path (--sysroot only adds
  # [root]/usr/include, we need [root]/include as well)
  export INCLUDES += $(addprefix -isystem ,$(abspath $(wildcard $(GCC_SYSROOT)/include)))

  # Tell clang to cross compile
  # Adding --sysroot removes all default system include directories (/usr/include,
  # /usr/local/include), just like -nostdinc does, but it leaves the compiler
  # specific include paths intact (e.g. /usr/lib/clang/3.9.1/include). This gets
  # rid of the GCC dependency for C code, we only need the GCC path for its C++
  # headers when building for libstdc++.
  # Adding a non-existent directory as sysroot is not an error in tested versions
  # of Clang (3.8.0, 3.9.1), so we always add --sysroot for removing the system
  # include paths.
  export CFLAGS     += -target $(TARGET_ARCH) --sysroot $(GCC_SYSROOT)
  export CXXFLAGS   += -target $(TARGET_ARCH) --sysroot $(GCC_SYSROOT)
  export LINKFLAGS  += -target $(TARGET_ARCH) --sysroot $(GCC_SYSROOT)

  # Use the wildcard Makefile function to search for existing directories matching
  # the patterns above. We use the -isystem gcc/clang argument to add the include
  # directories as system include directories, which means they will not be
  # searched until after all the project specific include directories (-I/path)
  # We sort the list of found directories and take the last one, it will likely be
  # the most recent GCC version. This avoids using old headers left over from
  # previous tool chain installations.
  GCC_CXX_INCLUDES ?= \
      $(addprefix \
          -isystem $(firstword \
              $(foreach pat, $(GCC_CXX_INCLUDE_PATTERNS), $(lastword $(sort $(wildcard $(pat)))))), \
          /. /$(TARGET_ARCH)/$(GCC_MULTI_DIR) /backward \
      )

  # If nothing was found we will try to fall back to searching for a cross-gcc in
  # the current PATH and use a relative path for the includes
  ifeq (,$(GCC_CXX_INCLUDES))
    GCC_CXX_INCLUDES := $(addprefix -isystem ,$(wildcard $(dir $(shell which $(PREFIX)gcc))../$(TARGET_TRIPLE)/include))
  endif

  # Pass the includes to the C++ compilation rule in Makefile.base
  export CXXINCLUDES += $(GCC_CXX_INCLUDES)
endif
