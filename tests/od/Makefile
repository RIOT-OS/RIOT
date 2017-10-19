# name of your application
APPLICATION = od
include ../Makefile.tests_common

USEMODULE += od

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
