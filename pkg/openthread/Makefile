PKG_NAME=openthread
PKG_URL=https://github.com/openthread/openthread.git
PKG_VERSION=thread-reference-20180926
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)

ifneq (,$(filter openthread-ftd,$(USEMODULE)))
  TD = ftd
  $(info Compile OpenThread for FTD device)
else ifneq (,$(filter openthread-mtd,$(USEMODULE)))
  $(info Compile OpenThread for MTD device)
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
$(info $$OPENTHREAD_ARGS is [${OPENTHREAD_ARGS}])

.PHONY: all

OPENTHREAD_COMMON_FLAGS = -fdata-sections -ffunction-sections -Os
OPENTHREAD_COMMON_FLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter

all: git-download
	cd $(PKG_BUILDDIR) && PREFIX="/" ./bootstrap
	cd $(PKG_BUILDDIR) && CPP="$(CPP)" CC="$(CC)" CXX="$(CXX)"\
		OBJC="" OBJCXX="" AR="$(AR)" RANLIB="$(RANLIB)" NM="$(NM)" \
		STRIP="$(STRIP)" \
		CPPFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) -D$(CONFIG_FILE)" \
		CFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) " \
		CXXFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) -fno-exceptions -fno-rtti " \
		LDFLAGS="$(OPENTHREAD_COMMON_FLAGS) $(CFLAGS_CPU) -nostartfiles -specs=nano.specs \
		-specs=nosys.specs -Wl,--gc-sections -Wl,-Map=map.map " \
		./configure --disable-docs --host=$(TARGET_ARCH) --target=$(TARGET_ARCH) \
		--prefix=/ --enable-default-logging $(OPENTHREAD_ARGS)
	cd $(PKG_BUILDDIR) &&  DESTDIR=$(PKG_BUILDDIR)/output PREFIX=/ make -j4 --no-print-directory install

	cp $(PKG_BUILDDIR)/output/lib/libmbedcrypto.a ${BINDIR}/mbedcrypto.a

	cp $(PKG_BUILDDIR)/output/lib/libopenthread-$(TD).a ${BINDIR}/openthread-$(TD).a
ifneq (,$(filter openthread-cli,$(USEMODULE)))
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-cli-$(TD).a ${BINDIR}/openthread-cli.a
endif

include $(RIOTBASE)/pkg/pkg.mk
