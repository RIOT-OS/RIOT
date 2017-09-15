# This file contains a collection of targets defined by tools shipped with RIOT.
# The reason for keeping those targets separated from their tool Makfiles is
# that these targets have to be defined after the default build targets
# (as `all`), so `all` will always be the first target defined and thereby the
# default target when `make` is called without any further argument.

# target for building the bossac binary
$(RIOTBASE)/dist/tools/bossa/bossac:
	@echo "[INFO] bossac binary not found - building it from source"
	@make -C $(RIOTBASE)/dist/tools/bossa
	@echo "[INFO] bossac binary successfully build!"

$(RIOTBASE)/dist/tools/edbg/edbg:
	@echo "[INFO] edbg binary not found - building it from source now"
	CC= CFLAGS= make -C $(RIOTBASE)/dist/tools/edbg
	@echo "[INFO] edbg binary successfully build!"

$(RIOTBASE)/dist/tools/inga_tool/inga_tool:
	@echo "[INFO] inga_tool binary not found - building it from source"
	@make -C $(RIOTBASE)/dist/tools/inga_tool
	@echo "[INFO] inga_tool binary successfully build!"
