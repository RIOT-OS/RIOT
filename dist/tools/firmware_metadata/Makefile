RIOTBASE := ../../..
RIOT_INCLUDE = $(RIOTBASE)/sys/include
SHA256_DIR := $(RIOTBASE)/sys/hashes
SHA256_INCLUDE := $(RIOT_INCLUDE)/hashes
METADATA_SRC := generate-metadata.c $(SHA256_DIR)/sha256.c
METADATA_HDR := $(RIOT_INCLUDE)/fw_slots.h $(RIOT_INCLUDE)/hashes/sha256.h

CFLAGS += -DFW_METADATA_SPACE=$(FW_METADATA_SPACE)
CFLAGS += -g -O3 -Wall -Wextra -pedantic -std=c99

all: bin/generate-metadata

bin/:
	mkdir -p bin

bin/generate-metadata: $(METADATA_HDR) $(METADATA_SRC) | bin/
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) $(METADATA_SRC) -o $@

clean:
	rm -rf bin/generate-metadata
