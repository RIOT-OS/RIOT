include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY = \
	arduino-duemilanove \
	arduino-nano \
	arduino-uno \
	nucleo-f031k6 \
	#

USEMODULE += posix_headers
USEMODULE += pthread
CFLAGS += -DMAXTHREADS=8

include $(RIOTBASE)/Makefile.include
