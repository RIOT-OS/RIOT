INCLUDES += -I$(APPDIR)

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_eth_minimal
USEMODULE += greth

BOARD ?= zedboard-noelv

# GRETH is a GRLIB on-SoC MAC: the driver uses RISC-V cache-management
# instructions and the PLIC, so it can only be built for the NOEL-V board
BOARDS_SUPPORTED := zedboard-noelv

include $(RIOTBASE)/Makefile.include
