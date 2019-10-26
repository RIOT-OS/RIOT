include ../Makefile.tests_common

USEMODULE += ds1307
USEMODULE += embunit
USEMODULE += xtimer

# Blacklist iotlab boards since a different device has the same i2c address
BOARD_BLACKLIST := \
  iotlab-a8-m3 \
  iotlab-m3 \
  #

include $(RIOTBASE)/Makefile.include
