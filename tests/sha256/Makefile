APPLICATION = sha256
include ../Makefile.tests_common

USEMODULE += crypto

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
	$(CURDIR)/tests/01-test
