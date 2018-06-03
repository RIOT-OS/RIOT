APPLICATION = $(shell basename $(shell pwd))
include ../Makefile.tests_common

USEMODULE += mci
USEMODULE += xtimer
USEMODULE += timex
USEMODULE += embunit

ifneq (,$(filter native,$(BOARD)))
	USEMODULE += flash_sim
endif

CFLAGS += -g

include $(RIOTBASE)/Makefile.include
