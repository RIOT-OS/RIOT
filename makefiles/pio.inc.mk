%.pio.h: %.pio
	$(Q) $(RIOTTOOLS)/pioasm/pioasm -o RIOT $< $@
