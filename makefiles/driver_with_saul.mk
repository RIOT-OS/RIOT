MODULE ?= $(shell basename $(CURDIR))
SAUL_INTERFACE ?= $(MODULE)_saul.c

# by default include all .c files except <module>_saul.c
SRC = $(filter-out $(SAUL_INTERFACE),$(wildcard *.c))

# only include <module>_saul.c if saul module is used
ifneq (,$(filter saul,$(USEMODULE)))
  SRC += $(SAUL_INTERFACE)
endif

include $(RIOTBASE)/Makefile.base
