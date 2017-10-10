PSEUDOMODULES += auto_init_gnrc_rpl
PSEUDOMODULES += can_mbox
PSEUDOMODULES += can_pm
PSEUDOMODULES += can_raw
PSEUDOMODULES += cbor_ctime
PSEUDOMODULES += cbor_float
PSEUDOMODULES += cbor_semantic_tagging
PSEUDOMODULES += conn_can_isotp_multi
PSEUDOMODULES += core_%
PSEUDOMODULES += emb6_router
PSEUDOMODULES += gnrc_ipv6_default
PSEUDOMODULES += gnrc_ipv6_router
PSEUDOMODULES += gnrc_ipv6_router_default
PSEUDOMODULES += gnrc_ipv6_nib_6lbr
PSEUDOMODULES += gnrc_ipv6_nib_6ln
PSEUDOMODULES += gnrc_ipv6_nib_6lr
PSEUDOMODULES += gnrc_ipv6_nib_router
PSEUDOMODULES += gnrc_netdev_default
PSEUDOMODULES += gnrc_neterr
PSEUDOMODULES += gnrc_netapi_callbacks
PSEUDOMODULES += gnrc_netapi_mbox
PSEUDOMODULES += gnrc_sixlowpan_border_router_default
PSEUDOMODULES += gnrc_sixlowpan_default
PSEUDOMODULES += gnrc_sixlowpan_iphc_nhc
PSEUDOMODULES += gnrc_sixlowpan_nd_border_router
PSEUDOMODULES += gnrc_sixlowpan_router
PSEUDOMODULES += gnrc_sixlowpan_router_default
PSEUDOMODULES += gnrc_sock_check_reuse
PSEUDOMODULES += gnrc_txtsnd
PSEUDOMODULES += l2filter_blacklist
PSEUDOMODULES += l2filter_whitelist
PSEUDOMODULES += log
PSEUDOMODULES += log_printfnoformat
PSEUDOMODULES += lwip_arp
PSEUDOMODULES += lwip_autoip
PSEUDOMODULES += lwip_dhcp
PSEUDOMODULES += lwip_ethernet
PSEUDOMODULES += lwip_igmp
PSEUDOMODULES += lwip_ipv6_autoconfig
PSEUDOMODULES += lwip_ipv6_mld
PSEUDOMODULES += lwip_raw
PSEUDOMODULES += lwip_sixlowpan
PSEUDOMODULES += lwip_stats
PSEUDOMODULES += lwip_tcp
PSEUDOMODULES += lwip_udp
PSEUDOMODULES += lwip_udplite
PSEUDOMODULES += mpu_stack_guard
PSEUDOMODULES += netdev_default
PSEUDOMODULES += netif
PSEUDOMODULES += netstats
PSEUDOMODULES += netstats_l2
PSEUDOMODULES += netstats_ipv6
PSEUDOMODULES += netstats_rpl
PSEUDOMODULES += newlib
PSEUDOMODULES += newlib_nano
PSEUDOMODULES += openthread
PSEUDOMODULES += pktqueue
PSEUDOMODULES += posix
PSEUDOMODULES += printf_float
PSEUDOMODULES += prng
PSEUDOMODULES += prng_%
PSEUDOMODULES += saul_adc
PSEUDOMODULES += saul_default
PSEUDOMODULES += saul_gpio
PSEUDOMODULES += schedstatistics
PSEUDOMODULES += sock
PSEUDOMODULES += sock_ip
PSEUDOMODULES += sock_tcp
PSEUDOMODULES += sock_udp

# include variants of the AT86RF2xx drivers as pseudo modules
PSEUDOMODULES += at86rf23%
PSEUDOMODULES += at86rf21%

# include variants of the BMX280 drivers as pseudo modules
PSEUDOMODULES += bmp280
PSEUDOMODULES += bme280

# variants of TI ADCXX1C
PSEUDOMODULES += adc081c
PSEUDOMODULES += adc101c
PSEUDOMODULES += adc121c

# include variants of SX127X drivers as pseudo modules
PSEUDOMODULES += sx1272
PSEUDOMODULES += sx1276

# add all pseudo random number generator variants as pseudomodules
PSEUDOMODULES += prng_%

# STM32 periph pseudomodules
PSEUDOMODULES += stm32_periph_%
