PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
# 2.4.0
PKG_VERSION=8ba0d1b6ee5a065a42f3b306771ad8e3c0d819bc
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

JERRY_PROFILE ?= minimal # Other possible values are es.next, es5.1 and es2015-subset
JERRY_HEAP ?= 16  # kB
JERRY_STACK ?= 1  # kB
JERRY_GC_LIMIT ?= 0  # Use default value, e.g. 1/32 of total heap size
JERRY_GC_MARK_LIMIT ?= 8  # maximum recursion depth during GC mark phase

EXT_CFLAGS := -D__TARGET_RIOT
CFLAGS += -Wno-cast-align

# disable warnings when compiling with LLVM for board native
ifeq ($(TOOLCHAIN)_$(CPU),llvm_native)
  CFLAGS += -Wno-macro-redefined -Wno-gnu-folding-constant
  EXT_CFLAGS += -Wno-conversion
else ifeq ($(OS)_$(CPU),FreeBSD_native)
  EXT_CFLAGS += -Wno-conversion
else ifeq (esp32,$(CPU))
  # The ESP-IDF uses #include_next that causes errors when compiling with warnings
  # that are enabled by jerryscript build system so disable them for these CPUs:
  EXT_CFLAGS += -Wno-pedantic
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
	"$(MAKE)" -C $(PKG_BUILD_DIR) jerry-core jerry-ext jerry-port-default
	@cp $(PKG_BUILD_DIR)/lib/libjerry-core.a $(BINDIR)/jerryscript.a
	@cp $(PKG_BUILD_DIR)/lib/libjerry-ext.a $(BINDIR)/jerryscript-ext.a
	@cp $(PKG_BUILD_DIR)/lib/libjerry-port-default.a $(BINDIR)/jerryscript-port-default.a

$(PKG_BUILD_DIR)/Makefile:
	cmake -B$(PKG_BUILD_DIR) -H$(PKG_SOURCE_DIR) \
	 -DCMAKE_SYSTEM_NAME=RIOT \
	 -DCMAKE_SYSTEM_PROCESSOR="$(MCPU)" \
	 -DCMAKE_C_COMPILER=$(CC) \
	 -DCMAKE_C_COMPILER_WORKS=TRUE \
	 -DENABLE_LTO=OFF \
	 -DENABLE_AMALGAM=OFF \
	 -DHAVE_TIME_H=0 \
	 -DEXTERNAL_COMPILE_FLAGS="$(INCLUDES) $(EXT_CFLAGS)" \
	 -DJERRY_MATH=OFF \
	 -DJERRY_PROFILE=$(JERRY_PROFILE) \
	 -DJERRY_CMDLINE=OFF \
	 -DJERRY_VALGRIND=OFF \
	 -DJERRY_GC_LIMIT=$(JERRY_GC_LIMIT) \
	 -DJERRY_GC_MARK_LIMIT=$(JERRY_GC_MARK_LIMIT) \
	 -DJERRY_STACK_LIMIT=$(JERRY_STACK) \
	 -DJERRY_GLOBAL_HEAP_SIZE=$(JERRY_HEAP)

clean::
	@rm -rf $(PKG_BUILD_DIR)
