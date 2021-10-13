include ../Makefile.tests_common

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# allow to use a smaller packet buffer in low-PDU drivers
ifdef GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=$(GNRC_PKTBUF_SIZE)
endif

# gnrc is a meta module including all required, basic gnrc networking modules
USEMODULE += gnrc

# automatically initialize the network interface
USEMODULE += auto_init_gnrc_netif

# shell command to send L2 packets with a simple string
USEMODULE += gnrc_txtsnd

# the application dumps received packets to stdout
USEMODULE += gnrc_pktdump

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
