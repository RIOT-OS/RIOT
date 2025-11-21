## @defgroup pseudomodules Generic pseudomodules
## @brief Modules influencing general RIOT behavior
##
## These are implemented in other modules or core components,
## and serve to enable certain functionality.
##
## Here, pseudomodules are used instead of plain defines (that would be set using `CFLAGS += -DMODULE_NAME`)
## because they can participate in dependency resolution:
## they can pull in other modules.
##
## Pseudomodules are often enabled automatically through module dependencies,
## but can also be enabled manually by stating `USEMODULE += module_name` in the Makefile.
##
## The list of documented pseudomodules is not comprehensive by far;
## @ref makefiles/pseudomodules.inc.mk lists all that are not defined inside their main modules.
## Not all modules listed there are "generic" pseudomodules;
## some are merely optional components of a particular subsystem and should be documented there.
##
## See also <a href="creating-modules.html#pseudomodules">the documentation on pseudomodules in general</a>.
##
## @{

PSEUDOMODULES += atomic_utils
PSEUDOMODULES += base64url

## @defgroup pseudomodule_board_software_reset board_software_reset
## @brief Use any software-only reset button on the board to reboot
##
## Some boards have reset buttons that are not wired to the MCU's reset line,
## but merely are configured to cause a reset by convention.
##
## If this module is active, the button will be configured thusly (and then not
## be advertised in any other capacity, e.g. through @ref sys_auto_init_saul).
PSEUDOMODULES += board_software_reset

PSEUDOMODULES += arduino_pwm
PSEUDOMODULES += arduino_serial_stdio
## @defgroup pseudomodule_arm_stack_limit arm_stack_limit
## @{
## @brief Set MSP/PSP stack lower limit
##
## Use PSPLIM and MSPLIM ARM registers to set the lower limit of a stack
## This is a protection mechanism to catch stack overflow early before it
## can corrupt adjacent memory. Only available on ARMv8-M architecture.
PSEUDOMODULES += cortexm_stack_limit
## @}

PSEUDOMODULES += can_mbox
PSEUDOMODULES += can_pm
PSEUDOMODULES += can_raw
PSEUDOMODULES += can_rx_mailbox
PSEUDOMODULES += ccn-lite-utils
PSEUDOMODULES += cc2538_rf_obs_sig
PSEUDOMODULES += conn_can_isotp_multi
PSEUDOMODULES += cord_ep_standalone
PSEUDOMODULES += core_%
PSEUDOMODULES += cortexm_fpu
PSEUDOMODULES += cortexm_svc
PSEUDOMODULES += cpp
PSEUDOMODULES += cpu_check_address
PSEUDOMODULES += crc16_fast
PSEUDOMODULES += crc32_fast
PSEUDOMODULES += credman_load
PSEUDOMODULES += dbgpin
PSEUDOMODULES += devfs_%
PSEUDOMODULES += dhcpv6_%
PSEUDOMODULES += dhcpv6_client_dns
PSEUDOMODULES += dhcpv6_client_ia_pd
PSEUDOMODULES += dhcpv6_client_ia_na
PSEUDOMODULES += dhcpv6_client_mud_url
PSEUDOMODULES += dhcpv6_relay
PSEUDOMODULES += dns_cache
PSEUDOMODULES += dns_msg
PSEUDOMODULES += ecc_%
PSEUDOMODULES += ethos_stdio
PSEUDOMODULES += event_%
PSEUDOMODULES += event_timeout
PSEUDOMODULES += event_timeout_ztimer
PSEUDOMODULES += evtimer_mbox
PSEUDOMODULES += fatfs_vfs_format
PSEUDOMODULES += fdcan
PSEUDOMODULES += fmt_%
PSEUDOMODULES += gcoap_forward_proxy
PSEUDOMODULES += gcoap_forward_proxy_thread
PSEUDOMODULES += gcoap_fileserver
PSEUDOMODULES += gcoap_dtls
## @addtogroup net_gcoap_dns
## @{
## Enable @ref net_gcoap_dns
PSEUDOMODULES += gcoap_dns
## Enable the @ref gcoap_dns_server_proxy_set function
PSEUDOMODULES += gcoap_dns_proxied
## @}
PSEUDOMODULES += fido2_tests
## @addtogroup net_dhcpv6_client
## @{
## @defgroup net_gnrc_dhcpv6_client	gnrc_dhcpv6_client: Basic DHCPv6 client implementation in GNRC
## @{
PSEUDOMODULES += gnrc_dhcpv6_client
## @}
## @defgroup net_gnrc_dhcpv6_client_6lbr gnrc_dhcpv6_client_6lbr: Basic client for GNRC 6LoWPAN BRs
## @{
PSEUDOMODULES += gnrc_dhcpv6_client_6lbr
## @}
## @addtogroup net_dhcpv6_client_simple_pd
## @{
PSEUDOMODULES += gnrc_dhcpv6_client_simple_pd
## @}
## @}
PSEUDOMODULES += gnrc_ipv6_auto_subnets_auto_init
PSEUDOMODULES += gnrc_ipv6_auto_subnets_eui
PSEUDOMODULES += gnrc_ipv6_auto_subnets_simple
PSEUDOMODULES += gnrc_ipv6_classic
PSEUDOMODULES += gnrc_ipv6_default
PSEUDOMODULES += gnrc_ipv6_ext_frag_stats
PSEUDOMODULES += gnrc_ipv6_router
PSEUDOMODULES += gnrc_ipv6_router_default
PSEUDOMODULES += gnrc_ipv6_nib_6lbr
PSEUDOMODULES += gnrc_ipv6_nib_6ln
PSEUDOMODULES += gnrc_ipv6_nib_6lr
PSEUDOMODULES += gnrc_ipv6_nib_dns
PSEUDOMODULES += gnrc_ipv6_nib_rio
PSEUDOMODULES += gnrc_ipv6_nib_router
PSEUDOMODULES += gnrc_ipv6_nib_rtr_adv_pio_cb
PSEUDOMODULES += gnrc_lorawan_1_1
PSEUDOMODULES += gnrc_neterr
PSEUDOMODULES += gnrc_netapi_callbacks
PSEUDOMODULES += gnrc_netapi_mbox
PSEUDOMODULES += gnrc_netif_bus
PSEUDOMODULES += gnrc_netif_timestamp
PSEUDOMODULES += gnrc_netif_6lo
PSEUDOMODULES += gnrc_netif_ipv6
PSEUDOMODULES += gnrc_netif_single
PSEUDOMODULES += gnrc_netif_dedup


## @addtogroup 	net_gnrc_nettype
## @{

## @defgroup 	net_gnrc_nettype_ccn        gnrc_nettype_ccn
## @{
## Enables @ref GNRC_NETTYPE_CCN and @ref GNRC_NETTYPE_CCN_CHUNK
PSEUDOMODULES += gnrc_nettype_ccn
## @}

## @defgroup 	net_gnrc_nettype_icmpv6     gnrc_nettype_icmpv6
## Enables @ref GNRC_NETTYPE_ICMPV6.
## @{
PSEUDOMODULES += gnrc_nettype_icmpv6
## @}

## @defgroup 	net_gnrc_nettype_ipv6       gnrc_nettype_ipv6
## Enables @ref GNRC_NETTYPE_IPV6.
## @{
PSEUDOMODULES += gnrc_nettype_ipv6
## @}

## @defgroup 	net_gnrc_nettype_ipv6_ext   gnrc_nettype_ipv6_ext
## @{
## Enables @ref GNRC_NETTYPE_IPV6_EXT.
PSEUDOMODULES += gnrc_nettype_ipv6_ext
## @}

## @defgroup 	net_gnrc_nettype_lorawan    gnrc_nettype_lorawan
## Enables @ref GNRC_NETTYPE_LORAWAN
##
## @deprecated  LoRaWAN payloads do not have a special type anymore and just use
##              @ref GNRC_NETTYPE_UNDEF. There is no module needed for that.
##              This module will be removed after 2024.10 release.
## @{
PSEUDOMODULES += gnrc_nettype_lorawan
## @}

## @defgroup 	net_gnrc_nettype_ndn        gnrc_nettype_ndn
## Enables @ref GNRC_NETTYPE_NDN
## @{
PSEUDOMODULES += gnrc_nettype_ndn
## @}

## @defgroup 	net_gnrc_nettype_sixlowpan  gnrc_nettype_sixlowpan
## Enables @ref GNRC_NETTYPE_SIXLOWPAN
## @{
PSEUDOMODULES += gnrc_nettype_sixlowpan
## @}

## @defgroup 	net_gnrc_nettype_tcp        gnrc_nettype_tcp
## Enables @ref GNRC_NETTYPE_TCP
## @{
PSEUDOMODULES += gnrc_nettype_tcp
## @}

## @defgroup 	net_gnrc_nettype_udp        gnrc_nettype_udp
## Enables @ref GNRC_NETTYPE_UDP
## @{
PSEUDOMODULES += gnrc_nettype_udp
## @}
## @}


PSEUDOMODULES += gnrc_sixloenc
PSEUDOMODULES += gnrc_sixlowpan_border_router_default
PSEUDOMODULES += gnrc_sixlowpan_default
PSEUDOMODULES += gnrc_sixlowpan_frag_hint
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_ecn
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_ecn_if_in
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_ecn_if_out
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_ecn_fqueue
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_stats
##
## @addtogroup net_gnrc_sixlowpan_frag_sfr_congure
## @{
##
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_congure
## @defgroup net_gnrc_sixlowpan_frag_sfr_congure_abe gnrc_sixlowpan_frag_sfr_congure_abe: TCP Reno with ABE
## @brief  Congestion control for SFR using the [TCP Reno congestion control algorithm with ABE](@ref sys_congure_abe)
##
## Provides an Alternative Backoff with Explicit Content Notification (ABE) to TCP-Reno-based congestion
## control
## @{
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_congure_abe
## @}
## @defgroup net_gnrc_sixlowpan_frag_sfr_congure_reno gnrc_sixlowpan_frag_sfr_congure_reno: TCP Reno
## @brief  Congestion control for SFR using the [TCP Reno congestion control algorithm](@ref sys_congure_reno)
## @{
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_congure_reno
## @}
## @defgroup net_gnrc_sixlowpan_frag_sfr_congure_quic gnrc_sixlowpan_frag_sfr_congure_quic: QUIC CC
## @brief  Congestion control for SFR using the [congestion control algorithm of QUIC](@ref sys_congure_quic)
## @{
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_congure_quic
## @}
## @defgroup net_gnrc_sixlowpan_frag_sfr_congure_sfr gnrc_sixlowpan_frag_sfr_congure_sfr: Appendix C
## @brief  Basic congestion control for 6LoWPAN SFR as proposed in Appendix C of RFC 8931
## @see    [RFC 8931, Appendix C](https://tools.ietf.org/html/rfc8931#section-appendix.c)
## @{
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_congure_sfr
## @}
## @}
PSEUDOMODULES += gnrc_sixlowpan_iphc_nhc
PSEUDOMODULES += gnrc_sixlowpan_nd_border_router
PSEUDOMODULES += gnrc_sixlowpan_router_default
PSEUDOMODULES += gnrc_sock_async
PSEUDOMODULES += gnrc_sock_check_reuse
PSEUDOMODULES += gnrc_txtsnd
PSEUDOMODULES += ieee802154_security
PSEUDOMODULES += ieee802154_submac
PSEUDOMODULES += ipv4
PSEUDOMODULES += ipv6
PSEUDOMODULES += l2filter_blacklist
PSEUDOMODULES += l2filter_whitelist
PSEUDOMODULES += libstdcpp
PSEUDOMODULES += log
PSEUDOMODULES += lora
PSEUDOMODULES += lwext4_no_gpl
PSEUDOMODULES += lwext2_vfs
PSEUDOMODULES += lwext3_vfs
PSEUDOMODULES += lwext4_vfs
PSEUDOMODULES += lwext4_vfs_format
## @defgroup pseudomodule_libc_gettimeofday libc_gettimeofday
## @brief Includes implementation of gettimeofday()
##
PSEUDOMODULES += libc_gettimeofday

## @defgroup pseudomodule_mpu_stack_guard mpu_stack_guard
## @brief MPU based stack guard
##
## When this module is active (which it is by default on supported MCUs),
## the Memory Protection Unit will be configured to detect stack overflows.
PSEUDOMODULES += mpu_stack_guard

## @defgroup pseudomodule_mpu_noexec_ram mpu_noexec_ram
## @{
## @brief Mark RAM as non-executable using the MPU
##
## Mark the RAM non executable.
## This is a protection mechanism which makes exploitation of buffer overflows significantly harder.
PSEUDOMODULES += mpu_noexec_ram
## @}

## @defgroup pseudomodule_pmp_noexec_ram pmp_noexec_ram
## @{
## @brief Mark RAM as non-executable using the PMP
##
## Mark the RAM non executable.
## This is a protection mechanism which makes exploitation of buffer overflows significantly harder.
PSEUDOMODULES += pmp_noexec_ram
## @}

PSEUDOMODULES += mtd_write_page
PSEUDOMODULES += nanocoap_%
PSEUDOMODULES += nanocoap_fileserver_callback
PSEUDOMODULES += nanocoap_fileserver_delete
PSEUDOMODULES += nanocoap_fileserver_put
PSEUDOMODULES += netdev_default
PSEUDOMODULES += netdev_ieee802154_%
PSEUDOMODULES += netdev_ieee802154_rx_timestamp
PSEUDOMODULES += netdev_ieee802154
PSEUDOMODULES += netdev_eth
PSEUDOMODULES += netdev_layer
PSEUDOMODULES += netdev_legacy_api
PSEUDOMODULES += netdev_new_api
PSEUDOMODULES += netdev_register
PSEUDOMODULES += netstats
PSEUDOMODULES += netstats_l2
PSEUDOMODULES += netstats_neighbor_etx
PSEUDOMODULES += netstats_neighbor_count
PSEUDOMODULES += netstats_neighbor_rssi
PSEUDOMODULES += netstats_neighbor_lqi
PSEUDOMODULES += netstats_neighbor_tx_time
PSEUDOMODULES += netstats_ipv6
PSEUDOMODULES += netstats_rpl
PSEUDOMODULES += nimble
PSEUDOMODULES += nimble_adv_ext
PSEUDOMODULES += nimble_autoconn_%
PSEUDOMODULES += nimble_netif_ext
PSEUDOMODULES += nimble_phy_coded
PSEUDOMODULES += nimble_phy_2mbit
PSEUDOMODULES += nimble_rpble_ext
PSEUDOMODULES += nimble_statconn_ext
PSEUDOMODULES += nimble_autoadv_shell
PSEUDOMODULES += nrfx
PSEUDOMODULES += newlib
PSEUDOMODULES += newlib_gnu_source
PSEUDOMODULES += newlib_nano
PSEUDOMODULES += nrf24l01p_ng_diagnostics
PSEUDOMODULES += opendsme
PSEUDOMODULES += openthread
PSEUDOMODULES += picolibc
PSEUDOMODULES += picolibc_stdout_buffered
PSEUDOMODULES += pktqueue
PSEUDOMODULES += posix_headers
PSEUDOMODULES += printf_float
PSEUDOMODULES += printf_long_long
PSEUDOMODULES += prng
PSEUDOMODULES += prng_%
PSEUDOMODULES += psa_riot_cipher_aes_common
PSEUDOMODULES += psa_riot_cipher_aes_128_ecb
PSEUDOMODULES += psa_riot_cipher_aes_128_cbc
PSEUDOMODULES += psa_riot_cipher_aes_192_cbc
PSEUDOMODULES += psa_riot_cipher_aes_256_cbc
PSEUDOMODULES += psa_riot_cipher_chacha20
PSEUDOMODULES += psa_riot_hashes_md5
PSEUDOMODULES += psa_riot_hashes_sha_1
PSEUDOMODULES += psa_riot_hashes_sha_224
PSEUDOMODULES += psa_riot_hashes_sha_256
PSEUDOMODULES += psa_riot_hashes_sha_384
PSEUDOMODULES += psa_riot_hashes_sha_512
PSEUDOMODULES += psa_riot_hashes_sha3_256
PSEUDOMODULES += psa_riot_hashes_sha3_384
PSEUDOMODULES += psa_riot_hashes_sha3_512
PSEUDOMODULES += psa_riot_hashes_sha_512_224
PSEUDOMODULES += psa_riot_hashes_sha_512_256
PSEUDOMODULES += psa_riot_hashes_hmac_sha256
PSEUDOMODULES += fortuna_reseed
PSEUDOMODULES += riotboot_%
PSEUDOMODULES += rtt_cmd
PSEUDOMODULES += saul_adc
PSEUDOMODULES += saul_bat_voltage
PSEUDOMODULES += saul_default
PSEUDOMODULES += saul_gpio
PSEUDOMODULES += saul_nrf_temperature
PSEUDOMODULES += saul_nrf_vddh
PSEUDOMODULES += saul_pwm
PSEUDOMODULES += scanf_float
PSEUDOMODULES += sched_cb
PSEUDOMODULES += sched_runq_callback
## @defgroup pseudomodule_sema_deprecated sema_deprecated
## @ingroup sys_sema
## @{
## @deprecated  Will be removed after 2021.07 release.
PSEUDOMODULES += sema_deprecated
## @}
PSEUDOMODULES += semtech_loramac_rx
PSEUDOMODULES += senml_cbor
PSEUDOMODULES += senml_phydat
PSEUDOMODULES += senml_saul
## @defgroup drivers_servo_pwm PWM based servo driver
## @ingroup drivers_servo
## @{
PSEUDOMODULES += servo_pwm
## @}
## @defgroup drivers_servo_timer periph_timer_periodic based servo driver
## @ingroup drivers_servo
## @{
PSEUDOMODULES += servo_timer
## @}
## @defgroup drivers_servo_saul SAUL integration of the servo driver
## @ingroup drivers_servo
## @{
PSEUDOMODULES += servo_saul
## @}

PSEUDOMODULES += shell_builtin_cmd_help_json
PSEUDOMODULES += shell_cmd_app_metadata
PSEUDOMODULES += shell_cmd_at30tse75x
PSEUDOMODULES += shell_cmd_benchmark_udp
PSEUDOMODULES += shell_cmd_ccn-lite-utils
PSEUDOMODULES += shell_cmd_conn_can
PSEUDOMODULES += shell_cmd_cord_ep
PSEUDOMODULES += shell_cmd_coreclk
PSEUDOMODULES += shell_cmd_cryptoauthlib
PSEUDOMODULES += shell_cmd_dfplayer
PSEUDOMODULES += shell_cmd_fib
PSEUDOMODULES += shell_cmd_genfile
PSEUDOMODULES += shell_cmd_gnrc_icmpv6_echo
PSEUDOMODULES += shell_cmd_gnrc_ipv6_blacklist
PSEUDOMODULES += shell_cmd_gnrc_ipv6_frag_stats
PSEUDOMODULES += shell_cmd_gnrc_ipv6_nib
PSEUDOMODULES += shell_cmd_gnrc_ipv6_whitelist
PSEUDOMODULES += shell_cmd_gnrc_netif
PSEUDOMODULES += shell_cmd_gnrc_netif_lora
PSEUDOMODULES += shell_cmd_gnrc_netif_lorawan
PSEUDOMODULES += shell_cmd_gnrc_pktbuf
PSEUDOMODULES += shell_cmd_gnrc_rpl
PSEUDOMODULES += shell_cmd_gnrc_sixlowpan_ctx
PSEUDOMODULES += shell_cmd_gnrc_sixlowpan_frag_stats
PSEUDOMODULES += shell_cmd_gnrc_txtsnd
PSEUDOMODULES += shell_cmd_gnrc_udp
PSEUDOMODULES += shell_cmd_heap
PSEUDOMODULES += shell_cmd_i2c_scan
PSEUDOMODULES += shell_cmd_iw
PSEUDOMODULES += shell_cmd_lwip_netif
PSEUDOMODULES += shell_cmd_mci
PSEUDOMODULES += shell_cmd_md5sum
PSEUDOMODULES += shell_cmd_mtd
PSEUDOMODULES += shell_cmd_nanocoap_vfs
PSEUDOMODULES += shell_cmd_netstats_neighbor
PSEUDOMODULES += shell_cmd_nice
PSEUDOMODULES += shell_cmd_nimble_netif
PSEUDOMODULES += shell_cmd_nimble_statconn
PSEUDOMODULES += shell_cmd_opendsme
PSEUDOMODULES += shell_cmd_openthread
PSEUDOMODULES += shell_cmd_openwsn
PSEUDOMODULES += shell_cmd_pm
PSEUDOMODULES += shell_cmd_ps
PSEUDOMODULES += shell_cmd_random
PSEUDOMODULES += shell_cmd_rtc
PSEUDOMODULES += shell_cmd_rtt
PSEUDOMODULES += shell_cmd_saul_reg
PSEUDOMODULES += shell_cmd_semtech-loramac
PSEUDOMODULES += shell_cmd_sha1sum
PSEUDOMODULES += shell_cmd_sha256sum
PSEUDOMODULES += shell_cmd_sht1x
PSEUDOMODULES += shell_cmd_sntp
PSEUDOMODULES += shell_cmd_suit
PSEUDOMODULES += shell_cmd_sys
PSEUDOMODULES += shell_cmd_udptty
PSEUDOMODULES += shell_cmd_xipfs
PSEUDOMODULES += shell_cmd_vfs
PSEUDOMODULES += shell_cmds_default
PSEUDOMODULES += shell_hooks
PSEUDOMODULES += shell_lock_auto_locking
PSEUDOMODULES += shield_llcc68
PSEUDOMODULES += shield_sx1262
PSEUDOMODULES += shield_w5100
PSEUDOMODULES += sock
PSEUDOMODULES += sock_async
PSEUDOMODULES += sock_aux_local
PSEUDOMODULES += sock_aux_rssi
PSEUDOMODULES += sock_aux_timestamp
PSEUDOMODULES += sock_aux_ttl
PSEUDOMODULES += sock_dtls
PSEUDOMODULES += sock_dtls_verify_public_key
PSEUDOMODULES += sock_ip
PSEUDOMODULES += sock_tcp
PSEUDOMODULES += sock_udp
PSEUDOMODULES += socket_zep_hello
PSEUDOMODULES += soft_uart_modecfg
PSEUDOMODULES += stdin
PSEUDOMODULES += stdio_available
PSEUDOMODULES += stdio_cdc_acm
## @defgroup sys_stdio_default	Default STDIO provider
## @ingroup sys_stdio
## @{
## This module selects the default STDIO method of a given board.
## It will be enabled by default if no other stdio method is selected.
PSEUDOMODULES += stdio_default
## @}
PSEUDOMODULES += stdio_dispatch
PSEUDOMODULES += stdio_ethos
PSEUDOMODULES += stdio_nimble_debug
PSEUDOMODULES += stdio_telnet
## @defgroup sys_stdio_uart_onlcr   Support for DOS line endings in STDIO-UART
## @ingroup sys_stdio_uart
## @{
## Enable this (pseudo-) module to emit DOS style line endings (`\r\n`) instead
## of UNIX style line endings (`\n`) via STDIO over UART.
PSEUDOMODULES += stdio_uart_onlcr
## @}
PSEUDOMODULES += stdio_uart_rx
PSEUDOMODULES += stm32_eth
PSEUDOMODULES += stm32_eth_auto
PSEUDOMODULES += stm32_eth_link_up
PSEUDOMODULES += stm32_eth_tracing
PSEUDOMODULES += stm32mp1_eng_mode
PSEUDOMODULES += suit_transport_%
PSEUDOMODULES += suit_storage_%
PSEUDOMODULES += sys_bus_%
PSEUDOMODULES += tiny_strerror_as_strerror
PSEUDOMODULES += tiny_strerror_minimal

# An umbrella module for the unicoap_driver_rfc7252_common_pdu
# and unicoap_driver_rfc7252_common_messaging modules
PSEUDOMODULES += unicoap_driver_rfc7252_common
# Alias for unicoap_driver_rfc7252_common_pdu, and is hence a pseudomodule
PSEUDOMODULES += unicoap_driver_rfc7252_pdu

PSEUDOMODULES += usbus_urb
PSEUDOMODULES += vdd_lc_filter_%
## @defgroup pseudomodule_vfs_auto_format vfs_auto_format
## @brief Format mount points at startup unless they can be mounted
##
## When this module is active, mount points configured through the @ref
## pseudomodule_vfs_auto_mount module that can not be mounted at startup are
## formatted and, if that operation is successful, attempted to mount again.
##
## Beware that this may be a harmful procedure in case a bug that corrupts a
## filesystem coincides with a bug that sends the device into a reboot loop.
PSEUDOMODULES += vfs_auto_format

## @defgroup pseudomodule_vfs_auto_mount vfs_auto_mount
## @brief Mount file systems at startup
##
## When this module is active, mount points specified through
## @ref VFS_AUTO_MOUNT are mounted at their designated mount points at startup.
## These mount points can be specified by the application, or are provided by
## some boards if the @ref pseudomodule_vfs_default module is active.
PSEUDOMODULES += vfs_auto_mount

## @defgroup pseudomodule_vfs_default vfs_default
## @brief Enable default assignments of a board's devices to VFS mount points
##
## When this module is active, boards with additional flash storage will
## automatically mount (and possibly format, if @ref
## pseudomodule_vfs_auto_format is enabled) their flash devices with a file
## system that is common for that board (or at least common for this board
## within RIOT).
##
## Boards will generally mount to `/nvm` unless they have several storage
## backends.
PSEUDOMODULES += vfs_default

PSEUDOMODULES += wakaama_objects_%
PSEUDOMODULES += wifi_scan_list
PSEUDOMODULES += wifi_enterprise
PSEUDOMODULES += xtimer_on_ztimer
PSEUDOMODULES += xtimer_no_ztimer_default
PSEUDOMODULES += zptr
PSEUDOMODULES += ztimer
PSEUDOMODULES += ztimer_%
PSEUDOMODULES += ztimer64_%

## @defgroup pseudomodule_ztimer_auto_adjust ztimer_auto_adjust
## @brief A module to set on init ztimer->adjust_sleep/adjust_set values
##
## When this module is active, then on init if no CONFIG_ZTIMER_USEC_ADJUST_%
## values are set for the BOARD correction values adjust_sleep and adjust_set
## will be calculated in set for the required clocks.
##
## Note that some BOARDs clocks require a startup time to get accuarate values,
## a configurable @ref CONFIG_ZTIMER_AUTO_ADJUST_SETTLE value can be set for this.
##
## Alternatively CONFIG_ZTIMER_USEC_ADJUST_% values can be set in the BOARDs
## configuration header board.h. These can be found out by running tests/sys/ztimer_overhead
PSEUDOMODULES += ztimer_auto_adjust

# core_lib is not a submodule
NO_PSEUDOMODULES += core_lib

# ztimer's main module is called "ztimer_core"
NO_PSEUDOMODULES += ztimer_core
NO_PSEUDOMODULES += netdev_ieee802154_submac

# print ascii representation in function od_hex_dump()
PSEUDOMODULES += od_string

# add all pseudo random number generator variants as pseudomodules
PSEUDOMODULES += prng_%

# STM32 periph pseudomodules
PSEUDOMODULES += stm32_periph_%

# declare periph submodules as pseudomodules, but exclude periph_common
PSEUDOMODULES += periph_%
NO_PSEUDOMODULES += periph_common

# Modules to automatically start PIO programs
PSEUDOMODULES += pio_autostart_%

# Submodules provided by Skald
PSEUDOMODULES += skald_ibeacon
PSEUDOMODULES += skald_eddystone

PSEUDOMODULES += crypto_aes_128
PSEUDOMODULES += crypto_aes_192
PSEUDOMODULES += crypto_aes_256
# By using this pseudomodule, T tables will be precalculated.
PSEUDOMODULES += crypto_aes_precalculated
# This pseudomodule causes a loop in AES to be unrolled (more flash, less CPU)
PSEUDOMODULES += crypto_aes_unroll

# declare shell version of test_utils_interactive_sync
PSEUDOMODULES += test_utils_interactive_sync_shell
PSEUDOMODULES += test_utils_main_exit_cb

# All auto_init modules are pseudomodules
PSEUDOMODULES += auto_init_%
NO_PSEUDOMODULES += auto_init_can
NO_PSEUDOMODULES += auto_init_loramac
NO_PSEUDOMODULES += auto_init_multimedia
NO_PSEUDOMODULES += auto_init_security
NO_PSEUDOMODULES += auto_init_usbus
NO_PSEUDOMODULES += auto_init_screen
NO_PSEUDOMODULES += auto_init_wdt_event
NO_PSEUDOMODULES += auto_init_wdt_thread

# Packages and drivers may also add modules to PSEUDOMODULES in their `Makefile.include`.

## @}
