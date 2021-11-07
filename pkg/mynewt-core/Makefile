PKG_NAME=mynewt-core
PKG_URL=https://github.com/apache/mynewt-core.git
PKG_VERSION=de203365f207dda658657a7525253e02f68503a1
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-cast-align

MYNEWT_CORE_MODULES := mynewt-core_os \
                       mynewt-core_util \
                       mynewt-core_nrf5x_hal \
                       #

MYNEWT_CORE_PATH_util      = util/mem/src
MYNEWT_CORE_PATH_os        = kernel/os/src

ifneq (,$(filter nrf52,$(CPU)))
  MYNEWT_CORE_PATH_nrf5x_hal = hw/mcu/nordic/nrf52xxx/src/
endif
ifneq (,$(filter nrf51,$(CPU)))
  MYNEWT_CORE_PATH_nrf5x_hal = hw/mcu/nordic/nrf51xxx/src/
endif

.PHONY: rm_riot_provided_headers

all: $(filter $(MYNEWT_CORE_MODULES),$(USEMODULE))
	@true

mynewt-core_%: rm_riot_provided_headers
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(MYNEWT_CORE_PATH_$*) -f $(RIOTPKG)/$(PKG_NAME)/$@.mk MODULE=$@

# The following mynewt-core headers are provided by RIOT, remove them from
# mynewt-core include paths to avoid header conflicts
MYNEWT_CORE_HAL_HEADERS         = hal_gpio.h hal_spi.h
MYNEWT_CORE_OS_HEADERS          = os.h mynewt.h os_dev.h os_eventq.h os_time.h
MYNEWT_CORE_HAL_HEADERS_PATH    = hw/hal/include/hal
MYNEWT_CORE_OS_HEADERS_PATH     = kernel/os/include/os/os_time

rm_riot_provided_headers:
	$(Q)for i in $(MYNEWT_CORE_OS_HEADERS); \
	  do rm -f "$(PKG_SOURCE_DIR)/$(MYNEWT_CORE_OS_HEADERS_PATH)/$$i"; done
	$(Q)for i in $(MYNEWT_CORE_HAL_HEADERS); \
	  do rm -f "$(PKG_SOURCE_DIR)/$(MYNEWT_CORE_HAL_HEADERS_PATH)/$$i"; done
