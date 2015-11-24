APPLICATION = pthread_tls
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: unknown type name: clockid_t

USEMODULE += posix
USEMODULE += pthread

include $(RIOTBASE)/Makefile.include
