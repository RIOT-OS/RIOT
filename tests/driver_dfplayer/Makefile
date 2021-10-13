include ../Makefile.tests_common

USEMODULE += dfplayer
USEMODULE += event_thread_lowest
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include

ifneq (,$(BUSY_PIN))
  CFLAGS += '-DDFPLAYER_PARAM_BUSY_PIN=$(BUSY_PIN)'
endif
