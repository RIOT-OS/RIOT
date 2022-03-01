include ../Makefile.tests_common

USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += xtimer_on_periph_timer

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native

include $(RIOTBASE)/Makefile.include
