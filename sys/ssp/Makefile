# module name is used below, thus set explicitly
MODULE = ssp

ifeq (,$(RIOT_CI_BUILD))
  # random canary value newly generated on each build
  CANARY := $(shell $(RIOTTOOLS)/randhex/randhex.py)
else
  # hardcoded canary value to not break CI test cache
  CANARY := 0xdeadbeef
endif

# pass the generated canary using a macro and mark the object file using
# it as PHONY to ensure that a new canary value is used on each build.
CFLAGS += -DSTACK_CHK_GUARD=$(CANARY)U
.PHONY: $(BINDIR)/$(MODULE)/$(MODULE).o

include $(RIOTBASE)/Makefile.base
