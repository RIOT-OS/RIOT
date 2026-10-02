MODULE = pio_i2c

include $(RIOTBASE)/Makefile.base
include $(RIOTCPU)/rpx0xx/pio/pio.inc.mk

# The .pio files are compiled into headers which are included by 'i2c.c' and
# not by an user application.
CFLAGS += -I$(BINDIR)/$(MODULE)

i2c.c: $(BINDIR)/$(MODULE)/i2c.pio.h $(BINDIR)/$(MODULE)/i2c_set_scl_sda.pio.h
