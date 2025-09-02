SRC = ads1x1x.c

ifneq (,$(filter ads1115 ads1114 ads1113,$(USEMODULE)))
  CFLAGS += -DADS111X
endif

ifneq (,$(filter ads1015 ads1014 ads1013,$(USEMODULE)))
  CFLAGS += -DADS101X
endif

include $(RIOTBASE)/Makefile.base
