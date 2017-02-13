PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=ea4047b1cfd7ff0bb5ddd1b06d3c41b70d5c08aa
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	    cd $(PKG_BUILDDIR) && PREFIX="/" ./bootstrap && ./configure INSTALL="/usr/bin/install -p" CPP="$(CPP)" CC="$(CC)" CXX="$(CXX)" OBJC="" OBJCXX="" AR="$(AR)" RANLIB="$(RANLIB)" NM="$(NM)" STRIP="$(STRIP)" CPPFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) " CFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) " CXXFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) -fno-exceptions -fno-rtti " LDFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) -nostartfiles -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections -Wl,-Map=map.map " --host=$(TARGET_ARCH) --target=$(TARGET_ARCH) --prefix=/ --enable-cli --enable-ncp --enable-default-logging &&  make -j99 --no-print-directory DESTDIR=$(PKG_BUILDDIR)/output install PREFIX=/
		cp $(PKG_BUILDDIR)/output/lib/libmbedcrypto.a ${BINDIR}/libmbedcrypto.a
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-ftd.a ${BINDIR}/libopenthread.a
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-cli.a ${BINDIR}/libopenthread-cli.a
include $(RIOTBASE)/pkg/pkg.mk
