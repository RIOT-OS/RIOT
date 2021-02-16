include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += netdev_test_ieee802154

TEST_PROTO ?= 1

ifneq (0,$(TEST_PROTO))
  USEMODULE += gnrc_nettype_sixlowpan
endif

include $(RIOTBASE)/Makefile.include
