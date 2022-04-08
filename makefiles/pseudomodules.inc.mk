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

PSEUDOMODULES += can_mbox
PSEUDOMODULES += can_pm
PSEUDOMODULES += can_raw
PSEUDOMODULES += ccn-lite-utils
PSEUDOMODULES += cc2538_rf_obs_sig
PSEUDOMODULES += conn_can_isotp_multi
PSEUDOMODULES += cord_ep_standalone
PSEUDOMODULES += core_%
PSEUDOMODULES += cortexm_fpu
PSEUDOMODULES += cortexm_svc
PSEUDOMODULES += cpp
PSEUDOMODULES += cpu_check_address
PSEUDOMODULES += credman_load
PSEUDOMODULES += dbgpin
PSEUDOMODULES += devfs_%
PSEUDOMODULES += dhcpv6_%
PSEUDOMODULES += dhcpv6_client_dns
PSEUDOMODULES += dhcpv6_client_ia_pd
PSEUDOMODULES += dhcpv6_client_ia_na
PSEUDOMODULES += dhcpv6_client_mud_url
PSEUDOMODULES += dhcpv6_relay
PSEUDOMODULES += dns_msg
PSEUDOMODULES += ecc_%
PSEUDOMODULES += ethos_stdio
PSEUDOMODULES += event_%
PSEUDOMODULES += event_timeout
PSEUDOMODULES += event_timeout_ztimer
PSEUDOMODULES += evtimer_mbox
PSEUDOMODULES += evtimer_on_ztimer
PSEUDOMODULES += fatfs_vfs_format
PSEUDOMODULES += fmt_%
PSEUDOMODULES += gcoap_forward_proxy
PSEUDOMODULES += gcoap_dtls
PSEUDOMODULES += fido2_tests
PSEUDOMODULES += gnrc_dhcpv6_%
PSEUDOMODULES += gnrc_ipv6_auto_subnets_auto_init
PSEUDOMODULES += gnrc_ipv6_auto_subnets_simple
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
PSEUDOMODULES += gnrc_netdev_default
PSEUDOMODULES += gnrc_neterr
PSEUDOMODULES += gnrc_netapi_callbacks
PSEUDOMODULES += gnrc_netapi_mbox
PSEUDOMODULES += gnrc_netif_bus
PSEUDOMODULES += gnrc_netif_events
PSEUDOMODULES += gnrc_netif_timestamp
PSEUDOMODULES += gnrc_pktbuf_cmd
PSEUDOMODULES += gnrc_netif_6lo
PSEUDOMODULES += gnrc_netif_ipv6
PSEUDOMODULES += gnrc_netif_mac
PSEUDOMODULES += gnrc_netif_single
PSEUDOMODULES += gnrc_netif_cmd_%
PSEUDOMODULES += gnrc_netif_dedup
PSEUDOMODULES += gnrc_nettype_%
PSEUDOMODULES += gnrc_sixloenc
PSEUDOMODULES += gnrc_sixlowpan_border_router_default
PSEUDOMODULES += gnrc_sixlowpan_default
PSEUDOMODULES += gnrc_sixlowpan_frag_hint
PSEUDOMODULES += gnrc_sixlowpan_frag_sfr_stats
PSEUDOMODULES += gnrc_sixlowpan_iphc_nhc
PSEUDOMODULES += gnrc_sixlowpan_nd_border_router
PSEUDOMODULES += gnrc_sixlowpan_router_default
PSEUDOMODULES += gnrc_udp_cmd
PSEUDOMODULES += gnrc_sock_async
PSEUDOMODULES += gnrc_sock_check_reuse
PSEUDOMODULES += gnrc_txtsnd
PSEUDOMODULES += heap_cmd
PSEUDOMODULES += i2c_scan
PSEUDOMODULES += ieee802154_security
PSEUDOMODULES += ieee802154_submac
PSEUDOMODULES += ipv4
PSEUDOMODULES += ipv6
PSEUDOMODULES += l2filter_blacklist
PSEUDOMODULES += l2filter_whitelist
PSEUDOMODULES += libstdcpp
PSEUDOMODULES += log
PSEUDOMODULES += log_printfnoformat
PSEUDOMODULES += log_color
PSEUDOMODULES += lora
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
## @brief Mark RAM as non-executable using the MPU
##
## Mark the RAM non executable.
## This is a protection mechanism which makes exploitation of buffer overflows significantly harder.
PSEUDOMODULES += mpu_noexec_ram

PSEUDOMODULES += mtd_write_page
PSEUDOMODULES += nanocoap_%
PSEUDOMODULES += netdev_default
PSEUDOMODULES += netdev_ieee802154_%
PSEUDOMODULES += netdev_ieee802154
PSEUDOMODULES += netdev_eth
PSEUDOMODULES += netdev_layer
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
PSEUDOMODULES += newlib
PSEUDOMODULES += newlib_gnu_source
PSEUDOMODULES += newlib_nano
PSEUDOMODULES += nice
PSEUDOMODULES += nrf24l01p_ng_diagnostics
PSEUDOMODULES += openthread
PSEUDOMODULES += picolibc
PSEUDOMODULES += picolibc_stdout_buffered
PSEUDOMODULES += pktqueue
PSEUDOMODULES += posix_headers
PSEUDOMODULES += printf_float
PSEUDOMODULES += prng
PSEUDOMODULES += prng_%
PSEUDOMODULES += fortuna_reseed
PSEUDOMODULES += random_cmd
PSEUDOMODULES += riotboot_%
PSEUDOMODULES += rtt_cmd
PSEUDOMODULES += saul_adc
PSEUDOMODULES += saul_default
PSEUDOMODULES += saul_gpio
PSEUDOMODULES += saul_nrf_temperature
PSEUDOMODULES += saul_nrf_vddh
PSEUDOMODULES += saul_pwm
PSEUDOMODULES += scanf_float
PSEUDOMODULES += sched_cb
PSEUDOMODULES += sched_runq_callback
PSEUDOMODULES += sema_deprecated
PSEUDOMODULES += semtech_loramac_rx
PSEUDOMODULES += senml_cbor
PSEUDOMODULES += senml_phydat
PSEUDOMODULES += senml_saul
PSEUDOMODULES += shell_hooks
PSEUDOMODULES += slipdev_stdio
PSEUDOMODULES += slipdev_l2addr
PSEUDOMODULES += sock
PSEUDOMODULES += sock_async
PSEUDOMODULES += sock_aux_local
PSEUDOMODULES += sock_aux_rssi
PSEUDOMODULES += sock_aux_timestamp
PSEUDOMODULES += sock_dtls
PSEUDOMODULES += sock_ip
PSEUDOMODULES += sock_tcp
PSEUDOMODULES += sock_udp
PSEUDOMODULES += socket_zep_hello
PSEUDOMODULES += soft_uart_modecfg
PSEUDOMODULES += stdin
PSEUDOMODULES += stdio_available
PSEUDOMODULES += stdio_cdc_acm
PSEUDOMODULES += stdio_ethos
PSEUDOMODULES += stdio_nimble_debug
PSEUDOMODULES += stdio_uart_rx
PSEUDOMODULES += stdio_telnet
PSEUDOMODULES += stm32_eth
PSEUDOMODULES += stm32_eth_auto
PSEUDOMODULES += stm32_eth_link_up
PSEUDOMODULES += stm32mp1_eng_mode
PSEUDOMODULES += suit_transport_%
PSEUDOMODULES += suit_storage_%
PSEUDOMODULES += sys_bus_%
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
## configuration header board.h. These can be found out by running tests/ztimer_overhead
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

# All auto_init modules are pseudomodules
PSEUDOMODULES += auto_init_%
NO_PSEUDOMODULES += auto_init_can
NO_PSEUDOMODULES += auto_init_csp
NO_PSEUDOMODULES += auto_init_loramac
NO_PSEUDOMODULES += auto_init_multimedia
NO_PSEUDOMODULES += auto_init_security
NO_PSEUDOMODULES += auto_init_usbus
NO_PSEUDOMODULES += auto_init_screen

# Packages and drivers may also add modules to PSEUDOMODULES in their `Makefile.include`.

## @}
