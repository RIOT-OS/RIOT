# Test if the input language was specified externally.
# Otherwise test if the compiler unterstands the "-std=c99" flag, and use it if so.
ifeq ($(filter -std=%,$(CFLAGS)),)
  ifeq ($(shell $(CC) -std=c99 -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    CFLAGS += -std=c99
  endif
endif

# Add `-fno-delete-null-pointer-checks` flag iff the compiler supports it.
# GCC removes moves tests whether `x == NULL`, if previously `x` or even `x->y` was accessed.
# 0x0 might be a sane memory location for embedded systems, so the test must not be removed.
# Right now clang does not use the *delete-null-pointer* optimization, and does not understand the parameter.
# Related issues: #628, #664.
ifeq ($(shell $(CC) -fno-delete-null-pointer-checks -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
  ifeq ($(shell LANG=C $(CC) -fno-delete-null-pointer-checks -E - 2>&1 1>/dev/null </dev/null | grep warning: | grep -- -fno-delete-null-pointer-checks),)
    CFLAGS += -fno-delete-null-pointer-checks

    ifneq ($(shell $(CXX) -fno-delete-null-pointer-checks -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
      CXXUWFLAGS += -fno-delete-null-pointer-checks
    else
      ifneq ($(shell LANG=C $(CXX) -fno-delete-null-pointer-checks -E - 2>&1 1>/dev/null </dev/null | grep warning: | grep -- -fno-delete-null-pointer-checks),)
        CXXUWFLAGS += -fno-delete-null-pointer-checks
      endif
    endif
  endif
endif

# Template for testing a compiler flag and adding it to CFLAGS (errors usually
# happens when using older toolchains which do not support the given flags)
define cflags_test_and_add
  ifeq ($(shell $(CC) -Werror $(1) -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    CFLAGS += $(1)
  endif
endef
# Use colored compiler output if the compiler supports this and if this is not
# disabled by the user
ifeq ($(CC_NOCOLOR),0)
  $(eval $(call cflags_test_and_add,-fdiagnostics-color))
endif

# Fast-out on old style function definitions.
# They cause unreadable error compiler errors on missing semicolons.
# Worse yet they hide errors by accepting wildcard argument types.
$(foreach flag,-Wstrict-prototypes -Wold-style-definition,$(eval $(call cflags_test_and_add,$(flag))))

# Unwanted flags for c++
CXXUWFLAGS += -std=%
CXXUWFLAGS += -Wstrict-prototypes -Wold-style-definition

ifeq ($(LTO),1)
  $(warning Building with Link-Time-Optimizations is currently an experimental feature. Expect broken binaries.)
  LTOFLAGS = -flto
  LINKFLAGS += $(LTOFLAGS)
endif

# Forbid common symbols to prevent accidental aliasing.
CFLAGS += -fno-common

# Enable all default warnings and all extra warnings
CFLAGS += -Wall -Wextra
# Enable additional checks for printf/scanf format strings
$(foreach flag,-Wformat=2 -Wformat-overflow -Wformat-truncation,$(eval $(call cflags_test_and_add,$(flag))))

# Warn if a user-supplied include directory does not exist.
CFLAGS += -Wmissing-include-dirs

ifeq (,$(filter -DDEVELHELP,$(CFLAGS)))
  ifneq (1,$(FORCE_ASSERTS))
    CFLAGS += -DNDEBUG
  endif
endif

# Default ARFLAGS for platforms which do not specify it.
# Note: make by default provides ARFLAGS=rv which we want to override
ifeq ($(origin ARFLAGS),default)
  ARFLAGS = rcs
endif
