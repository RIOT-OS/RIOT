# Bootloader binary blob
MODULE := esp_bootloader

# We are compiling the bootloader from the ESP8266_RTOS_SDK_DIR sources, so we
# can't use the automatic module SRC discovery rules. Also, the compiled code
# would not be linked into the application, instead it is linked into its own
# binary file.
NO_AUTO_SRC = 1

include $(RIOTBASE)/Makefile.base
include $(RIOTMAKE)/tools/esptool.inc.mk

# List of bootloader sources.
include $(CURDIR)/bootloader.inc.mk

# Bootloader baudrate, set to the application defined one if any or the default
# in serial.inc.mk
BOOTLOADER_BAUD ?= $(BAUD)

# Bootloader sdkconfig.h defined in CURDIR directory.
INCLUDES = \
  -I$(dir $(RIOTBUILD_CONFIG_HEADER_C)) \
  -I$(ESP8266_RTOS_SDK_DIR)/components/bootloader_support/include \
  -I$(ESP8266_RTOS_SDK_DIR)/components/bootloader_support/include_priv \
  -I$(ESP8266_RTOS_SDK_DIR)/components/esp8266/include \
  -I$(ESP8266_RTOS_SDK_DIR)/components/heap/include \
  -I$(ESP8266_RTOS_SDK_DIR)/components/heap/port/esp8266/include \
  -I$(ESP8266_RTOS_SDK_DIR)/components/log/include/ \
  -I$(ESP8266_RTOS_SDK_DIR)/components/spi_flash/include \
  -I$(ESP8266_RTOS_SDK_DIR)/components/util/include \
  -I$(ESP8266_RTOS_SDK_DIR)/build-libs \
  -I$(CURDIR)
  #

# BOOTLOADER_BUILD=1 signals to the SDK that's a bootloader build.
CFLAGS = \
  -DBOOTLOADER_BUILD=1 \
  -DESP_PLATFORM \
  -DRIOT_BOOTLOADER_BAUD=$(BOOTLOADER_BAUD) \
  -DRIOT_FLASH_SIZE=$(FLASH_SIZE) \
  -O2 \
  -ffunction-sections \
  -fdata-sections \
  -fstrict-volatile-bitfields \
  -mlongcalls \
  #

# Bootloader link flags. We use the SDK source and linking files instead of the
# RIOT-OS ones to link the bootloader. Note that we also use the unmodified
# SDK libraries.
LINKFLAGS = \
  -nostdlib \
  -u call_user_start_cpu0 \
  -Wl,--gc-sections \
  -Wl,-static \
  -Wl,-EL \
  -Wl,--start-group \
  $(ESP_SDK_BOOTLOADER_OBJS) \
  -Wl,--end-group \
  -lgcc \
  -lstdc++ \
  -lgcov \
  -L$(ESP8266_RTOS_SDK_DIR)/components/esp8266/lib \
  -lcore \
  -T$(ESP8266_RTOS_SDK_DIR)/components/esp8266/ld/esp8266.rom.ld \
  -L$(ESP8266_RTOS_SDK_DIR)/components/bootloader/subproject/main \
  -Tesp8266.bootloader.ld \
  -Tesp8266.bootloader.rom.ld \

# Build the bootloader on the application directory as it depends on the current
# app settings from riotbuild.h.
ESP_SDK_BOOTLOADER_DIR = $(BINDIR)/$(MODULE)
ESP_SDK_BOOTLOADER_BIN = $(ESP_SDK_BOOTLOADER_DIR)/bootloader.bin
ESP_SDK_BOOTLOADER_ELF = $(ESP_SDK_BOOTLOADER_DIR)/bootloader.elf

ESP_SDK_BOOTLOADER_OBJS = \
  $(addprefix $(ESP_SDK_BOOTLOADER_DIR)/,$(ESP_SDK_BOOTLOADER_SRCS:%.c=%.o))

DEPS := $(ESP_SDK_BOOTLOADER_OBJS:%.o=%.d)
-include $(DEPS)

# Main module dependency. We only build the bootloader.bin from this module.
$(MODULE).module: $(ESP_SDK_BOOTLOADER_BIN)

OBJ_DEPS += $(CURDIR)/sdkconfig.h

$(ESP_SDK_BOOTLOADER_DIR)/%.o: $(ESP8266_RTOS_SDK_DIR)/%.c $(OBJ_DEPS)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
	  $(CFLAGS) $(INCLUDES) -MQ '$@' -MD -MP -c -o $@ $(abspath $<)

$(ESP_SDK_BOOTLOADER_DIR):
	mkdir -p $@

$(ESP_SDK_BOOTLOADER_ELF): $(ESP_SDK_BOOTLOADER_OBJS) \
    | $(ESP_SDK_BOOTLOADER_DIR)
	$(Q)$(CC) -o $@ $(LINKFLAGS) -Wl,-Map=$(@:%.elf=%.map)

FLASH_CHIP = esp8266

# TODO: These should be exported/configurable from the app side. That would
# require to export these values.
FLASH_MODE ?= dout
FLASH_FREQ ?= 40m
FLASH_SIZE ?= 4

# We use esptool to extract a version 1 app from the bootloader.elf. This is
# like the regular objdump binary file but it contains a 16 byte header which
# specifies the flash size, mode and speed that the ROM bootloader uses to load
# this second-stage bootloader image.
$(ESP_SDK_BOOTLOADER_BIN): esptool $(ESP_SDK_BOOTLOADER_ELF)
	$(Q)echo "Generating bootloader image $(ESP_SDK_BOOTLOADER_BIN)"
	$(Q)$(ESPTOOL) --chip $(FLASH_CHIP) elf2image --flash_mode $(FLASH_MODE) \
	  --flash_size $(FLASH_SIZE)MB --flash_freq $(FLASH_FREQ) -o $@ $(ESP_SDK_BOOTLOADER_ELF)
	$(Q)mv $(@)0x00000.bin $@
