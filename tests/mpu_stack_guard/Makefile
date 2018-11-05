BOARD ?= samr21-xpro
include ../Makefile.tests_common

BOARD_WHITELIST += arduino-due      # cortex-m3
BOARD_WHITELIST += arduino-zero     # cortex-m0plus
BOARD_WHITELIST += cc2538dk         # cortex-m3
BOARD_WHITELIST += cc2650stk        # cortex-m3
BOARD_WHITELIST += ek-lm4f120xl     # cortex-m4f
BOARD_WHITELIST += f4vi1            # cortex-m4f
BOARD_WHITELIST += fox              # cortex-m3
BOARD_WHITELIST += frdm-k64f        # cortex-m4
BOARD_WHITELIST += ikea-tradfri     # cortex-m4f
BOARD_WHITELIST += iotlab-a8-m3     # cortex-m3
BOARD_WHITELIST += iotlab-m3        # cortex-m3
BOARD_WHITELIST += limifrog-v1      # cortex-m3
BOARD_WHITELIST += maple-mini       # cortex-m3
BOARD_WHITELIST += mbed_lpc1768     # cortex-m3
BOARD_WHITELIST += msbiot           # cortex-m4f
BOARD_WHITELIST += mulle            # cortex-m4
BOARD_WHITELIST += nrf52dk          # cortex-m4f
BOARD_WHITELIST += nucleo-f103rb    # cortex-m3
BOARD_WHITELIST += nucleo-f207zg    # cortex-m3
BOARD_WHITELIST += nucleo-f303re    # cortex-m4f
BOARD_WHITELIST += nucleo-f334r8    # cortex-m4f
BOARD_WHITELIST += nucleo-f401re    # cortex-m4f
BOARD_WHITELIST += nucleo-f446re    # cortex-m4f
BOARD_WHITELIST += nucleo-l152re    # cortex-m3
BOARD_WHITELIST += openmote-cc2538  # cortex-m3
BOARD_WHITELIST += pba-d-01-kw2x    # cortex-m4
BOARD_WHITELIST += remote           # cortex-m3
BOARD_WHITELIST += samd21-xpro      # cortex-m0plus
BOARD_WHITELIST += saml21-xpro      # cortex-m0plus
BOARD_WHITELIST += samr21-xpro      # cortex-m0plus
BOARD_WHITELIST += sltb001a         # cortex-m4f
BOARD_WHITELIST += slwstk6220a      # cortex-m4f
BOARD_WHITELIST += sodaq-autonomo   # cortex-m0plus
BOARD_WHITELIST += spark-core       # cortex-m3
BOARD_WHITELIST += stm32f3discovery # cortex-m4f
BOARD_WHITELIST += stm32f4discovery # cortex-m4f
BOARD_WHITELIST += udoo             # cortex-m3

USEMODULE += mpu_stack_guard

include $(RIOTBASE)/Makefile.include

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif
