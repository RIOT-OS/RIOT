# name of your application
APPLICATION = blinky

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf9160dk-ns

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEPKG += riot-tee
DISABLE_MODULE += core_idle_thread
DISABLE_MODULE += core_thread
# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Use a peripheral timer for the delay, if available
# FEATURES_OPTIONAL += periph_timer

include $(RIOTBASE)/Makefile.include
