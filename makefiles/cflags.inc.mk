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
OPTIONAL_CFLAGS += -fno-delete-null-pointer-checks

# Use colored compiler output if the compiler supports this and if this is not
# disabled by the user
ifneq ($(CC_NOCOLOR),1)
  OPTIONAL_CFLAGS += -fdiagnostics-color
endif

# Fast-out on old style function definitions.
# They cause unreadable error compiler errors on missing semicolons.
# Worse yet they hide errors by accepting wildcard argument types.
OPTIONAL_CFLAGS += -Wstrict-prototypes
OPTIONAL_CFLAGS += -Wold-style-definition

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

# Compress debug info. This saves approximately 50% of disk usage.
# It has no effect if debugging information is not emitted, so it can be left
# on unconditionally.
OPTIONAL_CFLAGS += -gz

# Enable all default warnings and all extra warnings
CFLAGS += -Wall -Wextra
# Enable additional checks for printf/scanf format strings
OPTIONAL_CFLAGS += -Wformat=2
OPTIONAL_CFLAGS += -Wformat-overflow
OPTIONAL_CFLAGS += -Wformat-truncation

# Warn if a user-supplied include directory does not exist.
CFLAGS += -Wmissing-include-dirs

ifeq (,$(filter -DDEVELHELP,$(CFLAGS)))
  ifneq (1,$(FORCE_ASSERTS))
    CFLAGS += -DNDEBUG
  endif
endif

# Add the optional flags that are not architecture/toolchain blacklisted
CFLAGS += $(filter-out $(OPTIONAL_CFLAGS_BLACKLIST),$(OPTIONAL_CFLAGS))

# Default ARFLAGS for platforms which do not specify it.
# Note: make by default provides ARFLAGS=rv which we want to override
ifeq ($(origin ARFLAGS),default)
  ARFLAGS = rcTs
endif
