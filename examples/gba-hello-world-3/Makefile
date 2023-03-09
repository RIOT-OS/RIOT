# name of your application
APPLICATION = hello-world

# If no BOARD is found in the environment, use this default:
BOARD ?= gba_cartridge

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP := 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1


USEMODULE += stdio_fb
USEMODULE += shell
#DISABLE_MODULE += core_msg
#DISABLE_MODULE += core_thread
#DISABLE_MODULE += core_panic
#DISABLE_MODULE += core_init
#DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
