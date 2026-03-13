MODULE = libcsp

# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out csp_yaml.c csp_init.c,$(wildcard *.c))

include $(RIOTBASE)/Makefile.base
