include ../Makefile.tests_common

CFLAGS += -DLOG_LEVEL=LOG_ALL

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += conn_can
USEMODULE += can_isotp
USEMODULE += conn_can_isotp_multi
USEMODULE += can_pm
USEMODULE += can_trx

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-mega2560 \
  arduino-uno \
  chronos \
  hifive1 \
  msb-430 \
  msb-430h \
  nucleo-f030r8 \
  nucleo-f031k6 \
  nucleo-f042k6 \
  nucleo-f070rb \
  nucleo-f072rb \
  nucleo-f302r8 \
  nucleo-f303k8 \
  nucleo-f303re \
  nucleo-f334r8 \
  nucleo-l031k6 \
  nucleo-l053r8 \
  stm32f0discovery \
  telosb \
  waspmote-pro \
  wsn430-v1_3b \
  wsn430-v1_4 \
  z1 \
  #

include $(RIOTBASE)/Makefile.include
