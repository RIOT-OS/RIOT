include ../Makefile.core_common

CONFIG_MSG_QUEUE_PRINT_MAX ?= 6

CFLAGS += -DCONFIG_MSG_QUEUE_PRINT_MAX=$(CONFIG_MSG_QUEUE_PRINT_MAX)

include $(RIOTBASE)/Makefile.include

# Make config available in Python test script via environment variables
export CONFIG_MSG_QUEUE_PRINT_MAX
