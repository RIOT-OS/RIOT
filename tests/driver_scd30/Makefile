include ../Makefile.tests_common

DRIVER ?= scd30

USEMODULE += $(DRIVER)
USEMODULE += printf_float

TEST_ITERATIONS ?= 10
# export iterations for continuous measurements
CFLAGS += -DTEST_ITERATIONS=$(TEST_ITERATIONS)

include $(RIOTBASE)/Makefile.include
