BOARD ?= nucleo-f767zi

include ../Makefile.sys_common

USEMODULE += shell_cmds_default
USEMODULE += ps

USEMODULE += conn_can
USEMODULE += can_isotp
USEMODULE += conn_can_isotp_multi
USEMODULE += can_pm
USEMODULE += can_trx
USEMODULE += auto_init_can

FEATURES_REQUIRED += periph_can
FEATURES_REQUIRED += periph_gpio_irq

TRX_TO_BUILD ?= tja1042

ifneq (,$(filter tja1042,$(TRX_TO_BUILD)))
  USEMODULE += tja1042
	TJA1042_STB_PIN ?= GPIO_PIN\(0,0\)

	CFLAGS += -DTJA1042_STB_PIN=$(TJA1042_STB_PIN)
endif

ifneq (,$(filter ncv7356,$(TRX_TO_BUILD)))
  USEMODULE += ncv7356
	NCV7356_MODE0_PIN ?= GPIO_PIN\(0,0\)
	NCV7356_MODE1_PIN ?= GPIO_PIN\(0,1\)

	CFLAGS += -DNCV7356_MODE0_PIN=$(NCV7356_MODE0_PIN)
	CFLAGS += -DNCV7356_MODE1_PIN=$(NCV7356_MODE1_PIN)
endif

CFLAGS += -DCAN_PKT_BUF_SIZE=64
CFLAGS += -DCAN_ROUTER_MAX_FILTER=32

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=4096
endif
