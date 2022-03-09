include ../Makefile.tests_common

BOARD ?= nrf9160dk

USEPKG += nrfxlib
USEMODULE += nrfxlib_nrf_modem

# Auto init
DEFAULT_MODULE += auto_init_nrfxlib_nrf_modem

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += ztimer_msec
USEMODULE += ztimer_sec
USEMODULE += ztimer

include $(RIOTBASE)/Makefile.include
