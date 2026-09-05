BOARD ?= sltb001a
include ../Makefile.cpu_common

# this test application is expected to build for every EFM32-based board
FEATURES_REQUIRED += cpu_efm32

# see cpu/efm32/efm32-features.mk for the supported flags
EFM32_LEUART_ENABLED = 0

include $(RIOTBASE)/Makefile.include
