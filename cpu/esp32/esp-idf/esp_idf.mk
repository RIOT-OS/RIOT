# common definitions for all ESP-IDF modules

# additional include pathes required by ESP-IDF module
INCLUDES += -I$(ESP32_SDK_DIR)/components/bootloader_support/bootloader_flash/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/bootloader_support/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/bootloader_support/private_include
INCLUDES += -I$(ESP32_SDK_DIR)/components/efuse/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/private_include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_app_format/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_gpio/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/ldo/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_mm/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_pm/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_system/port/include/private
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_timer/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_timer/private_include
INCLUDES += -I$(ESP32_SDK_DIR)/components/freertos/esp_additions/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/hal/platform_port/include/hal
INCLUDES += -I$(ESP32_SDK_DIR)/components/hal/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/newlib/priv_include
INCLUDES += -I$(ESP32_SDK_DIR)/components/spi_flash/include

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/xtensa/$(CPU_FAM)/include
endif

ifeq (esp32c6,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/esp_private
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/private_include
endif

ifeq (esp32h2,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/include/esp_private
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/port/$(CPU_FAM)/private_include
endif

ifeq (esp32s3,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/spi_flash/include/spi_flash
endif

ifneq (,$(filter periph_i2c%,$(USEMODULE)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/driver/i2c/include
endif

SRC := $(addprefix $(ESP32_SDK_DIR)/,$(ESP32_SDK_SRC))
SRCXX := $(addprefix $(ESP32_SDK_DIR)/,$(ESP32_SDK_SRCXX))

OBJC_LTO    := $(ESP32_SDK_SRC:%.c=$(ESP32_SDK_BIN)/%.o)
OBJC_NOLTO  := $(ESP32_SDK_SRC_NOLTO:%.c=$(ESP32_SDK_BIN)/%.o)
OBJC        := $(OBJC_NOLTO) $(OBJC_LTO)
OBJCXX      := $(ESP32_SDK_SRCXX:%.$(SRCXXEXT)=$(ESP32_SDK_BIN)/%.o)
ASMOBJ      := $(ESP32_SDK_ASMSRC:%.S=$(ESP32_SDK_BIN)/%.o)

OBJ := $(OBJC) $(OBJCXX) $(ASMOBJ) $(ASSMOBJ) $(GENOBJC)
DEP := $(OBJC:.o=.d) $(OBJCXX:.o=.d) $(ASSMOBJ:.o=.d)

$(MODULE).module compile-commands $(OBJ): | $(BINDIR)/$(MODULE)/

$(MODULE).module: $(OBJ) $(ESP32_LD_DST) \
    $(if $(OBJECTS_TO_REMOVE),$(MODULE).cleanup) | $(DIRS:%=ALL--%)

$(OBJC): $(BINDIR)/$(MODULE)/%.o: $(ESP32_SDK_DIR)/%.c $(OBJ_DEPS) \
            | $(if $(SHOULD_RUN_KCONFIG),$(KCONFIG_GENERATED_AUTOCONF_HEADER_C))
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
		-DRIOT_FILE_RELATIVE=\"$(patsubst $(RIOTBASE)/%,%,$(abspath $<))\" \
		-DRIOT_FILE_NOPATH=\"$(notdir $<)\" \
		$(CFLAGS) $(INCLUDES) -MQ '$@' -MD -MP -c $(abspath $<) -o $@
ifneq (,$(SHOULD_RUN_KCONFIG))
	$(Q)$(FIXDEP) $(@:.o=.d) $@ $(KCONFIG_SYNC_DIR) > $(@:.o=.tmp)
	$(Q)mv $(@:.o=.tmp) $(@:.o=.d)
endif

$(OBJCXX): $(BINDIR)/$(MODULE)/%.o: $(ESP32_SDK_DIR)/%.$(SRCXXEXT) $(OBJ_DEPS) \
            | $(if $(SHOULD_RUN_KCONFIG),$(KCONFIG_GENERATED_AUTOCONF_HEADER_C))
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CXX) \
		-DRIOT_FILE_RELATIVE=\"$(patsubst $(RIOTBASE)/%,%,$(abspath $<))\" \
		-DRIOT_FILE_NOPATH=\"$(notdir $<)\" \
		$(CXXFLAGS) $(CXXINCLUDES) $(INCLUDES) -MQ '$@' -MD -MP -c $(abspath $<) -o $@
ifneq (,$(SHOULD_RUN_KCONFIG))
	$(Q)$(FIXDEP) $(@:.o=.d) $@ $(KCONFIG_SYNC_DIR) > $(@:.o=.tmp)
	$(Q)mv $(@:.o=.tmp) $(@:.o=.d)
endif

$(ASMOBJ): $(BINDIR)/$(MODULE)/%.o: $(ESP32_SDK_DIR)/%.S $(OBJ_DEPS) \
            | $(if $(SHOULD_RUN_KCONFIG),$(KCONFIG_GENERATED_AUTOCONF_HEADER_C))
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CCACHE) $(CC) \
		-DRIOT_FILE_RELATIVE=\"$(patsubst $(RIOTBASE)/%,%,$(abspath $<))\" \
		-DRIOT_FILE_NOPATH=\"$(notdir $<)\" \
		$(CFLAGS) $(INCLUDES) -MQ '$@' -MD -MP -c $(abspath $<) -o $@
ifneq (,$(SHOULD_RUN_KCONFIG))
	$(Q)$(FIXDEP) $(@:.o=.d) $@ $(KCONFIG_SYNC_DIR) > $(@:.o=.tmp)
	$(Q)mv $(@:.o=.tmp) $(@:.o=.d)
endif

-include $(DEP)
