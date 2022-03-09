MODULE = mynewt-core_os

SRC := \
  endian.c \
  os_mbuf.c \
  os_mempool.c \
  os_msys.c \
  os_cputime_pwr2.c \
  #

ifneq (,$(filter nrf5%,$(CPU)))
  SRC += os_cputime.c
endif

include $(RIOTBASE)/Makefile.base
