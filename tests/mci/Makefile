APPLICATION = $(shell basename $(shell pwd))
include ../Makefile.tests_common

USEMODULE += mci
USEMODULE += flash_sim
USEMODULE += xtimer
USEMODULE += embunit

CFLAGS += -g

include $(RIOTBASE)/Makefile.include
