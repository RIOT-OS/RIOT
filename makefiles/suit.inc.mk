#
# This file contains stuff related to SUIT manifest generation.
# It depends on SUIT key generation, which can be found in
# makefiles/suit.base.inc.mk
#
#

# Mandatory APP_VER, set to epoch by default
EPOCH = $(call memoized,EPOCH,$(shell date +%s))
APP_VER ?= $(EPOCH)

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_CLASS ?= $(BOARD)

SUIT_COAP_BASEPATH ?= fw/$(APPLICATION)/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

BINDIR_SUIT = $(BINDIR)/suit_files
$(BINDIR_SUIT): $(CLEAN)
	$(Q)mkdir -p $(BINDIR_SUIT)

#
SUIT_MANIFEST_BASENAME ?= riot.suit
SUIT_MANIFEST ?= $(BINDIR_SUIT)/$(SUIT_MANIFEST_BASENAME)_unsigned.$(SUIT_SEQNR).bin
SUIT_MANIFEST_LATEST ?= $(BINDIR_SUIT)/$(SUIT_MANIFEST_BASENAME)_unsigned.latest.bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_SUIT)/$(SUIT_MANIFEST_BASENAME).$(SUIT_SEQNR).bin
SUIT_MANIFEST_SIGNED_LATEST ?= $(BINDIR_SUIT)/$(SUIT_MANIFEST_BASENAME).latest.bin

SUIT_NOTIFY_VERSION ?= latest
SUIT_NOTIFY_MANIFEST ?= $(SUIT_MANIFEST_BASENAME).$(SUIT_NOTIFY_VERSION).bin

ifneq (,$(SUIT_SEC_PASSWORD))
  SUIT_TOOL_ARGS += -p $(SUIT_SEC_PASSWORD)
endif

# Long manifest names require more buffer space when parsing
export CFLAGS += -DCONFIG_SOCK_URLPATH_MAXLEN=128
export CFLAGS += -DSUIT_VENDOR_DOMAIN="\"$(SUIT_VENDOR)\""

SUIT_MANIFEST_PAYLOADS ?= $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
SUIT_MANIFEST_SLOTFILES ?= $(SLOT0_RIOT_BIN):$(SLOT0_OFFSET) \
                           $(SLOT1_RIOT_BIN):$(SLOT1_OFFSET)

$(SUIT_MANIFEST): $(SUIT_MANIFEST_PAYLOADS) $(BINDIR_SUIT)
	$(Q)$(RIOTBASE)/dist/tools/suit/gen_manifest.py \
	  --urlroot $(SUIT_COAP_ROOT) \
	  --seqnr $(SUIT_SEQNR) \
	  --uuid-vendor $(SUIT_VENDOR) \
	  --uuid-class $(SUIT_CLASS) \
	  -o $@.tmp \
	  $(SUIT_MANIFEST_SLOTFILES)

	$(Q)$(SUIT_TOOL) create -f suit -i $@.tmp -o $@

	$(Q)rm -f $@.tmp

$(SUIT_MANIFEST_SIGNED): $(SUIT_MANIFEST) $(SUIT_SEC)
	$(Q)$(SUIT_TOOL) sign $(SUIT_TOOL_ARGS) -k $(SUIT_SEC_SIGN) -m $(SUIT_MANIFEST) -o $@

$(SUIT_MANIFEST_LATEST): $(SUIT_MANIFEST)
	$(Q)ln -f -s $< $@

$(SUIT_MANIFEST_SIGNED_LATEST): $(SUIT_MANIFEST_SIGNED)
	$(Q)ln -f -s $< $@

SUIT_MANIFESTS := $(SUIT_MANIFEST_SIGNED) \
                  $(SUIT_MANIFEST_SIGNED_LATEST)

suit/manifest: $(SUIT_MANIFESTS)

suit/publish: $(SUIT_MANIFESTS) $(SUIT_MANIFEST_PAYLOADS)
	$(Q)mkdir -p $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	$(Q)cp $^ $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	$(Q)for file in $^; do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$(basename $$file)\""; \
	done

suit/notify: | $(filter suit/publish, $(MAKECMDGOALS))
	$(Q)test -n "$(SUIT_CLIENT)" || { echo "error: SUIT_CLIENT unset!"; false; }
	aiocoap-client -m POST "coap://$(SUIT_CLIENT)/suit/trigger" \
		--payload "$(SUIT_COAP_ROOT)/$(SUIT_NOTIFY_MANIFEST)" && \
		echo "Triggered $(SUIT_CLIENT) to update."
