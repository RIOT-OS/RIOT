%.pio.h: %.pio
	$(Q) $(RIOTPKG)/picosdk/pioasm -o RIOT $< $@
