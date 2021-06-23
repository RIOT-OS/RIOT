$(CARGO_LIB): $(RIOTBUILD_CONFIG_HEADER_C) $(BUILDDEPS) FORCE
	$(Q)[ x"${CARGO_TARGET}" != x"" ] || (echo "Error: No CARGO_TARGET was set for this platform"; exit 1)
	$(Q)CC= CFLAGS= CPPFLAGS= CXXFLAGS= RIOT_CC="${CC}" RIOT_CFLAGS="$(CFLAGS_WITH_MACROS) $(INCLUDES) $(CARGO_EXTRACFLAGS)" cargo +$(CARGO_CHANNEL) build --target $(CARGO_TARGET) `if [ x$(CARGO_PROFILE) = xrelease ]; then echo --release; else if [ x$(CARGO_PROFILE) '!=' xdebug ]; then echo "--profile $(CARGO_PROFILE)"; fi; fi` $(CARGO_OPTIONS)

$(APPLICATION_RUST_MODULE).module: $(CARGO_LIB) FORCE
	$(Q)# Ensure no old object files persist. These would lead to duplicate
	$(Q)# symbols, or worse, lingering behaivor of XFA entries.
	$(Q)rm -rf $(BINDIR)/$(APPLICATION_RUST_MODULE)/
	$(Q)mkdir -p $(BINDIR)/$(APPLICATION_RUST_MODULE)/
	$(Q)cd $(BINDIR)/$(APPLICATION_RUST_MODULE)/ && $(AR) x $(PWD)/$<
