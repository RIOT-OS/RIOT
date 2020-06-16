PKG_NAME=openthread
PKG_URL=https://github.com/openthread/openthread.git
PKG_VERSION=thread-reference-20180926
PKG_LICENSE=BSD-3-Clause

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

OT_LIB_DIR = $(PKG_BUILDDIR)/output/lib
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

$(OT_LIB_DIR)/libopenthread-$(TD).a: $(PKG_BUILDDIR)/Makefile
	make -C $(PKG_BUILDDIR) -j4 --no-print-directory install DESTDIR=$(PKG_BUILDDIR)/output PREFIX=/
	$(Q)printf "OpenThread built for %s device\n" $(TD)

$(PKG_BUILDDIR)/Makefile: $(PKG_BUILDDIR)/configure
	$(Q)cd $(PKG_BUILDDIR) && CPP="$(CPP)" CC="$(CC)" CXX="$(CXX)"\
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

$(PKG_BUILDDIR)/configure: $(PKG_PREPARED)
	$(Q)printf "OPENTHREAD_ARGS is [$(OPENTHREAD_ARGS)]\n"
	$(Q)cd $(PKG_BUILDDIR) && PREFIX="/" ./bootstrap
