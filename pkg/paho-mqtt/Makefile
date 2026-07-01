PKG_NAME = paho-mqtt
PKG_URL = https://github.com/eclipse/paho.mqtt.embedded-c.git
PKG_VERSION = 29ab2aa29c5e47794284376d7f8386cfd54c3eed
# for EDL-1.0 licensing, cf. https://www.eclipse.org/org/documents/edl-v10.php
PKG_LICENSE = EPL-2.0 AND BSD-3-Clause

CFLAGS += -Wno-cast-align

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/MQTTPacket/src/ -f $(CURDIR)/Makefile.$(PKG_NAME)-packet
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/MQTTClient-C/src/ -f $(CURDIR)/Makefile.$(PKG_NAME)
