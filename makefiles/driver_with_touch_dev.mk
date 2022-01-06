MODULE ?= $(shell basename $(CURDIR))
TOUCH_DEV_INTERFACE ?= $(MODULE)_touch_dev.c

# by default include all .c files except <module>_touch_dev.c
SRC = $(filter-out $(TOUCH_DEV_INTERFACE),$(wildcard *.c))

# only include <module>_touch_dev.c if saul module is used
ifneq (,$(filter touch_dev,$(USEMODULE)))
  SRC += $(TOUCH_DEV_INTERFACE)
endif

include $(RIOTBASE)/Makefile.base
