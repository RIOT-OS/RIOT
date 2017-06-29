PKG_NAME=openthread
PKG_URL=https://github.com/openthread/openthread.git
PKG_VERSION=8dc31d6e279d4f04fa64b5357d96e1ed20f4967a
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

ifneq (,$(filter openthread-cli-ftd,$(USEMODULE)))
  $(info Compile OpenThread for FTD device)
  OPENTHREAD_ARGS += --enable-cli-app=ftd
endif
ifneq (,$(filter openthread-cli-mtd,$(USEMODULE)))
  $(info Compile OpenThread for MTD device)
  OPENTHREAD_ARGS += --enable-cli-app=mtd --enable-joiner
endif
ifneq (,$(filter openthread-ncp-ftd,$(USEMODULE)))
  $(info Compile OpenThread with NCP)
  OPENTHREAD_ARGS += --enable-ncp-app=ftd --with-ncp-bus=uart \
                     --enable-commissioner --enable-border-router
endif
OPENTHREAD_ARGS += --enable-application-coap
CONFIG_FILE      = OPENTHREAD_PROJECT_CORE_CONFIG_FILE='\"platform_config.h\"'
$(info $$OPENTHREAD_ARGS is [${OPENTHREAD_ARGS}])

.PHONY: all

OPENTHREAD_COMMON_FLAGS = -fdata-sections -ffunction-sections -Os
OPENTHREAD_COMMON_FLAGS += -Wno-implicit-fallthrough

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
ifneq (,$(filter openthread-cli-ftd,$(USEMODULE)))
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-ftd.a ${BINDIR}/openthread-ftd.a
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-cli-ftd.a ${BINDIR}/openthread-cli-ftd.a
endif
ifneq (,$(filter openthread-cli-mtd,$(USEMODULE)))
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-mtd.a ${BINDIR}/openthread-mtd.a
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-cli-mtd.a ${BINDIR}/openthread-cli-mtd.a
endif
ifneq (,$(filter openthread-ncp-ftd,$(USEMODULE)))
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-ftd.a ${BINDIR}/openthread-ftd.a
	cp $(PKG_BUILDDIR)/output/lib/libopenthread-ncp-ftd.a ${BINDIR}/openthread-ncp-ftd.a
endif
	sed -ie 's/BASE/_BASE/g' $(PKG_BUILDDIR)/output/include/openthread/types.h

include $(RIOTBASE)/pkg/pkg.mk
