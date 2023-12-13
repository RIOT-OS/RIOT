include ../Makefile.sys_common

USEMODULE += congure_reno
USEMODULE += congure_test
USEMODULE += fmt
USEMODULE += shell_cmds_default

INCLUDES += -I$(CURDIR)

ifeq (native, $(BOARD))
  RIOT_TERMINAL ?= native
endif
# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

CFLAGS += -DSTDIO_UART_RX_BUFSIZE=512  # Adapt to SHELL_BUFSIZE in app

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_SHELL_NO_ECHO
  CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
endif

ifndef CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE
  CFLAGS += -DCONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE=6
endif
