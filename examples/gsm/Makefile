# name of your application
APPLICATION = gsm

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += at_urc

USEMODULE += gsm
USEMODULE += gsm_generic
USEMODULE += gsm_gprs
USEMODULE += gsm_call

AT_PRINT_INCOMING ?= 1

MODEM_UART     ?= "UART_NUMOF-1"
MODEM_BAUDRATE ?= 115200

# export modem parameters
CFLAGS += -DMODEM_UART="UART_DEV($(MODEM_UART))"
CFLAGS += -DMODEM_BAUDRATE=$(MODEM_BAUDRATE)
CFLAGS += -DLOG_LEVEL=LOG_DEBUG

include $(RIOTBASE)/Makefile.include
