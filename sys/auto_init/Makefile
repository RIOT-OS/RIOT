
MODULE_DEFS_NEEDED += \
  asymcute \
  auto_init_benchmark_udp \
  auto_init_can \
  auto_init_dhcpv6_client \
  auto_init_dhcpv6_relay \
  auto_init_gnrc_ipv6 \
  auto_init_gnrc_ipv6_nib \
  auto_init_gnrc_netif \
  auto_init_gnrc_pktbuf \
  auto_init_gnrc_pktdump \
  auto_init_gnrc_rpl \
  auto_init_gnrc_sixlowpan \
  auto_init_gnrc_tcp \
  auto_init_gnrc_udp \
  auto_init_gnrc_uhcpc \
  auto_init_loramac \
  auto_init_lwip \
  auto_init_multimedia \
  auto_init_mynewt_core \
  auto_init_openwsn \
  auto_init_random \
  auto_init_saul \
  auto_init_screen \
  auto_init_security \
  auto_init_sock_dns \
  auto_init_telnet \
  auto_init_unicoap \
  auto_init_usbus \
  auto_init_uwb_core \
  auto_init_xtimer \
  auto_init_ztimer \
  auto_init_ztimer64 \
  cord_common \
  cord_ep_standalone \
  devfs \
  dfplayer \
  driver_cryptocell_310 \
  dsm \
  dummy_thread \
  esp_ble_nimble \
  event_thread \
  gcoap \
  gnrc_dhcpv6_client_simple_pd \
  gnrc_ipv6_auto_subnets_auto_init \
  gnrc_ipv6_static_addr \
  mbedtls \
  mci \
  nanocoap_server_auto_init \
  ndn_riot \
  nimble \
  openthread \
  profiling \
  psa_crypto \
  sched_round_robin \
  schedstatistics \
  shell \
  sht1x \
  skald \
  slipdev_net \
  slipmux \
  sock_dtls \
  suit \
  sys_bus \
  test_utils_interactive_sync \
  vfs_auto_mount \
  ztimer_xtimer_compat \
  #
ifneq (,$(filter gnrc_netif_init,$(USEMODULE)))
  DIRS += netif
endif

ifneq (,$(filter saul_init,$(USEMODULE)))
  DIRS += saul
endif

ifneq (,$(filter auto_init_can,$(USEMODULE)))
  DIRS += can
endif

ifneq (,$(filter auto_init_loramac,$(USEMODULE)))
  DIRS += loramac
endif

ifneq (,$(filter auto_init_multimedia,$(USEMODULE)))
  DIRS += multimedia
endif

ifneq (,$(filter auto_init_usbus,$(USEMODULE)))
  DIRS += usb
endif

ifneq (,$(filter auto_init_security,$(USEMODULE)))
  DIRS += security
endif

ifneq (,$(filter auto_init_screen,$(USEMODULE)))
  DIRS += screen
endif

ifneq (,$(filter auto_init_wdt_event,$(USEMODULE)))
  DIRS += wdt_event
endif

ifneq (,$(filter auto_init_wdt_thread,$(USEMODULE)))
  DIRS += wdt_thread
endif

include $(RIOTBASE)/Makefile.base
