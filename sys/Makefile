ifneq (,$(filter pktbuf,$(USEMODULE)))
    DIRS += net/crosslayer/pktbuf
endif
ifneq (,$(filter pnet,$(USEMODULE)))
    DIRS += posix/pnet
endif
ifneq (,$(filter pthread,$(USEMODULE)))
    DIRS += posix/pthread
endif
ifneq (,$(filter shell_commands,$(USEMODULE)))
    DIRS += shell/commands
endif
ifneq (,$(filter net_if,$(USEMODULE)))
    DIRS += net/link_layer/net_if
endif
ifneq (,$(filter l2_ping,$(USEMODULE)))
    DIRS += net/link_layer/ping
endif
ifneq (,$(filter nomac,$(USEMODULE)))
    DIRS += net/link_layer/nomac
endif
ifneq (,$(filter transport_layer,$(USEMODULE)))
	USEMODULE += udp
	USEMODULE += tcp
endif
ifneq (,$(filter socket_base,$(USEMODULE)))
    DIRS += net/transport_layer/socket_base
endif
ifneq (,$(filter udp,$(USEMODULE)))
    DIRS += net/transport_layer/udp
endif
ifneq (,$(filter tcp,$(USEMODULE)))
    DIRS += net/transport_layer/tcp
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
ifneq (,$(filter aodvv2,$(USEMODULE)))
    DIRS += net/routing/aodvv2
endif
ifneq (,$(filter ieee802154,$(USEMODULE)))
    DIRS += net/link_layer/ieee802154
endif
ifneq (,$(filter ccn_lite,$(USEMODULE)))
    DIRS += net/ccn_lite
endif
ifneq (,$(filter ccn_lite_client,$(USEMODULE)))
    DIRS += net/ccn_lite/util
endif
ifneq (,$(filter oneway_malloc,$(USEMODULE)))
    DIRS += oneway-malloc
endif
ifneq (,$(filter netapi,$(USEMODULE)))
    DIRS += net/crosslayer/netapi
endif
ifneq (,$(filter trickle,$(USEMODULE)))
    DIRS += trickle
endif
ifneq (,$(filter ng_netapi,$(USEMODULE)))
    DIRS += net/crosslayer/ng_netapi
endif

DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

include $(RIOTBASE)/Makefile.base
