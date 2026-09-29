include ../Makefile.drivers_common

USEMODULE += ads131m02
USEMODULE += event
USEMODULE += event_timeout
USEMODULE += ztimer_usec
USEMODULE += xtimer
USEMODULE += shell shell_cmds_default ps

CFLAGS += -DADS131M02_TEST_FRAMES_NUMOF=32 -DADS131M02_TEST_FRAME_BUFFERS_NUMOF=4
CFLAGS += -DCONFIG_SPI_DMA_DESC_EXP=7

include $(RIOTBASE)/Makefile.include
