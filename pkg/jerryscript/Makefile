PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=23bba1c6d9048e9b37cceff05ec6501646e48791  # 2.3.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

JERRY_HEAP ?= 16  # kB
JERRY_STACK ?= 1  # kB
JERRY_GC_LIMIT ?= 0  # Use default value, e.g. 1/32 of total heap size
JERRY_GC_MARK_LIMIT ?= 8  # maximum recursion depth during GC mark phase

EXT_CFLAGS := -D__TARGET_RIOT

# disable warnings when compiling with LLVM for board native
ifeq ($(TOOLCHAIN)_$(BOARD),llvm_native)
  CFLAGS += -Wno-macro-redefined -Wno-gnu-folding-constant
  EXT_CFLAGS += -Wno-conversion
else ifeq ($(OS)_$(BOARD),FreeBSD_native)
  EXT_CFLAGS += -Wno-conversion
else ifeq (esp32,$(CPU))
  # The esp32 C newlib version 2.2.0 has errors when compiling with warnings
  # that are enabled by jerryscript build system so disable them for this cpu:
  #  * -Wundef: stdio.h: `__GNU_VISIBLE` and `__BSD_VISIBLE` are not defined
  EXT_CFLAGS += -Wno-undef -Wno-error=undef
else ifeq (esp8266,$(CPU))
  # The esp8266 C newlib version 3.0.0 has errors when compiling with warnings
  # that are enabled by jerryscript build system so disable them for this cpu:
  #  * -Wundef
  #  * -Wconversion
  #  * -Wsign-conversion
  EXT_CFLAGS += -Wno-undef -Wno-error=undef
  EXT_CFLAGS += -Wno-conversion -Wno-error=conversion
  EXT_CFLAGS += -Wno-sign-conversion -Wno-error=sign-conversion
endif

all: $(BINDIR)/jerryscript.a

$(BINDIR)/jerryscript.a: $(PKG_BUILD_DIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILD_DIR) jerry-core jerry-ext jerry-port-default-minimal
	@cp $(PKG_BUILD_DIR)/lib/libjerry-core.a $(BINDIR)/jerryscript.a
	@cp $(PKG_BUILD_DIR)/lib/libjerry-ext.a $(BINDIR)/jerryscript-ext.a
	@cp $(PKG_BUILD_DIR)/lib/libjerry-port-default-minimal.a $(BINDIR)/jerryport-minimal.a

$(PKG_BUILD_DIR)/Makefile:
	cmake -B$(PKG_BUILD_DIR) -H$(PKG_SOURCE_DIR) \
	 -DCMAKE_SYSTEM_NAME=RIOT \
	 -DCMAKE_SYSTEM_PROCESSOR="$(MCPU)" \
	 -DCMAKE_C_COMPILER=$(CC) \
	 -DCMAKE_C_COMPILER_WORKS=TRUE \
	 -DENABLE_LTO=OFF \
	 -DENABLE_ALL_IN_ONE=OFF \
	 -DHAVE_TIME_H=0 \
	 -DEXTERNAL_COMPILE_FLAGS="$(INCLUDES) $(EXT_CFLAGS)" \
	 -DJERRY_LIBM=OFF \
	 -DJERRY_CMDLINE=OFF \
	 -DJERRY_VALGRIND=OFF \
	 -DJERRY_GC_LIMIT=$(JERRY_GC_LIMIT) \
	 -DJERRY_GC_MARK_LIMIT=$(JERRY_GC_MARK_LIMIT) \
	 -DJERRY_STACK_LIMIT=$(JERRY_STACK) \
	 -DJERRY_GLOBAL_HEAP_SIZE=$(JERRY_HEAP)

clean::
	@rm -rf $(PKG_BUILD_DIR)
