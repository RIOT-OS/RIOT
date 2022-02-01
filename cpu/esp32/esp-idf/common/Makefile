MODULE = esp_idf_common

# source files required from ESP-IDF in any case, regardless of additional modules
ESP32_SDK_SRC = \
  components/driver/periph_ctrl.c \
  components/esp_hw_support/cpu_util.c \
  components/esp_hw_support/sleep_modes.c \
  components/esp_hw_support/port/$(CPU)/rtc_sleep.c \
  components/esp_hw_support/esp_clk.c \
  components/esp_hw_support/port/$(CPU)/rtc_clk.c \
  components/esp_hw_support/port/$(CPU)/rtc_clk_init.c \
  components/esp_hw_support/port/$(CPU)/rtc_init.c \
  components/esp_hw_support/port/$(CPU)/rtc_time.c \
  components/esp_hw_support/port/$(CPU)/rtc_wdt.c \
  components/esp_hw_support/regi2c_ctrl.c \
  components/esp_pm/pm_impl.c \
  components/esp_system/esp_err.c \
  components/esp_system/esp_system.c \
  components/esp_system/port/soc/$(CPU)/clk.c \
  components/esp_system/port/soc/$(CPU)/reset_reason.c \
  components/esp_system/system_time.c \
  components/hal/mpu_hal.c \
  components/hal/wdt_hal_iram.c \
  components/newlib/port/esp_time_impl.c \
  components/soc/$(CPU)/rtc_io_periph.c \
  components/spi_flash/cache_utils.c \
  components/esp_timer/src/esp_timer.c \
  components/esp_timer/src/esp_timer_impl_frc_legacy.c \
  #

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
