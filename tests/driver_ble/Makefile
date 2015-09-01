APPLICATION = driver_ble
include ../Makefile.tests_common

BOARD = yunjia-nrf51822

FEATURES_REQUIRED = nrf51_ble

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += uart0
USEMODULE += nrf51_ble
USEMODULE += gnrc
USEMODULE += gnrc_pktbuf
USEMODULE += ble_ll

include $(RIOTBASE)/Makefile.include
