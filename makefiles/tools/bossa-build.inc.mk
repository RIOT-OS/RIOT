PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/bossac

$(CURDIR)/bossac:
	@echo "[INFO] compiling bossac from source now"
	@env -u CXX COMMON_CXXFLAGS="-std=c++11" $(MAKE) BINDIR=$(PKG_BUILD_DIR) -C $(PKG_BUILD_DIR) strip-bossac
	@mv $(PKG_BUILD_DIR)/bossac $(CURDIR)/bossac

clean::
	rm -f $(CURDIR)/bossac
