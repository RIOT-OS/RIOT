include ../Makefile.fuzzing_common
# name of your application
APPLICATION = ut_process

USEMODULE += gnrc_ipv6
USEMODULE += ut_process

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
