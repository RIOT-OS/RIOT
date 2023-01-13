#
# path to suit-tool
SUIT_TOOL ?= $(RIOTBASE)/dist/tools/suit/suit-manifest-generator/bin/suit-tool

#
# SUIT encryption keys
#

# Specify key to use.
# Will use $(SUIT_KEY_DIR)/$(SUIT_KEY).pem as combined private/public key
# files.
SUIT_KEY ?= default
XDG_DATA_HOME ?= $(HOME)/.local/share

ifeq (1, $(RIOT_CI_BUILD))
  SUIT_KEY_DIR ?= $(BINDIR)
else
  SUIT_KEY_DIR ?= $(XDG_DATA_HOME)/RIOT/keys
endif

SUIT_SEC ?= $(SUIT_KEY_DIR)/$(SUIT_KEY).pem

# Multiple keys can be specified with "key0:pw0 key1:pw1 …" (pw may be empty)
SUIT_SECS ?= $(SUIT_SEC):$(SUIT_SEC_PASSWORD)

SUIT_PUB_HDR = $(BINDIR)/riotbuild/public_key.h
SUIT_PUB_HDR_DIR = $(dir $(SUIT_PUB_HDR))
CFLAGS += -I$(SUIT_PUB_HDR_DIR)
BUILDDEPS += $(SUIT_PUB_HDR)

$(SUIT_SEC): | $(CLEAN)
	$(Q)echo suit: generating key in $(SUIT_KEY_DIR)
	$(Q)mkdir -p $(SUIT_KEY_DIR)
	$(Q)$(RIOTBASE)/dist/tools/suit/gen_key.py $(SUIT_SEC) $(SUIT_SEC_PASSWORD)

# set FORCE so switching between keys using "SUIT_KEY=foo make ..."
# triggers a rebuild even if the new key would otherwise not (because the other
# key's mtime is too far back).
$(SUIT_PUB_HDR): $(SUIT_SEC) FORCE | $(CLEAN)
	$(Q)mkdir -p $(SUIT_PUB_HDR_DIR)
	$(Q)(							\
		echo "const uint8_t public_key[][32] = {";	\
		for i in $(SUIT_SECS); do			\
			key=$${i%:*};				\
			pw=$${i#*:};				\
			if [ "$$key" = "$$pw" ]; then		\
				unset pw;			\
			fi;					\
			if [ -z "$$pw" ]; then			\
				$(SUIT_TOOL) pubkey -f header -k $$key; \
			else					\
				$(SUIT_TOOL) pubkey -f header -k $$key -p $$pw; \
			fi					\
		done;						\
		echo "};"					\
	) | '$(LAZYSPONGE)' $(LAZYSPONGE_FLAGS) '$@'

suit/genkey: $(SUIT_SEC)
