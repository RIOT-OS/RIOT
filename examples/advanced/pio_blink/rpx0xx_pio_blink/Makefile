include $(RIOTBASE)/Makefile.base
include $(RIOTCPU)/rpx0xx/pio/pio.inc.mk

# The .pio files are compiled into headers which are included by 'blink.c' and
# not by an user application.
CFLAGS += -I$(BINDIR)/$(MODULE)

# Force generation of not existing pio header file
blink.c: $(BINDIR)/$(MODULE)/blink.pio.h
