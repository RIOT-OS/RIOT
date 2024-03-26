include ../Makefile.pkg_common
USEPKG += fff
# If periph_i2c is pulled in the real implementation conflicts with the mock
FEATURES_BLACKLIST += periph_i2c

include $(RIOTBASE)/Makefile.include

ifneq ($(CPU),esp32)
  # only two threads used
  CFLAGS += -DMAXTHREADS=2
else
  # ESP32x SoCs uses an extra thread for esp_timer
  CFLAGS += -DMAXTHREADS=3
endif
