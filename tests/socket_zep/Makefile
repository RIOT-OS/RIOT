APPLICATION = socket_zep
include ../Makefile.tests_common

DISABLE_MODULE += auto_init

USEMODULE += od
USEMODULE += socket_zep

CFLAGS += -DDEVELHELP

TERMFLAGS ?= -z [::1]:17754

BOARD_WHITELIST := \
  available \
  is \
  native \
  native \
  on \
  only \
  socket_zep \
  #

include $(RIOTBASE)/Makefile.include
