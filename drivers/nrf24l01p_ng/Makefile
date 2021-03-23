# Makefile for the NRF24L01+ radio driver
MODULE = nrf24l01p_ng

# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out diagnostics.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
