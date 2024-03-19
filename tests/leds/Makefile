include ../Makefile.tests_common

# Some boards do not initialize LED0 by default
CFLAGS=-DPERIPH_INIT_LED0

# auto-init on board buttons as well
USEMODULE += periph_init_buttons

include $(RIOTBASE)/Makefile.include
