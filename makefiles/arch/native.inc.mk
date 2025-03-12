NATIVEINCLUDES += -DNATIVE_INCLUDES
NATIVEINCLUDES += -I$(RIOTBASE)/core/lib/include/
NATIVEINCLUDES += -I$(RIOTBASE)/core/include/
NATIVEINCLUDES += -I$(RIOTBASE)/sys/include/
NATIVEINCLUDES += -I$(RIOTBASE)/drivers/include/

ifeq ($(OS),Darwin)
  DEBUGGER ?= lldb
else
  DEBUGGER ?= gdb
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

HOST_BIT := $(shell getconf LONG_BIT)
ifeq ($(NATIVE_ARCH_BIT),64)
  ifneq ($(HOST_BIT),64)
    CFLAGS += -m64
    LINKFLAGS += -m64
  endif
else ifeq ($(NATIVE_ARCH_BIT),32)
  ifneq ($(HOST_BIT),32)
    CFLAGS += -m32
    LINKFLAGS += -m32
  endif
else
  $(error Unsupported native architecture)
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

ifeq ($(OS),Darwin)
  # XFA (cross file array) support
  LD_SCRIPTS  += xfa-darwin

  _XFA_DIR     = $(BINDIR)/core/xfa

  # This file defines an alias for the first element of each array
  # These are the XFAs themselves (names passed to XFA_INIT/XFA_USE)
  _XFA_ASM     = $(_XFA_DIR)/aliases.s

  # This is the object generated from the assembly file above
  _XFA_OBJ     = $(_XFA_DIR)/aliases.o

  # This is a list of symbols starting with __xfa__ extracted from all objects
  # During the build, we sort this list of symbols
  _XFA_SYMBOLS = $(_XFA_DIR)/symbols.txt

  # The order_file ensures the priorities are respected
	LINKFLAGS += $(LINKFLAGPREFIX)-order_file $(LINKFLAGPREFIX)$(_XFA_SYMBOLS)
else
  # XFA (cross file array) support
  LINKFLAGS += -T$(RIOTBASE)/cpu/native/ldscripts/xfa.ld
  
	# fix this warning:
	# ```
	# /usr/bin/ld: examples/basic/hello-world/bin/native/cpu/tramp.o: warning: relocation against `_native_saved_eip' in read-only section `.text'
	# /usr/bin/ld: warning: creating DT_TEXTREL in a PIE
	# ```
	LINKFLAGS += -no-pie
endif

# clean up unused functions
CFLAGS += -ffunction-sections -fdata-sections
ifeq ($(OS),Darwin)
  LINKFLAGS += -Wl,-dead_strip
else
  LINKFLAGS += -Wl,--gc-sections
endif
LINKFLAGS += -ffunction-sections


ASFLAGS =
ifeq ($(shell basename $(DEBUGGER)),lldb)
  DEBUGGER_FLAGS = -- $(EXECUTABLE) $(TERMFLAGS)
else
  DEBUGGER_FLAGS = -q --args $(EXECUTABLE) $(TERMFLAGS)
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
term-gprof: TERMPROG = GMON_OUT_PREFIX=gmon.out $(EXECUTABLE)
all-valgrind: CFLAGS += -DHAVE_VALGRIND_H
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

# workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
ifneq ($(shell gcc --version | head -1 | grep -E ' (4.6|4.7)'),)
  CFLAGS += -DHAVE_NO_BUILTIN_BSWAP16
endif

all: # do not override first target

all-gprof: all

all-asan: all

all-valgrind: all

all-cachegrind: all

term-valgrind:
	$(VALGRIND) $(VALGRIND_FLAGS) $(EXECUTABLE) $(PORT)

debug-valgrind-server:
	$(VALGRIND) $(VALGRIND_FLAGS) $(EXECUTABLE) $(PORT)

debug-valgrind:
	$(eval VALGRIND_PID ?= $(shell pgrep -n memcheck-x86-li -u $(USER) | cut -d" " -f1))
	$(eval DEBUGGER_FLAGS := -ex "target remote | vgdb --pid=$(VALGRIND_PID)" $(DEBUGGER_FLAGS))
	$(DEBUGGER) $(DEBUGGER_FLAGS)

term-cachegrind:
	$(VALGRIND) $(CACHEGRIND_FLAGS) $(EXECUTABLE) $(PORT)

term-gprof: term

eval-gprof:
	$(GPROF) $(EXECUTABLE) $(shell ls -rt gmon.out* | tail -1)

eval-cachegrind:
	$(CGANNOTATE) $(shell ls -rt cachegrind.out* | tail -1)

# On macOS, cross-file arrays cannot be built using __attribute__((section("..."))) annotations
# that are later sorted using a linker script as linker scripts aren't supported by the macOS
# linker. Additionally, section names cannot exceed a certain length, preventing us from using
# cross-file array names as section names. Instead, we redefine the name of each cross-file array
# with the redefine_extname pragma. Each XFA definition contributes an __xfa__$...$__end__ symbol.
# We collect these end symbols from each binary and create a corresponding __start__ symbol (alias)
# that points to the first element of the cross-file array. If no elements are defined,
# __start__ automatically refers to __end__. __start__ is an alias, __end__ takes up space.
xfa-darwin: $(BASELIBS) $(ARCHIVES)
	$(Q)mkdir -p $(_XFA_DIR)
	$(Q)echo "" > $(_XFA_SYMBOLS)
	$(Q)echo "" > $(_XFA_ASM)
	$(Q)for obj in $(ALL_OBJS); do \
		nm -g $$obj | awk '{print $$3}' | grep -E '^(__xfa__|__roxfa__)' >> $(_XFA_SYMBOLS) || true; \
	done
	$(Q)sort $(_XFA_SYMBOLS) -o $(_XFA_SYMBOLS) || true
	$(Q)for prefix in $$(grep -E '^(__xfa__|__roxfa__)\$$.*\$$__end__$$' $(_XFA_SYMBOLS) | sed -E 's/(^[^$$]+\$$[^$$]+).*$$/\1/'); do \
		first_symbol=$$(grep -m 1 "^$$prefix" $(_XFA_SYMBOLS)); \
		echo ".section __DATA,__data" >> $(_XFA_ASM); \
		echo ".globl $${prefix}\$$0\$$__start__" >> $(_XFA_ASM); \
		echo "$${prefix}\$$0\$$__start__ = $$first_symbol" >> $(_XFA_ASM); \
		echo "$${prefix}\$$0\$$__start__" >> $(_XFA_SYMBOLS); \
	done
	$(Q)sort $(_XFA_SYMBOLS) -o $(_XFA_SYMBOLS) || true
	$(Q)$(AS) $(ASFLAGS) -o $(_XFA_OBJ) $(_XFA_ASM)
