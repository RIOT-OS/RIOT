MODULE = libcsp_interfaces

# exclude submodule sources from *.c wildcard source selection
SRC := csp_if_can.c csp_if_can_pbuf.c csp_if_lo.c

include $(RIOTBASE)/Makefile.base

