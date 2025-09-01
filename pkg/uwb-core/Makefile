PKG_NAME=uwb-core
PKG_URL=https://github.com/Decawave/uwb-core
PKG_VERSION=66f468659ec3353cf7fd6f2bd14f3a6cef397f4e
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-enum-compare
CFLAGS += -Wno-implicit-int
CFLAGS += -Wno-int-conversion
CFLAGS += -Wno-strict-prototypes
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-missing-declarations
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-return-type
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -fms-extensions

ifneq (,$(filter llvm,$(TOOLCHAIN)))
  CFLAGS += -Wno-microsoft-anon-tag
endif

ifneq (,$(filter -DNDEBUG,$(CFLAGS)))
  CFLAGS += -Wno-sometimes-uninitialized
endif

IGNORE_MODULES := uwb-core_dpl \
                  uwb-core_config \
                  uwb-core_contrib \
                  #

UWB_CORE_MODULES := $(filter-out $(IGNORE_MODULES),$(filter uwb-core%,$(USEMODULE)))

UWB_CORE_PATH_dsp        = lib/dsp/src
UWB_CORE_PATH_uwb_json   = lib/json/src
UWB_CORE_PATH_uwbcfg     = sys/uwbcfg/src/
UWB_CORE_PATH_rng        = lib/uwb_rng/src
UWB_CORE_PATH_rng_math   = lib/rng_math/src
UWB_CORE_PATH_twr_ss     = lib/twr_ss/src
UWB_CORE_PATH_twr_ss_ack = lib/twr_ss_ack/src
UWB_CORE_PATH_twr_ss_ext = lib/twr_ss_ext/src
UWB_CORE_PATH_twr_ds     = lib/twr_ds/src
UWB_CORE_PATH_twr_ds_ext = lib/twr_ds_ext/src

all: $(UWB_CORE_MODULES)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/hw/drivers/uwb/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

uwb-core_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(UWB_CORE_PATH_$*) -f $(RIOTBASE)/Makefile.base MODULE=$@
