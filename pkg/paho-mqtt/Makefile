PKG_NAME = paho-mqtt
PKG_URL = https://github.com/eclipse/paho.mqtt.embedded-c.git
PKG_VERSION = 29ab2aa29c5e47794284376d7f8386cfd54c3eed
# EPL-2.0, EDL-1.0 dual-license
# Eclipse defines EDL-1.0 https://www.eclipse.org/org/documents/edl-v10.php
# to be identified through BSD-3-Clause SPDX license identifier.
# Although written "and" the eclipse dual-license has the effect of what SPDX describes with OR
# https://www.eclipse.org/legal/epl/faq/#DUALLIC
PKG_LICENSE = EPL-2.0 OR BSD-3-Clause

CFLAGS += -Wno-cast-align

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/MQTTPacket/src/ -f $(CURDIR)/Makefile.$(PKG_NAME)-packet
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/MQTTClient-C/src/ -f $(CURDIR)/Makefile.$(PKG_NAME)
