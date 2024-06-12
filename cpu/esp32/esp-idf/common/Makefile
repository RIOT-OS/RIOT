MODULE = esp_idf_common

# source files required from ESP-IDF in any case, regardless of additional modules
ESP32_SDK_SRC = \
  components/bootloader_support/src/bootloader_flash_config_$(CPU_FAM).c \
  components/bootloader_support/src/bootloader_flash.c \
  components/bootloader_support/src/bootloader_efuse_$(CPU_FAM).c \
  components/bootloader_support/src/bootloader_mem.c \
  components/bootloader_support/src/bootloader_random_$(CPU_FAM).c \
  components/driver/periph_ctrl.c \
  components/esp_hw_support/cpu_util.c \
  components/esp_hw_support/esp_clk.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_clk.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_clk_init.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_init.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_sleep.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_time.c \
  components/esp_hw_support/regi2c_ctrl.c \
  components/esp_hw_support/sleep_modes.c \
  components/esp_pm/pm_impl.c \
  components/esp_rom/patches/esp_rom_uart.c \
  components/esp_system/esp_err.c \
  components/esp_system/esp_system.c \
  components/esp_system/port/cpu_start.c \
  components/esp_system/port/soc/$(CPU_FAM)/cache_err_int.c \
  components/esp_system/port/soc/$(CPU_FAM)/clk.c \
  components/esp_system/port/soc/$(CPU_FAM)/reset_reason.c \
  components/esp_system/system_time.c \
  components/esp_timer/src/esp_timer.c \
  components/esp_timer/src/system_time.c \
  components/hal/cpu_hal.c \
  components/hal/mpu_hal.c \
  components/hal/timer_hal.c \
  components/hal/uart_hal.c \
  components/hal/wdt_hal_iram.c \
  components/newlib/port/esp_time_impl.c \
  components/spi_flash/cache_utils.c \
  components/spi_flash/flash_ops.c \
  #

ifneq (,$(filter periph_adc periph_dac,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/rtc_io_hal.c
endif

# TODO separate module
ifneq (,$(filter periph_can,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/twai_hal.c
  ESP32_SDK_SRC += components/hal/twai_hal_iram.c
endif

# TODO separate module
ifneq (,$(filter periph_dac,$(USEMODULE)))
  ESP32_SDK_SRC += components/driver/dac_common.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/dac_periph.c
endif

# TODO separate module
ifneq (,$(filter periph_hwrng,$(USEMODULE)))
  ESP32_SDK_SRC += components/esp_hw_support/hw_random.c
endif

# TODO separate module
ifneq (,$(filter periph_i2c%,$(USEMODULE)))
  ESP32_SDK_SRC += components/driver/i2c.c
  ESP32_SDK_SRC += components/hal/i2c_hal.c
  ESP32_SDK_SRC += components/hal/i2c_hal_iram.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/i2c_periph.c
endif

# TODO separate module
ifneq (,$(filter periph_pwm%,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/ledc_hal.c
  ESP32_SDK_SRC += components/hal/ledc_hal_iram.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/ledc_periph.c
endif

# TODO separate module
ifneq (,$(filter periph_spi,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/spi_hal.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/spi_periph.c
endif

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/rtc_wdt.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/rtc_io_periph.c
endif

ifneq (,$(filter riscv%,$(TARGET_ARCH)))
  ESP32_SDK_SRC += components/riscv/interrupt.c
  ESP32_SDK_ASMSRC += components/freertos/port/riscv/portasm.S
  ESP32_SDK_ASMSRC += components/riscv/vectors.S
endif

ifeq (esp32,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_frc_legacy.c
  ESP32_SDK_SRC += components/spi_flash/esp32/spi_flash_rom_patch.c
endif

ifeq (esp32c3,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/cpu_util_$(CPU_FAM).c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_retention.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/rtc_cntl_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
endif

ifeq (esp32s2,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/regi2c_ctrl.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/memprot.c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_retention.c
  ESP32_SDK_SRC += components/esp_system/port/brownout.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/brownout_hal.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/touch_sensor_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
endif

ifeq (esp32s3,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/sleep_retention.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/rtc_cntl_hal.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/touch_sensor_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
  ESP32_SDK_SRC += components/spi_flash/spi_flash_timing_tuning.c
  ESP32_SDK_SRC += components/spi_flash/$(CPU_FAM)/spi_timing_config.c
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
