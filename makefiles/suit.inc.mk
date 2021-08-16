#
# This file contains stuff related to SUIT manifest generation.
# It depends on SUIT key generation, which can be found in
# makefiles/suit.base.inc.mk
#
#
SUIT_COAP_BASEPATH ?= fw/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

#
SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suit.$(APP_VER).bin
SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-riot.suit.latest.bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suit_signed.$(APP_VER).bin
SUIT_MANIFEST_SIGNED_LATEST ?= $(BINDIR_APP)-riot.suit_signed.latest.bin

SUIT_NOTIFY_VERSION ?= latest
SUIT_NOTIFY_MANIFEST ?= $(APPLICATION)-riot.suit_signed.$(SUIT_NOTIFY_VERSION).bin

# Long manifest names require more buffer space when parsing
export CFLAGS += -DCONFIG_SOCK_URLPATH_MAXLEN=128

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_CLASS ?= $(BOARD)

#
$(SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(Q)$(RIOTBASE)/dist/tools/suit/gen_manifest.py \
	  --urlroot $(SUIT_COAP_ROOT) \
	  --seqnr $(SUIT_SEQNR) \
	  --uuid-vendor $(SUIT_VENDOR) \
	  --uuid-class $(SUIT_CLASS) \
	  -o $@.tmp \
	  $(SLOT0_RIOT_BIN):$(SLOT0_OFFSET) \
	  $(SLOT1_RIOT_BIN):$(SLOT1_OFFSET)

	$(Q)$(SUIT_TOOL) create -f suit -i $@.tmp -o $@

	$(Q)rm -f $@.tmp


$(SUIT_MANIFEST_SIGNED): $(SUIT_MANIFEST) $(SUIT_SEC)
	$(Q)$(SUIT_TOOL) sign -k $(SUIT_SEC) -m $(SUIT_MANIFEST) -o $@

$(SUIT_MANIFEST_LATEST): $(SUIT_MANIFEST)
	$(Q)ln -f -s $< $@

$(SUIT_MANIFEST_SIGNED_LATEST): $(SUIT_MANIFEST_SIGNED)
	$(Q)ln -f -s $< $@

SUIT_MANIFESTS := $(SUIT_MANIFEST) \
                  $(SUIT_MANIFEST_LATEST) \
                  $(SUIT_MANIFEST_SIGNED) \
                  $(SUIT_MANIFEST_SIGNED_LATEST)

suit/manifest: $(SUIT_MANIFESTS)

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
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
