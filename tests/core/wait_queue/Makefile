include ../Makefile.core_common

include Makefile.common

# disable early exit optimization to facilitate the testing of some edge cases
CFLAGS += -DCONFIG_QUEUE_WAIT_EARLY_EXIT=0

include $(RIOTBASE)/Makefile.include
