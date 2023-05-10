include ../Makefile.sys_common

USEMODULE += congure_abe
USEMODULE += congure_test
USEMODULE += fmt
USEMODULE += shell

INCLUDES += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_SHELL_NO_ECHO
  CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
endif

ifndef CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE
  CFLAGS += -DCONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE=6
endif
