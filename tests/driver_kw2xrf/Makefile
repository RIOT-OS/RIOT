include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc
USEMODULE += fmt
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DRIVER ?= kw2xrf

# finally include the actual chosen driver
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
