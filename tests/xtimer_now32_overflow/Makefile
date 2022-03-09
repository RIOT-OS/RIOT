include ../Makefile.tests_common

USEMODULE += xtimer
USEMODULE += xtimer_on_periph_timer

DISABLE_MODULE += auto_init_xtimer

# Boards that can't run only on xtimer orh that well select ztimer
BOARD_BLACKLIST += hamilton \
                   mulle \
                   stm32f429i-disco \
                   waspmote-pro  \
                   weact-f401cc \
                   weact-f411ce \
                   #

include $(RIOTBASE)/Makefile.include
