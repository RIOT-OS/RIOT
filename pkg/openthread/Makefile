PKG_NAME=openthread
PKG_URL=https://github.com/openthread/openthread.git
PKG_VERSION=thread-reference-20191113
PKG_LICENSE=BSD-3-Clause

# OpenThread build system doesn't support (yet) out-of-source builds
# so clone and build OpenThread within the application build directory
PKG_BUILD_OUT_OF_SOURCE = 0

include $(RIOTBASE)/pkg/pkg.mk

ifneq (,$(filter openthread-ftd,$(USEMODULE)))
  TD = ftd
else ifneq (,$(filter openthread-mtd,$(USEMODULE)))
  TD = mtd
  JOINER_ARG = --enable-joiner
else
  $(error "Please use either USEMODULE=openthread-ftd or USEMODULE=openthread-mtd")
endif

ifneq (,$(filter openthread-cli,$(USEMODULE)))
  CLI_ARG = --enable-cli --enable-$(TD)
endif

OPENTHREAD_ARGS += $(CLI_ARG) $(JOINER_ARG) --enable-application-coap
CONFIG_FILE      = OPENTHREAD_PROJECT_CORE_CONFIG_FILE='\"platform_config.h\"'

OPENTHREAD_COMMON_FLAGS = -fdata-sections -ffunction-sections -Os
OPENTHREAD_COMMON_FLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter
OPENTHREAD_CXXFLAGS += -Wno-class-memaccess
OPENTHREAD_CXXFLAGS += -DOPENTHREAD_TARGET_RIOT=1

OT_LIB_DIR = $(PKG_BUILD_DIR)/output/lib
MODULE_LIBS = mbedcrypto.a openthread-$(TD).a
ifneq (,$(filter openthread-cli,$(USEMODULE)))
  MODULE_LIBS += openthread-cli.a
endif

all: $(addprefix $(BINDIR)/,$(MODULE_LIBS))
	@true

$(BINDIR)/openthread-$(TD).a: $(OT_LIB_DIR)/libopenthread-$(TD).a
	@cp $< $@

$(BINDIR)/mbedcrypto.a: $(BINDIR)/openthread-$(TD).a
	@cp $(OT_LIB_DIR)/libmbedcrypto.a $@

$(BINDIR)/openthread-cli.a: $(BINDIR)/openthread-$(TD).a
	@cp $(OT_LIB_DIR)/libopenthread-cli-$(TD).a $@

$(OT_LIB_DIR)/libopenthread-$(TD).a: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR) --no-print-directory install DESTDIR=$(PKG_BUILD_DIR)/output PREFIX=/
	$(Q)printf "OpenThread built for %s device\n" $(TD)

$(PKG_BUILD_DIR)/Makefile: $(PKG_BUILD_DIR)/configure
	$(Q)cd $(PKG_BUILD_DIR) && CPP="$(CPP)" CC="$(CC)" CXX="$(CXX)"\
		OBJC="" OBJCXX="" AR="$(AR)" RANLIB="$(RANLIB)" NM="$(NM)" \
		STRIP="$(STRIP)" \
		CPPFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) -D$(CONFIG_FILE)" \
		CFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) " \
		CXXFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(OPENTHREAD_CXXFLAGS) \
		          $(CFLAGS_CPU) -fno-exceptions -fno-rtti " \
		LDFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) -nostartfiles -specs=nano.specs \
		-specs=nosys.specs -Wl,--gc-sections -Wl,-Map=map.map " \
		./configure --disable-docs --host=$(TARGET_ARCH) --target=$(TARGET_ARCH) \
		--prefix=/ --enable-default-logging $(OPENTHREAD_ARGS)

$(PKG_BUILD_DIR)/configure: $(PKG_PREPARED)
	$(Q)printf "OPENTHREAD_ARGS is [$(OPENTHREAD_ARGS)]\n"
	$(Q)cd $(PKG_BUILD_DIR) && PREFIX="/" ./bootstrap
