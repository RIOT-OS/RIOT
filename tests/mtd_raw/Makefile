include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += od
USEMODULE += mtd
USEMODULE += mtd_write_page

# enable true erase if MTD is an SD card
CFLAGS += -DCONFIG_MTD_SDCARD_ERASE=1

# Sometimes fails. See #16130.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

include $(RIOTBASE)/Makefile.include
