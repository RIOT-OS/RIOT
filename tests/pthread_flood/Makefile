include ../Makefile.tests_common

USEMODULE += posix_headers
USEMODULE += pthread
CFLAGS += -DMAXTHREADS=8

# include "fmt" so print_stack_usage needs less stack
USEMODULE += fmt

include $(RIOTBASE)/Makefile.include
