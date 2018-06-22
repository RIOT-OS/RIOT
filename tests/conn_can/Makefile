include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo \
                             arduino-mega2560 arduino-nano \
                             arduino-uno chronos hifive1 hifive1b i-nucleo-lrwan1 \
                             msb-430 msb-430h \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f303k8 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f070rb \
                             nucleo-f072rb nucleo-f302r8 nucleo-f303re \
                             nucleo-f334r8 nucleo-l053r8 saml10-xpro \
                             saml11-xpro stm32f0discovery stm32l0538-disco \
                             telosb waspmote-pro \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += conn_can
USEMODULE += can_isotp
USEMODULE += conn_can_isotp_multi
USEMODULE += can_pm
USEMODULE += can_trx

FEATURES_REQUIRED += periph_can
FEATURES_REQUIRED += periph_gpio_irq

CFLAGS += -DGNRC_PKTBUF_SIZE=4096
CFLAGS += -DCAN_PKT_BUF_SIZE=64
CFLAGS += -DCAN_ROUTER_MAX_FILTER=32

include $(RIOTBASE)/Makefile.include
