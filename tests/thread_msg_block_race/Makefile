DEVELHELP := 1

include ../Makefile.tests_common

DISABLE_MODULE += auto_init_xtimer
DISABLE_MODULE += auto_init_random

FEATURES_REQUIRED += periph_timer
USEMODULE += random

include $(RIOTBASE)/Makefile.include
