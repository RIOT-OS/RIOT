/**
 * Auto initialization for used modules
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup auto_init
 * @{
 * @file
 * @brief   initializes any used module that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */
#include <stdint.h>
#include <stdio.h>

#include "auto_init.h"

#ifdef MODULE_CONFIG
#include "config.h"
#endif

#ifdef MODULE_SHT11
#include "sht11.h"
#endif

#ifdef MODULE_GPIOINT
#include "gpioint.h"
#endif

#ifdef MODULE_CC110X_LEGACY_CSMA
#include "cc110x_legacy_csma.h"
#endif

#ifdef MODULE_LTC4150
#include "ltc4150.h"
#endif

#ifdef MODULE_UART0
#include "board_uart0.h"
#endif

#ifdef MODULE_MCI
#include "diskio.h"
#endif

#ifdef MODULE_VTIMER
#include "vtimer.h"
#endif

#ifdef MODULE_RTC
#include "periph/rtc.h"
#endif

#ifdef MODULE_SIXLOWPAN
#include "sixlowpan.h"
#endif

#ifdef MODULE_UDP
#include "udp.h"
#endif

#ifdef MODULE_TCP
#include "tcp.h"
#endif

#ifdef MODULE_NOMAC
#include "nomac.h"
#endif

#ifdef MODULE_NET_IF
#include "cpu-conf.h"
#include "cpu.h"
#include "kernel.h"
#include "net_if.h"
#include "transceiver.h"
#include "net_help.h"
#include "hashes.h"
#include "periph/cpuid.h"
#endif

#ifdef MODULE_NG_SIXLOWPAN
#include "net/ng_sixlowpan.h"
#endif

#ifdef MODULE_NG_IPV6
#include "net/ng_ipv6.h"
#endif

#ifdef MODULE_NG_IPV6_NETIF
#include "net/ng_ipv6/netif.h"
#endif

#ifdef MODULE_L2_PING
#include "l2_ping.h"
#endif

#ifdef MODULE_NG_PKTDUMP
#include "net/ng_pktdump.h"
#endif

#ifdef MODULE_NG_UDP
#include "net/ng_udp.h"
#endif

#ifdef MODULE_DEV_ETH_AUTOINIT
#include "net/dev_eth.h"
#include "dev_eth_autoinit.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef CONF_RADIO_ADDR
#define CONF_RADIO_ADDR (1)
#endif

#ifndef CONF_PAN_ID
#define CONF_PAN_ID     (0xabcd)
#endif

#ifdef MODULE_NET_IF
void auto_init_net_if(void)
{
    transceiver_type_t transceivers = 0;
#ifdef MODULE_AT86RF231
    transceivers |= TRANSCEIVER_AT86RF231;
#endif
#ifdef MODULE_CC1020
    transceivers |= TRANSCEIVER_CC1020;
#endif
#if (defined(MODULE_CC110X) || defined(MODULE_CC110X_LEGACY) || defined(MODULE_CC110X_LEGACY_CSMA))
    transceivers |= TRANSCEIVER_CC1100;
#endif
#ifdef MODULE_CC2420
    transceivers |= TRANSCEIVER_CC2420;
#endif
#ifdef MODULE_MC1322X
    transceivers |= TRANSCEIVER_MC1322X;
#endif
#ifdef MODULE_NATIVENET
    transceivers |= TRANSCEIVER_NATIVE;
#endif
    net_if_init();

    if (transceivers != 0) {
#if CPUID_ID_LEN && defined(MODULE_HASHES)
        uint8_t cpuid[CPUID_ID_LEN];

        cpuid_get(cpuid);
#endif
        transceiver_init(transceivers);
        transceiver_start();
        int iface = net_if_init_interface(0, transceivers);

#if CPUID_ID_LEN && defined(MODULE_HASHES)
        net_if_eui64_t eui64;
        uint32_t hash_h = djb2_hash(cpuid, CPUID_ID_LEN / 2);
#if CPUID_ID_LEN % 2 == 0
        uint32_t hash_l = djb2_hash(&(cpuid[CPUID_ID_LEN / 2]),
                                    CPUID_ID_LEN / 2);
#else /* CPUID_ID_LEN % 2 == 0 */
        uint32_t hash_l = djb2_hash(&(cpuid[CPUID_ID_LEN / 2]),
                                    CPUID_ID_LEN / 2 + 1);
#endif /* CPUID_ID_LEN % 2 == 0 */

        eui64.uint32[1] = hash_l;
        eui64.uint32[0] = hash_h;

        /* Set Local/Universal bit to Local since this EUI64 is made up. */
        eui64.uint8[0] |= 0x02;
        net_if_set_eui64(iface, &eui64);

#if ENABLE_DEBUG
        DEBUG("Auto init radio long address on interface %d to ", iface);

        for (size_t i = 0; i < 8; i++) {
            printf("%02x ", eui64.uint8[i]);
        }

        DEBUG("\n");
#endif /* ENABLE_DEBUG */

#undef CONF_RADIO_ADDR
#if (defined(MODULE_CC110X) || defined(MODULE_CC110X_LEGACY) || defined(MODULE_CC110X_LEGACY_CSMA))
        uint8_t hwaddr = (uint8_t)((hash_l ^ hash_h) ^ ((hash_l ^ hash_h) >> 24));
        /* do not combine more parts to keep the propability low that it just
         * becomes 0xff */
#else
        uint16_t hwaddr = HTONS((uint16_t)((hash_l ^ hash_h) ^ ((hash_l ^ hash_h) >> 16)));
#endif
        net_if_set_hardware_address(iface, hwaddr);
        DEBUG("Auto init radio address on interface %d to 0x%04x\n", iface, hwaddr);
#else /* CPUID_ID_LEN && defined(MODULE_HASHES) */

        if (!net_if_get_hardware_address(iface)) {
            DEBUG("Auto init radio address on interface %d to 0x%04x\n", iface, CONF_RADIO_ADDR);
            DEBUG("Change this value at compile time with macro CONF_RADIO_ADDR\n");
            net_if_set_hardware_address(iface, CONF_RADIO_ADDR);
        }

#endif /* CPUID_ID_LEN && defined(MODULE_HASHES) */

        if (net_if_set_src_address_mode(iface, NET_IF_TRANS_ADDR_M_SHORT)) {
            DEBUG("Auto init source address mode to short on interface %d\n",
                  iface);
        }
        else {
            net_if_set_src_address_mode(iface, NET_IF_TRANS_ADDR_M_LONG);
            DEBUG("Auto init source address mode to long on interface %d\n",
                  iface);
        }


        if (net_if_get_pan_id(iface) <= 0) {
            DEBUG("Auto init PAN ID on interface %d to 0x%04x\n", iface, CONF_PAN_ID);
            DEBUG("Change this value at compile time with macro CONF_PAN_ID\n");
            net_if_set_pan_id(iface, CONF_PAN_ID);
        }

        if (iface >= 0) {
            DEBUG("Auto init interface %d\n", iface);
        }
    }
}
#endif /* MODULE_NET_IF */

void auto_init(void)
{
#ifdef MODULE_CONFIG
    DEBUG("Auto init loading config\n");
    config_load();
#endif

#ifdef MODULE_VTIMER
    DEBUG("Auto init vtimer module.\n");
    vtimer_init();
#endif
#ifdef MODULE_UART0
    DEBUG("Auto init uart0 module.\n");
    board_uart0_init();
#endif
#ifdef MODULE_RTC
    DEBUG("Auto init rtc module.\n");
    rtc_init();
#endif
#ifdef MODULE_SHT11
    DEBUG("Auto init SHT11 module.\n");
    sht11_init();
#endif
#ifdef MODULE_GPIOINT
    DEBUG("Auto init gpioint module.\n");
    gpioint_init();
#endif
#ifdef MODULE_CC110X_LEGACY_CSMA
    DEBUG("Auto init CC1100 module.\n");
#ifndef MODULE_TRANSCEIVER
    cc1100_init();
#endif
#endif
#ifdef MODULE_LTC4150
    DEBUG("Auto init ltc4150 module.\n");
    ltc4150_init();
#endif
#ifdef MODULE_MCI
    DEBUG("Auto init mci module.\n");
    MCI_initialize();
#endif
#ifdef MODULE_L2_PING
    DEBUG("Auto init net_if module.\n");
    l2_ping_init();
#endif
#ifdef MODULE_NOMAC
    DEBUG("Auto init nomac module.\n");
    nomac_init_module();
#endif
#ifdef MODULE_NET_IF
    DEBUG("Auto init net_if module.\n");
    auto_init_net_if();
#endif
#ifdef MODULE_SIXLOWPAN
    DEBUG("Auto init 6LoWPAN module.\n");
    sixlowpan_lowpan_init();
#endif
#ifdef MODULE_PROFILING
    extern void profiling_init(void);
    profiling_init();
#endif
#ifdef MODULE_UDP
    DEBUG("Auto init transport layer module: [udp].\n");
    udp_init_transport_layer();
#endif

#ifdef MODULE_TCP
    DEBUG("Auto init transport layer module: [tcp].\n");
    tcp_init_transport_layer();
#endif
#ifdef MODULE_NG_PKTDUMP
    DEBUG("Auto init ng_pktdump module.\n");
    ng_pktdump_init();
#endif
#ifdef MODULE_NG_SIXLOWPAN
    DEBUG("Auto init ng_sixlowpan module.\n");
    ng_sixlowpan_init();
#endif
#ifdef MODULE_NG_IPV6
    DEBUG("Auto init ng_ipv6 module.\n");
    ng_ipv6_init();
#endif
#ifdef MODULE_NG_UDP
    DEBUG("Auto init UDP module.\n");
    ng_udp_init();
#endif


/* initialize network devices */
#ifdef MODULE_AUTO_INIT_NG_NETIF

#ifdef MODULE_NG_AT86RF2XX
    extern void auto_init_ng_at86rf2xx(void);
    auto_init_ng_at86rf2xx();
#endif

#ifdef MODULE_XBEE
    extern void auto_init_xbee(void);
    auto_init_xbee();
#endif

#ifdef MODULE_KW2XRF
    extern void auto_init_kw2xrf(void);
    auto_init_kw2xrf();
#endif

#endif /* MODULE_AUTO_INIT_NG_NETIF */

#ifdef MODULE_NG_IPV6_NETIF
    ng_ipv6_netif_init_by_dev();
#endif
}
