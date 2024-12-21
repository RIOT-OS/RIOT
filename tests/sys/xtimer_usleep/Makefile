include ../Makefile.sys_common

USEMODULE += xtimer

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native native32

# Port and pin configuration for probing with oscilloscope
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
#FEATURES_REQUIRED += periph_gpio
#CFLAGS += -DSLEEP_PIN=7
#CFLAGS += -DSLEEP_PORT=PORT_F

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
