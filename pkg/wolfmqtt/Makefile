PKG_NAME    = wolfmqtt
PKG_URL     = https://github.com/wolfssl/wolfmqtt.git
PKG_VERSION = 238a70ec96a7501853b81241352c5091a86e9251 # v1.7
PKG_LICENSE = GPLv2

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-parameter

all: $(filter wolfmqtt,$(USEMODULE))

wolfmqtt:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)
