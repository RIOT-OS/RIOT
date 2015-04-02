# name of your application
APPLICATION = driver_pn532

BOARD ?= udoo

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Modules to include:
FEATURES_REQUIRED = periph_spi periph_gpio
#FEATURES_PROVIDED += periph_uart periph_gpio periph_spi periph_random cpp

USEMODULE += uart0
#USEMODULE += shell
#USEMODULE += shell_commands
#USEMODULE += ps
#USEMODULE += lib
#USEMODULE += vtimer
#USEMODULE += defaulttransceiver
USEMODULE += pn532

#USEMODULE += auto_init

#DISABLE_MODULE += auto_init


include $(RIOTBASE)/Makefile.include
