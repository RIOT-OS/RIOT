# Ensure minimal size by default
DEVELHELP ?= 0

include ../Makefile.tests_common

EXAMPLE ?= hello_world

USEPKG += tensorflow-lite
USEMODULE += tensorflow-lite-$(EXAMPLE)

include $(RIOTBASE)/Makefile.include
