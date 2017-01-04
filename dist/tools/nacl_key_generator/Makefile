RIOTBASE := ../../..
RIOT_INCLUDE = $(RIOTBASE)/sys/include
TWEETNACL_INCLUDE := tweetnacl
TWEETNACL_DIR := tweetnacl
TWEETNACL_SRC := $(TWEETNACL_DIR)/tweetnacl.c randombytes.c
KEYPAIR_SRC := generate-nacl-keys.c $(TWEETNACL_SRC)
GITCACHE = ../git/git-cache
TWEETNACL_URL = https://github.com/RIOT-OS/tweetnacl.git
TWEETNACL_VERSION = 7ea05c7098a16c87fa66e9166ce301666f3f2623

CFLAGS += -g -O3 -Wall -Wextra -pedantic -std=c99

all: bin bin/generate-nacl-keys

bin:
	mkdir bin

git-fetch-tweetnacl:
	rm -Rf $(TWEETNACL_DIR)
	mkdir -p $(TWEETNACL_DIR)
	$(GITCACHE) clone "$(TWEETNACL_URL)" "$(TWEETNACL_VERSION)" "$(TWEETNACL_DIR)"
	touch $@

bin/generate-nacl-keys: git-fetch-tweetnacl
	$(CC) $(CFLAGS) -I$(TWEETNACL_INCLUDE) $(KEYPAIR_SRC) -o $@

clean:
	rm -rf bin/generate-nacl-keys
