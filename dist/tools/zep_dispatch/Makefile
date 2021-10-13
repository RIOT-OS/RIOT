CFLAGS ?= -g -O3 -Wall -Wextra
CFLAGS += $(RIOT_INCLUDE)
CFLAGS += -DNDEBUG # avoid assert re-definition

BINARY := bin/zep_dispatch
all: bin $(BINARY)

bin:
	mkdir bin

RIOTBASE := ../../..

ZEP_PORT_BASE ?= 17754
TOPOLOGY      ?= example.topo
GV_OUT        ?= example.gv

RIOT_INCLUDE += -I$(RIOTBASE)/core/include
RIOT_INCLUDE += -I$(RIOTBASE)/cpu/native/include
RIOT_INCLUDE += -I$(RIOTBASE)/drivers/include
RIOT_INCLUDE += -I$(RIOTBASE)/sys/include

SRCS := $(wildcard *.c)
SRCS += $(RIOTBASE)/sys/net/link_layer/ieee802154/ieee802154.c

$(BINARY): $(SRCS)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(SRCS) -o $@

.PHONY: clean run graph help
clean:
	rm -f $(BINARY)

run: $(BINARY)
	$(BINARY) -t $(TOPOLOGY) -g $(GV_OUT) ::1 $(ZEP_PORT_BASE)

graph:
	killall -USR1 zep_dispatch
	dot -Tpdf $(GV_OUT) > $(GV_OUT).pdf

help:
	@echo "run	start ZEP dispatcher with the given \$$TOPOLOGY file"
	@echo "graph 	print topology to \$$GV_OUT.pdf"
	@echo "clean	remove ZEP dispatcher binary"
