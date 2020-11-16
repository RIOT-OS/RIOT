PKG_NAME=cc2538-bsl
PKG_URL=https://github.com/JelmerT/cc2538-bsl.git
PKG_VERSION=733e6f5b496402e40ad6d12df3d0372e205b8883
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/cc2538-bsl.py

$(CURDIR)/cc2538-bsl.py: $(PKG_SOURCE_DIR)/cc2538-bsl.py
	cp $(PKG_SOURCE_DIR)/cc2538-bsl.py .

clean::
	rm -f $(CURDIR)/cc2538-bsl.py
