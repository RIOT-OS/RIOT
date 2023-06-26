# Test if the input language was specified externally.
# Otherwise test if the compiler unterstands the "-std=c11" flag, and use it if so.
ifeq ($(filter -std=%,$(CFLAGS)),)
  ifeq ($(shell $(CC) -std=c11 -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    CFLAGS += -std=c11
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

# Force the C compiler to not ignore signed integer overflows
# Background:   In practise signed integers overflow consistently and wrap
#               around to the lowest number. But this is undefined behaviour.
#               Branches that rely on this undefined behaviour will be silently
#               optimized out. For details, have a look at
#               https://gcc.gnu.org/bugzilla/show_bug.cgi?id=30475
# Note:         Please do not add new code that relies on this undefined
#               behaviour, even though this flag makes your code work. There are
#               safe ways to check for signed integer overflow.
CFLAGS += -fwrapv
# Enable warnings for code relying on signed integers to overflow correctly
# (see above for details).
# Note:         This warning is sadly not reliable, thus -fwrapv cannot be
#               dropped in favor of this
CFLAGS += -Wstrict-overflow

# Fast-out on old style function definitions.
# They cause unreadable error compiler errors on missing semicolons.
# Worse yet they hide errors by accepting wildcard argument types.
OPTIONAL_CFLAGS += -Wstrict-prototypes
OPTIONAL_CFLAGS += -Wold-style-definition

# Unwanted flags for c++
CXXUWFLAGS += -std=%
CXXUWFLAGS += -Wstrict-prototypes -Wold-style-definition

ifeq ($(filter -std=%,$(CXXEXFLAGS)),)
  ifeq ($(shell $(CC) -std=c++14 -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    CXXEXFLAGS += -std=c++14
  endif
endif

ifeq ($(LTO),1)
  $(warning Building with Link-Time-Optimizations is currently an experimental feature. Expect broken binaries.)
  LTOFLAGS = -flto
  LINKFLAGS += $(LTOFLAGS) -ffunction-sections -fdata-sections
endif

# Cannot test with `BUILD_IN_DOCKER=1`, as this is only the case when the
# actual build is done in the docker container and we are still running in the
# host's context.
ifeq (1,$(BUILD_IN_DOCKER))
  LINKER_SUPPORTS_NOEXECSTACK := determine-later-inside-docker
endif

# Check if linker supports `-z noexecstack`. Handle BUILD_IN_DOCKER separately,
# as this is run in the host environment rather than inside the container. We
# just hardcode this in the BUILD_IN_DOCKER case for now.
LINKER_SUPPORTS_NOEXECSTACK ?= $(shell LC_ALL=C $(LINK) $(RIOTTOOLS)/testprogs/minimal_linkable.c -o /dev/null -lc -Wall -Wextra -pedantic -z noexecstack 2> /dev/null && echo 1 || echo 0)

# As we do not use nested functions or other stuff requiring trampoline code,
# we can safely mark the stack as not executable. This avoids warnings on newer
# toolchains.
ifeq (1,$(LINKER_SUPPORTS_NOEXECSTACK))
  LINKFLAGS += -z noexecstack
endif

# Forbid common symbols to prevent accidental aliasing.
CFLAGS += -fno-common

# Place data and functions into their own sections. This helps the linker
# garbage collection to remove unused symbols when linking statically.
CFLAGS += -ffunction-sections -fdata-sections

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

# Don't include absolute path in __FILE__ macro
OPTIONAL_CFLAGS += -fmacro-prefix-map=$(RIOTBASE)/=

# Warn about casts that increase alignment requirements
OPTIONAL_CFLAGS += -Wcast-align

# Warn if a user-supplied include directory does not exist.
CFLAGS += -Wmissing-include-dirs

ifeq (,$(filter -DDEVELHELP,$(CFLAGS)))
  ifneq (1,$(FORCE_ASSERTS))
    CFLAGS += -DNDEBUG
  endif
endif
# Add the optional flags that are not architecture/toolchain blacklisted
CFLAGS += $(filter-out $(OPTIONAL_CFLAGS_BLACKLIST),$(OPTIONAL_CFLAGS))

# Improve C++ compatibility with our C headers: In C it is both valid and good
# practise to implicitly initialize struct members with zero by omitting them
# in a initializer list. The C++ compiler greatly frowns upon this, even within
# `extern "C" { ... }`. The best would be to configure the C++ compiler to
# accept good C practises within `extern "C" { ... }` while enforcing good C++
# practises elsewhere. But in absence of this, we disable the warning for now.
CXXEXFLAGS += -Wno-missing-field-initializers

# Reformat the RAM region for usage within code and expose them
ifneq (,$(RAM_START_ADDR))
  CFLAGS += -DCPU_RAM_BASE=$(RAM_START_ADDR)
endif
ifneq (,$(RAM_LEN))
  CFLAGS += -DCPU_RAM_SIZE=$(shell printf "0x%x" $$(($(RAM_LEN:%K=%*1024))))
endif
