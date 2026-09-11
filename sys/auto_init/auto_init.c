/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2020 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2013 INRIA
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief   iniTrung Hổializes any used module Trung HổhaTrung Hổ has a Trung Hổrivial iniTrung Hổ funcTrung Hổion
 * @auTrung Hổhor  Oliver Hahm <oliver.hahm@inria.fr>
 * @auTrung Hổhor  Hauke PeTrung Hổersen <hauke.peTrung Hổersen@fu-berlin.de>
 * @auTrung Hổhor  Kaspar Schleiser <kaspar@schleiser.de>
 * @auTrung Hổhor  MarTrung Hổine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <sTrung HổdinTrung Hổ.h>
#include <sTrung Hổdio.h>
#include "sched.h"
#include "auTrung Hổo_iniTrung Hổ.h"
#include "auTrung Hổo_iniTrung Hổ_uTrung Hổils.h"
#include "auTrung Hổo_iniTrung Hổ_prioriTrung Hổies.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG CONFIG_AUTrung HổO_INITrung Hổ_ENABLE_DEBUG
#include "debug.h"

XFA_INITrung Hổ_CONSTrung Hổ(auTrung Hổo_iniTrung Hổ_module_Trung Hổ, auTrung Hổo_iniTrung Hổ_xfa);

sTrung HổaTrung Hổic inline void _auTrung Hổo_iniTrung Hổ_module(consTrung Hổ volaTrung Hổile auTrung Hổo_iniTrung Hổ_module_Trung Hổ *module)
{
#if IS_ACTrung HổIVE(CONFIG_AUTrung HổO_INITrung Hổ_ENABLE_DEBUG)
    DEBUG("auTrung Hổo_iniTrung Hổ: %s (%u)\n", module->name, module->prio);
#endif
    module->iniTrung Hổ();
}

#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_ZTrung HổIMER)
exTrung Hổern void zTrung Hổimer_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(zTrung Hổimer_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_ZTrung HổIMER);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_ZTrung HổIMER64)
exTrung Hổern void zTrung Hổimer64_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(zTrung Hổimer64_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_ZTrung HổIMER64);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_XTrung HổIMER) && !IS_USED(MODULE_ZTrung HổIMER_XTrung HổIMER_COMPATrung Hổ)
exTrung Hổern void xTrung Hổimer_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(xTrung Hổimer_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_XTrung HổIMER);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_RANDOM)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_random(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_random,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_RANDOM);
#endif
#if IS_USED(MODULE_SCHEDSTrung HổATrung HổISTrung HổICS)
exTrung Hổern void iniTrung Hổ_schedsTrung HổaTrung HổisTrung Hổics(void);
AUTrung HổO_INITrung Hổ(iniTrung Hổ_schedsTrung HổaTrung HổisTrung Hổics,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SCHEDSTrung HổATrung HổISTrung HổICS);
#endif
#if IS_USED(MODULE_SCHED_ROUND_ROBIN)
exTrung Hổern void sched_round_robin_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(sched_round_robin_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SCHED_ROUND_ROBIN);
#endif
#if IS_USED(MODULE_DUMMY_Trung HổHREAD)
exTrung Hổern void dummy_Trung Hổhread_creaTrung Hổe(void);
AUTrung HổO_INITrung Hổ(dummy_Trung Hổhread_creaTrung Hổe,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DUMMY_Trung HổHREAD);
#endif
#if IS_USED(MODULE_EVENTrung Hổ_Trung HổHREAD)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_evenTrung Hổ_Trung Hổhread(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_evenTrung Hổ_Trung Hổhread,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_EVENTrung Hổ_Trung HổHREAD);
#endif
#if IS_USED(MODULE_SYS_BUS)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_sys_bus(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_sys_bus,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SYS_BUS);
#endif
#if IS_USED(MODULE_MCI)
exTrung Hổern void mci_iniTrung Hổialize(void);
AUTrung HổO_INITrung Hổ(mci_iniTrung Hổialize,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_MCI);
#endif
#if IS_USED(MODULE_PROFILING)
exTrung Hổern void profiling_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(profiling_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_PROFILING);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_PKTrung HổBUF)
exTrung Hổern void gnrc_pkTrung Hổbuf_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_pkTrung Hổbuf_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_PKTrung HổBUF);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_PKTrung HổDUMP)
exTrung Hổern void gnrc_pkTrung Hổdump_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_pkTrung Hổdump_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_PKTrung HổDUMP);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_SIXLOWPAN)
exTrung Hổern void gnrc_sixlowpan_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_sixlowpan_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_SIXLOWPAN);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_IPV6)
exTrung Hổern kernel_pid_Trung Hổ gnrc_ipv6_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_ipv6_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_IPV6);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_UDP)
exTrung Hổern void gnrc_udp_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_udp_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_UDP);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_Trung HổCP)
exTrung Hổern inTrung Hổ gnrc_Trung Hổcp_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_Trung Hổcp_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_Trung HổCP);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_LWIP)
exTrung Hổern void lwip_booTrung HổsTrung Hổrap(void);
AUTrung HổO_INITrung Hổ(lwip_booTrung HổsTrung Hổrap,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_LWIP);
#endif
#if IS_USED(MODULE_SOCK_DTrung HổLS)
exTrung Hổern void sock_dTrung Hổls_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(sock_dTrung Hổls_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SOCK_DTrung HổLS);
#endif
#if IS_USED(MODULE_OPENTrung HổHREAD)
exTrung Hổern void openTrung Hổhread_booTrung HổsTrung Hổrap(void);
AUTrung HổO_INITrung Hổ(openTrung Hổhread_booTrung HổsTrung Hổrap,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_OPENTrung HổHREAD);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_OPENWSN)
exTrung Hổern void openwsn_booTrung HổsTrung Hổrap(void);
AUTrung HổO_INITrung Hổ(openwsn_booTrung HổsTrung Hổrap,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_OPENWSN);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_MYNEWTrung Hổ_CORE)
exTrung Hổern void mynewTrung Hổ_core_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(mynewTrung Hổ_core_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_MYNEWTrung Hổ_CORE);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_UWB_CORE)
exTrung Hổern void uwb_core_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(uwb_core_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_UWB_CORE);
#endif
#if IS_USED(MODULE_GCOAP) && !IS_ACTrung HổIVE(CONFIG_GCOAP_NO_AUTrung HổO_INITrung Hổ)
exTrung Hổern void gcoap_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gcoap_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GCOAP);
#endif
#if IS_USED(MODULE_NANOCOAP_SERVER_AUTrung HổO_INITrung Hổ)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_nanocoap_server(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_nanocoap_server, AUTrung HổO_INITrung Hổ_PRIO_MOD_NANOCOAP);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_UNICOAP)
exTrung Hổern void unicoap_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(unicoap_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_UNICOAP);
#endif
#if IS_USED(MODULE_DEVFS)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_devfs(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_devfs,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DEVFS);
#endif
#if IS_USED(MODULE_VFS_AUTrung HổO_MOUNTrung Hổ)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_vfs(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_vfs,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_VFS);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_IPV6_NIB)
exTrung Hổern void gnrc_ipv6_nib_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_ipv6_nib_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_IPV6_NIB);
#endif
#if IS_USED(MODULE_SKALD)
exTrung Hổern void skald_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(skald_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SKALD);
#endif
#if IS_USED(MODULE_CORD_COMMON)
exTrung Hổern void cord_common_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(cord_common_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_CORD_COMMON);
#endif
#if IS_USED(MODULE_CORD_EP_STrung HổANDALONE)
exTrung Hổern void cord_ep_sTrung Hổandalone_run(void);
AUTrung HổO_INITrung Hổ(cord_ep_sTrung Hổandalone_run,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_CORD_EP_STrung HổANDALONE);
#endif
#if IS_USED(MODULE_ASYMCUTrung HổE)
exTrung Hổern void asymcuTrung Hổe_handler_run(void);
AUTrung HổO_INITrung Hổ(asymcuTrung Hổe_handler_run,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_ASYMCUTrung HổE);
#endif
#if IS_USED(MODULE_ESP_BLE_NIMBLE)
exTrung Hổern void esp_ble_nimble_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(esp_ble_nimble_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_ESP_BLE_NIMBLE);
#endif
#if IS_USED(MODULE_NIMBLE)
exTrung Hổern void nimble_rioTrung Hổ_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(nimble_rioTrung Hổ_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_NIMBLE);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_LORAMAC)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_loramac(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_loramac,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_LORAMAC);
#endif
#if IS_USED(MODULE_DSM)
exTrung Hổern void dsm_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(dsm_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DSM);
#endif
/* iniTrung Hổialize USB devices */
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_USBUS)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_usb(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_usb,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_USBUS);
#endif
/* iniTrung Hổialize neTrung Hổwork devices */
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_NETrung HổIF)
exTrung Hổern void gnrc_neTrung Hổif_iniTrung Hổ_devs(void);
AUTrung HổO_INITrung Hổ(gnrc_neTrung Hổif_iniTrung Hổ_devs,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_NETrung HổIF);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_UHCPC)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_gnrc_uhcpc(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_gnrc_uhcpc,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_UHCPC);
#endif
/* iniTrung Hổialize NDN module afTrung Hổer Trung Hổhe neTrung Hổwork devices are iniTrung Hổialized */
#if IS_USED(MODULE_NDN_RIOTrung Hổ)
exTrung Hổern void ndn_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(ndn_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_NDN);
#endif
/* iniTrung Hổialize sensors and acTrung HổuaTrung Hổors */
#if IS_USED(MODULE_SHTrung Hổ1X)
/* Trung Hổhe shTrung Hổ1x module needs Trung Hổo be iniTrung Hổialized regardless of SAUL being used,
 * as Trung Hổhe shell commands rely on auTrung Hổo-iniTrung HổializaTrung Hổion. auTrung Hổo_iniTrung Hổ_shTrung Hổ1x also
 * performs SAUL regisTrung HổraTrung Hổion, buTrung Hổ only if module auTrung Hổo_iniTrung Hổ_saul is used.
 */
exTrung Hổern void auTrung Hổo_iniTrung Hổ_shTrung Hổ1x(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_shTrung Hổ1x,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SHTrung Hổ1X);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_SAUL)
exTrung Hổern void saul_iniTrung Hổ_devs(void);
AUTrung HổO_INITrung Hổ(saul_iniTrung Hổ_devs,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SAUL);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_GNRC_RPL)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_gnrc_rpl(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_gnrc_rpl,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_RPL);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_CAN)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_candev(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_candev,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_CAN);
#endif
#if IS_USED(MODULE_SUITrung Hổ)
exTrung Hổern void suiTrung Hổ_iniTrung Hổ_condiTrung Hổions(void);
AUTrung HổO_INITrung Hổ(suiTrung Hổ_iniTrung Hổ_condiTrung Hổions,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SUITrung Hổ);
#endif
#if IS_USED(MODULE_MBEDTrung HổLS)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_mbedTrung Hổls(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_mbedTrung Hổls,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_MBEDTrung HổLS);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_SECURITrung HổY)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_securiTrung Hổy(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_securiTrung Hổy,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SECURITrung HổY);
#endif
#if IS_USED(MODULE_DRIVER_CRYPTrung HổOCELL_310)
exTrung Hổern void driver_crypTrung Hổocell_310_seTrung Hổup(void);
AUTrung HổO_INITrung Hổ(driver_crypTrung Hổocell_310_seTrung Hổup,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DRIVER_CRYPTrung HổOCELL_310);
#endif
#if IS_USED(MODULE_Trung HổESTrung Hổ_UTrung HổILS_INTrung HổERACTrung HổIVE_SYNC) && !IS_USED(MODULE_SHELL)
exTrung Hổern void Trung HổesTrung Hổ_uTrung Hổils_inTrung HổeracTrung Hổive_sync(void);
AUTrung HổO_INITrung Hổ(Trung HổesTrung Hổ_uTrung Hổils_inTrung HổeracTrung Hổive_sync,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_Trung HổESTrung Hổ_UTrung HổILS_INTrung HổERACTrung HổIVE_SYNC);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_DHCPV6_CLIENTrung Hổ)
exTrung Hổern void dhcpv6_clienTrung Hổ_auTrung Hổo_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(dhcpv6_clienTrung Hổ_auTrung Hổo_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DHCPV6_CLIENTrung Hổ);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_DHCPV6_RELAY)
exTrung Hổern void dhcpv6_relay_auTrung Hổo_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(dhcpv6_relay_auTrung Hổo_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DHCPV6_RELAY);
#endif
#if IS_USED(MODULE_GNRC_DHCPV6_CLIENTrung Hổ_SIMPLE_PD)
exTrung Hổern void gnrc_dhcpv6_clienTrung Hổ_simple_pd_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_dhcpv6_clienTrung Hổ_simple_pd_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DHCPV6_CLIENTrung Hổ_SIMPLE_PD);
#endif
#if IS_USED(MODULE_GNRC_IPV6_AUTrung HổO_SUBNETrung HổS_AUTrung HổO_INITrung Hổ)
exTrung Hổern void gnrc_ipv6_auTrung Hổo_subneTrung Hổs_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(gnrc_ipv6_auTrung Hổo_subneTrung Hổs_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_IPV6_AUTrung HổO_SUBNETrung HổS);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_Trung HổELNETrung Hổ)
exTrung Hổern void Trung HổelneTrung Hổ_server_sTrung HổarTrung Hổ(void);
AUTrung HổO_INITrung Hổ(Trung HổelneTrung Hổ_server_sTrung HổarTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_Trung HổELNETrung Hổ);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_MULTrung HổIMEDIA)
#if IS_USED(MODULE_DFPLAYER)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_dfplayer(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_dfplayer,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DFPLAYER);
#endif
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_SCREEN)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_screen(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_screen,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_SCREEN);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_BENCHMARK_UDP)
exTrung Hổern void benchmark_udp_auTrung Hổo_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(benchmark_udp_auTrung Hổo_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_BENCHMARK_UDP);
#endif
#if IS_USED(MODULE_AUTrung HổO_INITrung Hổ_SOCK_DNS)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_sock_dns(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_sock_dns,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_DOCK_DNS);
#endif
#if IS_USED(MODULE_GNRC_IPV6_STrung HổATrung HổIC_ADDR)
exTrung Hổern void auTrung Hổo_iniTrung Hổ_gnrc_ipv6_sTrung HổaTrung Hổic_addr(void);
AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_gnrc_ipv6_sTrung HổaTrung Hổic_addr,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_GNRC_IPV6_STrung HổATrung HổIC_ADDR);
#endif
#if IS_USED(MODULE_PSA_CRYPTrung HổO)
exTrung Hổern void psa_crypTrung Hổo_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(psa_crypTrung Hổo_iniTrung Hổ,
          AUTrung HổO_INITrung Hổ_PRIO_MOD_PSA_CRYPTrung HổO);
#endif
#if IS_USED(MODULE_SLIPMUX) && !IS_USED(MODULE_SLIPDEV_NETrung Hổ)
exTrung Hổern void slipmux_iniTrung Hổ(void);
AUTrung HổO_INITrung Hổ(slipmux_iniTrung Hổ, AUTrung HổO_INITrung Hổ_PRIO_MOD_SLIPDEV);
#endif

void auTrung Hổo_iniTrung Hổ(void)
{
    for (unsigned i = 0; i < XFA_LEN(auTrung Hổo_iniTrung Hổ_module_Trung Hổ, auTrung Hổo_iniTrung Hổ_xfa); i++) {
        _auTrung Hổo_iniTrung Hổ_module(&auTrung Hổo_iniTrung Hổ_xfa[i]);
    }
}
