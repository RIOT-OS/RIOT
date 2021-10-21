include ../Makefile.tests_common

# we use thread flags in this benchmark by default, disable on demand
USEMODULE += core_thread_flags
USEMODULE += benchmark

# select output format
OUTPUT_FORMAT ?= json
USEMODULE += test_utils_result_output_${OUTPUT_FORMAT}

include $(RIOTBASE)/Makefile.include
