PKG_NAME=cc26xx_cc13xx_driverlib
PKG_URL=https://github.com/contiki-ng/coresdk_cc13xx_cc26xx.git
PKG_VERSION=36ce7ddf1535f49ec55abfbd2c5180fa34320edb # 4.10.02
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:

prepare:
	$(Q)cp Makefile.cc26xx_cc13xx_driverlib $(PKG_BUILDDIR)/Makefile
	$(Q)cp Makefile.cc13x0 $(PKG_BUILDDIR)/source/ti/devices/cc13x0/driverlib/Makefile
	$(Q)cp Makefile.cc13x2_cc26x2 $(PKG_BUILDDIR)/source/ti/devices/cc13x2_cc26x2/driverlib/Makefile
	$(Q)cp Makefile.cc26x0 $(PKG_BUILDDIR)/source/ti/devices/cc26x0/driverlib/Makefile
