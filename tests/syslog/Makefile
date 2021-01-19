include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += syslog
# Choose syslog backends
USEMODULE += syslog_backend_stdio
USEMODULE += syslog_backend_file

# Choose syslog formatter
USEMODULE += syslog_formatter_default
# USEMODULE += syslog_formatter_fmt

# To use memarray for entries/logs allocation
USEMODULE += memarray
CFLAGS += -DCONFIG_SYSLOG_MEMARRAY=1

# To build openlog/closelog
CFLAGS += -DCONFIG_SYSLOG_HAS_OPEN=1

# To add an extra user-defined backend, in that case
# syslog_backend_extra.h must be provided
CFLAGS += -DCONFIG_SYSLOG_BACKEND_EXTRA=1
INCLUDES += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
