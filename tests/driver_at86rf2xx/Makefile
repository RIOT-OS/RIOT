APPLICATION = driver_at86rf2xx
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_MEMORY := nucleo-f334 stm32f0discovery weio

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netif
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# define parameters for selected boards
ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER               := at86rf233
  USE_BOARD_PARAMETERS := true
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER               := at86rf231
  USE_BOARD_PARAMETERS := true
endif

# only set specific parameters if not using board configuration
ifneq (true,$(USE_BOARD_PARAMETERS))

  # set default device parameters in case they are undefined
  DRIVER         ?= at86rf231
  ATRF_SPI       ?= SPI_0
  ATRF_SPI_SPEED ?= SPI_SPEED_5MHZ
  ATRF_CS        ?= GPIO_PIN\(0,0\)
  ATRF_INT       ?= GPIO_PIN\(0,1\)
  ATRF_SLEEP     ?= GPIO_PIN\(0,2\)
  ATRF_RESET     ?= GPIO_PIN\(0,3\)

  # export parameters
  CFLAGS += -DATRF_SPI=$(ATRF_SPI)
  CFLAGS += -DATRF_SPI_SPEED=$(ATRF_SPI_SPEED)
  CFLAGS += -DATRF_CS=$(ATRF_CS)
  CFLAGS += -DATRF_INT=$(ATRF_INT)
  CFLAGS += -DATRF_SLEEP=$(ATRF_SLEEP)
  CFLAGS += -DATRF_RESET=$(ATRF_RESET)

  # This adds . to include path so generic at86rf2xx_params.h gets picked
  # up.  All boards actually having such a device on board should define
  # USE_BOARD_PARAMETERS=true above to skip this step, as the board provides
  # this header.
  CFLAGS += -I$(CURDIR)

endif

# finally include the actual chosen driver
USEMODULE += $(DRIVER)

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
