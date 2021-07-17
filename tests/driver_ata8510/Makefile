BOARD ?= yarm
QUIET ?= 1
DEVHELP ?= 1
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od_string

USEMODULE += ata8510

CFLAGS += -DCONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE_EXP=12
ifneq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_IDLE=THREAD_STACKSIZE_DEFAULT
endif

include $(RIOTBASE)/Makefile.include
