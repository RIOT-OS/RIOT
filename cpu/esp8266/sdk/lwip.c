/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       lwIP functions required as symbols by the SDK
 *
 * This file defines a number of lwIP functions that are required as symbols by
 * Espressif's SDK libraries. Since RIOT doesn't need lwIP, these functions are
 * only dummies without real functionality. Using these functions instead of
 * the lwIP functions as provided with the SDK saves arround 4 kBytes of RAM.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifdef MODULE_ESP_SDK

#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcp_impl.h"
#include "netif/etharp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "log.h"

#ifndef ERR_OK
#define ERR_OK 0
#endif

const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};

err_t ethernet_input(struct pbuf *pb, struct netif* netif)
{
    DEBUG("%s\n", __func__);
    (void)pb;
    (void)netif;
    return ERR_OK;
}

err_t etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    (void)q;
    (void)ipaddr;
    return ERR_OK;
}

err_t etharp_request(struct netif *netif, ip_addr_t *ipaddr)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    (void)ipaddr;
    return ERR_OK;
}

void etharp_tmr(void)
{
    DEBUG("%s\n", __func__);
}

void etharp_cleanup_netif(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
}

void dhcp_cleanup(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
}

err_t dhcp_start(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    return ERR_OK;
}

err_t dhcp_renew(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    return ERR_OK;
}

err_t dhcp_release(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    return ERR_OK;
}

void dhcp_stop(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
}

void dhcp_network_changed(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
}

void dhcp_coarse_tmr(void)
{
    DEBUG("%s\n", __func__);
}

void dhcp_fine_tmr(void)
{
    DEBUG("%s\n", __func__);
}

void dhcps_start(struct ip_info *info)
{
    DEBUG("%s\n", __func__);
    (void)info;
}

void dhcps_stop(void)
{
    DEBUG("%s\n", __func__);
}

void dhcps_coarse_tmr(void)
{
    DEBUG("%s\n", __func__);
}

union tcp_listen_pcbs_t tcp_listen_pcbs;
struct tcp_pcb *tcp_active_pcbs;
struct tcp_pcb *tcp_tw_pcbs;

void tcp_seg_free(struct tcp_seg *seg)
{
    DEBUG("%s\n", __func__);
    (void)seg;
}

void tcp_abort (struct tcp_pcb *pcb)
{
    DEBUG("%s\n", __func__);
    (void)pcb;
}

void tcp_tmr(void)
{
    DEBUG("%s\n", __func__);
}

void igmp_init(void)
{
    DEBUG("%s\n", __func__);
}

err_t igmp_start(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    return ERR_OK;
}

err_t igmp_stop(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
    return ERR_OK;
}

void igmp_report_groups(struct netif *netif)
{
    DEBUG("%s\n", __func__);
    (void)netif;
}

void igmp_tmr(void)
{
    DEBUG("%s\n", __func__);
}

void dns_init(void)
{
    DEBUG("%s\n", __func__);
}

void dns_tmr(void)
{
    DEBUG("%s\n", __func__);
}

uint32_t espconn_init(uint32 arg)
{
    DEBUG("%s\n", __func__);
    (void)arg;
    return 1;
}

#endif /* MODULE_ESP_SDK */
