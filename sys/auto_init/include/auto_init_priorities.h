/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdebug
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_auto_init
 *
 * @{
 * @file
 * @brief       Priority values of auto-initialized modules.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AUTO_INIT_PRIO_MOD_ZTIMER
/**
 * @brief   ztimer priority
 */
#define AUTO_INIT_PRIO_MOD_ZTIMER                       1010
#endif
#ifndef AUTO_INIT_PRIO_MOD_ZTIMER64
/**
 * @brief   ztimer64 priority
 */
#define AUTO_INIT_PRIO_MOD_ZTIMER64                     1020
#endif
#ifndef AUTO_INIT_PRIO_MOD_XTIMER
/**
 * @brief   xtimer priority
 */
#define AUTO_INIT_PRIO_MOD_XTIMER                       1030
#endif
#ifndef AUTO_INIT_PRIO_WDT_THREAD
/**
 * @brief   WDT priority
 */
#define AUTO_INIT_PRIO_WDT_THREAD                       1035
#endif
#ifndef AUTO_INIT_PRIO_MOD_RANDOM
/**
 * @brief   RNG priority
 */
#define AUTO_INIT_PRIO_MOD_RANDOM                       1040
#endif
#ifndef AUTO_INIT_PRIO_MOD_SCHEDSTATISTICS
/**
 * @brief   scheduling statistics priority
 */
#define AUTO_INIT_PRIO_MOD_SCHEDSTATISTICS              1050
#endif
#ifndef AUTO_INIT_PRIO_MOD_SCHED_ROUND_ROBIN
/**
 * @brief   round robin scheduling priority
 */
#define AUTO_INIT_PRIO_MOD_SCHED_ROUND_ROBIN            1060
#endif
#ifndef AUTO_INIT_PRIO_MOD_DUMMY_THREAD
/**
 * @brief   dummy thread priority
 */
#define AUTO_INIT_PRIO_MOD_DUMMY_THREAD                 1070
#endif
#ifndef AUTO_INIT_PRIO_MOD_EVENT_THREAD
/**
 * @brief   event thread priority
 */
#define AUTO_INIT_PRIO_MOD_EVENT_THREAD                 1080
#endif
#ifndef AUTO_INIT_PRIO_WDT_EVENT
/**
 * @brief   WDT event priority
 */
#define AUTO_INIT_PRIO_WDT_EVENT                        1085
#endif
#ifndef AUTO_INIT_PRIO_MOD_SYS_BUS
/**
 * @brief   sys bus priority
 */
#define AUTO_INIT_PRIO_MOD_SYS_BUS                      1090
#endif
#ifndef AUTO_INIT_PRIO_MOD_MCI
/**
 * @brief   MCI priority
 */
#define AUTO_INIT_PRIO_MOD_MCI                          1100
#endif
#ifndef AUTO_INIT_PRIO_MOD_PROFILING
/**
 * @brief   profiling priority
 */
#define AUTO_INIT_PRIO_MOD_PROFILING                    1110
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_PKTBUF
/**
 * @brief   GNRC pktbuf priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_PKTBUF                  1120
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_PKTDUMP
/**
 * @brief   GNRC pktdump priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_PKTDUMP                 1130
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_SIXLOWPAN
/**
 * @brief   GNRC sixlowpan priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_SIXLOWPAN               1140
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_IPV6
/**
 * @brief   GNRC IPv6 priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6                    1150
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_UDP
/**
 * @brief   GNRC UDP priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_UDP                     1160
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_TCP
/**
 * @brief   GNRC TCP priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_TCP                     1170
#endif
#ifndef AUTO_INIT_PRIO_MOD_LWIP
/**
 * @brief   lwIP priority
 */
#define AUTO_INIT_PRIO_MOD_LWIP                         1180
#endif
#ifndef AUTO_INIT_PRIO_MOD_SOCK_DTLS
/**
 * @brief   DTLS socket priority
 */
#define AUTO_INIT_PRIO_MOD_SOCK_DTLS                    1190
#endif
#ifndef AUTO_INIT_PRIO_MOD_OPENTHREAD
/**
 * @brief   OpenThread priority
 */
#define AUTO_INIT_PRIO_MOD_OPENTHREAD                   1200
#endif
#ifndef AUTO_INIT_PRIO_MOD_OPENWSN
/**
 * @brief   OpenWSN priority
 */
#define AUTO_INIT_PRIO_MOD_OPENWSN                      1210
#endif
#ifndef AUTO_INIT_PRIO_MOD_MYNEWT_CORE
/**
 * @brief   Mynewt priority
 */
#define AUTO_INIT_PRIO_MOD_MYNEWT_CORE                  1220
#endif
#ifndef AUTO_INIT_PRIO_MOD_UWB_CORE
/**
 * @brief   UWB priority
 */
#define AUTO_INIT_PRIO_MOD_UWB_CORE                     1230
#endif
#ifndef AUTO_INIT_PRIO_MOD_GCOAP
/**
 * @brief   nanoCoAP server priority
 */
#define AUTO_INIT_PRIO_MOD_NANOCOAP                     1235
#endif
#ifndef AUTO_INIT_PRIO_MOD_GCOAP
/**
 * @brief   GCoAP priority
 */
#define AUTO_INIT_PRIO_MOD_GCOAP                        1240
#endif
#ifndef AUTO_INIT_PRIO_MOD_DEVFS
/**
 * @brief   DEVFS priority
 */
#define AUTO_INIT_PRIO_MOD_DEVFS                        1250
#endif
#ifndef AUTO_INIT_PRIO_MOD_VFS
/**
 * @brief   VFS priority
 */
#define AUTO_INIT_PRIO_MOD_VFS                          1260
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_IPV6_NIB
/**
 * @brief   GNRC IPv6 NIB priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6_NIB                1270
#endif
#ifndef AUTO_INIT_PRIO_MOD_SKALD
/**
 * @brief   Skald priority
 */
#define AUTO_INIT_PRIO_MOD_SKALD                        1280
#endif
#ifndef AUTO_INIT_PRIO_MOD_CORD_COMMON
/**
 * @brief   CoRE Resource Directory priority
 */
#define AUTO_INIT_PRIO_MOD_CORD_COMMON                  1290
#endif
#ifndef AUTO_INIT_PRIO_MOD_CORD_EP_STANDALONE
/**
 * @brief   CoRE Resource Directory endpoint priority
 */
#define AUTO_INIT_PRIO_MOD_CORD_EP_STANDALONE           1300
#endif
#ifndef AUTO_INIT_PRIO_MOD_ASYMCUTE
/**
 * @brief   asynchronous MQTT-SN priority
 */
#define AUTO_INIT_PRIO_MOD_ASYMCUTE                     1310
#endif
#ifndef AUTO_INIT_PRIO_MOD_ESP_BLE_NIMBLE
/**
 * @brief   ESP BLE NimBLE priority
 */
#define AUTO_INIT_PRIO_MOD_ESP_BLE_NIMBLE               1319
#endif
#ifndef AUTO_INIT_PRIO_MOD_NIMBLE
/**
 * @brief   NimBLE priority
 */
#define AUTO_INIT_PRIO_MOD_NIMBLE                       1320
#endif
#ifndef AUTO_INIT_PRIO_MOD_LORAMAC
/**
 * @brief   LoRaMAC priority
 */
#define AUTO_INIT_PRIO_MOD_LORAMAC                      1330
#endif
#ifndef AUTO_INIT_PRIO_MOD_DSM
/**
 * @brief   DSM priority
 */
#define AUTO_INIT_PRIO_MOD_DSM                          1340
#endif
#ifndef AUTO_INIT_PRIO_MOD_USBUS
/**
 * @brief   USB priority
 */
#define AUTO_INIT_PRIO_MOD_USBUS                        1350
#endif
#ifndef AUTO_INIT_PRIO_MOD_TINYUSB
/**
 * @brief   tinyUSB priority
 */
#define AUTO_INIT_PRIO_MOD_TINYUSB                      1350
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_NETIF
/**
 * @brief   GNRC netif priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_NETIF                   1360
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_UHCPC
/**
 * @brief   UHCPC priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_UHCPC                   1370
#endif
#ifndef AUTO_INIT_PRIO_MOD_NDN
/**
 * @brief   NDN priority
 */
#define AUTO_INIT_PRIO_MOD_NDN                          1380
#endif
#ifndef AUTO_INIT_PRIO_MOD_SOFT_I2C
/**
 * @brief   soft_i2c priority
 */
#  define AUTO_INIT_PRIO_MOD_SOFT_I2C                   1395
#endif
#ifndef AUTO_INIT_PRIO_MOD_SHT1X
/**
 * @brief   sht1x priority
 */
#define AUTO_INIT_PRIO_MOD_SHT1X                        1390
#endif
#ifndef AUTO_INIT_PRIO_MOD_SAUL
/**
 * @brief   SAUL priority
 */
#define AUTO_INIT_PRIO_MOD_SAUL                         1400
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_RPL
/**
 * @brief   GNRC RPL priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_RPL                     1410
#endif
#ifndef AUTO_INIT_PRIO_MOD_CAN
/**
 * @brief   CAN priority
 */
#define AUTO_INIT_PRIO_MOD_CAN                          1420
#endif
#ifndef AUTO_INIT_PRIO_MOD_SUIT
/**
 * @brief   SUIT priority
 */
#define AUTO_INIT_PRIO_MOD_SUIT                         1430
#endif
#ifndef AUTO_INIT_PRIO_MOD_MBEDTLS
/**
 * @brief   mbed TLS priority
 */
#define AUTO_INIT_PRIO_MOD_MBEDTLS                      1440
#endif
#ifndef AUTO_INIT_PRIO_MOD_SECURITY
/**
 * @brief   CryptoAuthLib priority
 */
#define AUTO_INIT_PRIO_MOD_SECURITY                1450
#endif
#ifndef AUTO_INIT_PRIO_MOD_TEST_UTILS_INTERACTIVE_SYNC
/**
 * @brief   test utils interactive sync priority
 */
#define AUTO_INIT_PRIO_MOD_TEST_UTILS_INTERACTIVE_SYNC  1460
#endif
#ifndef AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT
/**
 * @brief   DHCPv6 client priority
 */
#define AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT                1470
#endif
#ifndef AUTO_INIT_PRIO_MOD_DHCPV6_RELAY
/**
 * @brief   DHCPv6 relay priority
 */
#define AUTO_INIT_PRIO_MOD_DHCPV6_RELAY                 1480
#endif
#ifndef AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT_SIMPLE_PD
/**
 * @brief   DHCPv6 client simple PD priority
 */
#define AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT_SIMPLE_PD      1490
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_IPV6_AUTO_SUBNETS
/**
 * @brief   IPv6 auto subnets priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6_AUTO_SUBNETS       1500
#endif
#ifndef AUTO_INIT_PRIO_MOD_TELNET
/**
 * @brief   telnet server priority
 */
#define AUTO_INIT_PRIO_MOD_TELNET                       1510
#endif
#ifndef AUTO_INIT_PRIO_MOD_DFPLAYER
/**
 * @brief   DFPlayer priority
 */
#define AUTO_INIT_PRIO_MOD_DFPLAYER                     1520
#endif
#ifndef AUTO_INIT_PRIO_MOD_SCREEN
/**
 * @brief   screen priority
 */
#define AUTO_INIT_PRIO_MOD_SCREEN                       1530
#endif
#ifndef AUTO_INIT_PRIO_MOD_BENCHMARK_UDP
/**
 * @brief   UDP benchmark priority
 */
#define AUTO_INIT_PRIO_MOD_BENCHMARK_UDP                1540
#endif
#ifndef AUTO_INIT_PRIO_MOD_DOCK_DNS
/**
 * @brief   DNS socket priority
 */
#define AUTO_INIT_PRIO_MOD_DOCK_DNS                     1550
#endif
#ifndef AUTO_INIT_PRIO_MOD_GNRC_IPV6_STATIC_ADDR
/**
 * @brief   Static network configuration priority
 */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6_STATIC_ADDR        1560
#endif
#ifndef AUTO_INIT_PRIO_MOD_DRIVER_CRYPTOCELL_310
/**
 * @brief CryptoCell Driver Priority
 */
#define AUTO_INIT_PRIO_MOD_DRIVER_CRYPTOCELL_310            1570
#endif

#ifdef __cplusplus
}
#endif

/** @} */
