MODULE = periph

# Select the specific implementation for `periph_i2c`
ifneq (,$(filter periph_i2c,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),f0 f3 f7 g0 g4 l0 l4 l5 u5 wb wl c0))
    SRC += i2c_1.c
  else ifneq (,$(filter $(CPU_FAM),f1 f2 f4 l1))
    SRC += i2c_2.c
  else
    $(error STM32 series I2C implementation not found.)
  endif
endif

# Select the specific implementation for `periph_adc`
ifneq (,$(filter periph_adc,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),f4 f7))
    SRC += adc_f4_f7.c
  else ifneq (,$(filter $(CPU_FAM),f0 g0 c0))
    SRC += adc_f0_g0_c0.c
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

# Select the correct implementation for `periph_spi`
ifneq (,$(filter periph_spi,$(USEMODULE)))
  ifeq (u5,$(CPU_FAM))
    SRC += spi_$(CPU_FAM).c
  else
    SRC += spi_all.c
  endif
endif
include $(RIOTMAKE)/periph.mk
