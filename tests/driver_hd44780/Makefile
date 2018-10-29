include ../Makefile.tests_common

# the stm32f4discovery does not have the arduino pinout
BOARD_BLACKLIST := stm32f4discovery jiminy-mega256rfr2 slstk3401a slstk3402a \
                   sltb001a slwstk6000b stk3600 stk3700

# currently the test provides config params for arduinos only
FEATURES_REQUIRED += arduino

USEMODULE += hd44780

include $(RIOTBASE)/Makefile.include
