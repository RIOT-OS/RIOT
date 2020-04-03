include ../Makefile.tests_common

FEATURES_BLACKLIST += arch_msp430

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += conn_can
USEMODULE += can_isotp
USEMODULE += conn_can_isotp_multi
USEMODULE += can_pm
USEMODULE += can_trx
USEMODULE += auto_init_can

CFLAGS += -DGNRC_PKTBUF_SIZE=4096
CFLAGS += -DCAN_PKT_BUF_SIZE=64
CFLAGS += -DCAN_ROUTER_MAX_FILTER=32

include $(RIOTBASE)/Makefile.include
