SRC := ws281x.c # All other sources files provide ws281x_write as submodule
SUBMODULES := 1

# Since we have submodules we need to manually handle saul instead of
# including driver_with_saul.mk
MODULE ?= $(shell basename $(CURDIR))
SAUL_INTERFACE ?= $(MODULE)_saul.c

# only include <module>_saul.c if saul module is used
ifneq (,$(filter saul,$(USEMODULE)))
  SRC += $(SAUL_INTERFACE)
endif

include $(RIOTBASE)/Makefile.base
