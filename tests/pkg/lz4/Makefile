include ../Makefile.pkg_common

USEPKG += lz4

# lz4 requires a custom thread stack size
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=16384

# stk3200 and nucleo-l011k4 boards don't allow a custom default thread stacksize
BOARDS_UNSUPPORTED := \
    nucleo-l011k4 \
    stk3200 \
    #

include $(RIOTBASE)/Makefile.include
