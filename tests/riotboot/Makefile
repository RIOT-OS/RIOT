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

# The test script assumes initially version "0" is flashed,
# as done by the CI.
# Thus default to that (instead of epoch set by makefiles/boot/riotboot.inc.mk).
APP_VER?=0

# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves, thus add the required files here so they will be
# submitted along with the test jobs.
TEST_EXTRA_FILES=$(SLOT_RIOT_ELFS)

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
