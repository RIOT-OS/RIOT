include ../Makefile.fuzzing_common
# name of your application
APPLICATION = uriparser

USEMODULE += uri_parser

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Needed when using afl++ and persistent fuzzing
CFLAGS += -Wno-gnu-statement-expression

include $(RIOTBASE)/Makefile.include
