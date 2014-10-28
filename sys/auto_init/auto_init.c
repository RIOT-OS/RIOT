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
 * @file    auto_init_c
 * @brief   initializes any used module that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */
#include <stdint.h>
#include <stdio.h>

#include "auto_init.h"

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
#include "rtc.h"
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

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
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
#if MODULE_CC110X_LEGACY_CSMA || MODULE_CC110X_LEGACY
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
        int iface;
#if CPUID_ID_LEN && defined(MODULE_HASHES)
        uint8_t cpuid[CPUID_ID_LEN];

        cpuid_get(cpuid);
#endif
        transceiver_init(transceivers);
        transceiver_start();
        iface = net_if_init_interface(0, transceivers);

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

        memcpy(&(eui64.uint32[0]), &hash_h, sizeof(uint32_t));
        memcpy(&(eui64.uint32[1]), &hash_l, sizeof(uint32_t));
        net_if_set_eui64(iface, &eui64);

#ifdef DEBUG_ENABLED
        DEBUG("Auto init radio long address on interface %d to ", iface);

        for (size_t i = 0; i < 8; i++) {
            printf("%02x ", eui64.uint8[i]);
        }

        DEBUG("\n");
#endif /* DEBUG_ENABLED */

#undef CONF_RADIO_ADDR
        uint16_t hwaddr = HTONS((uint16_t)((hash_l ^ hash_h) ^ ((hash_l ^ hash_h) >> 16)));
        net_if_set_hardware_address(iface, hwaddr);
        DEBUG("Auto init radio address on interface %d to 0x%04x\n", iface, hwaddr);
#else /* CPUID_ID_LEN && defined(MODULE_HASHES) */

        if (net_if_set_src_address_mode(iface, NET_IF_TRANS_ADDR_M_SHORT)) {
            DEBUG("Auto init source address mode to short on interface %d\n",
                  iface);
        }
        else {
            net_if_set_src_address_mode(iface, NET_IF_TRANS_ADDR_M_LONG);
            DEBUG("Auto init source address mode to long on interface %d\n",
                  iface);
        }

        if (!net_if_get_hardware_address(iface)) {
            DEBUG("Auto init radio address on interface %d to 0x%04x\n", iface, CONF_RADIO_ADDR);
            DEBUG("Change this value at compile time with macro CONF_RADIO_ADDR\n");
            net_if_set_hardware_address(iface, CONF_RADIO_ADDR);
        }

#endif /* CPUID_ID_LEN && defined(MODULE_HASHES) */

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
    rtc_enable();
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
}
