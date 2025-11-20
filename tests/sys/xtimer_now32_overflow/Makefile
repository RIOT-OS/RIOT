include ../Makefile.sys_common

USEMODULE += xtimer
USEMODULE += xtimer_no_ztimer_default

DISABLE_MODULE += auto_init_xtimer

# Boards that can't run only on xtimer or that always select ztimer
BOARD_BLACKLIST += \
                   adafruit-feather-nrf52840-express \
                   bitcraze-crazyflie21-main \
                   esp32s2-wemos-mini \
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
                   weact-f401cc \
                   weact-f401ce \
                   weact-f411ce \
                   blackpill-stm32f103cb \
                   bluepill-stm32f103cb \
                   #

include $(RIOTBASE)/Makefile.include
