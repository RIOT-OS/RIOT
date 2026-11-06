$(BINDIR)/$(MODULE)/%.pio.h: %.pio | $(BINDIR)/$(MODULE)/
	$(Q) $(RIOTPKG)/picosdk/pioasm -o RIOT $< $@
