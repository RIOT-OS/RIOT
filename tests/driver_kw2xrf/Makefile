APPLICATION = driver_kw2xrf
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_RAM := stm32f0discovery

ifneq (,$(filter pba-d-01-kw2x,$(BOARD)))
  export KWRF_SPI ?= SPI_1
  export KWRF_CS ?= GPIO_24
  export KWRF_INT ?= GPIO_23
  export KWRF_SPI_SPEED ?= SPI_SPEED_10MHZ
  export KW2XRF_SHARED_SPI ?= 0
endif

USEMODULE += ng_netif
USEMODULE += ng_nomac
USEMODULE += ng_pktdump
USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += kw2xrf

CFLAGS += -DDEVELHELP

ifneq (,$(KWRF_SHARED_SPI))
  CFLAGS += -DKW2XRF_SHARED_SPI=$(DKW2XRF_SHARED_SPI)
else
  CFLAGS += -DKW2XRFD_SHARED_SPI=1  # activate spi-lock
endif
ifneq (,$(KWRF_SPI))
  CFLAGS += -DKWRF_SPI=$(KWRF_SPI)
else
  CFLAGS += -DKWRF_SPI=SPI_0        # set default
endif
ifneq (,$(KWRF_CS))
  CFLAGS += -DKWRF_CS=$(KWRF_CS)
else
  CFLAGS += -DKWRF_CS=GPIO_0        # set default
endif
ifneq (,$(KWRF_INT))
  CFLAGS += -DKWRF_INT=$(KWRF_INT)
else
  CFLAGS += -DKWRF_INT=GPIO_1        # set default
endif
ifneq (,$(KWRF_SPI_SPEED))
  CFLAGS += -DKWRF_SPI_SPEED=$(KWRF_SPI_SPEED)
endif

include $(RIOTBASE)/Makefile.include
