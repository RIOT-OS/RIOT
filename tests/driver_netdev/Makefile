APPLICATION = driver_netdev

include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# include specific driver makefiles in order to test compilation
# in order to make the test application work, comment all but one
include Makefile.enc28j60
include Makefile.encx24j600
include Makefile.kw2xrf
include Makefile.xbee

# test if multiple drivers are compiled in
ifneq (x,$(DRIVERS))
    CFLAGS += -DMULTIPLE_DRIVERS
    BOARD_INSUFFICIENT_MEMORY += airfy-beacon nrf6310 pca10005 yunjia-nrf51822
endif

# make sure local parameter headers can be found
CFLAGS += -I$(CURDIR)

# lower pktbuf size
CFLAGS += -DGNRC_PKTBUF_SIZE=2048

include $(RIOTBASE)/Makefile.include
