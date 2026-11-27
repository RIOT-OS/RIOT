include ../Makefile.riot-wrappers_common

APPLICATION_RUST_MODULE = riot_wrappers_test_gnrc_pktbuf

USEMODULE += gnrc_pktbuf

# thus we get `gnrc_pktbuf_is_empty` and `_is_sane`
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
