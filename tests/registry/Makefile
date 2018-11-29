APPLICATION = test_registry
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += registry
# Enable the use of int64 types on the registry
CFLAGS += -DCONFIG_REGISTRY_USE_INT64

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(2*THREAD_STACKSIZE_DEFAULT\)

# Choose one type of storage facility:
# USE DUMMY STORAGE (RAM)
USEMODULE += registry_store_dummy

ifneq (,$(filter registry_store_dummy,$(USEMODULE)))
  DIRS += dummy
endif

include $(RIOTBASE)/Makefile.include
