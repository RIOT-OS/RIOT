include ../Makefile.tests_common

USEPKG += qr-code-generator

MESSAGE_TO_ENCODE ?= "https://riot-os.org"

CFLAGS += -DMESSAGE_TO_ENCODE=\"$(MESSAGE_TO_ENCODE)\"

include $(RIOTBASE)/Makefile.include
