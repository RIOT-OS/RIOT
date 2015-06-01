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

#ifdef MODULE_GNRC_SIXLOWPAN
#include "net/gnrc/sixlowpan.h"
#endif

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef MODULE_GNRC_IPV6_NETIF
#include "net/gnrc/ipv6/netif.h"
#endif

#ifdef MODULE_L2_PING
#include "l2_ping.h"
#endif

#ifdef MODULE_GNRC_PKTBUF
#include "net/gnrc/pktbuf.h"
#endif

#ifdef MODULE_GNRC_PKTDUMP
#include "net/gnrc/pktdump.h"
#endif

#ifdef MODULE_GNRC_UDP
#include "net/gnrc/udp.h"
#endif

#ifdef MODULE_DEV_ETH_AUTOINIT
#include "net/dev_eth.h"
#include "dev_eth_autoinit.h"
#endif

#ifdef MODULE_FIB
#include "net/fib.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

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
#ifndef MODULE_UART_STDIO
#ifdef MODULE_UART0
    DEBUG("Auto init uart0 module.\n");
    board_uart0_init();
#endif
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
#ifdef MODULE_LTC4150
    DEBUG("Auto init ltc4150 module.\n");
    ltc4150_init();
#endif
#ifdef MODULE_MCI
    DEBUG("Auto init mci module.\n");
    MCI_initialize();
#endif
#ifdef MODULE_PROFILING
    extern void profiling_init(void);
    profiling_init();
#endif
#ifdef MODULE_GNRC_PKTBUF
    DEBUG("Auto init gnrc_pktbuf module\n");
    gnrc_pktbuf_init();
#endif
#ifdef MODULE_GNRC_PKTDUMP
    DEBUG("Auto init gnrc_pktdump module.\n");
    gnrc_pktdump_init();
#endif
#ifdef MODULE_GNRC_SIXLOWPAN
    DEBUG("Auto init gnrc_sixlowpan module.\n");
    gnrc_sixlowpan_init();
#endif
#ifdef MODULE_GNRC_IPV6
    DEBUG("Auto init gnrc_ipv6 module.\n");
    gnrc_ipv6_init();
#endif
#ifdef MODULE_GNRC_UDP
    DEBUG("Auto init UDP module.\n");
    gnrc_udp_init();
#endif
#ifdef MODULE_FIB
    DEBUG("Auto init FIB module.\n");
    fib_init();
#endif


/* initialize network devices */
#ifdef MODULE_AUTO_INIT_NG_NETIF

#ifdef MODULE_AT86RF2XX
    extern void auto_init_at86rf2xx(void);
    auto_init_at86rf2xx();
#endif

#ifdef MODULE_NG_SLIP
    extern void auto_init_slip(void);
    auto_init_slip();
#endif

#ifdef MODULE_XBEE
    extern void auto_init_xbee(void);
    auto_init_xbee();
#endif

#ifdef MODULE_KW2XRF
    extern void auto_init_kw2xrf(void);
    auto_init_kw2xrf();
#endif

#ifdef MODULE_GNRC_NETDEV_ETH
    extern void auto_init_ng_netdev_eth(void);
    auto_init_ng_netdev_eth();
#endif

#endif /* MODULE_AUTO_INIT_NG_NETIF */

#ifdef MODULE_GNRC_IPV6_NETIF
    gnrc_ipv6_netif_init_by_dev();
#endif
}
