# Bootloader binary blob
MODULE := esp_bootloader

# We are compiling the bootloader from the ESP32 SDK sources, so we
# can't use the automatic module SRC discovery rules. Also, the compiled code
# would not be linked into the application, instead it is linked into its own
# binary file.
NO_AUTO_SRC = 1

include $(RIOTBASE)/Makefile.base

# List of bootloader sources.
include $(CURDIR)/bootloader.inc.mk

# Bootloader baudrate, set to the application defined one if any or the default
# in serial.inc.mk
BOOTLOADER_BAUD ?= $(BAUD)

# Bootloader sdkconfig.h defined in CURDIR directory.
INCLUDES = \
  -I$(dir $(RIOTBUILD_CONFIG_HEADER_C)) \
  -I$(CURDIR) \
  -I$(ESP32_SDK_DIR)/components \
  -I$(ESP32_SDK_DIR)/components/esp32/include \
  -I$(ESP32_SDK_DIR)/components/soc/esp32/include \
  -I$(ESP32_SDK_DIR)/components/soc/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/include \
  -I$(ESP32_SDK_DIR)/components/bootloader_support/include_priv \
  -I$(ESP32_SDK_DIR)/components/log/include \
  -I$(ESP32_SDK_DIR)/components/spi_flash/include \
  #

# BOOTLOADER_BUILD=1 signals to the SDK that's a bootloader build.
CFLAGS = \
  -DBOOTLOADER_BUILD=1 \
  -DESP_PLATFORM \
  -DRIOT_BOOTLOADER_BAUD=$(BOOTLOADER_BAUD) \
  -DRIOT_FLASH_SIZE=$(FLASH_SIZE) \
  -DWITH_POSIX \
  -DHAVE_CONFIG_H \
  -std=gnu99 \
  -Og \
  -ggdb \
  -ffunction-sections \
  -fdata-sections \
  -fstrict-volatile-bitfields \
  -mlongcalls \
  -nostdlib \
  -Wall \
  -Werror=all \
  -Wno-error=unused-function \
  -Wno-error=unused-but-set-variable \
  -Wno-error=unused-variable \
  -Wno-error=deprecated-declarations \
  -Wextra \
  -Wno-unused-parameter \
  -Wno-sign-compare \
  -Wno-old-style-declaration \
  #

# Bootloader link flags. We use the SDK source and linking files instead of the
# RIOT-OS ones to link the bootloader. Note that we also use the unmodified
# SDK libraries.
LINKFLAGS = \
  -nostdlib \
  -u call_user_start_cpu0 \
  -Wl,--gc-sections \
  -Wl,-static \
  -Wl,--start-group \
  $(ESP_SDK_BOOTLOADER_OBJS) \
  -lgcc \
  -lstdc++ \
  -lgcov \
  -T$(ESP32_SDK_DIR)/components/bootloader/subproject/main/esp32.bootloader.ld \
  -T$(ESP32_SDK_DIR)/components/bootloader/subproject/main/esp32.bootloader.rom.ld \
  -T$(ESP32_SDK_DIR)/components/esp32/ld/esp32.rom.ld \
  -T$(ESP32_SDK_DIR)/components/esp32/ld/esp32.rom.spiram_incompatible_fns.ld \
  -T$(ESP32_SDK_DIR)/components/esp32/ld/esp32.peripherals.ld \
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
  components/bootloader_support/src/secure_boot.o \
  components/bootloader_support/src/secure_boot_signatures.o \
  components/micro-ecc/micro-ecc/uECC.o

ESP_SDK_BOOTLOADER_OBJS = \
  $(addprefix $(ESP_SDK_BOOTLOADER_DIR)/, \
              $(filter-out $(ESP_SDK_BOOTLOADER_OBJ_FILTER), \
                           $(ESP_SDK_BOOTLOADER_SRCS:%.c=%.o)))

DEPS := $(ESP_SDK_BOOTLOADER_OBJS:%.o=%.d)
-include $(DEPS)

# ESP32 SDK submodules required for compilation of bootloader
ESP32_SDK_SUBMODULES = $(ESP32_SDK_DIR)/components/micro-ecc/micro-ecc
ESP32_SDK_SUBMODULES_GIT = $(addprefix $(ESP32_SDK_SUBMODULES),/.git)

# Main module dependency. We only build the bootloader.bin from this module.
$(MODULE).module: $(ESP_SDK_BOOTLOADER_BIN)

OBJ_DEPS += $(CURDIR)/sdkconfig.h

$(ESP_SDK_BOOTLOADER_DIR)/%.o: $(ESP32_SDK_DIR)/%.c $(OBJ_DEPS)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
	  $(CFLAGS) $(INCLUDES) -MMD -MP -c $(abspath $<) -o $@

$(ESP_SDK_BOOTLOADER_DIR):
	mkdir -p $@

$(ESP_SDK_BOOTLOADER_ELF): $(ESP_SDK_BOOTLOADER_OBJS) $(ESP_SDK_BOOTLOADER_SUPPORT_AR) \
    | $(ESP_SDK_BOOTLOADER_DIR)
	$(Q)$(CC) -o $@ $(LINKFLAGS) -Wl,-Map=$(@:%.elf=%.map)

FLASH_CHIP = esp32
ESPTOOL ?= $(RIOTTOOLS)/esptool/esptool.py
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
