export PROJECT = test_shell
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DISABLE_MODULE += auto_init

CFLAGS += -DSCHEDSTATISTICS

include $(RIOTBASE)/Makefile.include
