# Bootloader binary blob
MODULE = esp_bootloader

# We are compiling the bootloader from the ESP32 SDK sources, so we
# can't use the automatic module SRC discovery rules. Also, the compiled code
# would not be linked into the application, instead it is linked into its own
# binary file.
NO_AUTO_SRC = 1

include $(RIOTBASE)/Makefile.base

# Bootloader baudrate, set to the application defined one if any or the default
# in serial.inc.mk
BOOTLOADER_BAUD ?= $(BAUD)

# List of bootloader sources determined by esp-idf/examples/get-started/hello-world
# TODO HowTo generate this list
ESP_SDK_BOOTLOADER_SRCS = \
  components/bootloader/subproject/main/bootloader_start.c \
  components/bootloader/subproject/components/micro-ecc/uECC_verify_antifault.c \
  components/bootloader_support/src/bootloader_clock_init.c \
  components/bootloader_support/src/bootloader_clock_loader.c \
  components/bootloader_support/src/bootloader_common.c \
  components/bootloader_support/src/bootloader_common_loader.c \
  components/bootloader_support/src/bootloader_console.c \
  components/bootloader_support/src/bootloader_console_loader.c \
  components/bootloader_support/src/bootloader_efuse_$(CPU).c \
  components/bootloader_support/src/bootloader_flash.c \
  components/bootloader_support/src/bootloader_flash_config_$(CPU).c \
  components/bootloader_support/src/bootloader_init.c \
  components/bootloader_support/src/bootloader_mem.c \
  components/bootloader_support/src/bootloader_panic.c \
  components/bootloader_support/src/bootloader_random.c \
  components/bootloader_support/src/bootloader_random_$(CPU).c \
  components/bootloader_support/src/bootloader_utility.c \
  components/bootloader_support/src/$(CPU)/bootloader_$(CPU).c \
  components/bootloader_support/src/$(CPU)/bootloader_sha.c \
  components/bootloader_support/src/$(CPU)/bootloader_soc.c \
  components/bootloader_support/src/esp_image_format.c \
  components/bootloader_support/src/flash_encrypt.c \
  components/bootloader_support/src/flash_partitions.c \
  components/bootloader_support/src/flash_qio_mode.c \
  components/bootloader_support/src/secure_boot.c \
  components/efuse/$(CPU)/esp_efuse_fields.c \
  components/efuse/$(CPU)/esp_efuse_table.c \
  components/efuse/$(CPU)/esp_efuse_utility.c \
  components/efuse/src/esp_efuse_api.c \
  components/efuse/src/esp_efuse_fields.c \
  components/efuse/src/esp_efuse_utility.c \
  components/esp_common/src/esp_err_to_name.c \
  components/esp_hw_support/compare_set.c \
  components/esp_hw_support/cpu_util.c \
  components/esp_hw_support/port/$(CPU)/chip_info.c \
  components/esp_hw_support/port/$(CPU)/rtc_clk.c \
  components/esp_hw_support/port/$(CPU)/rtc_clk_init.c \
  components/esp_hw_support/port/$(CPU)/rtc_init.c \
  components/esp_hw_support/port/$(CPU)/rtc_pm.c \
  components/esp_hw_support/port/$(CPU)/rtc_sleep.c \
  components/esp_hw_support/port/$(CPU)/rtc_time.c \
  components/esp_rom/patches/esp_rom_crc.c \
  components/esp_rom/patches/esp_rom_sys.c \
  components/esp_rom/patches/esp_rom_tjpgd.c \
  components/esp_rom/patches/esp_rom_uart.c \
  components/esp_system/esp_err.c \
  components/hal/cpu_hal.c \
  components/hal/mpu_hal.c \
  components/hal/wdt_hal_iram.c \
  components/log/log_buffers.c \
  components/log/log.c \
  components/log/log_noos.c \
  components/newlib/syscalls.c \
  components/soc/$(CPU)/adc_periph.c \
  components/soc/$(CPU)/gpio_periph.c \
  components/soc/$(CPU)/i2c_periph.c \
  components/soc/$(CPU)/i2s_periph.c \
  components/soc/$(CPU)/interrupts.c \
  components/soc/$(CPU)/ledc_periph.c \
  components/soc/$(CPU)/rmt_periph.c \
  components/soc/$(CPU)/sigmadelta_periph.c \
  components/soc/$(CPU)/spi_periph.c \
  components/soc/$(CPU)/timer_periph.c \
  components/soc/$(CPU)/uart_periph.c \
  components/soc/lldesc.c \
  components/spi_flash/$(CPU)/spi_flash_rom_patch.c \
  #

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  ESP_SDK_BOOTLOADER_SRCS += components/esp_hw_support/port/$(CPU)/rtc_wdt.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/lcd_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/pcnt_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/rtc_io_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/touch_sensor_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/xtensa/eri.c
  ESP_SDK_BOOTLOADER_SRCS += components/xtensa/xt_trax.c
else
  ESP_SDK_BOOTLOADER_SRCS += components/esp_hw_support/port/$(CPU)/cpu_util_$(CPU).c
endif

ifneq (,$(filter esp32 esp32s2,$(CPU)))
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/dac_periph.c
endif

ifneq (,$(filter esp32 esp32s3,$(CPU)))
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/mcpwm_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/sdio_slave_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/sdmmc_periph.c
endif

ifneq (,$(filter esp32c3 esp32h2 esp32s3,$(CPU)))
  ESP_SDK_BOOTLOADER_SRCS += components/efuse/$(CPU)/esp_efuse_rtc_calib.c
endif

ifneq (,$(filter esp32,$(CPU)))
  ESP_SDK_BOOTLOADER_SRCS += components/efuse/src/esp_efuse_api_key_esp32.c
  ESP_SDK_BOOTLOADER_ASMSRC = components/esp_rom/patches/esp_rom_longjmp.S
else
  ESP_SDK_BOOTLOADER_SRCS += components/efuse/src/esp_efuse_api_key_esp32xx.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/dedic_gpio_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/$(CPU)/gdma_periph.c
  ESP_SDK_BOOTLOADER_SRCS += components/soc/soc_include_legacy_warn.c
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
  -I$(ESP32_SDK_DIR)/components/bootloader_support/include_bootloader \
  -I$(ESP32_SDK_DIR)/components/efuse/include \
  -I$(ESP32_SDK_DIR)/components/efuse/private_include \
  -I$(ESP32_SDK_DIR)/components/efuse/$(CPU)/include \
  -I$(ESP32_SDK_DIR)/components/efuse/$(CPU)/private_include \
  -I$(ESP32_SDK_DIR)/components/esp_common/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/soc/$(CPU) \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU) \
  -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU)/private_include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU) \
  -I$(ESP32_SDK_DIR)/components/esp_rom/include \
  -I$(ESP32_SDK_DIR)/components/esp_rom/include/$(CPU) \
  -I$(ESP32_SDK_DIR)/components/hal/$(CPU)/include \
  -I$(ESP32_SDK_DIR)/components/hal/include \
  -I$(ESP32_SDK_DIR)/components/hal/platform_port/include \
  -I$(ESP32_SDK_DIR)/components/log/include \
  -I$(ESP32_SDK_DIR)/components/newlib/platform_include \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU)/. \
  -I$(ESP32_SDK_DIR)/components/soc/$(CPU)/include \
  -I$(ESP32_SDK_DIR)/components/soc/include \
  -I$(ESP32_SDK_DIR)/components/spi_flash/include \
  -I$(ESP32_SDK_DIR)/components/spi_flash/include/spi_flash \
  #

CFLAGS = -include '$(RIOTBUILD_CONFIG_HEADER_C)' \

ifneq (,$(filter riscv32%,$(TARGET_ARCH)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/riscv/include
  CFLAGS += -DCONFIG_IDF_TARGET_ARCH_RISCV
  CFLAGS += -march=rv32imc
  CFLAGS += -Wno-error=format=
  CFLAGS += -nostartfiles
  CFLAGS += -Wno-format
endif

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/$(CPU)/include
  CFLAGS += -mlongcalls -mtext-section-literals
endif

# BOOTLOADER_BUILD=1 signals to the SDK that's a bootloader build.
CFLAGS += \
  -Wno-frame-address \
  -ffunction-sections \
  -fdata-sections \
  -Wall \
  -Werror=all \
  -Wno-error=unused-function \
  -Wno-error=unused-variable \
  -Wno-error=deprecated-declarations \
  -Wextra \
  -Wno-unused-parameter \
  -Wno-sign-compare \
  -ggdb \
  -Os \
  -freorder-blocks \
  -fstrict-volatile-bitfields \
  -Wno-error=unused-but-set-variable \
  -fno-jump-tables \
  -fno-tree-switch-conversion \
  -fno-stack-protector \
  -std=gnu99 \
  -Wno-old-style-declaration \
  -D_GNU_SOURCE \
  -DESP_PLATFORM \
  -DBOOTLOADER_BUILD=1 \
  -DRIOT_BOOTLOADER_BAUD=$(BOOTLOADER_BAUD) \
  -DRIOT_FLASH_SIZE=$(FLASH_SIZE) \
  -DWITH_POSIX \
  -DHAVE_CONFIG_H \
  -MD \
  -MT \
  #

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  LINKFLAGS = -mlongcalls
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += -L$(ESP32_SDK_DIR)/components/xtensa/$(CPU) -lxt_hal
endif

ifneq (,$(filter esp32 esp32s2,$(CPU)))
  LINKFLAGS =
  ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS += \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU)/ld/$(CPU).rom.newlib-funcs.ld \
    -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU)/ld/$(CPU).rom.spiflash.ld \
    #
endif

# Bootloader link flags. We use the SDK source and linking files instead of the
# RIOT-OS ones to link the bootloader. Note that we also use the unmodified
# SDK libraries.
LINKFLAGS += \
  -Wno-frame-address \
  -o $(ESP_SDK_BOOTLOADER_DIR)/bootloader.elf \
  -Wl,--cref \
  -Wl,--defsym=IDF_TARGET_ESP32=0 \
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
  -Lgcc \
  $(ESP_SDK_BOOTLOADER_ADD_LINK_FLAGS) \
  -T$(ESP32_SDK_DIR)/components/bootloader/subproject/main/ld/$(CPU)/bootloader.ld \
  -T$(ESP32_SDK_DIR)/components/bootloader/subproject/main/ld/$(CPU)/bootloader.rom.ld \
  -T$(ESP32_SDK_DIR)/components/soc/$(CPU)/ld/$(CPU).peripherals.ld \
  -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU)/ld/$(CPU).rom.ld \
  -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU)/ld/$(CPU).rom.api.ld \
  -T$(ESP32_SDK_DIR)/components/esp_rom/$(CPU)/ld/$(CPU).rom.libgcc.ld \
  -Wl,--end-group \
  -Wl,-EL \

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
  components/bootloader_support/src/flash_encrypt.o \
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
	$(Q)$(AS) $(ASFLAGS) -o $@ $(abspath $<)

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

FLASH_CHIP = $(CPU)
ESPTOOL ?= $(RIOTTOOLS)/esptools/esptool_v3.2.py
# TODO: These should be exported/configurable from the app side. That would
# require to export these values.
FLASH_MODE ?= dout
FLASH_FREQ ?= 40m
FLASH_SIZE ?= 4

# We use esptool to extract a version 1 app from the bootloader.elf. This is
# like the regular objdump binary file but it contains a 16 byte header which
# specifies the flash size, mode and speed that the ROM bootloader uses to load
# this second-stage bootloader image.
$(ESP_SDK_BOOTLOADER_BIN): $(ESP_SDK_BOOTLOADER_ELF)
	$(Q)$(ESPTOOL) --chip $(FLASH_CHIP) elf2image --flash_mode $(FLASH_MODE) \
	  --flash_size $(FLASH_SIZE)MB --flash_freq $(FLASH_FREQ) -o $@ $<
