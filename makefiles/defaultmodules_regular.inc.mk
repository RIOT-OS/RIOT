# This file declare DEFAULT_MODULEs that MAY have dependencies themselfs
# and where there dependencies MAY have dependencies as well.
# These DEFAULT_MODULEs MUST only be disabled at application level or
# in BOARD/CPU Makefile.default

DEFAULT_MODULE += board board_common_init \
                  cpu \
                  core core_init core_lib core_msg core_panic core_thread \
                  sys libc

# Include potentially added default modules by the board
-include $(BOARDDIR)/Makefile.default

# Include potentially added default modules by the CPU
-include $(RIOTCPU)/$(CPU)/Makefile.default
