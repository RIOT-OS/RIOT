# Default Makefile, for unicoap message APIs

# name of your application
APPLICATION = unicoap_message

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

USEMODULE += unicoap

# This module is needed to get the RFC 7252 PDU parser
USEMODULE += unicoap_driver_rfc7252_pdu

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
