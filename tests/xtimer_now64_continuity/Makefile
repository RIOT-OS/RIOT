include ../Makefile.tests_common

USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += xtimer_no_ztimer_default

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native
# Boards that can't run only on xtimer or that always select ztimer
BOARD_BLACKLIST += \
                   feather-nrf52840 \
                   hamilton \
                   mulle \
                   pinetime \
                   ruuvitag \
                   stm32f429i-disco \
                   thingy52 \
                   waspmote-pro  \
                   weact-f401cc \
                   weact-f401ce \
                   weact-f411ce \
                   #

include $(RIOTBASE)/Makefile.include
