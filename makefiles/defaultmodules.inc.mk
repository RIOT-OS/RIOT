DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

DEFAULT_MODULE += auto_init

# Initialize all used peripherals by default
DEFAULT_MODULE += periph_init

# Include potentially added default modules by the board
-include $(BOARDDIR)/Makefile.default

# Include potentially added default modules by the CPU
-include $(RIOTCPU)/$(CPU)/Makefile.default
