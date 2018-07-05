PSEUDOMODULES += at_urc
PSEUDOMODULES += auto_init_gnrc_rpl
PSEUDOMODULES += can_mbox
PSEUDOMODULES += can_pm
PSEUDOMODULES += can_raw
PSEUDOMODULES += ccn-lite-utils
PSEUDOMODULES += cbor_ctime
PSEUDOMODULES += cbor_float
PSEUDOMODULES += cbor_semantic_tagging
PSEUDOMODULES += conn_can_isotp_multi
PSEUDOMODULES += core_%
PSEUDOMODULES += emb6_router
PSEUDOMODULES += event_%
PSEUDOMODULES += gnrc_ipv6_default
PSEUDOMODULES += gnrc_ipv6_router
PSEUDOMODULES += gnrc_ipv6_router_default
PSEUDOMODULES += gnrc_ipv6_nib_6lbr
PSEUDOMODULES += gnrc_ipv6_nib_6ln
PSEUDOMODULES += gnrc_ipv6_nib_6lr
PSEUDOMODULES += gnrc_ipv6_nib_dns
PSEUDOMODULES += gnrc_ipv6_nib_router
PSEUDOMODULES += gnrc_netdev_default
PSEUDOMODULES += gnrc_neterr
PSEUDOMODULES += gnrc_netapi_callbacks
PSEUDOMODULES += gnrc_netapi_mbox
PSEUDOMODULES += gnrc_pktbuf_cmd
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
PSEUDOMODULES += lis2dh12_spi
PSEUDOMODULES += log
PSEUDOMODULES += log_printfnoformat
PSEUDOMODULES += lora
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
PSEUDOMODULES += nanocoap_%
PSEUDOMODULES += netdev_default
PSEUDOMODULES += netif
PSEUDOMODULES += netstats
PSEUDOMODULES += netstats_l2
PSEUDOMODULES += netstats_ipv6
PSEUDOMODULES += netstats_rpl
PSEUDOMODULES += newlib
PSEUDOMODULES += newlib_gnu_source
PSEUDOMODULES += newlib_nano
PSEUDOMODULES += openthread
PSEUDOMODULES += pktqueue
PSEUDOMODULES += printf_float
PSEUDOMODULES += prng
PSEUDOMODULES += prng_%
PSEUDOMODULES += rdcli_simple_standalone
PSEUDOMODULES += saul_adc
PSEUDOMODULES += saul_default
PSEUDOMODULES += saul_gpio
PSEUDOMODULES += schedstatistics
PSEUDOMODULES += sock
PSEUDOMODULES += sock_ip
PSEUDOMODULES += sock_tcp
PSEUDOMODULES += sock_udp

# print ascii representation in function od_hex_dump()
PSEUDOMODULES += od_string

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

# include variants of SHT1X drivers as pseudo modules
PSEUDOMODULES += sht10
PSEUDOMODULES += sht11
PSEUDOMODULES += sht15

# include variants of Si114x drivers as pseudo modules
PSEUDOMODULES += si1145
PSEUDOMODULES += si1146
PSEUDOMODULES += si1147

# include variants of Si70xx drivers as pseudo modules
PSEUDOMODULES += si7006
PSEUDOMODULES += si7013
PSEUDOMODULES += si7020
PSEUDOMODULES += si7021

# include variants of RN2XX3 drivers as pseudo modules
PSEUDOMODULES += rn2483
PSEUDOMODULES += rn2903

# add all pseudo random number generator variants as pseudomodules
PSEUDOMODULES += prng_%

# STM32 periph pseudomodules
PSEUDOMODULES += stm32_periph_%

# declare periph submodules as pseudomodules, but exclude periph_common
PSEUDOMODULES += periph_%
NO_PSEUDOMODULES += periph_common

# Submodules and auto-init code provided by Skald
PSEUDOMODULES += auto_init_skald
PSEUDOMODULES += skald_ibeacon
PSEUDOMODULES += skald_eddystone

# Packages may also add modules to PSEUDOMODULES in their `Makefile.include`.
