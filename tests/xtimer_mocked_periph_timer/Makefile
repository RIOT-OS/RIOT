include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

USEMODULE += embunit

# Replace enabling xtimer through the build system to manually enabling it
# It allows replacing periph_timer implementation for xtimer by including c
# files
## USEMODULE += xtimer

# Dependencies
INCLUDES += -I$(RIOTBASE)/sys/xtimer
USEMODULE += div
# Ignore periph_timer dependency it will be mocked

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
