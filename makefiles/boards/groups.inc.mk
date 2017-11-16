MCUGROUP.arm7 := \
  avsextrem \
  msba2 \
  #

MCUGROUP.avr8 := \
  arduino-duemilanove \
  arduino-mega2560 \
  arduino-uno \
  waspmote-pro \
  #

MCUGROUP.cortexm := \
  airfy-beacon \
  arduino-due \
  arduino-mkr1000 \
  arduino-mkrzero \
  arduino-zero \
  b-l072z-lrwan1 \
  bluepill \
  calliope-mini \
  cc2538dk \
  cc2650-launchpad \
  cc2650stk \
  ek-lm4f120xl \
  f4vi1 \
  feather-m0 \
  fox \
  frdm-k22f \
  frdm-k64f \
  iotlab-a8-m3 \
  iotlab-m3 \
  limifrog-v1 \
  maple-mini \
  mbed_lpc1768 \
  microbit \
  msbiot \
  mulle \
  nrf51dongle \
  nrf52840dk \
  nrf52dk \
  nrf6310 \
  nucleo144-f207 \
  nucleo144-f303 \
  nucleo144-f412 \
  nucleo144-f413 \
  nucleo144-f429 \
  nucleo144-f446 \
  nucleo144-f722 \
  nucleo144-f746 \
  nucleo144-f767 \
  nucleo32-f031 \
  nucleo32-f042 \
  nucleo32-f303 \
  nucleo32-l031 \
  nucleo32-l432 \
  nucleo-f030 \
  nucleo-f070 \
  nucleo-f072 \
  nucleo-f091 \
  nucleo-f103 \
  nucleo-f302 \
  nucleo-f303 \
  nucleo-f334 \
  nucleo-f401 \
  nucleo-f410 \
  nucleo-f411 \
  nucleo-f446 \
  nucleo-l053 \
  nucleo-l073 \
  nucleo-l152 \
  nucleo-l476 \
  nz32-sc151 \
  opencm904 \
  openmote-cc2538 \
  pba-d-01-kw2x \
  remote-pa \
  remote-reva \
  remote-revb \
  samd21-xpro \
  saml21-xpro \
  samr21-xpro \
  seeeduino_arch-pro \
  sltb001a \
  slwstk6220a \
  sodaq-autonomo \
  sodaq-explorer \
  spark-core \
  stm32f0discovery \
  stm32f3discovery \
  stm32f4discovery \
  stm32f7discovery \
  udoo \
  yunjia-nrf51822 \
  #

MCUGROUP.native := \
  native \
  #

MCUGROUP.mips := \
  mips-malta \
  pic32-clicker \
  pic32-wifire \
  #

MCUGROUP.msp430 := \
  chronos \
  msb-430 \
  msb-430h \
  telosb \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

MCUGROUP.8bit := $(MCUGROUP.avr8)
MCUGROUP.16bit := $(MCUGROUP.msp430)
MCUGROUP.32bit := $(sort $(MCUGROUP.arm7) $(MCUGROUP.cortexm) $(MCUGROUP.mips) $(MCUGROUP.native))

MCUGROUPS.arch := \
  arm7 \
  avr8 \
  cortexm \
  native \
  mips \
  msp430 \
  #

ALL_BOARDS := $(foreach group,$(MCUGROUPS.arch),$(MCUGROUP.$(group)))

ifneq ($(BOARD), $(filter $(BOARD),$(ALL_BOARDS) none))
  $(error Board $(BOARD) is not in any group)
endif
