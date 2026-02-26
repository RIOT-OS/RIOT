BOARD ?= samr21-xpro
include ../Makefile.periph_common

SOFT_I2C ?= 0
PIN_SCL ?= GPIO_PIN(0,27)
PIN_SDA ?= GPIO_PIN(0,26)

ifneq (0,$(SOFT_I2C))
  USEMODULE += soft_i2c_as_periph_i2c
endif

FEATURES_REQUIRED = periph_i2c
FEATURES_OPTIONAL = periph_i2c_reconfigure
USEMODULE += shell_cmds_default
USEMODULE += shell_cmd_i2c_scan
USEMODULE += xtimer

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

EXTERNAL_BOARD_DIRS += $(RIOTBASE)/tests/build_system/external_board_dirs/esp-ci-boards

include $(RIOTBASE)/Makefile.include

ifneq (,$(PIN_SCL))
  CFLAGS += "-DSOFT_I2C_PARAM_SCL=$(PIN_SCL)"
endif
ifneq (,$(PIN_SDA))
  CFLAGS += "-DSOFT_I2C_PARAM_SDA=$(PIN_SDA)"
endif
