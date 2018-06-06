MODULE = nimble_porting_nimble

# host specific files
SRC += nimble_port.c
SRC += endian.c
SRC += mem.c
SRC += os_mbuf.c
SRC += os_mempool.c
SRC += os_msys_init.c

# additional files needed for the controller
SRC += os_cputime.c
SRC += os_cputime_pwr2.c
SRC += hal_timer.c

include $(RIOTBASE)/Makefile.base
