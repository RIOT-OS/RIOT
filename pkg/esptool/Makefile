PKG_NAME=esptool
PKG_URL=https://github.com/espressif/esptool
# esptool v4.9.0
PKG_VERSION=8c3e3bf5d4292f1451744a838b2e60f50aa02b3c
PKG_LICENSE=GPLv2

include $(RIOTBASE)/pkg/pkg.mk

all: $(PKG_SOURCE_DIR)/venv/bin/esptool.py

clean::
	rm -rf $(PKG_SOURCE_DIR)/venv*

$(PKG_SOURCE_DIR)/venv/bin/esptool.py:
	python3 -m venv $(PKG_SOURCE_DIR)/venv
	$(PKG_SOURCE_DIR)/venv/bin/pip install esptool
