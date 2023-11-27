MODULE = esp_lcd

ifneq (,$(filter esp32 esp32s2 esp32s3,$(CPU_FAM)))
  SRC = esp_lcd_mcu.c
else
  SRC = esp_lcd_gpio.c
endif

include $(RIOTBASE)/Makefile.base
