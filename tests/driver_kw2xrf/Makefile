APPLICATION = driver_kw2xrf
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_MEMORY := stm32f0discovery nucleo-f334 weio

ifneq (,$(filter pba-d-01-kw2x,$(BOARD)))
  DRIVER ?= kw2xrf
  USE_BOARD_PARAMETERS:=true
endif

ifneq (,$(DRIVER))
  USEMODULE += $(DRIVER)
else
  # default to kw2xrf
  USEMODULE += kw2xrf
endif

ifneq (true,$(USE_BOARD_PARAMETERS))
  # This adds . to include path so generic kw2xrf_params.h gets picked
  # up.  All boards actually having such a device on board should define
  # USE_BOARD_PARAMETERS=true above to skip this step, as the board provides
  # this header.
  CFLAGS += -I$(CURDIR)

ifneq (,$(KWRF_SHARED_SPI))
  CFLAGS += -DKW2XRF_SHARED_SPI=$(DKW2XRF_SHARED_SPI)
else
  CFLAGS += -DKW2XRFD_SHARED_SPI=1      # activate spi-lock
endif
ifneq (,$(KWRF_SPI))
  CFLAGS += -DKWRF_SPI=$(KWRF_SPI)
else
  CFLAGS += -DKWRF_SPI=SPI_0            # set default
endif
ifneq (,$(KWRF_CS))
  CFLAGS += -DKWRF_CS=$(KWRF_CS)
else
  CFLAGS += -DKWRF_CS=GPIO_PIN\(0,0\)       # set default
endif
ifneq (,$(KWRF_INT))
  CFLAGS += -DKWRF_INT=$(KWRF_INT)
else
  CFLAGS += -DKWRF_INT=GPIO_PIN\(0,1\)      # set default
endif
ifneq (,$(KWRF_SPI_SPEED))
  CFLAGS += -DKWRF_SPI_SPEED=$(KWRF_SPI_SPEED)
endif

endif # USE_BOARD_PARAMETERS=false

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netif
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
