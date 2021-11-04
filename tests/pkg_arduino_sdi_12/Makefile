include ../Makefile.tests_common

USEPKG += arduino_sdi_12

# Boards not fast enough, that can't keep up with the line data rate, should be
# added here
BOARD_BLACKLIST := \
  #

BOARD_INSUFFICIENT_MEMORY := \
	nucleo-l011k4 \
	#

include $(RIOTBASE)/Makefile.include
