APPLICATION = driver_at86rf2xx
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_RAM := stm32f0discovery
BOARD_BLACKLIST        := nucleo-f334
# nucleo-f334:  not enough GPIO pins defined

ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER ?= at86rf233
  USE_BOARD_PARAMETERS:=true
endif
ifneq (,$(filter iotlab-m3,$(BOARD)))
  DRIVER ?= at86rf231
  USE_BOARD_PARAMETERS:=true
endif

ifneq (,$(DRIVER))
  USEMODULE += $(DRIVER)
else
  # default to at86rf231
  USEMODULE += at86rf231
endif

ifneq (true,$(USE_BOARD_PARAMETERS))
  # This adds . to include path so generic at86rf2xx_params.h gets picked
  # up.  All boards actually having such a device on board should define
  # USE_BOARD_PARAMETERS=true above to skip this step, as the board provides
  # this header.
  CFLAGS += -I$(CURDIR)

ifneq (,$(ATRF_SPI))
  CFLAGS += -DATRF_SPI=$(ATRF_SPI)
else
  CFLAGS += -DATRF_SPI=SPI_0                # set default
endif
ifneq (,$(ATRF_CS))
  CFLAGS += -DATRF_CS=$(ATRF_CS)
else
  CFLAGS += -DATRF_CS=GPIO\(0,0\)           # set default
endif
ifneq (,$(ATRF_INT))
  CFLAGS += -DATRF_INT=$(ATRF_INT)
else
  CFLAGS += -DATRF_INT=GPIO\(0,1\)          # set default
endif
ifneq (,$(ATRF_SLEEP))
  CFLAGS += -DATRF_SLEEP=$(ATRF_SLEEP)
else
  CFLAGS += -DATRF_SLEEP=GPIO\(0,2\)        # set default
endif
ifneq (,$(ATRF_RESET))
  CFLAGS += -DATRF_RESET=$(ATRF_RESET)
else
  CFLAGS += -DATRF_RESET=GPIO\(0,3\)        # set default
endif
ifneq (,$(ATRF_SPI_SPEED))
  CFLAGS += -DATRF_SPI_SPEED=$(ATRF_SPI_SPEED)
endif

endif # USE_BOARD_PARAMETERS=false

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netif
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
