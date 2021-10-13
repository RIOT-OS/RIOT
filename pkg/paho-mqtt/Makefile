PKG_NAME = paho-mqtt
PKG_URL = https://github.com/eclipse/paho.mqtt.embedded-c.git
PKG_VERSION = 29ab2aa29c5e47794284376d7f8386cfd54c3eed
PKG_LICENSE = EDL

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/MQTTPacket/src/ -f $(CURDIR)/Makefile.$(PKG_NAME)-packet
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/MQTTClient-C/src/ -f $(CURDIR)/Makefile.$(PKG_NAME)
