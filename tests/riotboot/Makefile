# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

TEST_ON_CI_WHITELIST += all

# Select the boards with riotboot feature
FEATURES_REQUIRED += riotboot

# Include modules to test the bootloader
USEMODULE += riotboot_slot
USEMODULE += shell

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Target 'all' will generate the combined file directly.
# It also makes 'flash' and 'flash-only' work without specific command.
FLASHFILE = $(RIOTBOOT_COMBINED_BIN)

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
