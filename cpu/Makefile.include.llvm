export GDBPREFIX  ?= $(PREFIX)
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
export LINK        = $(CC)
export AS          = $(LLVMPREFIX)as
export AR          = $(LLVMPREFIX)ar
export NM          = $(LLVMPREFIX)nm
# There is no LLVM linker yet, use GNU binutils.
#export LINKER      = $(LLVMPREFIX)ld
# objcopy does not have a clear substitute in LLVM, use GNU binutils
#export OBJCOPY     = $(LLVMPREFIX)objcopy
export OBJCOPY     = $(PREFIX)objcopy
export OBJDUMP     = $(LLVMPREFIX)objdump
export SIZE        = $(LLVMPREFIX)size
export DBG         = $(GDBPREFIX)gdb
# LLVM lacks a binutils strip tool as well...
#export STRIP      = $(LLVMPREFIX)strip

ifneq (,$(TARGET_ARCH))
  # Clang on Linux uses GCC's C++ headers and libstdc++ (installed with GCC)
  # Ubuntu and Debian use /etc/alternatives/gcc-$(TARGET_ARCH)-include/c++/$(GCC_VERSION)
  # Arch uses /usr/$(TARGET_ARCH)/include/c++/$(GCC_VERSION)
  # Gentoo uses /usr/lib/gcc/$(TARGET_ARCH)/$(GCC_VERSION)/include/g++-v5
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

  # Tell clang to cross compile
  export CFLAGS     += -target $(TARGET_ARCH)
  export CXXFLAGS   += -target $(TARGET_ARCH)
  export LINKFLAGS  += -target $(TARGET_ARCH)

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

  # Some C headers (e.g. limits.h) are located with the GCC libraries
  GCC_C_INCLUDE_PATTERNS ?= \
    /usr/lib/gcc/$(TARGET_TRIPLE)/*/ \
    #

  GCC_C_INCLUDES ?= \
      $(addprefix -isystem ,$(wildcard $(addprefix \
          $(lastword $(sort \
              $(foreach pat, $(GCC_C_INCLUDE_PATTERNS), $(wildcard $(pat))))), \
          include include-fixed) \
      ))

  # If nothing was found we will try to fall back to searching for the libgcc used
  # by an installed cross-GCC and use its headers.
  ifeq (,$(GCC_C_INCLUDES))
    GCC_C_INCLUDES := $(addprefix -isystem ,$(wildcard $(addprefix $(dir $(shell $(PREFIX)gcc -print-libgcc-file-name)), include include-fixed)))
  endif

  export INCLUDES += $(GCC_C_INCLUDES)
endif
