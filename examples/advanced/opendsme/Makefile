# name of your application
APPLICATION = opendsme_example

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# DSME uses beacon transmission, which are usually filtered out in current drivers.
# So far, nrf802154 is the only driver that supports beacons, which means openDSME
# will run only in nrf802154-based CPUs.
FEATURES_REQUIRED += cpu_nrf52

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../../thirdparty_cpu
#EXTERNAL_BOARD_DIRS ?= $(CURDIR)/../../../thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_cmds_default

USEPKG += opendsme
USEMODULE += auto_init_gnrc_netif
USEMODULE += l2util
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_txtsnd

USEMODULE += netdev_default

# Some boards (e.g nrf52840dongle) require a higher CCA threshold to avoid
# false positives.
CFLAGS += -DCONFIG_IEEE802154_CCA_THRESH_DEFAULT=-30

include $(RIOTBASE)/Makefile.include
