PKG_NAME    = wolfmqtt
PKG_URL     = https://github.com/wolfssl/wolfmqtt.git
PKG_VERSION = v1.12.0
PKG_LICENSE = GPLv2

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-parameter

all: $(filter \
    wolfmqtt \
    wolfmqtt_examples \
    wolfmqtt_examples_mqttclient \
    wolfmqtt_examples_snclient \
    wolfmqtt_examples_nbclient \
    wolfmqtt_examples_multithread \
    ,$(USEMODULE))

wolfmqtt:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)

wolfmqtt_examples:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfmqtt_examples -f $(CURDIR)/Makefile.$(PKG_NAME).examples

wolfmqtt_examples_mqttclient:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfmqtt_examples/mqttclient -f $(CURDIR)/Makefile.$(PKG_NAME).examples.mqttclient

wolfmqtt_examples_snclient:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfmqtt_examples/sn-client -f $(CURDIR)/Makefile.$(PKG_NAME).examples.snclient

wolfmqtt_examples_nbclient:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfmqtt_examples/nbclient -f $(CURDIR)/Makefile.$(PKG_NAME).examples.nbclient

wolfmqtt_examples_multithread:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfmqtt_examples/multithread -f $(CURDIR)/Makefile.$(PKG_NAME).examples.multithread
