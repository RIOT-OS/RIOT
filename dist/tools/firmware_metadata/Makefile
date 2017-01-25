RIOTBASE := ../../..
RIOT_INCLUDE = $(RIOTBASE)/sys/include
SHA256_DIR := $(RIOTBASE)/sys/hashes
SHA256_INCLUDE := $(RIOT_INCLUDE)/hashes
METADATA_SRC := generate-metadata.c $(SHA256_DIR)/sha256.c
METADATA_HDR := $(RIOT_INCLUDE)/fw_slots.h $(RIOT_INCLUDE)/hashes/sha256.h

CFLAGS += -g -O3 -Wall -Wextra -pedantic -std=c99

all: bin bin/generate-metadata

bin:
	mkdir bin

bin/generate-metadata:
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) $(METADATA_SRC) -o $@

clean:
	rm -rf bin/generate-metadata