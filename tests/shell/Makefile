APPLICATION = shell
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += uart0

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
	$(CURDIR)/tests/01-basic
	$(CURDIR)/tests/02-inputlength-regression
