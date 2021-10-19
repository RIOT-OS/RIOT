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
#include "sched.h"
#include "auto_init.h"
#include "auto_init_priorities.h"
#include "kernel_defines.h"

XFA_INIT_CONST(auto_init_fn, auto_init_xfa);
#if IS_USED(MODULE_AUTO_INIT_ZTIMER)
extern void ztimer_init(void);
AUTO_INIT_MODULE(ztimer, AUTO_INIT_PRIO_MOD_ZTIMER, ztimer_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_ZTIMER64)
extern void ztimer64_init(void);
AUTO_INIT_MODULE(ztimer64, AUTO_INIT_PRIO_MOD_ZTIMER64, ztimer64_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_XTIMER) && !IS_USED(MODULE_ZTIMER_XTIMER_COMPAT)
extern void xtimer_init(void);
AUTO_INIT_MODULE(xtimer, AUTO_INIT_PRIO_MOD_XTIMER, xtimer_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_RANDOM)
extern void auto_init_random(void);
AUTO_INIT_MODULE(random, AUTO_INIT_PRIO_MOD_RANDOM, auto_init_random);
#endif
#if IS_USED(MODULE_SCHEDSTATISTICS)
extern void init_schedstatistics(void);
AUTO_INIT_MODULE(schedstatistics, AUTO_INIT_PRIO_MOD_SCHEDSTATISTICS, init_schedstatistics);
#endif
#if IS_USED(MODULE_SCHED_ROUND_ROBIN)
extern void sched_round_robin_init(void);
AUTO_INIT_MODULE(sched_round_robin, AUTO_INIT_PRIO_MOD_SCHED_ROUND_ROBIN, sched_round_robin_init);
#endif
#if IS_USED(MODULE_DUMMY_THREAD)
extern void dummy_thread_create(void);
AUTO_INIT_MODULE(dummy_thread, AUTO_INIT_PRIO_MOD_DUMMY_THREAD, dummy_thread_create);
#endif
#if IS_USED(MODULE_EVENT_THREAD)
extern void auto_init_event_thread(void);
AUTO_INIT_MODULE(event_thread, AUTO_INIT_PRIO_MOD_EVENT_THREAD, auto_init_event_thread);
#endif
#if IS_USED(MODULE_SYS_BUS)
extern void auto_init_sys_bus(void);
AUTO_INIT_MODULE(sys_bus, AUTO_INIT_PRIO_MOD_SYS_BUS, auto_init_sys_bus);
#endif
#if IS_USED(MODULE_MCI)
extern void mci_initialize(void);
AUTO_INIT_MODULE(mci, AUTO_INIT_PRIO_MOD_MCI, mci_initialize);
#endif
#if IS_USED(MODULE_PROFILING) /* ? */
extern void profiling_init(void);
AUTO_INIT_MODULE(profiling, AUTO_INIT_PRIO_PROFILING, profiling_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_PKTBUF)
extern void gnrc_pktbuf_init(void);
AUTO_INIT_MODULE(gnrc_pktbuf, AUTO_INIT_PRIO_MOD_GNRC_PKTBUF, gnrc_pktbuf_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_PKTDUMP)
extern void gnrc_pktdump_init(void);
AUTO_INIT_MODULE(gnrc_pktdump, AUTO_INIT_PRIO_MOD_GNRC_PKTDUMP, gnrc_pktdump_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_SIXLOWPAN)
extern void gnrc_sixlowpan_init(void);
AUTO_INIT_MODULE(gnrc_sixlowpan, AUTO_INIT_PRIO_MOD_GNRC_SIXLOWPAN, gnrc_sixlowpan_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_IPV6)
extern kernel_pid_t gnrc_ipv6_init(void);
AUTO_INIT_MODULE(gnrc_ipv6, AUTO_INIT_PRIO_MOD_GNRC_IPV6, gnrc_ipv6_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_UDP)
extern void gnrc_udp_init(void);
AUTO_INIT_MODULE(gnrc_udp, AUTO_INIT_PRIO_MOD_GNRC_UDP, gnrc_udp_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_TCP)
extern int gnrc_tcp_init(void);
AUTO_INIT_MODULE(gnrc_tcp, AUTO_INIT_PRIO_MOD_GNRC_TCP, gnrc_tcp_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_LWIP)
extern void lwip_bootstrap(void);
AUTO_INIT_MODULE(lwip, AUTO_INIT_PRIO_MOD_LWIP, lwip_bootstrap);
#endif
#if IS_USED(MODULE_SOCK_DTLS)
extern void sock_dtls_init(void);
AUTO_INIT_MODULE(sock_dtls, AUTO_INIT_PRIO_MOD_SOCK_DTLS, sock_dtls_init);
#endif
#if IS_USED(MODULE_OPENTHREAD)
extern void openthread_bootstrap(void);
AUTO_INIT_MODULE(openthread, AUTO_INIT_PRIO_MOD_OPENTHREAD, openthread_bootstrap);
#endif
#if IS_USED(MODULE_AUTO_INIT_OPENWSN)
extern void openwsn_bootstrap(void);
AUTO_INIT_MODULE(openwsn, AUTO_INIT_PRIO_MOD_OPENWSN, openwsn_bootstrap);
#endif
#if IS_USED(MODULE_AUTO_INIT_MYNEWT_CORE)
extern void mynewt_core_init(void);
AUTO_INIT_MODULE(mynewt_core, AUTO_INIT_PRIO_MOD_MYNEWT, mynewt_core_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_UWB_CORE)
extern void uwb_core_init(void);
AUTO_INIT_MODULE(uwb_core, AUTO_INIT_PRIO_MOD_UWB_CORE, uwb_core_init);
#endif
#if IS_USED(MODULE_GCOAP) && !IS_ACTIVE(CONFIG_GCOAP_NO_AUTO_INIT)
extern void gcoap_init(void);
AUTO_INIT_MODULE(gcoap, AUTO_INIT_PRIO_MOD_GCOAP, gcoap_init);
#endif
#if IS_USED(MODULE_DEVFS)
extern void auto_init_devfs(void);
AUTO_INIT_MODULE(devfs, AUTO_INIT_PRIO_MOD_DEVFS, auto_init_devfs);
#endif
#if IS_USED(MODULE_VFS_AUTO_MOUNT)
extern void auto_init_vfs(void);
AUTO_INIT_MODULE(vfs, AUTO_INIT_PRIO_MOD_VFS, auto_init_vfs);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_IPV6_NIB)
extern void gnrc_ipv6_nib_init(void);
AUTO_INIT_MODULE(gnrc_ipv6_nib, AUTO_INIT_PRIO_MOD_GNRC_IPV6_NIB, gnrc_ipv6_nib_init);
#endif
#if IS_USED(MODULE_SKALD)
extern void skald_init(void);
AUTO_INIT_MODULE(skald, AUTO_INIT_PRIO_MOD_SKALD, skald_init);
#endif
#if IS_USED(MODULE_CORD_COMMON)
extern void cord_common_init(void);
AUTO_INIT_MODULE(cord_common, AUTO_INIT_PRIO_MOD_CORD_COMMON, cord_common_init);
#endif
#if IS_USED(MODULE_CORD_EP_STANDALONE)
extern void cord_ep_standalone_run(void);
AUTO_INIT_MODULE(cord_ep_standalone, AUTO_INIT_PRIO_MOD_CORD_EP_STANDALONE, cord_ep_standalone_run);
#endif
#if IS_USED(MODULE_ASYMCUTE)
extern void asymcute_handler_run(void);
AUTO_INIT_MODULE(asymcute, AUTO_INIT_PRIO_MOD_ASYMCUTE, asymcute_handler_run);
#endif
#if IS_USED(MODULE_NIMBLE)
extern void nimble_riot_init(void);
AUTO_INIT_MODULE(nimble, AUTO_INIT_PRIO_NIMBLE, nimble_riot_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_LORAMAC)
extern void auto_init_loramac(void);
AUTO_INIT_MODULE(loramac, AUTO_INIT_PRIO_MOD_LORAMAC, auto_init_loramac);
#endif
#if IS_USED(MODULE_DSM)
extern void dsm_init(void);
AUTO_INIT_MODULE(dsm, AUTO_INIT_PRIO_MOD_DSM, dsm_init);
#endif
/* initialize USB devices */
#if IS_USED(MODULE_AUTO_INIT_USBUS)
extern void auto_init_usb(void);
AUTO_INIT_MODULE(usbus, AUTO_INIT_PRIO_MOD_USB, auto_init_usb);
#endif
/* initialize network devices */
#if IS_USED(MODULE_AUTO_INIT_GNRC_NETIF)
extern void gnrc_netif_init_devs(void);
AUTO_INIT_MODULE(gnrc_netif, AUTO_INIT_PRIO_MOD_GNRC_NETIF, gnrc_netif_init_devs);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_UHCPC)
extern void auto_init_gnrc_uhcpc(void);
AUTO_INIT_MODULE(gnrc_uhcpc, AUTO_INIT_PRIO_GNRC_UHCPC, auto_init_gnrc_uhcpc);
#endif
/* initialize NDN module after the network devices are initialized */
#if IS_USED(MODULE_NDN_RIOT)
extern void ndn_init(void);
AUTO_INIT_MODULE(ndn_riot, AUTO_INIT_PRIO_MOD_NDN, ndn_init);
#endif
/* initialize sensors and actuators */
#if IS_USED(MODULE_SHT1X)
/* The sht1x module needs to be initialized regardless of SAUL being used,
 * as the shell commands rely on auto-initialization. auto_init_sht1x also
 * performs SAUL registration, but only if module auto_init_saul is used.
 */
extern void auto_init_sht1x(void);
AUTO_INIT_MODULE(sht1x, AUTO_INIT_PRIO_MOD_SHT1X, auto_init_sht1x);
#endif
#if IS_USED(MODULE_AUTO_INIT_SAUL)
extern void saul_init_devs(void);
AUTO_INIT_MODULE(saul, AUTO_INIT_PRIO_MOD_SAUL, saul_init_devs);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_RPL)
extern void auto_init_gnrc_rpl(void);
AUTO_INIT_MODULE(gnrc_rpl, AUTO_INIT_PRIO_MOD_GNRC_RPL, auto_init_gnrc_rpl);
#endif
#if IS_USED(MODULE_AUTO_INIT_CAN)
extern void auto_init_candev(void);
AUTO_INIT_MODULE(can, AUTO_INIT_PRIO_MOD_CAN, auto_init_candev);
#endif
#if IS_USED(MODULE_SUIT)
extern void suit_init_conditions(void);
AUTO_INIT_MODULE(suit, AUTO_INIT_PRIO_MOD_SUIT_CONDITIONS, suit_init_conditions);
#endif
#if IS_USED(MODULE_MBEDTLS)
extern void auto_init_mbedtls(void);
AUTO_INIT_MODULE(mbedtls, AUTO_INIT_PRIO_MOD_MBEDTLS, auto_init_mbedtls);
#endif
#if IS_USED(MODULE_AUTO_INIT_SECURITY)
#if IS_USED(MODULE_CRYPTOAUTHLIB)
extern void auto_init_atca(void);
AUTO_INIT_MODULE(cryptoauthlib, AUTO_INIT_PRIO_MOD_CRYPTOAUTHLIB, auto_init_atca);
#endif
#endif
#if IS_USED(MODULE_TEST_UTILS_INTERACTIVE_SYNC) && !IS_USED(MODULE_SHELL)
extern void test_utils_interactive_sync(void);
AUTO_INIT_MODULE(test_utils_interactive_sync, AUTO_INIT_PRIO_MOD_TEST_UTILS_INTERACTIVE_SYNC,
                 test_utils_interactive_sync);
#endif
#if IS_USED(MODULE_AUTO_INIT_DHCPV6_CLIENT)
extern void dhcpv6_client_auto_init(void);
AUTO_INIT_MODULE(dhcpv6_client, AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT, dhcpv6_client_auto_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_DHCPV6_RELAY)
extern void dhcpv6_relay_auto_init(void);
AUTO_INIT_MODULE(dhcpv6_relay, AUTO_INIT_PRIO_MOD_DHCPV6_RELAY, dhcpv6_relay_auto_init);
#endif
#if IS_USED(MODULE_GNRC_DHCPV6_CLIENT_SIMPLE_PD)
extern void gnrc_dhcpv6_client_simple_pd_init(void);
AUTO_INIT_MODULE(gnrc_dhcpv6_client_simple_pd, AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT_SIMPLE_PD,
                 gnrc_dhcpv6_client_simple_pd_init);
#endif
#if IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_AUTO_INIT)
extern void gnrc_ipv6_auto_subnets_init(void);
AUTO_INIT_MODULE(gnrc_ipv6_auto_subnets, AUTO_INIT_PRIO_MOD_GNRC_IPV6_AUTO_SUBNETS,
                 gnrc_ipv6_auto_subnets_init);
#endif
#if IS_USED(MODULE_AUTO_INIT_MULTIMEDIA)
#if IS_USED(MODULE_DFPLAYER)
extern void auto_init_dfplayer(void);
AUTO_INIT_MODULE(multimedia, AUTO_INIT_PRIO_MOD_DFPLAYER, auto_init_dfplayer);
#endif
#endif
#if IS_USED(MODULE_AUTO_INIT_SCREEN)
extern void auto_init_screen(void);
AUTO_INIT_MODULE(screen, AUTO_INIT_PRIO_MOD_SCREEN, auto_init_screen);
#endif
#if IS_USED(MODULE_AUTO_INIT_BENCHMARK_UDP)
extern void benchmark_udp_auto_init(void);
AUTO_INIT_MODULE(benchmark_udp, AUTO_INIT_PRIO_BENCHMARK_UDP, benchmark_udp_auto_init);
#endif

void auto_init(void)
{
    for (unsigned i = 0; i < XFA_LEN(auto_init_fn, auto_init_xfa); i++) {
        auto_init_xfa[i]();
    }
}
