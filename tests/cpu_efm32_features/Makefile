BOARD ?= sltb001a
include ../Makefile.tests_common

BOARD_WHITELIST := ikea-tradfri \
                   slstk3401a \
                   slstk3402a \
                   sltb001a \
                   slwstk6000b-slwrb4150a \
                   slwstk6000b-slwrb4162a \
                   stk3600 \
                   stk3700

# see cpu/efm32/efm32-features.mk for the supported flags
EFM32_LEUART_ENABLED = 0

include $(RIOTBASE)/Makefile.include
