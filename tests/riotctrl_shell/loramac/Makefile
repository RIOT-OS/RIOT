USEMODULE += netdev_test
USEMODULE += shell_cmd_semtech-loramac

USEPKG += semtech-loramac

# this boards have issues with our periph_eeprom mock
# function declarations
BOARDS_UNSUPPORTED += derfmega256

# these features cause issues with our periph_eeprom mock
# function declarations
FEATURES_BLACKLIST += arch_esp32

include $(CURDIR)/../Makefile.riotctrl_shell_common

# only mock eeprom
CFLAGS += -DMODULE_PERIPH_EEPROM
# boards of these MCU families provide their own definition of EEPROM_SIZE
ifeq (,$(filter cpu_core_atmega cpu_stm32l0 cpu_stm32l1,$(FEATURES_USED)))
  CFLAGS += -DEEPROM_SIZE=1024
endif
