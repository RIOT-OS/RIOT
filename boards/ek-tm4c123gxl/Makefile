# tell the Makefile.base which module to build
MODULE = $(BOARD)_base

PART=TM4C123GH6PM
CFLAGS +=-DTARGET_IS_BLIZZARD_RB1 -DPART_${PART}

include $(RIOTBASE)/Makefile.base
