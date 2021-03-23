include ../Makefile.tests_common

USEMODULE += congure_mock
USEMODULE += congure_test
USEMODULE += fmt
USEMODULE += shell
USEMODULE += shell_commands

INCLUDES += -I$(CURDIR)

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

# As there is an 'app.config' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_SHELL_NO_ECHO
  CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
endif

ifndef CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE
  CFLAGS += -DCONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE=4
  export LOST_MSG_POOL_SIZE=4
else
  export LOST_MSG_POOL_SIZE=$(CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE)
endif
