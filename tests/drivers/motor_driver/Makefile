BOARD ?= native

include ../Makefile.drivers_common

USEMODULE += motor_driver
USEMODULE += shell_cmds_default
USEMODULE += xtimer

FEATURES_REQUIRED += periph_qdec
FEATURES_REQUIRED += motor_driver

CFLAGS += -DLOG_LEVEL=LOG_DEBUG
CFLAGS += -DDEBUG_ASSERT_VERBOSE

include $(RIOTBASE)/Makefile.include
