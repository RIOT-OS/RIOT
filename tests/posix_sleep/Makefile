APPLICATION = posix_sleep
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: warning: iteration 2u invokes undefined behavior
# [-Waggressive-loop-optimizations]

USEMODULE += posix

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
