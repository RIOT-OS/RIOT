include ../Makefile.tests_common

USEMODULE += od
# USEMODULE += od_string

TEST_ON_CI_WHITELIST += all

ifeq (,$(filter od_string,$(USEMODULE)))
  TESTS=$(APPDIR)/tests/01-run.py
else
  TESTS=$(APPDIR)/tests/02-run.py
endif

include $(RIOTBASE)/Makefile.include
