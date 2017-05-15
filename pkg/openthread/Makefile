PKG_NAME=openthread
PKG_URL=https://github.com/openthread/openthread.git
PKG_VERSION=fbfd76a990b81f007957e1bd774e51bce742e53e
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	    cd $(PKG_BUILDDIR) && PREFIX="/" ./bootstrap && ./configure --enable-application-coap \
			--enable-ncp-app --enable-cli-app --with-ncp-bus=uart --enable-ftd \
			INSTALL="/usr/bin/install -p" CPP="$(CPP)" CC="$(CC)" CXX="$(CXX)" OBJC="" \
			OBJCXX="" AR="$(AR)" RANLIB="$(RANLIB)" NM="$(NM)" STRIP="$(STRIP)" \
			CPPFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) " \
			CFLAGS="-fdata-sections -ffunction-sections -Os -g  $(CFLAGS_CPU) " \
			CXXFLAGS="-fdata-sections -ffunction-sections -Os -g  \
			$(CFLAGS_CPU) -fno-exceptions -fno-rtti " LDFLAGS="-fdata-sections \
			-ffunction-sections -Os -g  $(CFLAGS_CPU) -nostartfiles -specs=nano.specs \
			-specs=nosys.specs -Wl,--gc-sections -Wl,-Map=map.map " --host=$(TARGET_ARCH) \
			--target=$(TARGET_ARCH) --prefix=/ --enable-cli --enable-ncp \
			--enable-default-logging &&  make -j99 --no-print-directory \
			DESTDIR=$(PKG_BUILDDIR)/output install PREFIX=/
		cp $(PKG_BUILDDIR)/output/lib/libmbedcrypto.a ${BINDIR}/libmbedcrypto.a
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-ftd.a ${BINDIR}/libopenthread.a
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-cli-ftd.a ${BINDIR}/openthread_cli.a
		cp $(PKG_BUILDDIR)/output/lib/libopenthread-ncp-ftd.a ${BINDIR}/openthread_ncp.a
		sed -ie 's/BASE/_BASE/g' $(PKG_BUILDDIR)/output/include/openthread/types.h
include $(RIOTBASE)/pkg/pkg.mk
