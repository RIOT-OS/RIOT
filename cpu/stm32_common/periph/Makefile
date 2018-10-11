MODULE = stm32_common_periph

# Select the specific implementation for `periph_i2c`
ifneq (,$(filter periph_i2c,$(USEMODULE)))
  ifneq (,$(filter $(CPU),stm32f0 stm32f3 stm32f7 stm32l0 stm32l4))
    SRC += i2c_1.c
  else # stm32f1/f2/f4/l1
    SRC += i2c_2.c
  endif
endif

# flashpage and eeprom periph implementations share flash lock/unlock functions
# defined in flash_common.c
ifneq (,$(filter periph_flashpage periph_eeprom,$(USEMODULE)))
  SRC += flash_common.c
endif

include $(RIOTMAKE)/periph.mk
