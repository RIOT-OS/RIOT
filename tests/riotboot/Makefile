# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# Select the boards with riotboot feature
FEATURES_REQUIRED += riotboot

# Include modules to test the bootloader
USEMODULE += riotboot_slot
USEMODULE += shell
# Add shell_commands to use the shell version of `test_utils_interactive_sync`
USEMODULE += shell_commands

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Ensure both slot bin files are always generated and linked to avoid compiling
# during the test. This ensures that "BUILD_IN_DOCKER=1 make test"
# can rely on them being present without having to trigger re-compilation.
BUILD_FILES += $(SLOT_RIOT_ELFS:%.elf=%.bin)

# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves, thus add the required files here so they will be
# submitted along with the test jobs.
TEST_EXTRA_FILES=$(SLOT_RIOT_ELFS)

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
