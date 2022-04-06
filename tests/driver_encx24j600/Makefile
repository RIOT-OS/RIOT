INCLUDES += -I$(APPDIR)

include ../Makefile.tests_common

USEMODULE += test_utils_netdev_eth_minimal
USEMODULE += encx24j600

# set board specific peripheral configurations
ifneq (,$(filter nucleo-f334r8,$(BOARD)))
# these settings are probably valid for PoEll-i on most nucelo boards, but
# tested only on nucleo-f334r8
  ENC_SPI ?= SPI_DEV\(0\)
  ENC_CS  ?= GPIO_PIN\(PORT_C,10\)
  ENC_INT ?= GPIO_PIN\(PORT_D,2\)

  # export SPI and pins
  CFLAGS += -DENCX24J600_SPI=$(ENC_SPI)
  CFLAGS += -DENCX24J600_CS=$(ENC_CS)
  CFLAGS += -DENCX24J600_INT=$(ENC_INT)
endif

include $(RIOTBASE)/Makefile.include
