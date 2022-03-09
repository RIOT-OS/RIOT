# Workaround to pass distclean to nrf_modem_fw Makefile
PKG_PATHS += $(RIOTTOOLS)/nrf_modem_fw

nrf_modem_fw_flash: $(RIOTTOOLS)/nrf_modem_fw/Makefile
	@$(MAKE) -C $(RIOTTOOLS)/nrf_modem_fw
