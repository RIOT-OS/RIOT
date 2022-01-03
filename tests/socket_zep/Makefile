include ../Makefile.tests_common

BOARD_WHITELIST = native    # socket_zep is only available on native

USEMODULE += od
USEMODULE += socket_zep
USEMODULE += netdev

TERMFLAGS ?= -z [::1]:17754

include $(RIOTBASE)/Makefile.include
