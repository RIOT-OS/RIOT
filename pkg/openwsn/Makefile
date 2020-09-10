PKG_NAME=openwsn
PKG_URL=https://github.com/fjmolinas/openwsn-fw.git
PKG_VERSION=0669d5fd7fb8b2906a0088fe7edaa1d4c4a9f8c9
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# openwsn_% RIOT Modules or PSEUDOMODULES that don't have custom rules
IGNORE_MODULES  := openwsn_leds \
                   openwsn_debugpins \
                   openwsn_6lo_frag \
                   openwsn_icmpv6_echo \
                   openwsn_iee802154e_security \
                   openwsn_mac \
                   openwsn_radio \
                   openwsn_riotos \
                   openwsn_sap \
                   openwsn_stack \
                   openwsn_serial \
                   openwsn_sctimer \
                   openwsn_sctimer_rtt \
                   openwsn_sctimer_ztimer \
                   openwsn_txtsnd \
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
CFLAGS += -Wno-unused-variable
CFLAGS += -DLOG_LEVEL=$(OPENWSN_LOG_LEVEL)

OPENWSN_PATH_openstack      = openstack
OPENWSN_PATH_openapps       = openapps
OPENWSN_PATH_openweb        = openweb
OPENWSN_PATH_drivers        = drivers/common
OPENWSN_PATH_crypto         = drivers/common/crypto
OPENWSN_PATH_openos         = kernel/openos
OPENWSN_PATH_riotos         = kernel/riotos
OPENWSN_PATH_cjoin          = openapps/cjoin
OPENWSN_PATH_coap           = openweb/opencoap
OPENWSN_PATH_mac_low        = openstack/02a-MAClow
OPENWSN_PATH_mac_high       = openstack/02b-MAChigh
OPENWSN_PATH_iphc           = openstack/03a-IPHC
OPENWSN_PATH_ipv6           = openstack/03b-IPv6
OPENWSN_PATH_transport      = openstack/04-TRAN
OPENWSN_PATH_crosslayers    = openstack/cross-layers

all: $(OPENWSN_MODULES)

openwsn_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(OPENWSN_PATH_$*) -f $(RIOTBASE)/Makefile.base MODULE=$@
