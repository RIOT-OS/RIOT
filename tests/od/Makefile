include ../Makefile.tests_common

USEMODULE += od
# USEMODULE += od_string

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include

test:
ifeq (,$(filter od_string,$(USEMODULE)))
	tests/01-run.py
else
	tests/02-run.py
endif
