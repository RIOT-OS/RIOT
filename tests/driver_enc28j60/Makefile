INCLUDES += -I$(APPDIR)

include ../Makefile.tests_common

USEMODULE += test_utils_netdev_eth_minimal
USEMODULE += enc28j60

# set board specific peripheral configurations
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  ENC_SPI ?= SPI_DEV\(1\)
  ENC_CS  ?= GPIO_PIN\(PORT_B,12\)
  ENC_INT ?= GPIO_PIN\(PORT_B,11\)
  ENC_RST ?= GPIO_PIN\(PORT_B,10\)
  # export SPI and pins
  CFLAGS += -DENC28J60_PARAM_SPI=$(ENC_SPI)
  CFLAGS += -DENC28J60_PARAM_CS=$(ENC_CS)
  CFLAGS += -DENC28J60_PARAM_INT=$(ENC_INT)
  CFLAGS += -DENC28J60_PARAM_RESET=$(ENC_RST)
endif

include $(RIOTBASE)/Makefile.include
