BOARD ?= nrf52840dk
include ../Makefile.tests_common

# we want shell support
USEMODULE += shell
USEMODULE += shell_commands
# setup NimBLE specific modules
USEMODULE += nimble_scanner
USEMODULE += nimble_scanlist
USEMODULE += nimble_adv_ext
FEATURES_OPTIONAL += ble_phy_2mbit
FEATURES_OPTIONAL += ble_phy_coded

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
