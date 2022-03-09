include ../Makefile.tests_common

USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += xtimer_on_periph_timer

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native
# Boards that can't run only on xtimer orh that well select ztimer
BOARD_BLACKLIST += hamilton \
                   mulle \
                   stm32f429i-disco \
                   waspmote-pro  \
                   weact-f401cc \
                   weact-f411ce \
                   #

include $(RIOTBASE)/Makefile.include
