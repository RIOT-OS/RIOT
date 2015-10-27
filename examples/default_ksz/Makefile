# name of your application
APPLICATION = default

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..
#CFLAGS_OPT += -O0
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
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

BOARD ?= samr21-xpro

BOARD_PROVIDES_NETIF := airfy-beacon fox iotlab-m3 mulle native nrf51dongle \
	nrf6310 pba-d-01-kw2x pca10000 pca10005 saml21-xpro samr21-xpro spark-core \
	yunjia-nrf51822

# Modules to include:

GNRC_NETIF_NUMOF = 2


USEMODULE += ps
USEMODULE += config

USEMODULE += ksz8851snl
USEMODULE += gnrc_netdev2
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc
USEMODULE += gnrc_pktdump

USEMODULE += gnrc_sixlowpan_border_router_default
USEMODULE += gnrc_icmpv6_echo

USEMODULE += shell
USEMODULE += shell_commands
FEATURES_OPTIONAL += config
FEATURES_OPTIONAL += periph_rtc

ifneq (,$(filter msb-430,$(BOARD)))
	USEMODULE += sht11
endif
ifneq (,$(filter msba2,$(BOARD)))
	USEMODULE += sht11
	USEMODULE += mci
	USEMODULE += random
endif
ifneq (,$(filter iotlab-m3,$(BOARD)))
	USEMODULE += isl29020
	USEMODULE += lps331ap
	USEMODULE += l3g4200d
    USEMODULE += lsm303dlhc
endif
ifneq (,$(filter fox,$(BOARD)))
        USEMODULE += lps331ap
        USEMODULE += l3g4200d
        USEMODULE += lsm303dlhc
endif

include $(RIOTBASE)/Makefile.include
