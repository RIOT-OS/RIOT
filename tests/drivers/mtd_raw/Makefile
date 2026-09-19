include ../Makefile.drivers_common

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += shell_cmd_mtd

USEMODULE += mtd_write_page

# enable true erase if MTD is an SD card
CFLAGS += -DCONFIG_MTD_SDCARD_ERASE=1

# test needs at least 2 sectors
CFLAGS += -DMTD_EEPROM_SECTORS_NUMOF=2

# Sometimes fails. See #16130.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
