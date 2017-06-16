RIOTBASE       := ../../..
RIOT_INCLUDE   := $(RIOTBASE)/sys/include
SHA256_DIR     := $(RIOTBASE)/sys/hashes
SHA256_INCLUDE := $(RIOT_INCLUDE)/hashes
TWEETNACL_DIR  := tweetnacl
TWEETNACL_SRC  := $(TWEETNACL_DIR)/tweetnacl.c randombytes.c
TWEETNACL_HDR  := $(TWEETNACL_DIR)/tweetnacl.h
COMMON_SRC     := common.c
COMMON_HDR     := common.h

RIOT_FIRMWARE_SRC := \
	$(SHA256_DIR)/sha256.c \
	$(RIOTBASE)/sys/checksum/fletcher32.c \
	$(RIOTBASE)/sys/firmware/firmware.c

RIOT_FIRMWARE_HDR := $(RIOT_INCLUDE)/firmware.h \
	$(RIOT_INCLUDE)/hashes/sha256.h \
	$(RIOT_INCLUDE)/checksum/fletcher32.h

FIRMWARE_SRC := $(COMMON_SRC) $(TWEETNACL_SRC) $(RIOT_FIRMWARE_SRC) \
	firmware.c verify.c genkeys.c genmeta.c

FIRMWARE_HDR := $(COMMON_HDR) $(RIOT_FIRMWARE_HDR)

GITCACHE = ../git/git-cache
TWEETNACL_URL = https://github.com/RIOT-OS/tweetnacl.git
TWEETNACL_VERSION = 7ea05c7098a16c87fa66e9166ce301666f3f2623

CFLAGS += -g -I. -O3 -Wall -Wextra -pedantic -std=c99

all: bin/firmware

bin/:
	mkdir -p bin

git-fetch-tweetnacl:
	rm -Rf $(TWEETNACL_DIR)
	mkdir -p $(TWEETNACL_DIR)
	$(GITCACHE) clone "$(TWEETNACL_URL)" "$(TWEETNACL_VERSION)" "$(TWEETNACL_DIR)"
	touch $@

bin/firmware: git-fetch-tweetnacl $(FIRMWARE_HDR) $(FIRMWARE_SRC) Makefile | bin/
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) -I$(TWEETNACL_DIR) $(FIRMWARE_SRC) -o $@

clean:
	rm -rf bin/firmware

distclean:
	rm -rf $(TWEETNACL_DIR)
	rm -f git-fetch-tweetnacl
