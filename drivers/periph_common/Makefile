
MODULE_DEFS_NEEDED += \
  fmt \
  periph_gpio \
  periph_gpio_ll_disconnect \
  periph_gpio_ll_open_drain \
  periph_gpio_ll_open_source \
  periph_init \
  periph_init_buttons \
  periph_init_fmc \
  periph_init_gpio_tamper_wake \
  periph_init_hwrng \
  periph_init_i2c \
  periph_init_led \
  periph_init_pio \
  periph_init_ptp \
  periph_init_rtc \
  periph_init_rtt \
  periph_init_sdmmc \
  periph_init_spi \
  periph_init_usbdev \
  periph_init_vbat \
  periph_init_wdt \
  periph_timer_query_freqs \
  periph_wdt_auto_start \
  #
PERIPH_COMMON_MODULES_SRCS_ALL = $(filter-out init.c,$(wildcard *.c))
PERIPH_COMMON_MODULES_NAMES = $(subst .c,,$(PERIPH_COMMON_MODULES_SRCS_ALL))
PERIPH_COMMON_MODULES = $(addprefix periph_, $(PERIPH_COMMON_MODULES_NAMES))
PERIPH_COMMON_MODULES_USED = $(filter $(PERIPH_COMMON_MODULES),$(USEMODULE))
SRC = init.c $(subst periph_,,$(addsuffix .c,$(PERIPH_COMMON_MODULES_USED)))

include $(RIOTBASE)/Makefile.base
