include ../Makefile.tests_common

OUTPUT_FORMAT ?= json
USEMODULE += test_utils_result_output_${OUTPUT_FORMAT}
USEMODULE += shell
USEMODULE += fmt

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

ifndef CONFIG_SHELL_NO_ECHO
  CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
endif

include $(RIOTBASE)/Makefile.include
