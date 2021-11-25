DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

DEFAULT_MODULE += auto_init

# Initialize all used peripherals by default
DEFAULT_MODULE += periph_init

# If ztimer is used enable rtt autoselection for ztimer_msec, and ztimer_sec.
# This will enable inclusion of `ztimer_periph_rtt` if `periph_rtt` is supported.
DEFAULT_MODULE += ztimer_periph_rtt_auto_select

# Include potentially added default modules by the board
-include $(BOARDDIR)/Makefile.default

# Include potentially added default modules by the CPU
-include $(RIOTCPU)/$(CPU)/Makefile.default
