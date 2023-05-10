include ../Makefile.drivers_common

USEMODULE += shell
USEMODULE += shell_cmds_default

USEMODULE += od
USEMODULE += mtd
USEMODULE += mtd_write_page

# enable true erase if MTD is an SD card
CFLAGS += -DCONFIG_MTD_SDCARD_ERASE=1

# Sometimes fails. See #16130.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
