# name of your application
APPLICATION = nrf52_resetpin_cfg

# configure RIOT basics
BOARD ?= nrf52dk
RIOTBASE ?= $(CURDIR)/../../..

# This application uses getchar and thus expects input from stdio
USEMODULE += stdin

# the RESET_PIN environment variable allows for manually specifying the reset
# pin that is programmed. Below this Makefile already contains the specific pins
# for some known platforms
ifeq (nrf52dk,$(BOARD))
  RESET_PIN ?= 21
endif
ifeq (nrf52840dk,$(BOARD))
  RESET_PIN ?= 18
endif
ifeq (,$(RESET_PIN))
  $(error Please specify the target reset pin using the RESET_PIN env variable)
else
  CFLAGS += -DRESET_PIN=$(RESET_PIN)
endif


include $(RIOTBASE)/Makefile.include
