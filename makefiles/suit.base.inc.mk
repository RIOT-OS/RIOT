#
# path to suit-tool
SUIT_TOOL ?= $(RIOTBASE)/dist/tools/suit_v3/suit-manifest-generator/bin/suit-tool

#
# SUIT encryption keys
#

# Specify key to use.
# Will use $(SUIT_KEY_DIR)/$(SUIT_KEY).pem as combined private/public key
# files.
SUIT_KEY ?= default

ifeq (1, $(RIOT_CI_BUILD))
  SUIT_KEY_DIR ?= $(BINDIR)
else
  SUIT_KEY_DIR ?= $(RIOTBASE)/keys
endif

SUIT_SEC ?= $(SUIT_KEY_DIR)/$(SUIT_KEY).pem

SUIT_PUB_HDR = $(BINDIR)/riotbuild/public_key.h
SUIT_PUB_HDR_DIR = $(dir $(SUIT_PUB_HDR))
CFLAGS += -I$(SUIT_PUB_HDR_DIR)
BUILDDEPS += $(SUIT_PUB_HDR)

$(SUIT_SEC): $(CLEAN)
	@echo suit: generating key in $(SUIT_KEY_DIR)
	@mkdir -p $(SUIT_KEY_DIR)
	@$(RIOTBASE)/dist/tools/suit_v3/gen_key.py $(SUIT_SEC)

# set FORCE so switching between keys using "SUIT_KEY=foo make ..."
# triggers a rebuild even if the new key would otherwise not (because the other
# key's mtime is too far back).
$(SUIT_PUB_HDR): $(SUIT_SEC) FORCE | $(CLEAN)
	@mkdir -p $(SUIT_PUB_HDR_DIR)
	@$(SUIT_TOOL) pubkey -k $(SUIT_SEC) \
	  | '$(LAZYSPONGE)' $(LAZYSPONGE_FLAGS) '$@'

suit/genkey: $(SUIT_SEC)
