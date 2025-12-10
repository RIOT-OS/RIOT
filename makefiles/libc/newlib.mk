ifneq (,$(filter newlib_nano,$(USEMODULE)))
  # Test if nano.specs is available
  ifeq ($(shell $(LINK) -specs=nano.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    USE_NEWLIB_NANO = 1
    ifeq ($(shell LC_ALL=C $(LINK) $(RIOTTOOLS)/testprogs/minimal_linkable.c -o /dev/null -lc -specs=nano.specs -Wall -Wextra -pedantic 2>&1 | grep -q "use of wchar_t values across objects may fail" ; echo $$?),0)
        CFLAGS += -fshort-wchar
        LINKFLAGS += -Wl,--no-wchar-size-warning
    endif
  endif
endif

ifneq (,$(filter newlib_gnu_source,$(USEMODULE)))
  CFLAGS += -D_GNU_SOURCE=1
endif

ifeq (1,$(USE_NEWLIB_NANO))
  LINKFLAGS += -specs=nano.specs
endif

LINKFLAGS += -lc

# Note on `realpath` vs `abspath`
#
# `realpath` is used instead of `abspath` to support Mingw32
# It fixes issues when using windows formatted paths like in the gcc output
# It does leave the trailing `/` on windows though


# Search for Newlib include directories

# Try to search for newlib in the standard search path of the compiler for includes
ifeq (,$(NEWLIB_INCLUDE_DIR))
  COMPILER_INCLUDE_PATHS := $(shell $(PREFIX)gcc -v -x c -E /dev/null 2>&1 | \
                              sed \
                              -e '1,/\#include <...> search starts here:/d' \
                              -e '/End of search list./,$$d' \
                              -e 's/^ *//')
  NEWLIB_INCLUDE_DIR := $(firstword $(realpath $(dir $(wildcard $(addsuffix /newlib.h, $(COMPILER_INCLUDE_PATHS))))))
endif

ifeq (,$(NEWLIB_INCLUDE_DIR))
  # Since Clang is not installed as a separate instance for each crossdev target
  # we need to tell it where to look for platform specific includes (Newlib
  # headers instead of Linux/Glibc headers.)
  # On GCC this is done when building the cross compiler toolchain so we do not
  # actually need to specify the include paths for system includes.
  # Ubuntu gcc-arm-embedded toolchain (https://launchpad.net/gcc-arm-embedded)
  # places newlib headers in several places, but the primary source seem to be
  # /etc/alternatives/gcc-arm-none-eabi-include
  # Gentoo Linux crossdev place the newlib headers in /usr/arm-none-eabi/include
  # Arch Linux also place the newlib headers in /usr/arm-none-eabi/include
  # Ubuntu seem to put a copy of the newlib headers in the same place as
  # Gentoo crossdev, but we prefer to look at /etc/alternatives first.
  # On OSX, newlib includes are possibly located in
  # /usr/local/opt/arm-none-eabi*/arm-none-eabi/include or /usr/local/opt/gcc-arm/arm-none-eabi/include
  NEWLIB_INCLUDE_PATTERNS ?= \
    /etc/alternatives/gcc-$(TARGET_ARCH)-include \
    /usr/$(TARGET_ARCH)/include \
    /usr/local/opt/$(TARGET_ARCH)*/$(TARGET_ARCH)/include \
    /usr/local/opt/gcc-*/$(TARGET_ARCH)/include \
    #
  # Use the wildcard Makefile function to search for existing directories matching
  # the patterns above. We use the -isystem gcc/clang argument to add the include
  # directories as system include directories, which means they will not be
  # searched until after all the project specific include directories (-I/path)
  NEWLIB_INCLUDE_DIR := $(firstword $(realpath $(dir $(wildcard $(addsuffix /newlib.h, $(NEWLIB_INCLUDE_PATTERNS))))))
endif

# If nothing was found we will try to fall back to searching for a cross-gcc in
# the current PATH and use a relative path for the includes
ifeq (,$(NEWLIB_INCLUDE_DIR))
  NEWLIB_INCLUDE_DIR := $(realpath $(wildcard $(dir $(shell command -v $(PREFIX)gcc 2>/dev/null))/../$(TARGET_ARCH)/include))
endif

ifeq ($(TOOLCHAIN),llvm)
  # A cross GCC already knows the target libc include path (build-in at compile time)
  # but Clang, when cross-compiling, needs to be told where to find the headers
  # for the system being built.
  # We also add -nostdinc to avoid including the host system headers by mistake
  # in case some header is missing from the cross tool chain
  NEWLIB_INCLUDES := -isystem $(NEWLIB_INCLUDE_DIR) -nostdinc
  NEWLIB_INCLUDES += $(addprefix -isystem ,$(realpath $(wildcard $(dir $(NEWLIB_INCLUDE_DIR))/usr/include)))

  # Newlib includes should go before GCC includes. This is especially important
  # when using Clang, because Clang will yield compilation errors on some GCC-
  # bundled headers. Clang compatible versions of those headers are already
  # provided by Newlib, so placing this directory first will eliminate those problems.
  # The above problem was observed with LLVM 3.9.1 when building against GCC 6.3.0 headers.
  INCLUDES := $(NEWLIB_INCLUDES) $(INCLUDES)
endif

ifeq (1,$(USE_NEWLIB_NANO))
  NEWLIB_NANO_INCLUDE_DIR ?= $(firstword $(wildcard $(NEWLIB_INCLUDE_DIR)/newlib-nano \
                                                    $(NEWLIB_INCLUDE_DIR)/newlib/nano \
                                                    $(NEWLIB_INCLUDE_DIR)/nano))

  ifneq (,$(NEWLIB_NANO_INCLUDE_DIR))
     # newlib-nano overrides newlib.h and its include dir should therefore go before
     # the regular system include dirs.
     INCLUDES := -isystem $(NEWLIB_NANO_INCLUDE_DIR) $(INCLUDES)
  endif
endif

# In case externally compiled code gets linked in, it may use newlib's assert
# implementation. We wrap that to RIOT's assert implementation for consistency.
LINKFLAGS += -Wl,-wrap=__assert_func
