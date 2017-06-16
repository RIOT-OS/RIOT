PKG_NAME = tweetnacl
PKG_URL = https://github.com/RIOT-OS/tweetnacl
PKG_VERSION = 7ea05c7098a16c87fa66e9166ce301666f3f2623
PKG_LICENSE = PD
PKG_BUILDDIR = bin/tweetnacl_src
GITCACHE = ../git/git-cache

RIOTBASE       := ../../..
RIOT_INCLUDE   := $(RIOTBASE)/sys/include
SHA256_DIR     := $(RIOTBASE)/sys/hashes
SHA256_INCLUDE := $(RIOT_INCLUDE)/hashes
TWEETNACL_DIR  := $(PKG_BUILDDIR)
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
	main.c verify.c genkeys.c sign.c

FIRMWARE_HDR := $(COMMON_HDR) $(RIOT_FIRMWARE_HDR)

CFLAGS += -g -I. -O3 -Wall -Wextra -pedantic -std=c99

all: bin/firmware

bin/:
	mkdir -p bin

bin/firmware: git-download $(FIRMWARE_HDR) $(FIRMWARE_SRC) Makefile | bin/
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) -I$(TWEETNACL_DIR) $(FIRMWARE_SRC) -o $@

clean::
	rm -rf bin/firmware

include $(RIOTBASE)/pkg/pkg.mk
