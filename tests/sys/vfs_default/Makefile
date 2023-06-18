include ../Makefile.sys_common

USEMODULE += vfs_default
USEMODULE += vfs_auto_format

USEMODULE += ps
USEMODULE += shell_cmd_genfile
USEMODULE += shell_cmds_default

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter avr8_common,$(USEMODULE)))
  # the main thread stack on AVR is too small for the vfs shell command
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
endif
