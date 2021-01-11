include ../Makefile.tests_common

USEMODULE += syslog
USEMODULE += syslog_backend_stdio
USEMODULE += syslog_backend_file
USEMODULE += memarray
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# To use fmt instead of snprintf
# USEMODULE += fmt
# CFLAGS += -DCONFIG_SYSLOG_USE_FMT=1

# To build openlog/closelog
CFLAGS += -DCONFIG_SYSLOG_HAS_OPEN=1

include $(RIOTBASE)/Makefile.include
