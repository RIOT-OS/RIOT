include ../Makefile.tests_common

USEMODULE += pthread

# The test compiles, but these boards have too little RAM for
# dynamic allocation of thread stacks
BOARD_BLACKLIST := \
  arduino-duemilanove \
  arduino-nano \
  arduino-uno \
  nucleo-f031k6 \
  stm32f030f4-demo \
  #

include $(RIOTBASE)/Makefile.include
