/**
 * Auto initialization for used modules
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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

#ifdef MODULE_CC110X
#include "cc110x.h"
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

#ifdef MODULE_DESTINY
#include "destiny.h"
#endif

#ifdef MODULE_NET_IF
#include "net_if.h"
#include "transceiver.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef CONF_RADIO_ADDR
#define CONF_RADIO_ADDR (1)
#endif

#ifndef CONF_PAN_ID
#define CONF_PAN_ID     (0xabcd)
#endif

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
#ifdef MODULE_CC110X
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
    int iface;
    DEBUG("Auto init net_if module.\n");
    transceiver_type_t transceivers = 0;
#ifdef MODULE_AT86RF231
    transceivers |= TRANSCEIVER_AT86RF231;
#endif
#ifdef MODULE_CC1020
    transceivers |= TRANSCEIVER_CC1020;
#endif
#if MODULE_CC110X || MODULE_CC110X_NG
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
        transceiver_init(transceivers);
        transceiver_start();
        iface = net_if_init_interface(0, transceivers);

        if (net_if_set_src_address_mode(iface, NET_IF_TRANS_ADDR_M_SHORT)) {
            DEBUG("Auto init source address mode to short on interface %d\n",
                  iface);
        }
        else {
            net_if_set_hardware_address(iface, NET_IF_TRANS_ADDR_M_LONG);
            DEBUG("Auto init source address mode to long on interface %d\n",
                  iface);
        }

        if (!net_if_get_hardware_address(iface)) {
            DEBUG("Auto init radio address on interface %d to 0x%04x\n", iface, CONF_RADIO_ADDR);
            DEBUG("Change this value at compile time with macro CONF_RADIO_ADDR\n");
            net_if_set_hardware_address(iface, CONF_RADIO_ADDR);
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
    else {
        iface = -1;
    }

#ifdef MODULE_SIXLOWPAN
    DEBUG("Auto init 6LoWPAN module.\n");
    sixlowpan_lowpan_init();
#endif
#endif
#ifdef MODULE_PROFILING
    extern void profiling_init(void);
    profiling_init();
#endif
#ifdef MODULE_DESTINY
    DEBUG("Auto init transport layer [destiny] module.\n");
    destiny_init_transport_layer();
#endif
}
