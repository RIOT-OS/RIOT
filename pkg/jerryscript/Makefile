PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=6e94414f9c3ad9b77c4635a0ca9e796752a205f0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

JERRYSCRIPT_BUILD_DIR   ?= $(PKG_BUILDDIR)/build
JERRYSCRIPT_SOURCE_DIR  ?= $(PKG_BUILDDIR)

CFLAGS += -Wno-implicit-fallthrough

JERRYHEAP ?= 16

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

.PHONY: libjerry

all: libjerry

libjerry: $(JERRYSCRIPT_BUILD_DIR)/Makefile
	"$(MAKE)" -C $(JERRYSCRIPT_BUILD_DIR) jerry-core jerry-ext jerry-port-default-minimal
	@cp $(JERRYSCRIPT_BUILD_DIR)/lib/libjerry-core.a $(BINDIR)/jerryscript.a
	@cp $(JERRYSCRIPT_BUILD_DIR)/lib/libjerry-ext.a $(BINDIR)/jerryscript-ext.a
	@cp $(JERRYSCRIPT_BUILD_DIR)/lib/libjerry-port-default-minimal.a $(BINDIR)/jerryport-minimal.a

$(JERRYSCRIPT_BUILD_DIR)/Makefile:
	cmake -B$(JERRYSCRIPT_BUILD_DIR) -H$(JERRYSCRIPT_SOURCE_DIR) \
	 -DCMAKE_SYSTEM_NAME=RIOT \
	 -DCMAKE_SYSTEM_PROCESSOR="$(MCPU)" \
	 -DCMAKE_C_COMPILER=$(CC) \
	 -DCMAKE_C_COMPILER_WORKS=TRUE \
	 -DENABLE_LTO=OFF \
	 -DFEATURE_VALGRIND=OFF \
	 -DENABLE_ALL_IN_ONE=OFF \
	 -DJERRY_LIBM=OFF \
	 -DJERRY_CMDLINE=OFF \
	 -DHAVE_TIME_H=0 \
	 -DEXTERNAL_COMPILE_FLAGS="$(INCLUDES) $(EXT_CFLAGS)" \
	 -DMEM_HEAP_SIZE_KB=$(JERRYHEAP)

clean::
	@rm -rf $(JERRYSCRIPT_BUILD_DIR)
