include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno \
                             chronos msb-430 msb-430h nucleo-f031k6 telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += xtimer
FEATURES_REQUIRED = periph_spi
USEPKG += bme680_driver

ifeq ($(ENABLE_FP),1)
CFLAGS += -DBME680_FLOAT_POINT_COMPENSATION
endif

include $(RIOTBASE)/Makefile.include
