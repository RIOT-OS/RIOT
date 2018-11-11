include ../Makefile.tests_common

# currently the test provides config params for arduinos only
FEATURES_REQUIRED += arduino

USEMODULE += hd44780

# the stm32f4discovery does not have the arduino pinout
BOARD_BLACKLIST := \
  jiminy-mega256rfr2 \
  stm32f4discovery \
  #

include $(RIOTBASE)/Makefile.include
