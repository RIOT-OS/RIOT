include ../Makefile.tests_common

BOARD_BLACKLIST := wsn430-v1_3b wsn430-v1_4

USEPKG += tiny-asn1

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
