MODULE = periph

MODULE_DEFS_NEEDED += \
  esp_ble \
  esp_hw_counter \
  esp_idf_gpio_hal \
  esp_qemu \
  esp_rtc_timer_32k \
  esp_wifi_any \
  fmt \
  gpio_ll \
  periph_gpio_irq \
  periph_sdmmc_8bit \
  periph_sdmmc_hs \
  pm_layered \
  stdio_uart \
  ztimer_msec \
  ztimer_usec \
  #

ifneq (,$(filter periph_adc periph_dac,$(USEMODULE)))
  SRC += adc_arch_$(CPU_FAM).c
endif

ifneq (,$(filter periph_gpio,$(USEMODULE)))
  SRC += gpio_arch_$(CPU_FAM).c
endif

include $(RIOTMAKE)/periph.mk
