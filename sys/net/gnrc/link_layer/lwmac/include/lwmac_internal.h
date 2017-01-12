/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lwmac Simplest possible MAC layer
 * @ingroup     net
 * @brief       Internal functions if LWMAC
 * @{
 *
 * @file
 * @brief       Interface definition for internal functions of LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 */

#ifndef GNRC_LWMAC_INTERNAL_H_
#define GNRC_LWMAC_INTERNAL_H_

#include <stdint.h>
#include "periph/rtt.h"
#include "lwmac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief   Type to pass information about parsing */
typedef struct {
    lwmac_hdr_t* header;    /**< lwmac header of packet */
    l2_addr_t  src_addr;    /**< copied source address of packet  */
    l2_addr_t  dst_addr;    /**< copied destination address of packet */
} lwmac_packet_info_t;

/* @brief   Next RTT event must be at least this far in the future
 *
 * When setting an RTT alarm to short in the future it could be possible that
 * the counter already passed the calculated alarm before it could be set. This
 * margin will be applied when using `_next_inphase_event()`.
 */
#define LWMAC_RTT_EVENT_MARGIN_TICKS    ( RTT_MS_TO_TICKS(2) )

/* @brief Extract the destination address out of an GNRC_NETTYPE_NETIF pktsnip
 *
 * @param[in]   pkt                 pktsnip from whom to extract
 * @param[out]  pointer_to_addr     pointer to address will be stored here
 *
 * @return                          length of destination address
 */
int _get_dest_address(gnrc_pktsnip_t* pkt, uint8_t* pointer_to_addr[]);

/* @brief Find the first pktsnip of @p type
 *
 * Will search linearly through the packet buffer @p pkt and yield
 * gnrc_pktsnip_t::data of the first pktsnip match the type @p type.
 *
 * @param[in]   pkt     pktsnip that will be searched
 * @param[in]   type    type to search for
 *
 * @return              pointer to data, NULL is not found
 */
void* _gnrc_pktbuf_find(gnrc_pktsnip_t* pkt, gnrc_nettype_t type);

/* @brief Parse an incoming packet and extract important information
 *
 * Copies addresses into @p info, but header points inside @p pkt.
 *
 * @param[in]   pkt             packet that will be parsed
 * @param[out]  info            structure that will hold parsed information
 *
 * @return                      0 if correctly parsed
 * @return                      <0 on error
 */
int _parse_packet(gnrc_pktsnip_t* pkt, lwmac_packet_info_t* info);


/* @brief Shortcut to get the state of netdev
 *
 * @param[in]   lwmac           lwmac state that stores netdev pointer
 *
 * @return                      state of netdev
 */
netopt_state_t _get_netdev_state(lwmac_t* lwmac);

/* @brief Shortcut to set the state of netdev
 *
 * @param[in]   lwmac           lwmac state that stores netdev pointer
 * @param[in]   devstate        new state for netdev
 */
void _set_netdev_state(lwmac_t* lwmac, netopt_state_t devstate);

/* @brief Check if packet is broadcast
 *
 * @param[in]   pkt             packet to check
 */
static inline bool _packet_is_broadcast(gnrc_pktsnip_t* pkt)
{
    gnrc_netif_hdr_t* netif_hdr = _gnrc_pktbuf_find(pkt, GNRC_NETTYPE_NETIF);
    return ( (netif_hdr == NULL) ? false :
                              (netif_hdr->flags & (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) );
}

/* TX queue handling */
int _find_neighbour(lwmac_t* lwmac, uint8_t* dst_addr, int addr_len);
int _free_neighbour(lwmac_t* lwmac);
int _alloc_neighbour(lwmac_t* lwmac);
void _init_neighbour(lwmac_tx_neighbour_t* neighbour, uint8_t* addr, int len);

static inline lwmac_tx_neighbour_t* _get_neighbour(lwmac_t* lwmac, unsigned int id)
{
    return &(lwmac->tx.neighbours[id]);
}

/* RTT phase calculation */
uint32_t _ticks_to_phase(uint32_t ticks);
uint32_t _phase_to_ticks(uint32_t phase);
uint32_t _phase_now(void);
uint32_t _ticks_until_phase(uint32_t phase);

lwmac_tx_neighbour_t* _next_tx_neighbour(lwmac_t* lwmac);
int _time_until_tx_us(lwmac_t* lwmac);
bool _queue_tx_packet(lwmac_t* lwmac,  gnrc_pktsnip_t* pkt);
uint32_t _next_inphase_event(uint32_t last, uint32_t interval);

int _dispatch_defer(gnrc_pktsnip_t* buffer[], gnrc_pktsnip_t* pkt);

void _dispatch(gnrc_pktsnip_t* buffer[]);


static inline bool _addr_match(l2_addr_t* addr1, l2_addr_t* addr2)
{
    assert(addr1);
    assert(addr2);

    if(addr1->len != addr2->len)
        return false;

    return (memcmp(addr1->addr, addr2->addr, addr1->len) == 0);
}

#ifdef __cplusplus
}
#endif

#endif /* GNRC_LWMAC_INTERNAL_H_ */
/** @} */
