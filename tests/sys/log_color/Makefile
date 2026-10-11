include ../Makefile.sys_common

USEMODULE += log_color

# Enable debug log level
CFLAGS += -DLOG_LEVEL=LOG_DEBUG

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
