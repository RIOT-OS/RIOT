PKG_NAME=tinydtls
PKG_URL=https://github.com/eclipse/tinydtls.git
PKG_VERSION=5e14e4930b0f329f35809c623976df1e08ca4593
PKG_LICENSE=EPL-1.0,EDL-1.0

include $(RIOTBASE)/pkg/pkg.mk

# tinyDTLS custom logging functions pass non-literals to printf(), so disable them
ifneq (,$(filter arch_avr8,$(FEATURES_USED)))
  CFLAGS += -DNDEBUG
endif

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
