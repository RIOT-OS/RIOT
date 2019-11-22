PSEUDOMODULES += at_urc
PSEUDOMODULES += auto_init_gnrc_rpl
PSEUDOMODULES += can_mbox
PSEUDOMODULES += can_pm
PSEUDOMODULES += can_raw
PSEUDOMODULES += ccn-lite-utils
PSEUDOMODULES += conn_can_isotp_multi
PSEUDOMODULES += cord_ep_standalone
PSEUDOMODULES += core_%
PSEUDOMODULES += cortexm_fpu
PSEUDOMODULES += cpu_check_address
PSEUDOMODULES += devfs_%
PSEUDOMODULES += ecc_%
PSEUDOMODULES += emb6_router
PSEUDOMODULES += event_%
PSEUDOMODULES += fmt_%
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
PSEUDOMODULES += gnrc_netif_cmd_%
PSEUDOMODULES += gnrc_netif_dedup
PSEUDOMODULES += gnrc_sixloenc
PSEUDOMODULES += gnrc_sixlowpan_border_router_default
PSEUDOMODULES += gnrc_sixlowpan_default
PSEUDOMODULES += gnrc_sixlowpan_frag_hint
PSEUDOMODULES += gnrc_sixlowpan_frag_stats
PSEUDOMODULES += gnrc_sixlowpan_iphc_nhc
PSEUDOMODULES += gnrc_sixlowpan_nd_border_router
PSEUDOMODULES += gnrc_sixlowpan_router
PSEUDOMODULES += gnrc_sixlowpan_router_default
PSEUDOMODULES += gnrc_sock_check_reuse
PSEUDOMODULES += gnrc_txtsnd
PSEUDOMODULES += heap_cmd
PSEUDOMODULES += i2c_scan
PSEUDOMODULES += ina3221_alerts
PSEUDOMODULES += l2filter_blacklist
PSEUDOMODULES += l2filter_whitelist
PSEUDOMODULES += lis2dh12_i2c
PSEUDOMODULES += lis2dh12_spi
PSEUDOMODULES += log
PSEUDOMODULES += log_printfnoformat
PSEUDOMODULES += log_color
PSEUDOMODULES += lora
PSEUDOMODULES += mpu_stack_guard
PSEUDOMODULES += nanocoap_%
PSEUDOMODULES += netdev_default
PSEUDOMODULES += netstats
PSEUDOMODULES += netstats_l2
PSEUDOMODULES += netstats_ipv6
PSEUDOMODULES += netstats_rpl
PSEUDOMODULES += nimble
PSEUDOMODULES += nimble_autoconn_%
PSEUDOMODULES += newlib
PSEUDOMODULES += newlib_gnu_source
PSEUDOMODULES += newlib_nano
PSEUDOMODULES += openthread
PSEUDOMODULES += pktqueue
PSEUDOMODULES += posix_headers
PSEUDOMODULES += printf_float
PSEUDOMODULES += prng
PSEUDOMODULES += prng_%
PSEUDOMODULES += qmc5883l_int
PSEUDOMODULES += riotboot_%
PSEUDOMODULES += saul_adc
PSEUDOMODULES += saul_default
PSEUDOMODULES += saul_gpio
PSEUDOMODULES += saul_nrf_temperature
PSEUDOMODULES += scanf_float
PSEUDOMODULES += sched_cb
PSEUDOMODULES += semtech_loramac_rx
PSEUDOMODULES += sock
PSEUDOMODULES += sock_async
PSEUDOMODULES += sock_ip
PSEUDOMODULES += sock_tcp
PSEUDOMODULES += sock_udp
PSEUDOMODULES += stdin
PSEUDOMODULES += stdio_ethos
PSEUDOMODULES += stdio_cdc_acm
PSEUDOMODULES += stdio_uart_rx
PSEUDOMODULES += suit_%

# handle suit_v4 being a distinct module
NO_PSEUDOMODULES += suit_v4

# print ascii representation in function od_hex_dump()
PSEUDOMODULES += od_string

# include variants of the AT86RF2xx drivers as pseudo modules
PSEUDOMODULES += at86rf23%
PSEUDOMODULES += at86rf21%
PSEUDOMODULES += at86rfa1
PSEUDOMODULES += at86rfr2

# include variants of the BMX280 drivers as pseudo modules
PSEUDOMODULES += bmp280_i2c
PSEUDOMODULES += bmp280_spi
PSEUDOMODULES += bme280_i2c
PSEUDOMODULES += bme280_spi

# variants of TI ADCXX1C
PSEUDOMODULES += adc081c
PSEUDOMODULES += adc101c
PSEUDOMODULES += adc121c

# full featured version of CCS811 driver as pseudo module
PSEUDOMODULES += ccs811_full

# include variants of CC110X drivers as pseudo modules
PSEUDOMODULES += cc1100
PSEUDOMODULES += cc1100e
PSEUDOMODULES += cc1101

# include variants of MPU9X50 drivers as pseudo modules
PSEUDOMODULES += mpu9150
PSEUDOMODULES += mpu9250

# include variants of INA2xx drivers as pseudo modules
PSEUDOMODULES += ina219
PSEUDOMODULES += ina220

# include variants of mrf24j40 drivers as pseudo modules
PSEUDOMODULES += mrf24j40m%

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

#include variants of tmp00x drivers as pseudo modules
PSEUDOMODULES += tmp006
PSEUDOMODULES += tmp007

# include variants of RN2XX3 drivers as pseudo modules
PSEUDOMODULES += rn2483
PSEUDOMODULES += rn2903

# include variants of VCNL40x0 drivers as pseudo modules
PSEUDOMODULES += vcnl4010
PSEUDOMODULES += vcnl4020
PSEUDOMODULES += vcnl4040

# implementations of ws281x_write as submodules of ws281x:
PSEUDOMODULES += ws281x_%

# include variants of lpsxxx drivers as pseudo modules
PSEUDOMODULES += lps331ap
PSEUDOMODULES += lps22hb
PSEUDOMODULES += lps25hb

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

# define optimized read function of DS18 driver as a pseudo module
PSEUDOMODULES += ds18_optimized

# By using this pseudomodule, T tables will be precalculated.
PSEUDOMODULES += crypto_aes_precalculated
# This pseudomodule causes a loop in AES to be unrolled (more flash, less CPU)
PSEUDOMODULES += crypto_aes_unroll

# Packages may also add modules to PSEUDOMODULES in their `Makefile.include`.
