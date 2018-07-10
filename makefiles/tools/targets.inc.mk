# This file contains a collection of targets defined by tools shipped with RIOT.
# The reason for keeping those targets separated from their tool Makfiles is
# that these targets have to be defined after the default build targets
# (as `all`), so `all` will always be the first target defined and thereby the
# default target when `make` is called without any further argument.

.PHONY: mosquitto_rsmb

# target for building the bossac binary
$(RIOTTOOLS)/bossa/bossac:
	@echo "[INFO] bossac binary not found - building it from source"
	@make -C $(RIOTTOOLS)/bossa
	@echo "[INFO] bossac binary successfully build!"

$(RIOTTOOLS)/edbg/edbg: $(RIOTTOOLS)/edbg/Makefile
	@echo "[INFO] edbg binary not found - building it from source now"
	CC= CFLAGS= make -C $(RIOTTOOLS)/edbg
	@echo "[INFO] edbg binary successfully build!"

$(RIOTTOOLS)/mosquitto_rsmb/mosquitto_rsmb:
	@echo "[INFO] rsmb binary not found - building it from source now"
	@make -C $(RIOTTOOLS)/mosquitto_rsmb
	@echo "[INFO] rsmb binary successfully build!"

mosquitto_rsmb: $(RIOTTOOLS)/mosquitto_rsmb/mosquitto_rsmb
	@make -C $(RIOTTOOLS)/mosquitto_rsmb run
