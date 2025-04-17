CFLAGS ?= -g -O3 -Wall -Wextra
CFLAGS += $(RIOT_INCLUDE)
CFLAGS += -DNDEBUG # avoid assert re-definition

DISPATCH := bin/zep_dispatch
TOPOGEN  := bin/topogen
all: $(DISPATCH) $(TOPOGEN)

bin:
	mkdir bin

RIOTBASE := ../../..

ZEP_PORT_BASE ?= 17754
TOPOLOGY      ?= network.topo
GV_OUT        ?= $(TOPOLOGY).gv

RIOT_INCLUDE += -I$(RIOTBASE)/core/lib/include
RIOT_INCLUDE += -I$(RIOTBASE)/cpu/native/include
RIOT_INCLUDE += -I$(RIOTBASE)/drivers/include
RIOT_INCLUDE += -I$(RIOTBASE)/sys/include

SRCS := main.c topology.c zep_parser.c
SRCS += $(RIOTBASE)/sys/net/link_layer/ieee802154/ieee802154.c
SRCS += $(RIOTBASE)/sys/fmt/fmt.c
SRCS += $(RIOTBASE)/sys/net/link_layer/l2util/l2util.c

$(DISPATCH): $(SRCS) bin
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(SRCS) -o $@

$(TOPOGEN): topogen.c bin
	$(CC) $(CFLAGS) $< -o $@ -lm

.PHONY: clean distclean run graph help
clean:
	rm -fr bin

distclean: clean

run: $(DISPATCH) $(TOPOLOGY)
	$(DISPATCH) -t $(TOPOLOGY) -g $(GV_OUT) ::1 $(ZEP_PORT_BASE)

$(TOPOLOGY): $(TOPOGEN)
	./topogen.sh $(TOPOLOGY)

graph:
	killall -USR1 zep_dispatch
	dot -Tpdf $(GV_OUT) > $(GV_OUT).pdf

help:
	@echo "run	start ZEP dispatcher with the given \$$TOPOLOGY file"
	@echo "graph 	print topology to \$$GV_OUT.pdf"
	@echo "clean	remove ZEP dispatcher binary"
