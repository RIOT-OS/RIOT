ifneq (,$(filter auto_init,$(USEMODULE)))
    DIRS += auto_init
endif
ifneq (,$(filter cbor,$(USEMODULE)))
    DIRS += cbor
endif
ifneq (,$(filter config,$(USEMODULE)))
    DIRS += config
endif
ifneq (,$(filter lib,$(USEMODULE)))
    DIRS += lib
endif
ifneq (,$(filter ping,$(USEMODULE)))
    DIRS += ping
endif
ifneq (,$(filter ps,$(USEMODULE)))
    DIRS += ps
endif
ifneq (,$(filter posix,$(USEMODULE)))
    DIRS += posix
endif
ifneq (,$(filter pnet,$(USEMODULE)))
    DIRS += posix/pnet
endif
ifneq (,$(filter pthread,$(USEMODULE)))
    DIRS += posix/pthread
endif
ifneq (,$(filter shell,$(USEMODULE)))
    DIRS += shell
endif
ifneq (,$(filter shell_commands,$(USEMODULE)))
    DIRS += shell/commands
endif
ifneq (,$(filter timex,$(USEMODULE)))
    DIRS += timex
endif
ifneq (,$(filter transceiver,$(USEMODULE)))
    DIRS += transceiver
endif
ifneq (,$(filter uart0,$(USEMODULE)))
    DIRS += uart0
endif
ifneq (,$(filter vtimer,$(USEMODULE)))
    DIRS += vtimer
endif
ifneq (,$(filter net_if,$(USEMODULE)))
    DIRS += net/link_layer/net_if
endif
ifneq (,$(filter destiny,$(USEMODULE)))
    DIRS += net/transport_layer/destiny
endif
ifneq (,$(filter net_help,$(USEMODULE)))
    DIRS += net/crosslayer/net_help
endif
ifneq (,$(filter protocol_multiplex,$(USEMODULE)))
    DIRS += net/link_layer/protocol-multiplex
endif
ifneq (,$(filter sixlowpan,$(USEMODULE)))
    DIRS += net/network_layer/sixlowpan
endif
ifneq (,$(filter sixlowborder,$(USEMODULE)))
    DIRS += net/network_layer/sixlowpan/border
endif
ifneq (,$(filter rpl,$(USEMODULE)))
    DIRS += net/routing/rpl
endif
ifneq (,$(filter routing,$(USEMODULE)))
	DIRS += net/routing
endif
ifneq (,$(filter ieee802154,$(USEMODULE)))
    DIRS += net/link_layer/ieee802154
endif
ifneq (,$(filter bloom,$(USEMODULE)))
    DIRS += bloom
endif
ifneq (,$(filter crypto,$(USEMODULE)))
    DIRS += crypto
endif
ifneq (,$(filter random,$(USEMODULE)))
    DIRS += random
endif
ifneq (,$(filter hashes,$(USEMODULE)))
    DIRS += hashes
endif
ifneq (,$(filter ccn_lite,$(USEMODULE)))
    DIRS += net/ccn_lite
endif
ifneq (,$(filter ccn_lite_client,$(USEMODULE)))
    DIRS += net/ccn_lite/util
endif
ifneq (,$(filter quad_math,$(USEMODULE)))
    DIRS += quad_math
endif
ifneq (,$(filter oneway_malloc,$(USEMODULE)))
    DIRS += oneway-malloc
endif
ifneq (,$(filter color,$(USEMODULE)))
    DIRS += color
endif
ifneq (,$(filter pipe,$(USEMODULE)))
    DIRS += pipe
endif

include $(RIOTBASE)/Makefile.base
