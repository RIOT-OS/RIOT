# Makefile for the NRF24L01+ radio driver
MODULE = nrf24l01p

# enable submodules
SUBMODULES := 1

 # exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out advanced.c,$(wildcard *.c))

include $(RIOTBASE)/Makefile.base
