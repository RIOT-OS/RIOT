MODULE = periph

MODULE_DEFS_NEEDED += \
  cortexm_common \
  disp_dev \
  fmt \
  periph_dma \
  periph_eth \
  periph_fmc_nor_sram \
  periph_fmc_sdram \
  periph_gpio_irq \
  periph_init_ptp \
  periph_lpuart \
  periph_ptp \
  periph_ptp_timer \
  periph_sdmmc_8bit \
  periph_spi_gpio_mode \
  periph_timer_periodic \
  periph_uart_hw_fc \
  periph_uart_modecfg \
  periph_uart_nonblocking \
  periph_vbat \
  ptp_timer \
  stm32_eth \
  stm32_eth_auto \
  stm32_eth_link_up \
  stm32_eth_tracing \
  stm32mp1_eng_mode \
  ztimer_msec \
  ztimer_usec \
  #

# Select the specific implementation for `periph_i2c`
ifneq (,$(filter periph_i2c,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),f0 f3 f7 g0 g4 h7 l0 l4 l5 u5 wb wl c0))
    SRC += i2c_1.c
  else ifneq (,$(filter $(CPU_FAM),f1 f2 f4 l1))
    SRC += i2c_2.c
  else
    $(error STM32 series I2C implementation not found.)
  endif
endif

# Select the specific implementation for `periph_adc`
ifneq (,$(filter periph_adc,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),f3 h7))
    SRC += adc_f3_h7.c
  else ifneq (,$(filter $(CPU_FAM),f4 f7))
    SRC += adc_f4_f7.c
  else ifneq (,$(filter $(CPU_FAM),f0 g0 c0))
    SRC += adc_f0_g0_c0.c
  else ifneq (,$(filter $(CPU_FAM),l4 wb))
    SRC += adc_l4_wb.c
  else
    SRC += adc_$(CPU_FAM).c
  endif
endif

# Select the correct implementation for `periph_gpio`
ifneq (,$(filter periph_gpio,$(USEMODULE)))
  ifeq (f1,$(CPU_FAM))
    SRC += gpio_f1.c
  else
    SRC += gpio_all.c
  endif
endif

# Select the correct implementation for `periph_rtc`
ifneq (,$(filter periph_rtc,$(USEMODULE)))
  ifeq (f1,$(CPU_FAM))
    SRC += rtc_f1.c
  else
    SRC += rtc_all.c
  endif
endif

# Select the correct implementation for `periph_rtt`
ifneq (,$(filter periph_rtt,$(USEMODULE)))
  ifeq (f1,$(CPU_FAM))
    SRC += rtt_f1.c
  else
    SRC += rtt_all.c
  endif
endif

# Select the correct implementation for `periph_usbdev`
ifneq (,$(filter periph_usbdev,$(USEMODULE)))
  ifeq (,$(filter usbdev_synopsys_dwc2,$(USEMODULE)))
    SRC += usbdev_fs.c
  endif
endif

# flashpage and eeprom periph implementations share flash lock/unlock functions
# defined in flash_common.c
ifneq (,$(filter periph_flashpage periph_eeprom,$(USEMODULE)))
  SRC += flash_common.c
endif

ifneq (,$(filter periph_wdt,$(USEMODULE)))
  $(warning Attention! WDT is clocked by CLOCK_LSI, it needs manual measuring\
    since values can deviate up to 50% from reference)
endif

ifneq (,$(filter periph_can,$(FEATURES_USED)))
  ifneq (,$(filter g4,$(CPU_FAM)))
	SRC += fdcan.c
  else
	SRC += classiccan.c
  endif
endif

include $(RIOTMAKE)/periph.mk
