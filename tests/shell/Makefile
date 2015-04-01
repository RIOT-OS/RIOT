APPLICATION = shell
include ../Makefile.tests_common

CFLAGS += -DUSE_SHELL_COLORS
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += uart0

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
