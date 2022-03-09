include ../Makefile.tests_common
USEPKG += etl

# Kconfig build is broken due to invalid module resolution with Kconfig for this board
BOARD_BLACKLIST := \
    6lowpan-clicker \
    #

include $(RIOTBASE)/Makefile.include
