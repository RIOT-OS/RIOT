#
# path to suit-tool
SUIT_TOOL ?= $(RIOTBASE)/dist/tools/suit/suit-manifest-generator/bin/suit-tool

#
# SUIT encryption keys
#

# Specify key(s) to use.
# Will use $(SUIT_KEY_DIR)/$(SUIT_KEY).pem as combined private/public key
# files.
# Multiple keys can be specified, that means that the firmware will accept
# updates signed with either one of those keys.
# If the firmware accepts multiple keys, let the first key be the signing key.
SUIT_KEY ?= default
SUIT_KEY_SIGN ?= $(word 1, $(SUIT_KEY))
XDG_DATA_HOME ?= $(HOME)/.local/share

ifeq (1, $(RIOT_CI_BUILD))
  SUIT_KEY_DIR ?= $(BINDIR)
else
  SUIT_KEY_DIR ?= $(XDG_DATA_HOME)/RIOT/keys
endif

# we may accept multiple keys for the firmware
SUIT_SEC ?= $(foreach item,$(SUIT_KEY),$(SUIT_KEY_DIR)/$(item).pem)
# but there can only be one signing key
SUIT_SEC_SIGN ?= $(SUIT_KEY_DIR)/$(SUIT_KEY_SIGN).pem

# generate a list of the public keys
SUIT_PUBS ?= $(SUIT_SEC:.pem=.pem.pub)

SUIT_PUB_HDR = $(BINDIR)/riotbuild/public_key.h
SUIT_PUB_HDR_DIR = $(dir $(SUIT_PUB_HDR))
CFLAGS += -I$(SUIT_PUB_HDR_DIR)
BUILDDEPS += $(SUIT_PUB_HDR)

# OpenSSL leaves an empty file if key generation fails - remove it manually
# see https://github.com/openssl/openssl/issues/25440
$(SUIT_SEC): | $(CLEAN)
	$(Q)echo suit: generating key in $(SUIT_KEY_DIR)
	$(Q)mkdir -p $(SUIT_KEY_DIR)
	$(Q)(										\
	printf "0) none\n";								\
	printf "1) aes-256-cbc\n";							\
	printf "Choose encryption for key file $@: ";					\
	if [ -z "$(RIOT_CI_BUILD)" ]; then read encryption; else encryption=0; fi;	\
	case $$encryption in								\
		0)									\
			openssl genpkey -algorithm ed25519 -out $@;			\
			;;								\
		1)									\
			openssl genpkey -algorithm ed25519 -aes-256-cbc -out $@ || :;	\
			;;								\
		*)									\
			echo "Invalid choice";						\
			exit 1;								\
			;;								\
	esac;										\
	)
	$(Q)if [ ! -s $@ ]; then rm $@; fi

# Allow to disable auto-generating public key from private key, e.g. if only the
# public key is available at build time and manifest is created elsewhere.
SUIT_GEN_PUBKEY ?= 1
ifeq (1, $(SUIT_GEN_PUBKEY))
  _PUB_FROM_SEC := %.pem
endif
%.pem.pub: $(_PUB_FROM_SEC)
	$(Q)openssl ec -inform pem -in $< -outform pem -pubout -out $@

# Convert public keys to C headers - only last 32 bytes are key material
#
# set FORCE so switching between keys using "SUIT_KEY=foo make ..."
# triggers a rebuild even if the new key would otherwise not (because the other
# key's mtime is too far back).
$(SUIT_PUB_HDR): $(SUIT_PUBS) FORCE | $(CLEAN)
	$(Q)mkdir -p $(SUIT_PUB_HDR_DIR)
	$(Q)(							\
		echo "const uint8_t public_key[][32] = {";	\
		for i in $(SUIT_PUBS); do			\
			echo " {";				\
			openssl ec -inform pem -pubin -in $$i -outform der | tail -c 32 | xxd -i; \
			echo " },";				\
		done;						\
		echo "};"					\
	) > '$@.tmp'
	$(Q)cmp '$@' '$@.tmp' > /dev/null 2>&1 || mv '$@.tmp' '$@'

suit/genkey: $(SUIT_SEC)
