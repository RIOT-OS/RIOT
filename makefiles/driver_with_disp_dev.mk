MODULE ?= $(shell basename $(CURDIR))
DISP_DEV_INTERFACE ?= $(MODULE)_disp_dev.c

# by default include all .c files except <module>_disp_dev.c
SRC = $(filter-out $(DISP_DEV_INTERFACE),$(wildcard *.c))

# only include <module>_disp_dev.c if saul module is used
ifneq (,$(filter disp_dev,$(USEMODULE)))
  SRC += $(DISP_DEV_INTERFACE)
endif

include $(RIOTBASE)/Makefile.base
