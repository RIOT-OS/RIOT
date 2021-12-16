CARGO_COMPILE_COMMANDS = $(BINDIR)/cargo-compile-commands.json
CARGO_COMPILE_COMMANDS_FLAGS = --clang

# This is duplicating the compile-commands rule because unlike in the use case
# when a $(RIOTBASE)/compile_commands.json is built, we *want* this to be
# per-board and per-application. (The large mechanisms are shared anyway).
#
# Changes relative to the compile-commands rule: This uses lazysponge to keep
# Rust from rebuilding, and uses a custom output file and
# CARGO_COMPILE_COMMAND_FLAGS.
$(CARGO_COMPILE_COMMANDS): $(BUILDDEPS)
	$(Q)DIRS="$(DIRS)" APPLICATION_BLOBS="$(BLOBS)" \
	  "$(MAKE)" -C $(APPDIR) -f $(RIOTMAKE)/application.inc.mk compile-commands
	$(Q)$(RIOTTOOLS)/compile_commands/compile_commands.py $(CARGO_COMPILE_COMMANDS_FLAGS) $(BINDIR) \
	  | $(LAZYSPONGE) $@


$(CARGO_LIB): $(RIOTBUILD_CONFIG_HEADER_C) $(BUILDDEPS) $(CARGO_COMPILE_COMMANDS) FORCE
	$(Q)[ x"${RUST_TARGET}" != x"" ] || (echo "Error: No RUST_TARGET was set for this platform. (Set FEATURES_REQUIRED+=rust_target to catch this earlier)."; exit 1)
	$(Q)CC= CFLAGS= CPPFLAGS= CXXFLAGS= RIOT_COMPILE_COMMANDS_JSON="$(CARGO_COMPILE_COMMANDS)" RIOT_USEMODULE="$(USEMODULE)" cargo $(patsubst +,,+${CARGO_CHANNEL}) build --target $(RUST_TARGET) `if [ x$(CARGO_PROFILE) = xrelease ]; then echo --release; else if [ x$(CARGO_PROFILE) '!=' xdebug ]; then echo "--profile $(CARGO_PROFILE)"; fi; fi` $(CARGO_OPTIONS)

$(APPLICATION_RUST_MODULE).module: $(CARGO_LIB) FORCE
	$(Q)# Ensure no old object files persist. These would lead to duplicate
	$(Q)# symbols, or worse, lingering behaivor of XFA entries.
	$(Q)rm -rf $(BINDIR)/$(APPLICATION_RUST_MODULE)/
	$(Q)mkdir -p $(BINDIR)/$(APPLICATION_RUST_MODULE)/

	$(Q)# On cortex-m0 boards like airfy-beacon, the archive contains a
	$(Q)# bin/thumbv6m-none-eabi.o file; the directory must be present for
	$(Q)# ar to unpack it...
	$(Q)mkdir -p $(BINDIR)/$(APPLICATION_RUST_MODULE)/bin/
	$(Q)cd $(BINDIR)/$(APPLICATION_RUST_MODULE)/ && $(AR) x $<
	$(Q)# ... and move them back if any exist, careful to err if anything is duplicate
	$(Q)rmdir $(BINDIR)/$(APPLICATION_RUST_MODULE)/bin/ || (mv -n $(BINDIR)/$(APPLICATION_RUST_MODULE)/bin/* $(BINDIR)/$(APPLICATION_RUST_MODULE)/ && rmdir $(BINDIR)/$(APPLICATION_RUST_MODULE)/bin/)
