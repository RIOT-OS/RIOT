BOARD ?= native

include ../Makefile.tests_common

USEMODULE += motor_driver
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += xtimer

FEATURES_REQUIRED += periph_qdec
FEATURES_REQUIRED += motor_driver

CFLAGS += -DLOG_LEVEL=LOG_DEBUG
CFLAGS += -DDEBUG_ASSERT_VERBOSE

include $(RIOTBASE)/Makefile.include
