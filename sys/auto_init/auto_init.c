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
#include "auto_init_utils.h"
#include "auto_init_priorities.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG CONFIG_AUTO_INIT_ENABLE_DEBUG
#include "debug.h"

XFA_INIT_CONST(auto_init_module_t, auto_init_xfa);

static inline void _auto_init_module(const volatile auto_init_module_t *module)
{
#if IS_ACTIVE(CONFIG_AUTO_INIT_ENABLE_DEBUG)
    DEBUG("auto_init: %s (%u)\n", module->name, module->prio);
#endif
    module->init();
}

#if IS_USED(MODULE_AUTO_INIT_ZTIMER)
extern void ztimer_init(void);
AUTO_INIT(ztimer_init,
          AUTO_INIT_PRIO_MOD_ZTIMER);
#endif
#if IS_USED(MODULE_AUTO_INIT_ZTIMER64)
extern void ztimer64_init(void);
AUTO_INIT(ztimer64_init,
          AUTO_INIT_PRIO_MOD_ZTIMER64);
#endif
#if IS_USED(MODULE_AUTO_INIT_XTIMER) && !IS_USED(MODULE_ZTIMER_XTIMER_COMPAT)
extern void xtimer_init(void);
AUTO_INIT(xtimer_init,
          AUTO_INIT_PRIO_MOD_XTIMER);
#endif
#if IS_USED(MODULE_AUTO_INIT_RANDOM)
extern void auto_init_random(void);
AUTO_INIT(auto_init_random,
          AUTO_INIT_PRIO_MOD_RANDOM);
#endif
#if IS_USED(MODULE_SCHEDSTATISTICS)
extern void init_schedstatistics(void);
AUTO_INIT(init_schedstatistics,
          AUTO_INIT_PRIO_MOD_SCHEDSTATISTICS);
#endif
#if IS_USED(MODULE_SCHED_ROUND_ROBIN)
extern void sched_round_robin_init(void);
AUTO_INIT(sched_round_robin_init,
          AUTO_INIT_PRIO_MOD_SCHED_ROUND_ROBIN);
#endif
#if IS_USED(MODULE_DUMMY_THREAD)
extern void dummy_thread_create(void);
AUTO_INIT(dummy_thread_create,
          AUTO_INIT_PRIO_MOD_DUMMY_THREAD);
#endif
#if IS_USED(MODULE_EVENT_THREAD)
extern void auto_init_event_thread(void);
AUTO_INIT(auto_init_event_thread,
          AUTO_INIT_PRIO_MOD_EVENT_THREAD);
#endif
#if IS_USED(MODULE_SYS_BUS)
extern void auto_init_sys_bus(void);
AUTO_INIT(auto_init_sys_bus,
          AUTO_INIT_PRIO_MOD_SYS_BUS);
#endif
#if IS_USED(MODULE_MCI)
extern void mci_initialize(void);
AUTO_INIT(mci_initialize,
          AUTO_INIT_PRIO_MOD_MCI);
#endif
#if IS_USED(MODULE_PROFILING)
extern void profiling_init(void);
AUTO_INIT(profiling_init,
          AUTO_INIT_PRIO_MOD_PROFILING);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_PKTBUF)
extern void gnrc_pktbuf_init(void);
AUTO_INIT(gnrc_pktbuf_init,
          AUTO_INIT_PRIO_MOD_GNRC_PKTBUF);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_PKTDUMP)
extern void gnrc_pktdump_init(void);
AUTO_INIT(gnrc_pktdump_init,
          AUTO_INIT_PRIO_MOD_GNRC_PKTDUMP);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_SIXLOWPAN)
extern void gnrc_sixlowpan_init(void);
AUTO_INIT(gnrc_sixlowpan_init,
          AUTO_INIT_PRIO_MOD_GNRC_SIXLOWPAN);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_IPV6)
extern kernel_pid_t gnrc_ipv6_init(void);
AUTO_INIT(gnrc_ipv6_init,
          AUTO_INIT_PRIO_MOD_GNRC_IPV6);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_UDP)
extern void gnrc_udp_init(void);
AUTO_INIT(gnrc_udp_init,
          AUTO_INIT_PRIO_MOD_GNRC_UDP);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_TCP)
extern int gnrc_tcp_init(void);
AUTO_INIT(gnrc_tcp_init,
          AUTO_INIT_PRIO_MOD_GNRC_TCP);
#endif
#if IS_USED(MODULE_AUTO_INIT_LWIP)
extern void lwip_bootstrap(void);
AUTO_INIT(lwip_bootstrap,
          AUTO_INIT_PRIO_MOD_LWIP);
#endif
#if IS_USED(MODULE_SOCK_DTLS)
extern void sock_dtls_init(void);
AUTO_INIT(sock_dtls_init,
          AUTO_INIT_PRIO_MOD_SOCK_DTLS);
#endif
#if IS_USED(MODULE_OPENTHREAD)
extern void openthread_bootstrap(void);
AUTO_INIT(openthread_bootstrap,
          AUTO_INIT_PRIO_MOD_OPENTHREAD);
#endif
#if IS_USED(MODULE_AUTO_INIT_OPENWSN)
extern void openwsn_bootstrap(void);
AUTO_INIT(openwsn_bootstrap,
          AUTO_INIT_PRIO_MOD_OPENWSN);
#endif
#if IS_USED(MODULE_AUTO_INIT_MYNEWT_CORE)
extern void mynewt_core_init(void);
AUTO_INIT(mynewt_core_init,
          AUTO_INIT_PRIO_MOD_MYNEWT_CORE);
#endif
#if IS_USED(MODULE_AUTO_INIT_UWB_CORE)
extern void uwb_core_init(void);
AUTO_INIT(uwb_core_init,
          AUTO_INIT_PRIO_MOD_UWB_CORE);
#endif
#if IS_USED(MODULE_GCOAP) && !IS_ACTIVE(CONFIG_GCOAP_NO_AUTO_INIT)
extern void gcoap_init(void);
AUTO_INIT(gcoap_init,
          AUTO_INIT_PRIO_MOD_GCOAP);
#endif
#if IS_USED(MODULE_NANOCOAP_SERVER_AUTO_INIT)
extern void auto_init_nanocoap_server(void);
AUTO_INIT(auto_init_nanocoap_server, AUTO_INIT_PRIO_MOD_NANOCOAP);
#endif
#if IS_USED(MODULE_DEVFS)
extern void auto_init_devfs(void);
AUTO_INIT(auto_init_devfs,
          AUTO_INIT_PRIO_MOD_DEVFS);
#endif
#if IS_USED(MODULE_VFS_AUTO_MOUNT)
extern void auto_init_vfs(void);
AUTO_INIT(auto_init_vfs,
          AUTO_INIT_PRIO_MOD_VFS);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_IPV6_NIB)
extern void gnrc_ipv6_nib_init(void);
AUTO_INIT(gnrc_ipv6_nib_init,
          AUTO_INIT_PRIO_MOD_GNRC_IPV6_NIB);
#endif
#if IS_USED(MODULE_SKALD)
extern void skald_init(void);
AUTO_INIT(skald_init,
          AUTO_INIT_PRIO_MOD_SKALD);
#endif
#if IS_USED(MODULE_CORD_COMMON)
extern void cord_common_init(void);
AUTO_INIT(cord_common_init,
          AUTO_INIT_PRIO_MOD_CORD_COMMON);
#endif
#if IS_USED(MODULE_CORD_EP_STANDALONE)
extern void cord_ep_standalone_run(void);
AUTO_INIT(cord_ep_standalone_run,
          AUTO_INIT_PRIO_MOD_CORD_EP_STANDALONE);
#endif
#if IS_USED(MODULE_ASYMCUTE)
extern void asymcute_handler_run(void);
AUTO_INIT(asymcute_handler_run,
          AUTO_INIT_PRIO_MOD_ASYMCUTE);
#endif
#if IS_USED(MODULE_ESP_BLE_NIMBLE)
extern void esp_ble_nimble_init(void);
AUTO_INIT(esp_ble_nimble_init,
          AUTO_INIT_PRIO_MOD_ESP_BLE_NIMBLE);
#endif
#if IS_USED(MODULE_NIMBLE)
extern void nimble_riot_init(void);
AUTO_INIT(nimble_riot_init,
          AUTO_INIT_PRIO_MOD_NIMBLE);
#endif
#if IS_USED(MODULE_AUTO_INIT_LORAMAC)
extern void auto_init_loramac(void);
AUTO_INIT(auto_init_loramac,
          AUTO_INIT_PRIO_MOD_LORAMAC);
#endif
#if IS_USED(MODULE_DSM)
extern void dsm_init(void);
AUTO_INIT(dsm_init,
          AUTO_INIT_PRIO_MOD_DSM);
#endif
/* initialize USB devices */
#if IS_USED(MODULE_AUTO_INIT_USBUS)
extern void auto_init_usb(void);
AUTO_INIT(auto_init_usb,
          AUTO_INIT_PRIO_MOD_USBUS);
#endif
/* initialize network devices */
#if IS_USED(MODULE_AUTO_INIT_GNRC_NETIF)
extern void gnrc_netif_init_devs(void);
AUTO_INIT(gnrc_netif_init_devs,
          AUTO_INIT_PRIO_MOD_GNRC_NETIF);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_UHCPC)
extern void auto_init_gnrc_uhcpc(void);
AUTO_INIT(auto_init_gnrc_uhcpc,
          AUTO_INIT_PRIO_MOD_GNRC_UHCPC);
#endif
/* initialize NDN module after the network devices are initialized */
#if IS_USED(MODULE_NDN_RIOT)
extern void ndn_init(void);
AUTO_INIT(ndn_init,
          AUTO_INIT_PRIO_MOD_NDN);
#endif
/* initialize sensors and actuators */
#if IS_USED(MODULE_SHT1X)
/* The sht1x module needs to be initialized regardless of SAUL being used,
 * as the shell commands rely on auto-initialization. auto_init_sht1x also
 * performs SAUL registration, but only if module auto_init_saul is used.
 */
extern void auto_init_sht1x(void);
AUTO_INIT(auto_init_sht1x,
          AUTO_INIT_PRIO_MOD_SHT1X);
#endif
#if IS_USED(MODULE_AUTO_INIT_SAUL)
extern void saul_init_devs(void);
AUTO_INIT(saul_init_devs,
          AUTO_INIT_PRIO_MOD_SAUL);
#endif
#if IS_USED(MODULE_AUTO_INIT_GNRC_RPL)
extern void auto_init_gnrc_rpl(void);
AUTO_INIT(auto_init_gnrc_rpl,
          AUTO_INIT_PRIO_MOD_GNRC_RPL);
#endif
#if IS_USED(MODULE_AUTO_INIT_CAN)
extern void auto_init_candev(void);
AUTO_INIT(auto_init_candev,
          AUTO_INIT_PRIO_MOD_CAN);
#endif
#if IS_USED(MODULE_SUIT)
extern void suit_init_conditions(void);
AUTO_INIT(suit_init_conditions,
          AUTO_INIT_PRIO_MOD_SUIT);
#endif
#if IS_USED(MODULE_MBEDTLS)
extern void auto_init_mbedtls(void);
AUTO_INIT(auto_init_mbedtls,
          AUTO_INIT_PRIO_MOD_MBEDTLS);
#endif
#if IS_USED(MODULE_AUTO_INIT_SECURITY)
extern void auto_init_security(void);
AUTO_INIT(auto_init_security,
          AUTO_INIT_PRIO_MOD_SECURITY);
#endif
#if IS_USED(MODULE_DRIVER_CRYPTOCELL_310)
extern void driver_cryptocell_310_setup(void);
AUTO_INIT(driver_cryptocell_310_setup,
          AUTO_INIT_PRIO_MOD_DRIVER_CRYPTOCELL_310);
#endif
#if IS_USED(MODULE_TEST_UTILS_INTERACTIVE_SYNC) && !IS_USED(MODULE_SHELL)
extern void test_utils_interactive_sync(void);
AUTO_INIT(test_utils_interactive_sync,
          AUTO_INIT_PRIO_MOD_TEST_UTILS_INTERACTIVE_SYNC);
#endif
#if IS_USED(MODULE_AUTO_INIT_DHCPV6_CLIENT)
extern void dhcpv6_client_auto_init(void);
AUTO_INIT(dhcpv6_client_auto_init,
          AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT);
#endif
#if IS_USED(MODULE_AUTO_INIT_DHCPV6_RELAY)
extern void dhcpv6_relay_auto_init(void);
AUTO_INIT(dhcpv6_relay_auto_init,
          AUTO_INIT_PRIO_MOD_DHCPV6_RELAY);
#endif
#if IS_USED(MODULE_GNRC_DHCPV6_CLIENT_SIMPLE_PD)
extern void gnrc_dhcpv6_client_simple_pd_init(void);
AUTO_INIT(gnrc_dhcpv6_client_simple_pd_init,
          AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT_SIMPLE_PD);
#endif
#if IS_USED(MODULE_GNRC_IPV6_AUTO_SUBNETS_AUTO_INIT)
extern void gnrc_ipv6_auto_subnets_init(void);
AUTO_INIT(gnrc_ipv6_auto_subnets_init,
          AUTO_INIT_PRIO_MOD_GNRC_IPV6_AUTO_SUBNETS);
#endif
#if IS_USED(MODULE_AUTO_INIT_TELNET)
extern void telnet_server_start(void);
AUTO_INIT(telnet_server_start,
          AUTO_INIT_PRIO_MOD_TELNET);
#endif
#if IS_USED(MODULE_AUTO_INIT_MULTIMEDIA)
#if IS_USED(MODULE_DFPLAYER)
extern void auto_init_dfplayer(void);
AUTO_INIT(auto_init_dfplayer,
          AUTO_INIT_PRIO_MOD_DFPLAYER);
#endif
#endif
#if IS_USED(MODULE_AUTO_INIT_SCREEN)
extern void auto_init_screen(void);
AUTO_INIT(auto_init_screen,
          AUTO_INIT_PRIO_MOD_SCREEN);
#endif
#if IS_USED(MODULE_AUTO_INIT_BENCHMARK_UDP)
extern void benchmark_udp_auto_init(void);
AUTO_INIT(benchmark_udp_auto_init,
          AUTO_INIT_PRIO_MOD_BENCHMARK_UDP);
#endif
#if IS_USED(MODULE_AUTO_INIT_SOCK_DNS)
extern void auto_init_sock_dns(void);
AUTO_INIT(auto_init_sock_dns,
          AUTO_INIT_PRIO_MOD_DOCK_DNS);
#endif
#if IS_USED(MODULE_GNRC_IPV6_STATIC_ADDR)
extern void auto_init_gnrc_ipv6_static_addr(void);
AUTO_INIT(auto_init_gnrc_ipv6_static_addr,
          AUTO_INIT_PRIO_MOD_GNRC_IPV6_STATIC_ADDR);
#endif

void auto_init(void)
{
    for (unsigned i = 0; i < XFA_LEN(auto_init_module_t, auto_init_xfa); i++) {
        _auto_init_module(&auto_init_xfa[i]);
    }
}
