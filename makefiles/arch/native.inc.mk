NATIVEINCLUDES += -DNATIVE_INCLUDES
# the cpu/native include needs to before even core to allow native to override
# headers, inject some ugly hacks, and do an `#include_next`
NATIVEINCLUDES += -I$(RIOTCPU)/native/include/
INCLUDES := -I$(RIOTCPU)/native/include/ $(INCLUDES)
NATIVEINCLUDES += -I$(RIOTBASE)/core/lib/include/
NATIVEINCLUDES += -I$(RIOTBASE)/core/include/
NATIVEINCLUDES += -I$(RIOTBASE)/sys/include/
NATIVEINCLUDES += -I$(RIOTBASE)/drivers/include/

ifeq ($(OS),Darwin)
  DEBUGGER ?= lldb
else
  DEBUGGER ?= gdb
  DEBUGSERVER ?= gdbserver
endif


export VALGRIND ?= valgrind
export CGANNOTATE ?= cg_annotate
export GPROF ?= gprof

# basic cflags:
CFLAGS += -Wall -Wextra $(CFLAGS_DBG) $(CFLAGS_OPT)
CFLAGS += -U_FORTIFY_SOURCE
CFLAGS_DBG ?= -g3

ifneq (,$(filter backtrace,$(USEMODULE)))
  $(warning module backtrace is used, do not omit frame pointers)
  CFLAGS_OPT ?= -Og -fno-omit-frame-pointer
else
  CFLAGS_OPT ?= -Og
endif

# default std set to gnu11 if not overwritten by user
ifeq (,$(filter -std=%, $(CFLAGS)))
  CFLAGS += -std=gnu11
endif

ifeq ($(NATIVE_ARCH_BIT),64)
  CFLAGS += -m64
  LINKFLAGS += -m64
else ifeq ($(NATIVE_ARCH_BIT),32)
  CFLAGS += -m32
  LINKFLAGS += -m32
else
  $(error Unsupported native architecture)
endif

# Native does not compile on ARM, so cross-compile to x86 instead
ifneq (,$(filter arm64 aarch64,$(OS_ARCH)))
  _CROSS_TARGET := x86_64-linux-gnu
  _CROSS_PREFIX := $(_CROSS_TARGET)-
  _CROSS_LIBS := /usr/$(_CROSS_TARGET)/lib
  _CROSS_AVAILABLE := $(and $(shell command -v $(_CROSS_PREFIX)gcc 2>/dev/null),$(wildcard $(_CROSS_LIBS)/ld-linux-x86-64.so.2),$(wildcard $(_CROSS_LIBS)/libc.so.6))
  ifneq (,$(_CROSS_AVAILABLE))
    $(info Experimental cross-compilation to x86 64-bit)
    PREFIX = $(_CROSS_PREFIX)
    ifeq (llvm,$(TOOLCHAIN))
      # GCC still does linking
      TARGET_ARCH_LLVM = $(_CROSS_TARGET)
      CFLAGS += -target $(_CROSS_TARGET)
      CXXFLAGS += -target $(_CROSS_TARGET)
    endif
    LD_LIBRARY_PATH = $(_CROSS_LIBS)
    LINKFLAGS += -Wl,--dynamic-linker=$(_CROSS_LIBS)/ld-linux-x86-64.so.2
    LINKFLAGS += -Wl,-rpath,$(_CROSS_LIBS)
  endif
endif

ifneq (,$(filter -DDEVELHELP,$(CFLAGS)))
  CFLAGS += -fstack-protector-all
endif
ifeq ($(OS),FreeBSD)
  ifeq ($(OS_ARCH),amd64)
    ifeq ($(NATIVE_ARCH_BIT),32)
      CFLAGS += -DCOMPAT_32BIT -B/usr/lib32
      LINKFLAGS += -DCOMPAT_32BIT -L/usr/lib32 -B/usr/lib32
    endif
  endif
endif
ifeq ($(OS),Darwin)
  CFLAGS += -Wno-deprecated-declarations
endif

ifneq ($(filter $(OS),Darwin FreeBSD),)
  CFLAGS += -D_XOPEN_SOURCE
else
  CFLAGS += -D_GNU_SOURCE
endif

# unwanted (CXXUWFLAGS) and extra (CXXEXFLAGS) flags for c++
CXXUWFLAGS +=
CXXEXFLAGS +=

ifeq ($(OS),FreeBSD)
  LINKFLAGS += -L $(BINDIR)
else
  LINKFLAGS += -ldl
endif

# XFA (cross file array) support
LINKFLAGS += -T$(RIOTBASE)/cpu/native/ldscripts/xfa.ld

# fix this warning:
# ```
# /usr/bin/ld: examples/basic/hello-world/bin/native/cpu/tramp.o: warning: relocation against `_native_saved_eip' in read-only section `.text'
# /usr/bin/ld: warning: creating DT_TEXTREL in a PIE
# ```
LINKFLAGS += -no-pie

# clean up unused functions
CFLAGS += -ffunction-sections -fdata-sections
ifeq ($(OS),Darwin)
  LINKFLAGS += -Wl,-dead_strip
else
  LINKFLAGS += -Wl,--gc-sections
endif
LINKFLAGS += -ffunction-sections


ASFLAGS =
ifeq ($(notdir $(DEBUGGER)),lldb)
  DEBUGGER_FLAGS = -- $(ELFFILE) $(TERMFLAGS)
else
  DEBUGGER_FLAGS = -q --args $(ELFFILE) $(TERMFLAGS)
  DEBUGSERVER_BIND_ADDR ?= 127.0.0.1
  DEBUGSERVER_PORT ?= 3333
  DEBUGSERVER_FLAGS = --no-startup-with-shell --disable-randomization $(DEBUGSERVER_BIND_ADDR):$(DEBUGSERVER_PORT) $(ELFFILE) $(TERMFLAGS)
endif
term-valgrind: export VALGRIND_FLAGS ?= \
	--leak-check=full \
	--track-origins=yes \
	--fullpath-after=$(RIOTBASE)/ \
	--read-var-info=yes
debug-valgrind-server: export VALGRIND_FLAGS ?= --vgdb=yes --vgdb-error=0 -v \
	--leak-check=full --track-origins=yes --fullpath-after=$(RIOTBASE) \
	--read-var-info=yes
term-cachegrind: export CACHEGRIND_FLAGS += --tool=cachegrind
term-gprof: TERMPROG = GMON_OUT_PREFIX=gmon.out $(ELFFILE)
all-valgrind: CFLAGS += -DHAVE_VALGRIND
all-valgrind: NATIVEINCLUDES += $(shell pkg-config valgrind --cflags)
all-gprof: CFLAGS += -pg
all-gprof: LINKFLAGS += -pg

CFLAGS_ASAN += -fsanitize=address -fno-omit-frame-pointer -DNATIVE_MEMORY
LINKFLAGS_ASAN += -fsanitize=address -fno-omit-frame-pointer
all-asan: CFLAGS += $(CFLAGS_ASAN)
all-asan: LINKFLAGS += $(LINKFLAGS_ASAN)
all-asan: export AFL_USE_ASAN=1

INCLUDES += $(NATIVEINCLUDES)

CFLAGS += -DDEBUG_ASSERT_VERBOSE

ifeq (gnu,$(TOOLCHAIN))
  # workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
  ifneq ($(shell $(PREFIX)gcc --version | head -1 | grep -E ' (4.6|4.7)'),)
    CFLAGS += -DHAVE_NO_BUILTIN_BSWAP16
  endif
endif

# clumsy way to enable building native on osx:
BUILDOSXNATIVE = 0
ifeq ($(CPU),native)
  ifeq ($(OS),Darwin)
    BUILDOSXNATIVE = 1
  endif
endif

all: # do not override first target

all-gprof: all

all-asan: all

all-valgrind: all

all-cachegrind: all

term-valgrind:
	$(VALGRIND) $(VALGRIND_FLAGS) $(ELFFILE) $(PORT)

debug-valgrind-server:
	$(VALGRIND) $(VALGRIND_FLAGS) $(ELFFILE) $(PORT)

debug-valgrind:
	$(eval VALGRIND_PID ?= $(shell pgrep -n memcheck-x86-li -u $(USER) | cut -d" " -f1))
	$(eval DEBUGGER_FLAGS := -ex "target remote | vgdb --pid=$(VALGRIND_PID)" $(DEBUGGER_FLAGS))
	$(DEBUGGER) $(DEBUGGER_FLAGS)

term-cachegrind:
	$(VALGRIND) $(CACHEGRIND_FLAGS) $(ELFFILE) $(PORT)

term-gprof: term

eval-gprof:
	$(GPROF) $(ELFFILE) $(shell ls -rt gmon.out* | tail -1)

eval-cachegrind:
	$(CGANNOTATE) $(shell ls -rt cachegrind.out* | tail -1)
