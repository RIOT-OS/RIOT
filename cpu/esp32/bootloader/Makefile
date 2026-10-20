# Bootloader binary blob
MODULE = esp_bootloader

# We are compiling the bootloader from the ESP32 SDK sources, so we
# can't use the automatic module SRC discovery rules. Also, the compiled code
# would not be linked into the application, instead it is linked into its own
# binary file.
NO_AUTO_SRC = 1

include $(RIOTBASE)/Makefile.base
include $(RIOTMAKE)/tools/esptool.inc.mk

# Bootloader baudrate, set to the application defined one if any or the default
# in serial.inc.mk
BOOTLOADER_BAUD ?= $(BAUD)

# The following lists of ESP-IDF source files, include paths and linker
# scripts are derived from the bootloader of the ESP-IDF example
# `examples/get-started/hello_world` as compiled with the ESP-IDF build
# system for each supported ESP32x SoC (compile_commands.json, build.ninja).

ESP_SDK_BOOTLOADER_SRCS = \
  components/bootloader/subproject/components/micro-ecc/uECC_verify_antifault.c \
  components/bootloader/subproject/main/bootloader_start.c \
  components/bootloader_support/bootloader_flash/src/bootloader_flash.c \
  components/bootloader_support/bootloader_flash/src/bootloader_flash_config_$(CPU_FAM).c \
  components/bootloader_support/bootloader_flash/src/flash_qio_mode.c \
  components/bootloader_support/src/$(CPU_FAM)/bootloader_$(CPU_FAM).c \
  components/bootloader_support/src/$(CPU_FAM)/bootloader_soc.c \
  components/bootloader_support/src/bootloader_clock_init.c \
  components/bootloader_support/src/bootloader_clock_loader.c \
  components/bootloader_support/src/bootloader_common.c \
  components/bootloader_support/src/bootloader_common_loader.c \
  components/bootloader_support/src/bootloader_console.c \
  components/bootloader_support/src/bootloader_console_loader.c \
  components/bootloader_support/src/bootloader_efuse.c \
  components/bootloader_support/src/bootloader_init.c \
  components/bootloader_support/src/bootloader_mem.c \
  components/bootloader_support/src/bootloader_panic.c \
  components/bootloader_support/src/bootloader_random.c \
  components/bootloader_support/src/bootloader_random_$(CPU_FAM).c \
  components/bootloader_support/src/bootloader_reset_common.c \
  components/bootloader_support/src/bootloader_sha.c \
  components/bootloader_support/src/bootloader_utility.c \
  components/bootloader_support/src/esp_image_format.c \
  components/bootloader_support/src/flash_encrypt.c \
  components/bootloader_support/src/flash_partitions.c \
  components/bootloader_support/src/secure_boot.c \
  components/efuse/$(CPU_FAM)/esp_efuse_fields.c \
  components/efuse/$(CPU_FAM)/esp_efuse_table.c \
  components/efuse/$(CPU_FAM)/esp_efuse_utility.c \
  components/efuse/src/esp_efuse_api.c \
  components/efuse/src/esp_efuse_fields.c \
  components/efuse/src/esp_efuse_utility.c \
  components/esp_bootloader_format/esp_bootloader_desc.c \
  components/esp_common/src/esp_err_to_name.c \
  components/esp_hal_ana_conv/$(CPU_FAM)/adc_periph.c \
  components/esp_hal_ana_conv/adc_hal.c \
  components/esp_hal_ana_conv/adc_hal_common.c \
  components/esp_hal_ana_conv/adc_oneshot_hal.c \
  components/esp_hal_clock/$(CPU_FAM)/clk_tree_hal.c \
  components/esp_hal_gpio/$(CPU_FAM)/sdm_periph.c \
  components/esp_hal_gpio/gpio_hal.c \
  components/esp_hal_gpio/sdm_hal.c \
  components/esp_hal_gpspi/$(CPU_FAM)/spi_periph.c \
  components/esp_hal_gpspi/spi_hal.c \
  components/esp_hal_gpspi/spi_hal_iram.c \
  components/esp_hal_gpspi/spi_slave_hal.c \
  components/esp_hal_gpspi/spi_slave_hal_iram.c \
  components/esp_hal_mspi/$(CPU_FAM)/mspi_periph.c \
  components/esp_hal_pmu/brownout_hal.c \
  components/esp_hal_timg/$(CPU_FAM)/timer_periph.c \
  components/esp_hal_timg/timer_hal.c \
  components/esp_hal_uart/$(CPU_FAM)/uart_periph.c \
  components/esp_hal_uart/uart_hal.c \
  components/esp_hal_uart/uart_hal_iram.c \
  components/esp_hal_wdt/$(CPU_FAM)/mwdt_periph.c \
  components/esp_hw_support/cpu.c \
  components/esp_hw_support/esp_memory_utils.c \
  components/esp_hw_support/port/$(CPU_FAM)/chip_info.c \
  components/esp_hw_support/port/$(CPU_FAM)/cpu_region_protect.c \
  components/esp_hw_support/port/$(CPU_FAM)/esp_cpu_intr.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_clk.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_clk_init.c \
  components/esp_hw_support/port/$(CPU_FAM)/rtc_time.c \
  components/esp_rom/patches/esp_rom_crc.c \
  components/esp_rom/patches/esp_rom_efuse.c \
  components/esp_rom/patches/esp_rom_gpio.c \
  components/esp_rom/patches/esp_rom_print.c \
  components/esp_rom/patches/esp_rom_serial_output.c \
  components/esp_rom/patches/esp_rom_spiflash.c \
  components/esp_rom/patches/esp_rom_sys.c \
  components/esp_security/src/$(CPU_FAM)/esp_crypto_clk.c \
  components/esp_security/src/esp_crypto_lock.c \
  components/esp_security/src/esp_crypto_periph_clk.c \
  components/esp_system/esp_err.c \
  components/hal/$(CPU_FAM)/efuse_hal.c \
  components/hal/efuse_hal.c \
  components/hal/hal_utils.c \
  components/hal/mmu_hal.c \
  components/log/src/buffer/log_buffers.c \
  components/log/src/log.c \
  components/log/src/log_format_text.c \
  components/log/src/log_print.c \
  components/log/src/log_timestamp_common.c \
  components/log/src/noos/log_lock.c \
  components/log/src/noos/log_timestamp.c \
  components/log/src/noos/util.c \
  components/log/src/util.c \
  components/soc/$(CPU_FAM)/gpio_periph.c \
  components/soc/$(CPU_FAM)/interrupts.c \
  components/soc/$(CPU_FAM)/mpi_periph.c \
  components/soc/$(CPU_FAM)/power_supply_periph.c \
  components/soc/dport_access_common.c \
  components/soc/lldesc.c \
  components/spi_flash/spi_flash_wrap.c \
  #

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/esp_hal_gpio/$(CPU_FAM)/rtc_io_periph.c \
    components/esp_hal_gpio/rtc_io_hal.c \
    components/esp_hal_security/mpu_hal.c \
    components/esp_hw_support/port/$(CPU_FAM)/rtc_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/rtc_sleep.c \
    components/xtensa/eri.c \
    components/xtensa/xt_trax.c \
    #
  ESP_SDK_BOOTLOADER_ASMSRC += \
    components/esp_rom/patches/esp_rom_longjmp.S \
    #
else
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_hal_ana_conv/$(CPU_FAM)/temperature_sensor_periph.c \
    components/esp_hal_ana_conv/temperature_sensor_hal.c \
    components/esp_hal_dma/$(CPU_FAM)/gdma_periph.c \
    components/esp_hal_dma/gdma_hal_ahb_v1.c \
    components/esp_hal_dma/gdma_hal_top.c \
    components/esp_hal_gpio/$(CPU_FAM)/dedic_gpio_periph.c \
    components/esp_hal_gpspi/spi_slave_hd_hal.c \
    components/esp_hal_uart/uhci_hal.c \
    components/esp_hal_usb/usb_serial_jtag_hal.c \
    components/hal/cache_hal.c \
    components/riscv/rv_utils.c \
    #
endif

ifeq (esp32,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/src/efuse_controller/keys/without_key_purposes/three_key_blocks/esp_efuse_api_key.c \
    components/esp_hal_ana_conv/$(CPU_FAM)/dac_periph.c \
    components/esp_hal_i2s/$(CPU_FAM)/i2s_periph.c \
    components/esp_hal_i2s/i2s_hal.c \
    components/esp_hal_wdt/wdt_hal_iram.c \
    components/hal/$(CPU_FAM)/cache_hal_$(CPU_FAM).c \
    components/soc/$(CPU_FAM)/dport_access.c \
    #
else ifeq (esp32s2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_table.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_hal_ana_conv/$(CPU_FAM)/dac_periph.c \
    components/esp_hal_ana_conv/$(CPU_FAM)/temperature_sensor_periph.c \
    components/esp_hal_ana_conv/temperature_sensor_hal.c \
    components/esp_hal_dma/cp_dma_hal.c \
    components/esp_hal_gpio/$(CPU_FAM)/dedic_gpio_periph.c \
    components/esp_hal_gpspi/spi_slave_hd_hal.c \
    components/esp_hal_regi2c/$(CPU_FAM)/regi2c_impl.c \
    components/esp_hal_systimer/systimer_hal.c \
    components/esp_hal_usb/$(CPU_FAM)/usb_dwc_periph.c \
    components/esp_hal_usb/usb_dwc_hal.c \
    components/esp_hal_usb/usb_wrap_hal.c \
    components/esp_hal_wdt/wdt_hal_iram.c \
    components/esp_hal_wdt/xt_wdt_hal.c \
    components/esp_rom/$(CPU_FAM)/usb_patches.c \
    components/esp_rom/patches/esp_rom_cache_$(CPU_FAM)_esp32s3.c \
    components/hal/cache_hal.c \
    #
else ifeq (esp32s3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_hal_ana_conv/$(CPU_FAM)/temperature_sensor_periph.c \
    components/esp_hal_ana_conv/temperature_sensor_hal.c \
    components/esp_hal_dma/$(CPU_FAM)/gdma_periph.c \
    components/esp_hal_dma/gdma_hal_ahb_v1.c \
    components/esp_hal_dma/gdma_hal_top.c \
    components/esp_hal_gpio/$(CPU_FAM)/dedic_gpio_periph.c \
    components/esp_hal_gpspi/spi_slave_hd_hal.c \
    components/esp_hal_pmu/$(CPU_FAM)/rtc_cntl_hal.c \
    components/esp_hal_systimer/systimer_hal.c \
    components/esp_hal_uart/uhci_hal.c \
    components/esp_hal_usb/$(CPU_FAM)/usb_dwc_periph.c \
    components/esp_hal_usb/usb_dwc_hal.c \
    components/esp_hal_usb/usb_serial_jtag_hal.c \
    components/esp_hal_usb/usb_wrap_hal.c \
    components/esp_hal_wdt/rom_patch.c \
    components/esp_hal_wdt/xt_wdt_hal.c \
    components/esp_rom/patches/esp_rom_cache_esp32s2_$(CPU_FAM).c \
    components/hal/cache_hal.c \
    #
  ESP_SDK_BOOTLOADER_ASMSRC += \
    components/esp_rom/patches/esp_rom_cache_writeback_$(CPU_FAM).S \
    #
else ifeq (esp32c3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/esp_hal_pmu/$(CPU_FAM)/rtc_cntl_hal.c \
    components/esp_hal_systimer/systimer_hal.c \
    components/esp_hal_wdt/wdt_hal_iram.c \
    components/esp_hal_wdt/xt_wdt_hal.c \
    components/esp_hw_support/port/$(CPU_FAM)/rtc_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/rtc_sleep.c \
    #
else ifeq (esp32c6,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/bootloader_support/src/$(CPU_FAM)/bootloader_ecdsa.c \
    components/esp_hal_gpio/$(CPU_FAM)/rtc_io_periph.c \
    components/esp_hal_gpio/rtc_io_hal.c \
    components/esp_hal_pmu/$(CPU_FAM)/pau_hal.c \
    components/esp_hal_pmu/$(CPU_FAM)/pmu_hal.c \
    components/esp_hal_regi2c/$(CPU_FAM)/regi2c_impl.c \
    components/esp_hal_systimer/rom_patch.c \
    components/esp_hal_wdt/rom_patch.c \
    components/esp_hw_support/port/$(CPU_FAM)/ocode_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_param.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_sleep.c \
    components/esp_rom/patches/esp_rom_usb_serial.c \
    #
else ifeq (esp32h2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_table_v0.0_v1.1.c \
    components/esp_hal_gpio/$(CPU_FAM)/rtc_io_periph.c \
    components/esp_hal_gpio/rtc_io_hal.c \
    components/esp_hal_pmu/$(CPU_FAM)/pau_hal.c \
    components/esp_hal_pmu/$(CPU_FAM)/pmu_hal.c \
    components/esp_hal_pmu/vbat_hal.c \
    components/esp_hal_regi2c/$(CPU_FAM)/regi2c_impl.c \
    components/esp_hal_systimer/rom_patch.c \
    components/esp_hal_wdt/rom_patch.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_param.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_sleep.c \
    components/esp_rom/patches/esp_rom_ecdsa.c \
    components/esp_rom/patches/esp_rom_usb_serial.c \
    #
else
  $(error Missing make configuration for ESP32x SoC variant (family): $(CPU_FAM))
endif

# Bootloader sdkconfig.h defined in CURDIR directory.
INCLUDES = \
  -I$(dir $(RIOTBUILD_CONFIG_HEADER_C)) \
  -I$(CURDIR) \
  -I$(ESP32_SDK_DIR)/components \
  -I$(ESP32_SDK_DIR)/components/log/include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/include/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_common/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/ldo/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/debug_probe/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/etm/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/mspi/mspi_timing_tuning/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/mspi/mspi_timing_tuning/tuning_scheme_impl/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/mspi/mspi_intr/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/power_supply/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/modem/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_libc/platform_include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_gpio/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_gpio/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/soc/include \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/register \
  -I$(ESP32_SDK_DIR)/components/hal/platform_port/include \
  -I$(ESP32_SDK_DIR)/components/hal/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/hal/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_usb/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_pmu/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_pmu/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_regi2c/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_regi2c/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_uart/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_uart/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_ana_conv/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_ana_conv/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_dma/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_gpspi/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_gpspi/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_clock/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_clock/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_mspi/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_mspi/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/spi_flash/include \
  -I$(ESP32_SDK_DIR)/components/esp_blockdev/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/bootloader_flash/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/private_include \
  -I$(ESP32_SDK_DIR)/components/esp_bootloader_format/include \
  -I$(ESP32_SDK_DIR)/components/bootloader/subproject/components/micro-ecc \
  -I$(ESP32_SDK_DIR)/components/bootloader/subproject/components/micro-ecc/micro-ecc \
  -I$(ESP32_SDK_DIR)/components/esp_hal_timg/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_timg/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_wdt/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_wdt/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_security/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_security/include \
  -I$(ESP32_SDK_DIR)/components/esp_security/include \
  -I$(ESP32_SDK_DIR)/components/esp_security/src/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/efuse/include \
  -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_app_format/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_rtc_timer/include \
  -I$(ESP32_SDK_DIR)/components/esp_hal_rtc_timer/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/efuse/private_include \
  -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/private_include \
  -I$(ESP32_SDK_DIR)/components/esp_system/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/esp_private \
  -I$(ESP32_SDK_DIR)/components/esp_hal_systimer/include \
  -I$(ESP32_SDK_DIR)/components/log/include/esp_private \
  #

ifeq (esp32,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/deprecated_include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_i2s/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_i2s/$(CPU_FAM)/include
endif
ifeq (esp32s2,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/deprecated_include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc/$(CPU_FAM)
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_usb/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_dma/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)/include
endif
ifeq (esp32s3,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/deprecated_include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc/$(CPU_FAM)
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/mspi/mspi_timing_tuning/port/$(CPU_FAM)
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/mspi/mspi_timing_tuning/port/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_usb/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_dma/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)/include
endif
ifeq (esp32c3,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc/$(CPU_FAM)
  INCLUDES += -I$(ESP32_SDK_DIR)/components/riscv/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_usb/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_dma/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)/include
endif
ifeq (esp32c6,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/private_include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/riscv/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_usb/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_dma/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)/include
endif
ifeq (esp32h2,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/private_include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/riscv/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_usb/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_dma/$(CPU_FAM)/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)/include
endif

# ROM and peripheral linker scripts as used by the ESP-IDF build system
ifeq (esp32,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_regi2c/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libgcc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc-funcs.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).peripherals.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.regi2c.ld
else ifeq (esp32s2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libgcc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.spiflash_legacy.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc-funcs.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).peripherals.ld
else ifeq (esp32s3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_regi2c/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_wdt/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.bt_funcs.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libgcc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.version.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.newlib.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).peripherals.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.regi2c.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.wdt.ld
else ifeq (esp32c3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/riscv/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_regi2c/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.bt_funcs.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libgcc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.version.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc-suboptimal_for_misaligned_mem.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.newlib.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).peripherals.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.regi2c.ld
else ifeq (esp32c6,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/riscv/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_wdt/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.rvfp.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.version.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc-suboptimal_for_misaligned_mem.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.newlib.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).peripherals.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.wdt.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.systimer.ld
else ifeq (esp32h2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/riscv/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_wdt/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/esp_hal_systimer/$(CPU_FAM)
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libgcc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.version.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.libc-suboptimal_for_misaligned_mem.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).rom.newlib.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.api.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -T$(CPU_FAM).peripherals.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.wdt.ld
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -Trom.systimer.ld
endif

ESP_SDK_BOOTLOADER_UNDEF_SYMS = __assert_func __ubsan_include abort bootloader_hooks_include esp_bootloader_desc esp_system_include_startup_funcs
ifeq (esp32,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_UNDEF_SYMS += esp_dport_access_reg_read
endif
ifeq (esp32s3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_UNDEF_SYMS += esp_sleep_gpio_include
endif
ifeq (esp32c3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_UNDEF_SYMS += esp_sleep_gpio_include
endif
ifeq (esp32c6,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_UNDEF_SYMS += esp_sleep_gpio_include sleep_system_peripheral_dummy
endif
ifeq (esp32h2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_UNDEF_SYMS += esp_sleep_gpio_include sleep_system_peripheral_dummy
endif

# include CFLAGS from cpu/esp_common/Makefile.include including the defines of
# CONFIG_ESPTOOLPY_FLASHFREQ_* and CONFIG_FLASHMODE_*
CFLAGS = -include '$(RIOTBUILD_CONFIG_HEADER_C)' \

# TODO: required to be able to compile with GCC 12.1, remove them after upgrade to ESP-IDF 5.1
CFLAGS += -Wno-error=format=
CFLAGS += -Wno-format

ifneq (,$(filter riscv32%,$(TARGET_ARCH)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/riscv/include
  CFLAGS += -DCONFIG_IDF_TARGET_ARCH_RISCV
  CFLAGS += -Wno-error=format=
  CFLAGS += -nostartfiles
  GCC_NEW_RISCV_ISA ?= $(shell echo "typedef int dont_be_pedantic;" | \
                               $(TARGET_ARCH)-gcc -march=rv32imac -mabi=ilp32 \
                                                  -misa-spec=2.2 -E - > /dev/null 2>&1 && \
                               echo 1 || echo 0)
  ifeq (1,$(GCC_NEW_RISCV_ISA))
    CFLAGS += -misa-spec=2.2
  endif

  ifeq (esp32c3,$(CPU_FAM))
    CFLAGS += -march=rv32imc_zicsr_zifencei
  else ifneq (,$(filter esp32c6 esp32h2,$(CPU_FAM)))
    CFLAGS += -march=rv32imac_zicsr_zifencei
  else
      $(error Missing -march option for ESP32x RISC-V SoC variant: $(CPU_FAM))
  endif
endif

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/$(CPU_FAM)/include
  CFLAGS += -DCONFIG_IDF_TARGET_ARCH_XTENSA
  CFLAGS += -mlongcalls
  CFLAGS += -mtext-section-literals
  CFLAGS += -fno-builtin-memcpy
  CFLAGS += -fno-builtin-memset
  CFLAGS += -fno-builtin-bzero
  CFLAGS += -fno-builtin-stpcpy
  CFLAGS += -fno-builtin-strncpy
endif

ifneq (,$(filter esp32,$(CPU_FAM)))
  CFLAGS += -Wno-frame-address
endif

# BOOTLOADER_BUILD=1 signals to the SDK that's a bootloader build.
CFLAGS += \
  -Wall \
  -Wextra \
  -Werror=all \
  -Wno-error=deprecated-declarations \
  -Wno-error=unused-but-set-variable \
  -Wno-error=unused-function \
  -Wno-error=unused-variable \
  -Wno-old-style-declaration \
  -Wno-sign-compare \
  -Wno-unused-parameter \
  -fdata-sections \
  -ffunction-sections \
  -fdiagnostics-color=always \
  -fno-jump-tables \
  -fno-tree-switch-conversion \
  -fno-stack-protector \
  -freorder-blocks \
  -fstrict-volatile-bitfields \
  -ggdb \
  -Os \
  -std=gnu99 \
  -D_GNU_SOURCE \
  -DESP_PLATFORM \
  -DBOOTLOADER_BUILD=1 \
  -DNON_OS_BUILD=1 \
  -DSOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE \
  -DSOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ \
  -D_GLIBCXX_HAVE_POSIX_SEMAPHORE \
  -D_GLIBCXX_USE_POSIX_SEMAPHORE \
  -DRIOT_BOOTLOADER_BAUD=$(BOOTLOADER_BAUD) \
  -DRIOT_FLASH_SIZE=$(FLASH_SIZE) \
  -DHAVE_CONFIG_H \
  -MD \
  -MT \
  #

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  LINKFLAGS += -mlongcalls
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/xtensa/$(CPU_FAM) -lxt_hal
endif

ifneq (,$(filter riscv32%,$(TARGET_ARCH)))
  LINKFLAGS += -nostartfiles
  LINKFLAGS += --specs=nosys.specs
endif

# Bootloader link flags. We use the SDK source and linking files instead of the
# RIOT-OS ones to link the bootloader. Note that we also use the unmodified
# SDK libraries.
LINKFLAGS += \
  -Wno-frame-address \
  -o $(ESP_SDK_BOOTLOADER_DIR)/bootloader.elf \
  -Wl,--cref \
  -Wl,--gc-sections \
  -fno-lto \
  -fno-rtti \
  -Wl,--no-warn-rwx-segments \
  $(addprefix -u ,$(ESP_SDK_BOOTLOADER_UNDEF_SYMS)) \
  -Wl,--start-group \
  $(ESP_SDK_BOOTLOADER_OBJS) \
  $(ESP_SDK_BOOTLOADER_ADDOBJS) \
  $(ESP_SDK_BOOTLOADER_ASMOBJS) \
  $(ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS) \
  -T$(ESP_SDK_BOOTLOADER_DIR)/bootloader.memory.ld \
  -T$(ESP_SDK_BOOTLOADER_DIR)/bootloader.sections.ld \
  -Wl,--end-group \
  -Wl,-EL \
  #

# Build the bootloader on the application directory as it depends on the current
# app settings from riotbuild.h.
ESP_SDK_BOOTLOADER_DIR = $(BINDIR)/$(MODULE)
ESP_SDK_BOOTLOADER_BIN = $(ESP_SDK_BOOTLOADER_DIR)/bootloader.bin
ESP_SDK_BOOTLOADER_ELF = $(ESP_SDK_BOOTLOADER_DIR)/bootloader.elf

# Objects that need to be filtered out because they are not part of the SDK
# but are used as submodules and required by the Secure Boot which is not yet
# supported due to a conflict of the packages `tinycrypt` and `micro-ecc`
ESP_SDK_BOOTLOADER_OBJ_FILTER = \
  components/bootloader/subproject/components/micro-ecc/uECC_verify_antifault.o \
  components/bootloader_support/src/secure_boot.o \
  #

ESP_SDK_BOOTLOADER_OBJS = \
  $(addprefix $(ESP_SDK_BOOTLOADER_DIR)/, \
              $(filter-out $(ESP_SDK_BOOTLOADER_OBJ_FILTER), \
                           $(ESP_SDK_BOOTLOADER_SRCS:%.c=%.o)))

ESP_SDK_BOOTLOADER_ASMOBJS = \
  $(addprefix $(ESP_SDK_BOOTLOADER_DIR)/, \
              $(ESP_SDK_BOOTLOADER_ASMSRC:%.S=%.o))

ESP_SDK_BOOTLOADER_ADDOBJS = \
  $(addprefix $(ESP_SDK_BOOTLOADER_DIR)/,stub.o) \

DEPS := $(ESP_SDK_BOOTLOADER_OBJS:%.o=%.d) $(ESP_SDK_BOOTLOADER_ASMOBJS:.o=.d)
-include $(DEPS)

# Main module dependency. We only build the bootloader.bin from this module.
$(MODULE).module: $(ESP_SDK_BOOTLOADER_BIN)

OBJ_DEPS += $(CURDIR)/sdkconfig.h

$(ESP_SDK_BOOTLOADER_OBJS): \
$(ESP_SDK_BOOTLOADER_DIR)/%.o: $(ESP32_SDK_DIR)/%.c $(OBJ_DEPS)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
	  $(CFLAGS) $(INCLUDES) -MMD -MP -c $(abspath $<) -o $@

$(ESP_SDK_BOOTLOADER_ASMOBJS): \
$(ESP_SDK_BOOTLOADER_DIR)/%.o: $(ESP32_SDK_DIR)/%.S
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
	  $(CFLAGS) $(INCLUDES) -MMD -MP -c $(abspath $<) -o $@

$(ESP_SDK_BOOTLOADER_ADDOBJS): \
$(ESP_SDK_BOOTLOADER_DIR)/%.o: $(CURDIR)/stub.c $(OBJ_DEPS)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
	  $(CFLAGS) $(INCLUDES) -MMD -MP -c $(abspath $<) -o $@

$(ESP_SDK_BOOTLOADER_DIR):
	mkdir -p $@

# The bootloader linker scripts of ESP-IDF are templates that have to be
# preprocessed with the bootloader configuration.
ESP_SDK_BOOTLOADER_LD_DIR = $(ESP32_SDK_DIR)/components/bootloader/subproject/main/ld
ESP_SDK_BOOTLOADER_LDS = \
  $(ESP_SDK_BOOTLOADER_DIR)/bootloader.memory.ld \
  $(ESP_SDK_BOOTLOADER_DIR)/bootloader.sections.ld \
  #

$(ESP_SDK_BOOTLOADER_LDS): \
$(ESP_SDK_BOOTLOADER_DIR)/%.ld: $(ESP_SDK_BOOTLOADER_LD_DIR)/$(CPU_FAM)/%.ld.in $(OBJ_DEPS) \
    | $(ESP_SDK_BOOTLOADER_DIR)
	$(Q)$(CC) -C -P -x c -E -include '$(RIOTBUILD_CONFIG_HEADER_C)' \
	  -DBOOTLOADER_BUILD=1 -DNON_OS_BUILD=1 $(INCLUDES) \
	  -I$(ESP_SDK_BOOTLOADER_LD_DIR) $< -o $@

$(ESP_SDK_BOOTLOADER_ELF): \
    $(ESP_SDK_BOOTLOADER_OBJS) $(ESP_SDK_BOOTLOADER_ASMOBJS) \
    $(ESP_SDK_BOOTLOADER_ADDOBJS) $(ESP_SDK_BOOTLOADER_LDS) | $(ESP_SDK_BOOTLOADER_DIR)
	$(Q)$(CC) -o $@ $(LINKFLAGS) -Wl,-Map=$(@:%.elf=%.map)

FLASH_CHIP = $(CPU_FAM)

# TODO: These should be exported/configurable from the app side. That would
# require to export these values.
FLASH_MODE ?= dio   # ESP-IDF uses dio as default flash mode for the bootloader
FLASH_FREQ ?= 40m   # lowest frequency all ESP32x SoC support
FLASH_SIZE ?= 2     # smallesrt size all ESP32x SoC support

# We use esptool to extract a version 1 app from the bootloader.elf. This is
# like the regular objdump binary file but it contains a 16 byte header which
# specifies the flash size, mode and speed that the ROM bootloader uses to load
# this second-stage bootloader image.
# esptool is specified as a BUILDDEP in $(RIOTMAKE)/tools/esptool.inc.mk
$(ESP_SDK_BOOTLOADER_BIN): $(ESP_SDK_BOOTLOADER_ELF)
	$(Q)echo "Generating bootloader image $(ESP_SDK_BOOTLOADER_BIN)"
	$(Q)$(ESPTOOL) --chip $(FLASH_CHIP) elf2image --flash-mode $(FLASH_MODE) \
	  --flash-size $(FLASH_SIZE)MB --flash-freq $(FLASH_FREQ) -o $@ $(ESP_SDK_BOOTLOADER_ELF)
