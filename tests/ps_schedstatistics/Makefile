include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += schedstatistics
USEMODULE += printf_float
USEMODULE += ztimer_usec
USEMODULE += ztimer_sec

# For this test we don't want to use the shell version of
# test_utils_interactive_sync, since we want to synchronize before
# the start of the shell
DISABLE_MODULE += test_utils_interactive_sync_shell

include $(RIOTBASE)/Makefile.include

# Increase stacksize for AVR idle thread
ifneq (,$(filter avr8_common,$(USEMODULE)))
  CFLAGS +=-DTHREAD_STACKSIZE_IDLE=THREAD_STACKSIZE_DEFAULT
endif
