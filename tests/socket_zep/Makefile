include ../Makefile.tests_common

BOARD_WHITELIST = native    # socket_zep is only available on native

# Cannot run the test on `murdock`
#   ZEP: Unable to connect socket: Cannot assign requested address
TEST_ON_CI_BLACKLIST += native

USEMODULE += od
USEMODULE += socket_zep

TERMFLAGS ?= -z [::1]:17754

include $(RIOTBASE)/Makefile.include
