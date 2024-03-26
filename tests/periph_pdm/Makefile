# name of your application
APPLICATION = shell

BOARD ?= samr21-xpro
include ../Makefile.tests_common

CFLAGS +=-DPDM_BUF_SIZE=\(64\)

FEATURES_REQUIRED = periph_pdm

# use the shell module
USEMODULE += shell

USEMODULE += ztimer_msec

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

include $(RIOTBASE)/Makefile.include
