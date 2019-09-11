include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += posix_headers
USEMODULE += pthread
USEMODULE += xtimer

CFLAGS += -DNATIVE_AUTO_EXIT

include $(RIOTBASE)/Makefile.include
