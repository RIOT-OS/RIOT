PKG_NAME=openwsn
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw.git
PKG_VERSION=e2958a031ad50a6f3eeca3a98ad1ae85aa773a48
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# openwsn_% RIOT Modules or PSEUDOMODULES that don't have custom rules
IGNORE_MODULES  := openwsn_leds \
                   openwsn_debugpins \
                   openwsn_6lo_fragmentation \
                   openwsn_icmpv6_echo \
                   openwsn_iee802154e_security \
                   openwsn_radio \
                   openwsn_riotos \
                   openwsn_sock \
                   openwsn_sock_async \
                   openwsn_sock_udp \
                   openwsn_serial \
                   openwsn_sctimer \
                   openwsn_sctimer_rtt \
                   openwsn_sctimer_ztimer \
                   #

OPENWSN_MODULES := $(filter-out $(IGNORE_MODULES),$(filter openwsn_%,$(USEMODULE)))

.PHONY: openwsn_%

OPENWSN_LOG_LEVEL ?= LOG_NONE

CFLAGS += -Wno-array-bounds
CFLAGS += -Wno-implicit-fallthrough
CFLAGS += -Wno-implicit-function-declaration
CFLAGS += -Wno-incompatible-pointer-types
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-return-type
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-strict-prototypes
CFLAGS += -DLOG_LEVEL=$(OPENWSN_LOG_LEVEL)

OPENWSN_PATH_openstack      = openstack
OPENWSN_PATH_openapps       = openapps
OPENWSN_PATH_openweb        = openweb
OPENWSN_PATH_drivers        = drivers/common
OPENWSN_PATH_crypto         = drivers/common/crypto
OPENWSN_PATH_cjoin          = openapps/cjoin
OPENWSN_PATH_coap           = openweb/opencoap
OPENWSN_PATH_mac_low        = openstack/02a-MAClow
OPENWSN_PATH_mac_high       = openstack/02b-MAChigh
OPENWSN_PATH_iphc           = openstack/03a-IPHC
OPENWSN_PATH_ipv6           = openstack/03b-IPv6
OPENWSN_PATH_udp            = openstack/04-TRAN
OPENWSN_PATH_crosslayers    = openstack/cross-layers

all: $(OPENWSN_MODULES)

openwsn_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(OPENWSN_PATH_$*) -f $(RIOTBASE)/Makefile.base MODULE=$@
