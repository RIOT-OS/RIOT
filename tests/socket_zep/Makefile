APPLICATION = socket_zep
include ../Makefile.tests_common

BOARD_WHITELIST = native    # socket_zep is only available on native

DISABLE_MODULE += auto_init

USEMODULE += od
USEMODULE += socket_zep

CFLAGS += -DDEVELHELP

NATIVE_FLAGS ?= -z [::1]:17754

include $(RIOTBASE)/Makefile.include
