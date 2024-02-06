# This file contains a collection of targets defined by tools shipped with RIOT.
# The reason for keeping those targets separated from their tool Makfiles is
# that these targets have to be defined after the default build targets
# (as `all`), so `all` will always be the first target defined and thereby the
# default target when `make` is called without any further argument.

.PHONY: mosquitto_rsmb

# target for building the bossac binary
$(RIOTTOOLS)/bossa-$(BOSSA_VERSION)/bossac:
	@echo "[INFO] bossac $(BOSSA_VERSION) binary not found - building it from source"
	@$(MAKE) -C $(RIOTTOOLS)/bossa-$(BOSSA_VERSION)
	@echo "[INFO] bossac $(BOSSA_VERSION) binary successfully built!"

$(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py: $(RIOTTOOLS)/cc2538-bsl/Makefile
	@echo "[INFO] cc2538-bsl.py not found - fetching it from GitHub now"
	@CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/cc2538-bsl
	@echo "[INFO] cc2538-bsl.py successfully fetched!"

$(RIOTTOOLS)/bootterm/bt: $(RIOTTOOLS)/bootterm/Makefile
	@echo "[INFO] bt binary not found - building it from source now"
	@CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/bootterm
	@echo "[INFO] bt binary successfully built!"

$(PKGDIRBASE)/cosy/cosy.py: $(RIOTTOOLS)/cosy/Makefile
	@echo "[INFO] cosy.py not found - fetching it from GitHub now"
	@CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/cosy
	@echo "[INFO] cosy.py successfully fetched!"

$(RIOTTOOLS)/edbg/edbg: $(RIOTTOOLS)/edbg/Makefile
	@echo "[INFO] edbg binary not found - building it from source now"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/edbg
	@echo "[INFO] edbg binary successfully built!"

$(RIOTTOOLS)/mspdebug/mspdebug: $(RIOTTOOLS)/mspdebug/Makefile
	@echo "[INFO] mspdebug binary not found - building it from source now"
	@echo "[INFO] mspdebug requires readline and libusb-compat headers to build"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/mspdebug
	@echo "[INFO] mspdebug binary successfully built!"

$(RIOTTOOLS)/mosquitto_rsmb/mosquitto_rsmb:
	@echo "[INFO] rsmb binary not found - building it from source now"
	@$(MAKE) -C $(RIOTTOOLS)/mosquitto_rsmb
	@echo "[INFO] rsmb binary successfully built!"

mosquitto_rsmb: $(RIOTTOOLS)/mosquitto_rsmb/mosquitto_rsmb
	@$(MAKE) -C $(RIOTTOOLS)/mosquitto_rsmb run

$(RIOTTOOLS)/setsid/setsid: $(RIOTTOOLS)/setsid/Makefile
	@echo "[INFO] setsid binary not found - building it from source now"
	@$(MAKE) -C $(RIOTTOOLS)/setsid
	@echo "[INFO] setsid binary successfully built!"

$(RIOTTOOLS)/flatc/flatc: $(RIOTTOOLS)/flatc/Makefile
	@echo "[INFO] flatc binary not found - building it from source now"
	$(MAKE) -C $(RIOTTOOLS)/flatc
	@echo "[INFO] flatc binary successfully built!"

$(RIOTTOOLS)/lpc2k_pgm/bin/lpc2k_pgm: $(RIOTTOOLS)/lpc2k_pgm/Makefile
	@echo "[INFO] lpc2k_pgm binary not found - building it from source now"
	@$(MAKE) -C $(RIOTTOOLS)/lpc2k_pgm
	@echo "[INFO] lpc2k_pgm binary successfully built!"

$(RIOTTOOLS)/uf2/uf2conv.py: $(RIOTTOOLS)/uf2/Makefile
	@echo "[INFO] uf2conv.py not found - fetching it from GitHub now"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/uf2
	@echo "[INFO] uf2conv.py successfully fetched!"

$(RIOTTOOLS)/elf2uf2/elf2uf2: $(RIOTTOOLS)/elf2uf2/Makefile
	@echo "[INFO] elf2uf2 not found - fetching it from GitHub now"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/elf2uf2
	@echo "[INFO] elf2uf2 successfully fetched!"

$(RIOTTOOLS)/pioasm/pioasm: $(RIOTTOOLS)/pioasm/Makefile
	@echo "[INFO] pioasm not found - fetching it from GitHub now"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/pioasm
	@echo "[INFO] pioasm successfully fetched!"
