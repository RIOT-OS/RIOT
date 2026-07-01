PKG_NAME=cc2538-bsl
PKG_URL=https://github.com/JelmerT/cc2538-bsl.git
PKG_VERSION=11de8cb2933d23ce001a1af14225ff4f5cd1983c
PKG_LICENSE=BSD-3-Clause

RIOTBASE ?= $(CURDIR)/../../..

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/cc2538-bsl.py

$(CURDIR)/cc2538-bsl.py: $(PKG_SOURCE_DIR)/cc2538-bsl.py
	cp $(PKG_SOURCE_DIR)/cc2538-bsl.py .

clean::
	rm -f $(CURDIR)/cc2538-bsl.py
