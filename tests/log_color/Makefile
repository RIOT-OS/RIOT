include ../Makefile.tests_common

USEMODULE += log_color

# Enable debug log level
CFLAGS += -DLOG_LEVEL=4

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
