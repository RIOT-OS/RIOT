APPLICATION = driver_at86rf2xx
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

DISABLE_MODULE += auto_init

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# define the driver to be used for selected boards
ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER := at86rf231
endif
ifneq (,$(filter mulle,$(BOARD)))
  DRIVER := at86rf212b
endif

# use the at86rf231 as fallback device
DRIVER ?= at86rf231

# include the selected driver
USEMODULE += $(DRIVER)

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
