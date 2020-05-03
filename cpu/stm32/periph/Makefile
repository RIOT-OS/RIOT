MODULE = periph

# Select the specific implementation for `periph_i2c`
ifneq (,$(filter periph_i2c,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),stm32f0 stm32f3 stm32f7 stm32l0 stm32l4 stm32wb))
    SRC += i2c_1.c
  else # stm32f1/f2/f4/l1
    SRC += i2c_2.c
  endif
endif

# Select the specific implementation for `periph_adc`
ifneq (,$(filter periph_adc,$(USEMODULE)))
  SRC += adc_$(subst stm32,,$(CPU_FAM)).c
endif

# Select the correct implementation for `periph_gpio`
ifneq (,$(filter periph_gpio,$(USEMODULE)))
  ifeq (stm32f1,$(CPU_FAM))
    SRC += gpio_f1.c
  else
    SRC += gpio_all.c
  endif
endif

# Select the correct implementation for `periph_rtc`
ifneq (,$(filter periph_rtc,$(USEMODULE)))
  ifeq (stm32f1,$(CPU_FAM))
    SRC += rtc_f1.c
  else
    SRC += rtc_all.c
  endif
endif

# Select the correct implementation for `periph_rtt`
ifneq (,$(filter periph_rtt,$(USEMODULE)))
  ifeq (stm32f1,$(CPU_FAM))
    SRC += rtt_f1.c
  else
    SRC += rtt_all.c
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

include $(RIOTMAKE)/periph.mk
