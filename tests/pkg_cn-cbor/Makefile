include ../Makefile.tests_common

USEPKG += cn-cbor
USEMODULE += embunit
USEMODULE += fmt
USEMODULE += memarray

# Tests will fail on platforms <64 bit if not set.
# Workaround for missing overflow detection in cn-cbor.
CFLAGS += -DCBOR_NO_LL

# Skips test cases for floating point data types.
# CFLAGS += -DCBOR_NO_FLOAT

include $(RIOTBASE)/Makefile.include
