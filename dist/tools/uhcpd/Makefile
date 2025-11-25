CFLAGS?=-g -O3 -Wall
CFLAGS_EXTRA=-DMODULE_UHCPD
all: bin bin/uhcpd

bin:
	mkdir bin

RIOTBASE:=../../..
UHCP_DIR:=$(RIOTBASE)/sys/net/application_layer/uhcp
RIOT_INCLUDE=$(RIOTBASE)/sys/include
SRCS:=$(UHCP_DIR)/uhcp.c uhcpd.c
HDRS:=$(RIOT_INCLUDE)/net/uhcp.h
bin/uhcpd: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -I$(RIOT_INCLUDE) -I. $(SRCS) -o $@

clean:
	rm -f bin/uhcpd
