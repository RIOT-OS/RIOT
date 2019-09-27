include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno nucleo-f031k6

USEMODULE += gnrc_sixlowpan_frag
USEMODULE += embunit

# GNRC modules should not be initialized unless we want to
DISABLE_MODULE += auto_init

# we don't need all this packet buffer space so reduce it a little
CFLAGS += -DTEST_SUITES -DGNRC_PKTBUF_SIZE=2048

include $(RIOTBASE)/Makefile.include
