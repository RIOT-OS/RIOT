include ../Makefile.tests_common

# the stm32f4discovery does not have the arduino pinout
# Sodaq boards don't have ARDUINO_PIN_*
BOARD_BLACKLIST := stm32f4discovery slstk3401a slstk3402a \
                   sltb001a slwstk6000b-slwrb4150a slwstk6000b-slwrb4162a \
                   stk3600 stk3700 \
                   sodaq-autonomo sodaq-explorer sodaq-one sodaq-sara-aff

# currently the test provides config params for arduinos only
FEATURES_REQUIRED += arduino

USEMODULE += hd44780

include $(RIOTBASE)/Makefile.include
