MODULE = esp_lcd

MODULE_DEFS_NEEDED += \
  esp_lcd_gpio \
  lcd_parallel_16bit \
  #

ifneq (,$(filter esp32 esp32s2 esp32s3,$(CPU_FAM)))
  SRC = esp_lcd_mcu.c
else
  SRC = esp_lcd_gpio.c
endif

include $(RIOTBASE)/Makefile.base
