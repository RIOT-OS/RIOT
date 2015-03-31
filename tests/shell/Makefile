APPLICATION = shell
include ../Makefile.tests_common

CFLAGS += -DUSE_SHELL_COLORS
CFLAGS += -DSHELL_HISTORY_SIZE=10

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += uart0
USEMODULE += readline

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
