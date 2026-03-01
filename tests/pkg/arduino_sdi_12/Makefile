BOARD ?= nucleo-f767zi

include ../Makefile.pkg_common

USEPKG += arduino_sdi_12

# Boards not fast enough, that can't keep up with the line data rate, should be
# added here
BOARDS_UNSUPPORTED := \
  #

include $(RIOTBASE)/Makefile.include
