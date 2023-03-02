PKG_NAME=tinydtls
PKG_URL=https://github.com/eclipse/tinydtls.git
PKG_VERSION=c58f484ac417afe036273d65506b63a0902c740c
PKG_LICENSE=EPL-1.0,EDL-1.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-implicit-fallthrough
# following is require due to known issue with newlib 2.4.x, see bug report:
# http://lists-archives.com/cygwin/97008-gettimeofday-not-defined.html
CFLAGS += -D_XOPEN_SOURCE=600

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(PKG_SOURCE_DIR)/Makefile.riot
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/aes -f $(PKG_SOURCE_DIR)/aes/Makefile.riot
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/ecc -f $(PKG_SOURCE_DIR)/ecc/Makefile.riot

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-format-nonliteral
endif

ifneq (,$(filter gcoap,$(USEMODULE)))
    # Configuring the buffer large enough that a full Gcoap packet can be
    # encrypted or decrypted.

    # This is the default in gcoap.h, which we don't have access to, so it is copied over.
    CONFIG_GCOAP_PDU_BUF_SIZE := $(or $(CONFIG_GCOAP_PDU_BUF_SIZE),128)

    # If there were another way to set up DTLS_MAX_BUF, we'd need to set the
    # maximum of these here.
    #
    # 29 bytes are the overhead measured with Wireshark on packets exchanged in
    # default configuration; adding some to be safe against variable size fields.
    CFLAGS += "-DDTLS_MAX_BUF=($(CONFIG_GCOAP_PDU_BUF_SIZE) + 36)"
endif

# TinyDTLS emits several messages during connection establishment at the info
# level; this is way more verbose than common in RIOT.
TINYDTLS_LOG_LEVEL ?= LOG_WARNING
CFLAGS += -DLOG_LEVEL=$(TINYDTLS_LOG_LEVEL)
