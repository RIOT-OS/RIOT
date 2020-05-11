include ../Makefile.tests_common

USEMODULE += gnrc_sixlowpan_frag
USEMODULE += embunit

# GNRC modules should not be initialized unless we want to
DISABLE_MODULE += auto_init_gnrc_%

# we don't need all this packet buffer space so reduce it a little
CFLAGS += -DTEST_SUITES -DCONFIG_GNRC_PKTBUF_SIZE=2048

include $(RIOTBASE)/Makefile.include
