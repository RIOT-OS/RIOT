include ../Makefile.tests_common

USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

# Port and pin configuration for probing with oscilloscope
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
#FEATURES_REQUIRED += periph_gpio
#CFLAGS += -DSLEEP_PIN=7
#CFLAGS += -DSLEEP_PORT=PORT_F

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
