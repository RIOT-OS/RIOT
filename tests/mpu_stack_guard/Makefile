BOARD ?= samr21-xpro
include ../Makefile.tests_common


USEMODULE += mpu_stack_guard

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif

BOARD_WHITELIST += \
  arduino-due \
  arduino-zero \
  cc2538dk \
  cc2650stk \
  ek-lm4f120xl \
  f4vi1 \
  fox \
  frdm-k64f \
  ikea-tradfri \
  iotlab-a8-m3 \
  iotlab-m3 \
  limifrog-v1 \
  maple-mini \
  mbed_lpc1768 \
  msbiot \
  mulle \
  nrf52dk \
  nucleo-f103rb \
  nucleo-f207zg \
  nucleo-f303re \
  nucleo-f334r8 \
  nucleo-f401re \
  nucleo-f446re \
  nucleo-l152re \
  openmote-cc2538 \
  pba-d-01-kw2x \
  remote \
  samd21-xpro \
  saml21-xpro \
  samr21-xpro \
  sltb001a \
  slwstk6220a \
  sodaq-autonomo \
  spark-core \
  stm32f3discovery \
  stm32f4discovery \
  udoo \
  #

include $(RIOTBASE)/Makefile.include
