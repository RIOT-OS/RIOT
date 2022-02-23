DEFAULT_MODULE += board board_common_init cpu core core_init core_msg core_panic sys

# Include potentially added default modules by the board
-include $(BOARDDIR)/Makefile.default

# Include potentially added default modules by the CPU
-include $(RIOTCPU)/$(CPU)/Makefile.default
