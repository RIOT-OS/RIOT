BOARD ?= nucleo-l476rg
include ../Makefile.tests_common

USEPKG += cmsis-nn

BLOBS += input

# Boards that were tested and are known to work
#
# This package only works with Cortex M23, M3, M33, M4 and M7 CPUs, but there
# is no easy way provided by the build system to filter them at that level
# (cpu_core_cortexm is the only feature available) for the moment.
BOARD_WHITELIST := \
  b-l475e-iot01a \
  iotlab-m3 \
  nrf52832-mdk \
  nrf52dk \
  nucleo-l476rg \
  same54-xpro \
  stm32f723e-disco
  #

include $(RIOTBASE)/Makefile.include
