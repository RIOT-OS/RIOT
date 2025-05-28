RIOTBASE       := ../../..
RIOT_INCLUDE   := $(RIOTBASE)/sys/include
RIOT_CORE_INC  := $(RIOTBASE)/core/include
NATIVE_INCLUDE := $(RIOTBASE)/cpu/native/include
COMMON_SRC     := common.c
COMMON_HDR     := common.h

RIOT_HDR_SRC := \
	$(RIOTBASE)/sys/checksum/fletcher32.c \
	$(RIOTBASE)/sys/riotboot/hdr.c

RIOT_HDR_HDR := $(RIOT_INCLUDE)/riotboot/hdr.h \
	$(RIOT_INCLUDE)/checksum/fletcher32.h \
	$(RIOTBASE)/sys/include/byteorder.h

GENHDR_SRC := $(COMMON_SRC) $(RIOT_HDR_SRC) \
	main.c genhdr.c

GENHDR_HDR := $(COMMON_HDR) $(RIOT_HDR_HDR)

CFLAGS += -g -I. -O3 -Wall -Wextra

ifeq ($(QUIET),1)
  Q=@
else
  Q=
endif

all: bin/genhdr

bin/:
	$(Q)mkdir -p bin

bin/genhdr: $(GENHDR_HDR) $(GENHDR_SRC) Makefile | bin/
	$(Q)$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) -I$(RIOT_CORE_INC) \
		-I$(NATIVE_INCLUDE) $(GENHDR_SRC) -o $@

clean:
	$(Q)rm -rf bin/

distclean: clean
