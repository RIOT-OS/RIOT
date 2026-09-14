include ../Makefile.sys_common

# the feature under test
USEMODULE += stdio_notify

# used to drain stdin once notified
USEMODULE += stdio_available

# used to wake the main thread from the notify and timer callbacks
USEMODULE += core_thread_flags
USEMODULE += ztimer
USEMODULE += ztimer_sec

include $(RIOTBASE)/Makefile.include
