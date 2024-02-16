include ../Makefile.sys_common

USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += xtimer_no_ztimer_default

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native native64
# Boards that can't run only on xtimer or that always select ztimer
BOARD_BLACKLIST += \
                   esp32s2-wemos-mini \
                   feather-nrf52840 \
                   hamilton \
                   mulle \
                   pinetime \
                   ruuvitag \
                   seeedstudio-gd32 \
                   sipeed-longan-nano \
                   sipeed-longan-nano-tft \
                   stm32f429i-disco \
                   stm32f4discovery \
                   thingy52 \
                   waspmote-pro  \
                   weact-f401cc \
                   weact-f401ce \
                   weact-f411ce \
                   blackpill-stm32f103cb \
                   bluepill-stm32f103cb \
                   #

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
