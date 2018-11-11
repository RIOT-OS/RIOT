include ../Makefile.tests_common

USEMODULE += sdcard_spi
USEMODULE += auto_init_storage
USEMODULE += fmt
USEMODULE += shell

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  nucleo-f031k6 \
  #

include $(RIOTBASE)/Makefile.include
