include ../Makefile.tests_common

USEMODULE += vfs_default
USEMODULE += vfs_auto_format

USEMODULE += shell
USEMODULE += ps
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter avr8_common,$(USEMODULE)))
  # the main thread stack on AVR is too small for the vfs shell command
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
endif
