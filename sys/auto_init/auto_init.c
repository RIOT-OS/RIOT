/**
 * Auto initialization for used modules
 *
 * Copyright (C) 2020 Freie Universität Berlin
 *               2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_auto_init
 * @{
 * @file
 * @brief   initializes any used module that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */
#include <stdint.h>
#include <stdio.h>

#include "auto_init.h"
#include "kernel_defines.h"
#include "log.h"

void auto_init(void)
{
   if (IS_USED(MODULE_AUTO_INIT_ZTIMER)) {
        LOG_DEBUG("Auto init ztimer.\n");
        void ztimer_init(void);
        ztimer_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_XTIMER) &&
            !IS_USED(MODULE_ZTIMER_XTIMER_COMPAT)) {
        LOG_DEBUG("Auto init xtimer.\n");
        extern void xtimer_init(void);
        xtimer_init();
    }
   if (IS_USED(MODULE_AUTO_INIT_RANDOM)) {
        LOG_DEBUG("Auto init random.\n");
        extern void auto_init_random(void);
        auto_init_random();
    }
    if (IS_USED(MODULE_SCHEDSTATISTICS)) {
        LOG_DEBUG("Auto init schedstatistics.\n");
        extern void init_schedstatistics(void);
        init_schedstatistics();
    }
    if (IS_USED(MODULE_DUMMY_THREAD)) {
        extern void dummy_thread_create(void);
        dummy_thread_create();
    }
    if (IS_USED(MODULE_EVENT_THREAD)) {
        LOG_DEBUG("Auto init event threads.\n");
        extern void auto_init_event_thread(void);
        auto_init_event_thread();
    }
    if (IS_USED(MODULE_SYS_BUS)) {
        LOG_DEBUG("Auto init system buses.\n");
        extern void auto_init_sys_bus(void);
        auto_init_sys_bus();
    }
    if (IS_USED(MODULE_MCI)) {
        LOG_DEBUG("Auto init mci.\n");
        extern void mci_initialize(void);
        mci_initialize();
    }
    if (IS_USED(MODULE_PROFILING)) {
        LOG_DEBUG("Auto init profiling.\n");
        extern void profiling_init(void);
        profiling_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_PKTBUF)) {
        LOG_DEBUG("Auto init gnrc_pktbuf.\n");
        extern void gnrc_pktbuf_init(void);
        gnrc_pktbuf_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_PKTDUMP)) {
        LOG_DEBUG("Auto init gnrc_pktdump.\n");
        extern void gnrc_pktdump_init(void);
        gnrc_pktdump_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_SIXLOWPAN)) {
        LOG_DEBUG("Auto init gnrc_sixlowpan.\n");
        extern void gnrc_sixlowpan_init(void);
        gnrc_sixlowpan_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_IPV6)) {
        LOG_DEBUG("Auto init gnrc_ipv6.\n");
        extern void gnrc_ipv6_init(void);
        gnrc_ipv6_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_UDP)) {
        LOG_DEBUG("Auto init gnrc_udp.\n");
        extern void gnrc_udp_init(void);
        gnrc_udp_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_TCP)) {
        LOG_DEBUG("Auto init gnrc_tcp.\n");
        extern void gnrc_tcp_init(void);
        gnrc_tcp_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_LWIP)) {
        LOG_DEBUG("Bootstraping lwIP.\n");
        extern void lwip_bootstrap(void);
        lwip_bootstrap();
    }
    if (IS_USED(MODULE_SOCK_DTLS)) {
        LOG_DEBUG("Auto init sock_dtls.\n");
        extern void sock_dtls_init(void);
        sock_dtls_init();
    }
    if (IS_USED(MODULE_OPENTHREAD)) {
        LOG_DEBUG("Bootstrapping openthread.\n");
        extern void openthread_bootstrap(void);
        openthread_bootstrap();
    }
    if (IS_USED(MODULE_AUTO_INIT_OPENWSN)) {
        LOG_DEBUG("Bootstrapping openwsn.\n");
        extern void openwsn_bootstrap(void);
        openwsn_bootstrap();
    }
    if (IS_USED(MODULE_AUTO_INIT_MYNEWT_CORE)) {
        LOG_DEBUG("Bootstrapping mynewt-core.\n");
        extern void mynewt_core_init(void);
        mynewt_core_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_UWB_CORE)) {
        LOG_DEBUG("Bootstrapping uwb core.\n");
        extern void uwb_core_init(void);
        uwb_core_init();
    }
    if (IS_USED(MODULE_GCOAP) &&
        !IS_ACTIVE(CONFIG_GCOAP_NO_AUTO_INIT)) {
        LOG_DEBUG("Auto init gcoap.\n");
        extern void gcoap_init(void);
        gcoap_init();
    }
    if (IS_USED(MODULE_DEVFS)) {
        LOG_DEBUG("Mounting /dev.\n");
        extern void auto_init_devfs(void);
        auto_init_devfs();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_IPV6_NIB)) {
        LOG_DEBUG("Auto init gnrc_ipv6_nib.\n");
        extern void gnrc_ipv6_nib_init(void);
        gnrc_ipv6_nib_init();
    }
    if (IS_USED(MODULE_SKALD)) {
        LOG_DEBUG("Auto init Skald.\n");
        extern void skald_init(void);
        skald_init();
    }
    if (IS_USED(MODULE_CORD_COMMON)) {
        LOG_DEBUG("Auto init cord_common.\n");
        extern void cord_common_init(void);
        cord_common_init();
    }
    if (IS_USED(MODULE_CORD_EP_STANDALONE)) {
        LOG_DEBUG("Auto init cord_ep_standalone.\n");
        extern void cord_ep_standalone_run(void);
        cord_ep_standalone_run();
    }
    if (IS_USED(MODULE_ASYMCUTE)) {
        LOG_DEBUG("Auto init Asymcute.\n");
        extern void asymcute_handler_run(void);
        asymcute_handler_run();
    }
    if (IS_USED(MODULE_NIMBLE)) {
        LOG_DEBUG("Auto init NimBLE.\n");
        extern void nimble_riot_init(void);
        nimble_riot_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_LORAMAC)) {
        LOG_DEBUG("Auto init loramac.\n");
        extern void auto_init_loramac(void);
        auto_init_loramac();
    }
    if (IS_USED(MODULE_DSM)) {
        LOG_DEBUG("Auto init dsm.\n");
        extern void dsm_init(void);
        dsm_init();
    }

    /* initialize USB devices */
    if (IS_USED(MODULE_AUTO_INIT_USBUS)) {
        LOG_DEBUG("Auto init USB.\n");
        extern void auto_init_usb(void);
        auto_init_usb();
    }

    /* initialize network devices */
    if (IS_USED(MODULE_AUTO_INIT_GNRC_NETIF)) {
        LOG_DEBUG("Auto init gnrc_netif.\n");
        extern void gnrc_netif_init_devs(void);
        gnrc_netif_init_devs();
    }

    if (IS_USED(MODULE_AUTO_INIT_GNRC_UHCPC)) {
        LOG_DEBUG("Auto init gnrc_uhcpc.\n");
        extern void auto_init_gnrc_uhcpc(void);
        auto_init_gnrc_uhcpc();
    }

    /* initialize NDN module after the network devices are initialized */
    if (IS_USED(MODULE_NDN_RIOT)) {
        LOG_DEBUG("Auto init NDN.\n");
        extern void ndn_init(void);
        ndn_init();
    }

    /* initialize sensors and actuators */
    if (IS_USED(MODULE_SHT1X)) {
        /* The sht1x module needs to be initialized regardless of SAUL being used,
         * as the shell commands rely on auto-initialization. auto_init_sht1x also
         * performs SAUL registration, but only if module auto_init_saul is used.
         */
        LOG_DEBUG("Auto init sht1x.\n");
        extern void auto_init_sht1x(void);
        auto_init_sht1x();
    }

    if (IS_USED(MODULE_AUTO_INIT_SAUL)) {
        LOG_DEBUG("Auto init SAUL.\n");
        extern void saul_init_devs(void);
        saul_init_devs();
    }

    if (IS_USED(MODULE_AUTO_INIT_GNRC_RPL)) {
        LOG_DEBUG("Auto init gnrc_rpl.\n");
        extern void auto_init_gnrc_rpl(void);
        auto_init_gnrc_rpl();
    }

    if (IS_USED(MODULE_AUTO_INIT_CAN)) {
        LOG_DEBUG("Auto init CAN.\n");

        extern void auto_init_candev(void);
        auto_init_candev();
    }

    if (IS_USED(MODULE_SUIT)) {
        LOG_DEBUG("Auto init SUIT conditions.\n");
        extern void suit_init_conditions(void);
        suit_init_conditions();
    }

    if (IS_USED(MODULE_AUTO_INIT_SECURITY)) {
        if (IS_USED(MODULE_CRYPTOAUTHLIB)) {
            LOG_DEBUG("Auto init cryptoauthlib.\n");
            extern void auto_init_atca(void);
            auto_init_atca();
        }
    }

    if (IS_USED(MODULE_TEST_UTILS_INTERACTIVE_SYNC) && !IS_USED(MODULE_SHELL)) {
        extern void test_utils_interactive_sync(void);
        test_utils_interactive_sync();
    }

    if (IS_USED(MODULE_AUTO_INIT_DHCPV6_CLIENT)) {
        LOG_DEBUG("Auto init DHCPv6 client.\n");
        extern void dhcpv6_client_auto_init(void);
        dhcpv6_client_auto_init();
    }

    if (IS_USED(MODULE_AUTO_INIT_DHCPV6_RELAY)) {
        LOG_DEBUG("Auto init DHCPv6 relay agent.\n");
        extern void dhcpv6_relay_auto_init(void);
        dhcpv6_relay_auto_init();
    }

    if (IS_USED(MODULE_GNRC_DHCPV6_CLIENT_SIMPLE_PD)) {
        LOG_DEBUG("Auto init DHCPv6 client for simple prefix delegation\n");
        extern void gnrc_dhcpv6_client_simple_pd_init(void);
        gnrc_dhcpv6_client_simple_pd_init();
    }

    if (IS_USED(MODULE_AUTO_INIT_MULTIMEDIA)) {
        LOG_DEBUG("auto_init MULTIMEDIA\n");
        if (IS_USED(MODULE_DFPLAYER)) {
            extern void auto_init_dfplayer(void);
            auto_init_dfplayer();
        }
    }

    if (IS_USED(MODULE_AUTO_INIT_SCREEN)) {
        LOG_DEBUG("Auto init screen devices\n");
        extern void auto_init_screen(void);
        auto_init_screen();
    }

    if (IS_USED(MODULE_AUTO_INIT_BENCHMARK_UDP)) {
        LOG_DEBUG("Auto init UDP benchmark\n");
        extern void benchmark_udp_auto_init(void);
        benchmark_udp_auto_init();
    }
}
