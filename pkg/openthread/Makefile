PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=0e3b2456f9e472c433cc4f8ce358a1e40be9352e
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

ifneq (,$(filter libopenthread-ftd,$(USEMODULE)))
    $(info Compile OpenThread for FTD device)
    OPENTHREAD_ARGS+=--enable-ftd
endif
ifneq (,$(filter libopenthread-mtd,$(USEMODULE)))
      $(info Compile OpenThread for MTD device)
endif
ifneq (,$(filter libopenthread-cli,$(USEMODULE)))
      $(info Compile OpenThread with CLI)
      OPENTHREAD_ARGS += --enable-cli
endif
ifneq (,$(filter libopenthread-ncp,$(USEMODULE)))
      $(info Compile OpenThread with NCP)
      OPENTHREAD_ARGS += --enable-ncp
endif
$(info $$OPENTHREAD_ARGS is [${OPENTHREAD_ARGS}])

.PHONY: all

all: git-download

		cd $(PKG_BUILDDIR) && \
			PREFIX="/" ./bootstrap && \
			./configure \
				--enable-application-coap INSTALL="/usr/bin/install -p" CPP="$(CPP)" \
				CC="$(CC)" CXX="$(CXX)" OBJC="" OBJCXX="" AR="$(AR)" RANLIB="$(RANLIB)" NM="$(NM)" \
				STRIP="$(STRIP)" \
				CPPFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) " \
				CFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) " \
				CXXFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) -fno-exceptions -fno-rtti " \
				LDFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) -nostartfiles -specs=nano.specs \
				-specs=nosys.specs -Wl,--gc-sections -Wl,-Map=map.map " \
				--host=$(TARGET_ARCH) \
				--target=$(TARGET_ARCH) \
				--prefix=/ \
				${OPENTHREAD_ARGS} && \
			make -j99 --no-print-directory DESTDIR=$(PKG_BUILDDIR)/output install PREFIX=/
		cp $(PKG_BUILDDIR)/output/lib/libmbedcrypto.a ${BINDIR}/libmbedcrypto.a

ifneq (,$(filter libopenthread-ftd,$(USEMODULE)))
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-ftd.a ${BINDIR}/libopenthread-ftd.a
endif
ifneq (,$(filter libopenthread-mtd,$(USEMODULE)))
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-mtd.a ${BINDIR}/libopenthread-mtd.a
endif
ifneq (,$(filter libopenthread-cli,$(USEMODULE)))
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-cli.a ${BINDIR}/libopenthread-cli.a
endif
ifneq (,$(filter libopenthread-NCP,$(USEMODULE)))
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-ncp.a ${BINDIR}/libopenthread-ncp.a
endif
		sed -ie 's/BASE/_BASE/g' $(PKG_BUILDDIR)/output/include/openthread/types.h
include $(RIOTBASE)/pkg/pkg.mk
