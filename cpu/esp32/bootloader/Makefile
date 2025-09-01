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

# List of bootloader sources determined by esp-idf/examples/get-started/hello-world
# TODO HowTo generate this list
ESP_SDK_BOOTLOADER_SRCS = \
  components/bootloader/subproject/main/bootloader_start.c \
  components/bootloader/subproject/components/micro-ecc/uECC_verify_antifault.c \
  components/bootloader_support/bootloader_flash/src/bootloader_flash.c \
  components/bootloader_support/bootloader_flash/src/bootloader_flash_config_$(CPU_FAM).c \
  components/bootloader_support/bootloader_flash/src/flash_qio_mode.c \
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
  components/bootloader_support/src/bootloader_utility.c \
  components/bootloader_support/src/$(CPU_FAM)/bootloader_$(CPU_FAM).c \
  components/bootloader_support/src/$(CPU_FAM)/bootloader_sha.c \
  components/bootloader_support/src/$(CPU_FAM)/bootloader_soc.c \
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
  components/esp_rom/patches/esp_rom_spiflash.c \
  components/esp_rom/patches/esp_rom_sys.c \
  components/esp_rom/patches/esp_rom_uart.c \
  components/esp_system/esp_err.c \
  components/hal/efuse_hal.c \
  components/hal/$(CPU_FAM)/efuse_hal.c \
  components/hal/hal_utils.c \
  components/hal/mmu_hal.c \
  components/log/src/log_timestamp_common.c \
  components/log/src/noos/log_lock.c \
  components/log/src/noos/log_timestamp.c \
  components/soc/dport_access_common.c \
  components/soc/$(CPU_FAM)/adc_periph.c \
  components/soc/$(CPU_FAM)/gpio_periph.c \
  components/soc/$(CPU_FAM)/i2c_periph.c \
  components/soc/$(CPU_FAM)/i2s_periph.c \
  components/soc/$(CPU_FAM)/interrupts.c \
  components/soc/$(CPU_FAM)/ledc_periph.c \
  components/soc/$(CPU_FAM)/mpi_periph.c \
  components/soc/$(CPU_FAM)/rmt_periph.c \
  components/soc/$(CPU_FAM)/sdm_periph.c \
  components/soc/$(CPU_FAM)/spi_periph.c \
  components/soc/$(CPU_FAM)/timer_periph.c \
  components/soc/$(CPU_FAM)/twai_periph.c \
  components/soc/$(CPU_FAM)/uart_periph.c \
  components/soc/$(CPU_FAM)/wdt_periph.c \
  components/soc/lldesc.c \
  components/spi_flash/spi_flash_wrap.c \
  #

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  ESP_SDK_BOOTLOADER_ASMSRC = components/esp_rom/patches/esp_rom_longjmp.S
  ESP_SDK_BOOTLOADER_SRCS += components/xtensa/eri.c
  ESP_SDK_BOOTLOADER_SRCS += components/xtensa/xt_trax.c
else
endif

ifneq (,$(filter esp32 esp32s2 esp32s3 esp32c3,$(CPU_FAM)))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/esp_hw_support/port/$(CPU_FAM)/rtc_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/rtc_sleep.c \
    #
endif

ifeq (esp32,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/src/efuse_controller/keys/without_key_purposes/three_key_blocks/esp_efuse_api_key.c \
    components/hal/$(CPU_FAM)/cache_hal_$(CPU_FAM).c \
    components/hal/mpu_hal.c \
    components/hal/wdt_hal_iram.c \
    components/soc/$(CPU_FAM)/dac_periph.c \
    components/soc/$(CPU_FAM)/dport_access.c \
    components/soc/$(CPU_FAM)/emac_periph.c \
    components/soc/$(CPU_FAM)/lcd_periph.c \
    components/soc/$(CPU_FAM)/mcpwm_periph.c \
    components/soc/$(CPU_FAM)/pcnt_periph.c \
    components/soc/$(CPU_FAM)/rtc_io_periph.c \
    components/soc/$(CPU_FAM)/sdio_slave_periph.c \
    components/soc/$(CPU_FAM)/sdmmc_periph.c \
    components/soc/$(CPU_FAM)/touch_sensor_periph.c \
    #
else ifeq (esp32s2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_table.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_rom/$(CPU_FAM)/usb_patches.c \
    components/esp_rom/patches/esp_rom_cache_esp32s2_esp32s3.c \
    components/esp_rom/patches/esp_rom_regi2c_$(CPU_FAM).c \
    components/hal/cache_hal.c \
    components/hal/mpu_hal.c \
    components/hal/wdt_hal_iram.c \
    components/esp_rom/patches/esp_rom_systimer.c \
    components/soc/$(CPU_FAM)/dedic_gpio_periph.c \
    components/soc/$(CPU_FAM)/lcd_periph.c \
    components/soc/$(CPU_FAM)/pcnt_periph.c \
    components/soc/$(CPU_FAM)/rtc_io_periph.c \
    components/soc/$(CPU_FAM)/temperature_sensor_periph.c \
    components/soc/$(CPU_FAM)/touch_sensor_periph.c \
    components/soc/$(CPU_FAM)/usb_dwc_periph.c \
    #
else ifeq (esp32s3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_ASMSRC += \
    components/esp_rom/patches/esp_rom_cache_writeback_$(CPU_FAM).S
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_rom/patches/esp_rom_cache_esp32s2_esp32s3.c \
    components/esp_rom/patches/esp_rom_systimer.c \
    components/esp_rom/patches/esp_rom_wdt.c \
    components/hal/cache_hal.c \
    components/hal/mpu_hal.c \
    components/soc/$(CPU_FAM)/dedic_gpio_periph.c \
    components/soc/$(CPU_FAM)/gdma_periph.c \
    components/soc/$(CPU_FAM)/lcd_periph.c \
    components/soc/$(CPU_FAM)/mcpwm_periph.c \
    components/soc/$(CPU_FAM)/pcnt_periph.c \
    components/soc/$(CPU_FAM)/rtc_io_periph.c \
    components/soc/$(CPU_FAM)/sdmmc_periph.c \
    components/soc/$(CPU_FAM)/temperature_sensor_periph.c \
    components/soc/$(CPU_FAM)/touch_sensor_periph.c \
    components/soc/$(CPU_FAM)/usb_dwc_periph.c \
    #
else ifeq (esp32c3,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_rom/patches/esp_rom_systimer.c \
    components/hal/cache_hal.c \
    components/hal/wdt_hal_iram.c \
    components/soc/$(CPU_FAM)/dedic_gpio_periph.c \
    components/soc/$(CPU_FAM)/gdma_periph.c \
    components/soc/$(CPU_FAM)/temperature_sensor_periph.c \
    #
else ifeq (esp32c6,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_param.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_sleep.c \
    components/esp_rom/patches/esp_rom_hp_regi2c_$(CPU_FAM).c \
    components/esp_rom/patches/esp_rom_systimer.c \
    components/esp_rom/patches/esp_rom_wdt.c \
    components/hal/cache_hal.c \
    components/hal/wdt_hal_iram.c \
    components/hal/lp_timer_hal.c \
    #
ESP_SDK_BOOTLOADER_SRCS += components/bootloader_support/src/$(CPU_FAM)/bootloader_ecdsa.c

else ifeq (esp32h2,$(CPU_FAM))
  ESP_SDK_BOOTLOADER_SRCS += \
    components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
    components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_init.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_param.c \
    components/esp_hw_support/port/$(CPU_FAM)/pmu_sleep.c \
    components/esp_rom/patches/esp_rom_regi2c_$(CPU_FAM).c \
    components/esp_rom/patches/esp_rom_systimer.c \
    components/esp_rom/patches/esp_rom_wdt.c \
    components/hal/cache_hal.c \
    components/hal/lp_timer_hal.c \
    components/hal/wdt_hal_iram.c \
    #
else
  $(error Missing make configuration for ESP32x SoC variant (family): $(CPU_FAM))
endif

# Bootloader sdkconfig.h defined in CURDIR directory.
INCLUDES = \
  -I$(dir $(RIOTBUILD_CONFIG_HEADER_C)) \
  -I$(CURDIR) \
  -I$(ESP32_SDK_DIR)/components \
  -I$(ESP32_SDK_DIR)/components/bootloader/subproject/components/micro-ecc \
  -I$(ESP32_SDK_DIR)/components/bootloader/subproject/components/micro-ecc/micro-ecc \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/bootloader_flash/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/private_include \
  -I$(ESP32_SDK_DIR)/components/efuse/include \
  -I$(ESP32_SDK_DIR)/components/efuse/private_include \
  -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/private_include \
  -I$(ESP32_SDK_DIR)/components/esp_app_format/include \
  -I$(ESP32_SDK_DIR)/components/esp_bootloader_format/include \
  -I$(ESP32_SDK_DIR)/components/esp_common/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/esp_private \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/include/ \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/private_include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/include/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/esp_rom/include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/include/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/hal/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/hal/include \
  -I$(ESP32_SDK_DIR)/components/hal/platform_port/include \
  -I$(ESP32_SDK_DIR)/components/log/include \
  -I$(ESP32_SDK_DIR)/components/newlib/platform_include \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM) \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/include \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/register \
  -I$(ESP32_SDK_DIR)/components/soc/include \
  -I$(ESP32_SDK_DIR)/components/spi_flash/include \
  -I$(ESP32_SDK_DIR)/components/spi_flash/include/spi_flash \
  #

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

ifneq (,$(filter esp32,$(CPU_FAM)))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.newlib-funcs.ld \
    #
else ifneq (,$(filter esp32s2,$(CPU_FAM)))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.newlib-funcs.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.spiflash_legacy.ld \
    #
else ifneq (,$(filter esp32s3,$(CPU_FAM)))
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.newlib.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.wdt.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.version.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.bt_funcs.ld \
    #
else ifneq (,$(filter esp32c3,$(CPU_FAM)))
  LINKFLAGS += -nostartfiles
  LINKFLAGS += --specs=nosys.specs
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.newlib.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.version.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.bt_funcs.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.eco3.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.eco3_bt_funcs.ld \
    #
else ifneq (,$(filter esp32c6,$(CPU_FAM)))
  LINKFLAGS += -nostartfiles
  LINKFLAGS += --specs=nosys.specs
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.newlib.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.version.ld \
    #
else ifneq (,$(filter esp32h2,$(CPU_FAM)))
  LINKFLAGS += -nostartfiles
  LINKFLAGS += --specs=nosys.specs
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.newlib.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.version.ld \
    #
else
  $(error Missing make configuration for ESP32x SoC variant (family): $(CPU_FAM))
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
  -u __assert_func \
  -u abort \
  -u __ubsan_include \
  -u bootloader_hooks_include \
  -Wl,--start-group \
  $(ESP_SDK_BOOTLOADER_OBJS) \
  $(ESP_SDK_BOOTLOADER_ADDOBJS) \
  $(ESP_SDK_BOOTLOADER_ASMOBJS) \
  $(ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS) \
  -T$(ESP32_SDK_DIR)/components/bootloader/subproject/main/ld/$(CPU_FAM)/bootloader.ld \
  -T$(ESP32_SDK_DIR)/components/bootloader/subproject/main/ld/$(CPU_FAM)/bootloader.rom.ld \
  -T$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/ld/$(CPU_FAM).peripherals.ld \
  -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.ld \
  -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.api.ld \
  -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)/ld/$(CPU_FAM).rom.libgcc.ld \
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

$(ESP_SDK_BOOTLOADER_ELF): \
    $(ESP_SDK_BOOTLOADER_OBJS) $(ESP_SDK_BOOTLOADER_ASMOBJS) \
    $(ESP_SDK_BOOTLOADER_ADDOBJS) | $(ESP_SDK_BOOTLOADER_DIR)
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
$(ESP_SDK_BOOTLOADER_BIN): esptool $(ESP_SDK_BOOTLOADER_ELF)
	$(Q)echo "Generating bootloader image $(ESP_SDK_BOOTLOADER_BIN)"
	$(Q)$(ESPTOOL) --chip $(FLASH_CHIP) elf2image --flash-mode $(FLASH_MODE) \
	  --flash-size $(FLASH_SIZE)MB --flash-freq $(FLASH_FREQ) -o $@ $(ESP_SDK_BOOTLOADER_ELF)
