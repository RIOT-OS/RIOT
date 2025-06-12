MODULE = esp_idf_common

# source files required from ESP-IDF in any case, regardless of additional modules
ESP32_SDK_SRC = \
  components/bootloader_support/bootloader_flash/src/bootloader_flash_config_$(CPU_FAM).c \
  components/bootloader_support/bootloader_flash/src/bootloader_flash.c \
  components/bootloader_support/src/bootloader_common.c \
  components/bootloader_support/src/bootloader_efuse.c \
  components/bootloader_support/src/bootloader_mem.c \
  components/bootloader_support/src/bootloader_random_$(CPU_FAM).c \
  components/esp_app_format/esp_app_desc.c \
  components/esp_hw_support/clk_ctrl_os.c \
  components/esp_hw_support/esp_clk.c \
  components/esp_hw_support/esp_gpio_reserve.c \
  components/esp_hw_support/periph_ctrl.c \
  components/esp_hw_support/port/$(CPU_FAM)/esp_clk_tree.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_clk.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_time.c \
  components/esp_hw_support/port/$(CPU_FAM)/sar_periph_ctrl.c \
  components/esp_hw_support/port/esp_clk_tree_common.c \
  components/esp_hw_support/regi2c_ctrl.c \
  components/esp_hw_support/sleep_gpio.c \
  components/esp_hw_support/sleep_event.c \
  components/esp_hw_support/sleep_modes.c \
  components/esp_hw_support/sleep_modem.c \
  components/esp_mm/esp_cache.c \
  components/esp_mm/esp_mmu_map.c \
  components/esp_mm/port/$(CPU_FAM)/ext_mem_layout.c \
  components/esp_rom/patches/esp_rom_sys.c \
  components/esp_rom/patches/esp_rom_uart.c \
  components/esp_system/esp_err.c \
  components/esp_system/port/cpu_start.c \
  components/esp_system/port/soc/$(CPU_FAM)/cache_err_int.c \
  components/esp_system/port/soc/$(CPU_FAM)/clk.c \
  components/esp_system/port/esp_system_chip.c \
  components/esp_timer/src/esp_timer.c \
  components/esp_timer/src/esp_timer_impl_common.c \
  components/esp_timer/src/esp_timer_init.c \
  components/esp_timer/src/system_time.c \
  components/hal/efuse_hal.c \
  components/hal/mmu_hal.c \
  components/hal/$(CPU_FAM)/clk_tree_hal.c \
  components/hal/$(CPU_FAM)/efuse_hal.c \
  components/hal/uart_hal.c \
  components/hal/wdt_hal_iram.c \
  components/newlib/port/esp_time_impl.c \
  components/spi_flash/cache_utils.c \
  components/spi_flash/flash_ops.c \
  #

ifneq (,$(filter esp_idf_nvs_flash,$(USEMODULE)))
  ESP32_SDK_SRC += components/bootloader_support/src/flash_encrypt.c
  ESP32_SDK_SRC += components/esp_partition/partition.c
  ESP32_SDK_SRC += components/esp_partition/partition_target.c
  INCLUDES += -I$(ESP32_SDK_DIR)/components/app_update/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_bootloader_format/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_partition/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/nvs_flash/include
endif

ifneq (,$(filter esp_spi_ram,$(USEMODULE)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_psram/include
endif

# TODO separate module
ifneq (,$(filter esp_idf_phy,$(USEMODULE)))
  ESP32_SDK_SRC += components/esp_phy/src/phy_common.c
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
  ifeq (esp32h2,$(CPU_FAM))
    ESP32_SDK_SRC += components/esp_phy/src/btbb_init.c
    ESP32_SDK_SRC += components/esp_phy/src/phy_init_esp32hxx.c
    ESP32_SDK_SRC += components/esp_phy/src/phy_override.c
  else
    ESP32_SDK_SRC += components/esp_phy/$(CPU_FAM)/phy_init_data.c
    ESP32_SDK_SRC += components/esp_phy/src/phy_init.c
    ESP32_SDK_SRC += components/esp_system/port/soc/$(CPU_FAM)/reset_reason.c
    ESP32_SDK_SRC += components/soc/esp32/dport_access.c
    INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_event/include
    INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_netif/include
    INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/$(CPU_FAM)/include
    INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_wifi/include
    ifeq (,$(filter esp_idf_wifi,$(USEMODULE)))
      ESP32_SDK_SRC += components/esp_wifi/src/wifi_init.c
      INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include
      INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_coex/include
    endif
  endif
endif

# TODO separate module
ifneq (,$(filter periph_can,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/twai_hal.c
  ESP32_SDK_SRC += components/hal/twai_hal_iram.c
endif

# TODO separate module
ifneq (,$(filter periph_dac,$(USEMODULE)))
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/dac_periph.c
endif

# TODO separate module
ifneq (,$(filter periph_hwrng,$(USEMODULE)))
  ESP32_SDK_SRC += components/esp_hw_support/hw_random.c
endif

# TODO separate module
ifneq (,$(filter periph_i2c%,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/i2c_hal.c
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
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/spi_periph.c
endif

# TODO separate module
ifneq (,$(filter periph_uart,$(USEMODULE)))
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/uart_periph.c
endif

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/rtc_io_periph.c
endif

ifneq (,$(filter riscv%,$(TARGET_ARCH)))
  ESP32_SDK_SRC += components/riscv/interrupt.c
  ESP32_SDK_SRC += components/riscv/interrupt_intc.c
  ESP32_SDK_ASMSRC += components/freertos/FreeRTOS-Kernel/portable/riscv/portasm.S
  ESP32_SDK_ASMSRC += components/riscv/vectors.S
  ESP32_SDK_ASMSRC += components/riscv/vectors_intc.S
endif

ifneq (,$(filter esp32 esp32s2 esp32s3 esp32c3,$(CPU_FAM)))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/rtc_init.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/rtc_sleep.c
else ifneq (,$(filter esp32h2 esp32c5 esp32c6 esp32c61,$(CPU_FAM)))
  ESP32_SDK_SRC += components/esp_hw_support/lowpower/port/$(CPU_FAM)/sleep_clock.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/io_mux.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/pmu_init.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/pmu_param.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/pmu_sleep.c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_retention.c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_system_peripheral.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/modem_clock_hal.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/pmu_hal.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/rtc_io_periph.c
endif

ifeq (esp32,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_mm/cache_esp32.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_lac.c
  ESP32_SDK_SRC += components/hal/esp32/cache_hal_esp32.c
else ifeq (esp32c3,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/lowpower/port/$(CPU_FAM)/sleep_cpu.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/systimer.c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_console.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/rtc_cntl_hal.c
  ESP32_SDK_SRC += components/hal/cache_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
else ifeq (esp32h2,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_rom/patches/esp_rom_regi2c_$(CPU_FAM).c
  ESP32_SDK_SRC += components/esp_hw_support/lowpower/port/$(CPU_FAM)/sleep_cpu.c
  ESP32_SDK_SRC += components/esp_hw_support/modem_clock.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/systimer.c
  ESP32_SDK_SRC += components/esp_hw_support/port/pau_regdma.c
  ESP32_SDK_SRC += components/esp_hw_support/port/regdma_link.c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_console.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/pau_hal.c
  ESP32_SDK_SRC += components/hal/apm_hal.c
  ESP32_SDK_SRC += components/hal/cache_hal.c
  ESP32_SDK_SRC += components/hal/lp_timer_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
  ESP32_SDK_ASMSRC += components/esp_hw_support/lowpower/port/$(CPU_FAM)/sleep_cpu_asm.S
else ifeq (esp32s2,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_rom/patches/esp_rom_regi2c_$(CPU_FAM).c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/systimer.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/memprot.c
  ESP32_SDK_SRC += components/esp_system/port/brownout.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/touch_sensor_hal.c
  ESP32_SDK_SRC += components/hal/brownout_hal.c
  ESP32_SDK_SRC += components/hal/cache_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
else ifeq (esp32s3,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/mspi_timing_tuning.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/mspi_timing_config.c
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/systimer.c
  ESP32_SDK_SRC += components/esp_hw_support/sleep_console.c
  ESP32_SDK_SRC += components/esp_rom/patches/esp_rom_cache_esp32s2_esp32s3.c
  ESP32_SDK_SRC += components/esp_rom/patches/esp_rom_efuse.c
  ESP32_SDK_SRC += components/esp_timer/src/esp_timer_impl_systimer.c
  ESP32_SDK_SRC += components/hal/$(CPU_FAM)/touch_sensor_hal.c
  ESP32_SDK_SRC += components/hal/cache_hal.c
  ESP32_SDK_SRC += components/hal/systimer_hal.c
  ESP32_SDK_SRC += components/spi_flash/$(CPU_FAM)/spi_flash_oct_flash_init.c
else
  $(error Missing configuration for ESP32x SoC variant (family): $(CPU_FAM))
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
