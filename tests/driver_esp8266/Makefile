APPLICATION = driver_esp8266
include ../Makefile.tests_common

BOARD ?= native

FEATURES_REQUIRED = periph_uart periph_gpio

USEMODULE += esp8266
USEMODULE += ipv4_addr
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
include $(RIOTBASE)/pkg/tlsf/Makefile.include
