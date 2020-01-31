MODULE = cc13x2_rf

CFLAGS += -Wno-unused-function

ifeq (cc1312r1f3,$(CPU_MODEL))
  DIRS += prop
else ifeq (cc1352r1,$(CPU_MODEL))
  DIRS += prop
endif

include $(RIOTBASE)/Makefile.base
