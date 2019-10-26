include ../Makefile.tests_common

USEMODULE += pca9685
USEMODULE += shell

# These boards are blacklisted since efm32 CPU dosn't support PWM_RIGHT
BOARD_BLACKLIST := \
  slstk3401a \
  slstk3402a \
  sltb001a \
  slwstk6000b-slwrb4150a \
  slwstk6000b-slwrb4162a \
  stk3600 \
  stk3700 \
  #

include $(RIOTBASE)/Makefile.include
