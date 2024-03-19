include ../Makefile.pkg_common

USEMODULE += embunit
USEMODULE += event
USEMODULE += event_timeout_ztimer
USEMODULE += event_thread
USEMODULE += fmt
USEMODULE += ipv6_addr
USEMODULE += od
USEMODULE += shell
USEMODULE += ztimer_msec

USEPKG += libschc
USEMODULE += libschc_coap

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=THREAD_STACKSIZE_DEFAULT

DEBUG_TESTS ?= 0

$(call target-export-variables, test, DEBUG_TESTS)

include $(RIOTBASE)/Makefile.include
