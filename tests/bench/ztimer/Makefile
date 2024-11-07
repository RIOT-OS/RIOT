include ../Makefile.bench_common

USEMODULE += ztimer_usec ztimer_msec

# this test uses 1000 timers by default. for boards that boards don't have
# enough memory, reduce that to 100 or 20, unless NUMOF_TIMERS has been overridden.
LOW_MEMORY_BOARDS += \
  airfy-beacon \
  arduino-mega2560 \
  arduino-mkr1000 \
  arduino-mkrfox1200 \
  arduino-mkrwan1300 \
  arduino-mkrzero \
  arduino-nano-33-iot \
  atmega1284p \
  b-l072z-lrwan1 \
  bastwan \
  blackpill-stm32f103c8 \
  blackpill-stm32f103cb \
  bluepill-stm32f103c8 \
  bluepill-stm32f103cb \
  calliope-mini \
  cc1312-launchpad \
  cc1350-launchpad \
  cc1352-launchpad \
  cc2650-launchpad \
  cc2650stk \
  e104-bt5010a-tb \
  e104-bt5011a-tb \
  derfmega128 \
  feather-m0 \
  feather-m0-lora \
  feather-m0-wifi \
  hifive1 \
  hifive1b \
  i-nucleo-lrwan1 \
  lsn50 \
  maple-mini \
  mega-xplained \
  microbit \
  microduino-corerf \
  msb-430 \
  msb-430h \
  nrf51dongle \
  nucleo-f030r8 \
  nucleo-f042k6 \
  nucleo-f070rb \
  nucleo-f072rb \
  nucleo-f103rb \
  nucleo-f303k8 \
  nucleo-f334r8 \
  nucleo-l031k6 \
  nucleo-l053r8 \
  nucleo-l073rz \
  opencm904 \
  saml10-xpro \
  saml11-xpro \
  seeeduino_xiao \
  sensebox_samd21 \
  serpente \
  sodaq-autonomo \
  sodaq-explorer \
  sodaq-one \
  sodaq-sara-aff \
  sodaq-sara-sff \
  spark-core \
  stm32f0discovery \
  stm32l0538-disco \
  telosb \
  wemos-zero \
  yarm \
  yunjia-nrf51822 \
  z1 \
  #

SUPER_LOW_MEMORY_BOARDS += \
  arduino-duemilanove \
  arduino-leonardo \
  arduino-nano \
  arduino-uno \
  atmega328p \
  atmega328p-xplained-mini \
  nucleo-f031k6 \
  stm32f030f4-demo \
  #

ifneq (, $(filter $(BOARD), $(LOW_MEMORY_BOARDS)))
  NUMOF_TIMERS ?= 100
endif

ifneq (, $(filter $(BOARD), $(SUPER_LOW_MEMORY_BOARDS)))
  NUMOF_TIMERS ?= 12
endif

NUMOF_TIMERS ?= 1000

CFLAGS += -DNUMOF_TIMERS=$(NUMOF_TIMERS)

include $(RIOTBASE)/Makefile.include
