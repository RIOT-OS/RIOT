include ../Makefile.tests_common

USEPKG += qr-code-generator

MESSAGE_TO_ENCODE ?= "https://riot-os.org"

CFLAGS += -DMESSAGE_TO_ENCODE=\"$(MESSAGE_TO_ENCODE)\"

# This application provides a 'Kconfig' file and we want to explicitly disable
# Kconfig by default by setting this variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
