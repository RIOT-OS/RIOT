APPLICATION = driver_esp8266
include ../Makefile.tests_common

BOARD ?= native

FEATURES_REQUIRED = periph_uart periph_gpio

USEMODULE += esp8266
USEMODULE += esp8266_sock_udp
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6

CFLAGS += -DMY_SSID=\"test\" -DMY_PSK=\"test\"

include $(RIOTBASE)/Makefile.include
include $(RIOTBASE)/pkg/tlsf/Makefile.include
