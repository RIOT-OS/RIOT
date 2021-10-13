include ../Makefile.tests_common

DRIVER ?= sdp3x_irq

USEMODULE += $(DRIVER)
USEMODULE += printf_float

TEST_ITERATIONS ?= 10
# export iterations for continuous measurements
CFLAGS += -DTEST_ITERATIONS=$(TEST_ITERATIONS)

include $(RIOTBASE)/Makefile.include
