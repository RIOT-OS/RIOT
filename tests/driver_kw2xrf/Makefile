APPLICATION = driver_kw2xrf
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_MEMORY := stm32f0discovery nucleo-f334 weio

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netif
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# define parameters for selected boards
ifneq (,$(filter pba-d-01-kw2x,$(BOARD)))
  DRIVER               := kw2xrf
  USE_BOARD_PARAMETERS := true
endif

# only set specific parameters if not using board configuration
ifneq (true,$(USE_BOARD_PARAMETERS))

  # set default device parameters in case they are undefined
  DRIVER            ?= kw2xrf
  KW2XRF_SHARED_SPI ?= 1
  KWRF_SPI          ?= SPI_0
  KWRF_SPI_SPEED    ?= SPI_SPEED_5MHZ
  KWRF_CS           ?= GPIO_PIN\(0,0\)
  KWRF_INT          ?= GPIO_PIN\(0,1\)

  # export parameters
  CFLAGS += -DKW2XRF_SHARED_SPI=$(KW2XRF_SHARED_SPI)
  CFLAGS += -DKWRF_SPI=$(KWRF_SPI)
  CFLAGS += -DKWRF_SPI_SPEED=$(KWRF_SPI_SPEED)
  CFLAGS += -DKWRF_CS=$(KWRF_CS)
  CFLAGS += -DKWRF_INT=$(KWRF_INT)

  # This adds . to include path so generic kw2xrf_params.h gets picked
  # up.  All boards actually having such a device on board should define
  # USE_BOARD_PARAMETERS=true above to skip this step, as the board provides
  # this header.
  CFLAGS += -I$(CURDIR)

endif

# finally include the actual chosen driver
USEMODULE += $(DRIVER)

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
