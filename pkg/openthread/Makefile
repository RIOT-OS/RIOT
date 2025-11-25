PKG_NAME=openthread
PKG_URL=https://github.com/openthread/openthread.git
# thread-reference-20200818
PKG_VERSION=54b31928cf65803a9cd83c9f97061b64e465aaf7
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

ifneq (,$(filter openthread-ftd,$(USEMODULE)))
  TD = ftd
  OT_JOINER := OFF
else ifneq (,$(filter openthread-mtd,$(USEMODULE)))
  TD = mtd
  OT_JOINER := ON
else
  $(error "Please use either USEMODULE=openthread-ftd or USEMODULE=openthread-mtd")
endif

OT_MODULES = mbedcrypto mbedtls openthread-$(TD)
ifneq (,$(filter openthread-cli-$(TD),$(USEMODULE)))
  OT_MODULES += openthread-cli-$(TD)
  OT_APP_CLI ?= ON
endif

# Enable CoAP
OT_COAP ?= ON

OT_C_FLAGS = $(CFLAGS_CPU)
OT_C_FLAGS += -fdata-sections
OT_C_FLAGS += -ffunction-sections -Os
OT_C_FLAGS += -ffunction-sections
OT_C_FLAGS += -fshort-enums
OT_C_FLAGS += -Wno-implicit-fallthrough
OT_C_FLAGS += -Wno-unused-parameter
OT_CXXFLAGS += -Wno-class-memaccess
OT_CXXFLAGS += -DOPENTHREAD_TARGET_RIOT

OT_MODULES_ARCHIVES = $(addsuffix .a,$(addprefix $(BINDIR)/,$(OT_MODULES)))
OT_CORE_LIB_DIR = $(PKG_BUILD_DIR)/src/core
OT_CLI_LIB_DIR = $(PKG_BUILD_DIR)/src/cli
OT_MBEDCRYPTO_LIB_DIR = $(PKG_BUILD_DIR)/third_party/mbedtls/repo/library

ifeq (llvm,$(TOOLCHAIN))
  OT_C_FLAGS += -target $(TARGET_ARCH_LLVM)
  OT_C_FLAGS += $(GCC_C_INCLUDES)
  OT_CXXFLAGS += $(GCC_CXX_INCLUDES)
endif

all: $(OT_MODULES_ARCHIVES)
	@:

$(BINDIR)/mbedtls.a: $(BINDIR)/openthread-$(TD).a
	@cp $(OT_MBEDCRYPTO_LIB_DIR)/libmbedtls.a $@

$(BINDIR)/mbedcrypto.a: $(BINDIR)/openthread-$(TD).a
	@cp $(OT_MBEDCRYPTO_LIB_DIR)/libmbedcrypto.a $@

$(BINDIR)/openthread-cli-$(TD).a: $(BINDIR)/openthread-$(TD).a
	@cp $(OT_CLI_LIB_DIR)/libopenthread-cli-$(TD).a $@

$(BINDIR)/openthread-$(TD).a: $(OT_CORE_LIB_DIR)/libopenthread-$(TD).a
	@cp $< $@

$(OT_CORE_LIB_DIR)/libopenthread-$(TD).a: $(PKG_BUILD_DIR)/Makefile
	$(QQ)"$(MAKE)" -C $(PKG_BUILD_DIR) $(OT_MODULES)

$(PKG_BUILD_DIR)/Makefile:
	cmake -Wno-dev -B$(PKG_BUILD_DIR) -H$(PKG_SOURCE_DIR) \
	 -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	 -DCMAKE_C_COMPILER="$(CC)" \
	 -DCMAKE_C_COMPILER_AR="$(AR)" \
	 -DCMAKE_C_COMPILER_RANLIB="$(RANLIB)" \
	 -DCMAKE_C_FLAGS="$(OT_C_FLAGS)" \
	 -DCMAKE_CXX_COMPILER="$(CXX)" \
	 -DCMAKE_CXX_FLAGS="$(OT_C_FLAGS) $(OT_CXXFLAGS) -fno-exceptions -fno-rtti" \
	 -DCMAKE_NM="$(NM)" \
	 -DCMAKE_STRIP="$(STRIP)" \
	 -DOT_PLATFORM=NO \
	 -DOT_CONFIG="$(RIOTBASE)/pkg/openthread/include/platform_config.h" \
	 -DOT_APP_CLI=$(OT_APP_CLI) \
	 -DOT_JOINER=$(OT_JOINER) \
	 -DOT_COAP=$(OT_COAP)
