#ifndef NET_GNRC_IPV6_ESP_H
#define NET_GNRC_IPV6_ESP_H

#include <stdbool.h>

#include "sched.h"
#include "net/gnrc/pkt.h"

#include "net/gnrc/ipv6/ipsec/config.h"
#include "net/gnrc/ipv6/ipsec/ipsec_db.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data type to represent an ESP packet header.
 */
typedef struct __attribute__((packed)) {
    network_uint32_t spi;
    network_uint32_t sn;
} ipv6_esp_hdr_t;

/**
 * @brief Data type to represent an ESP packet trailer.
 */
typedef struct __attribute__((packed)) {
    uint8_t pl;
    uint8_t nh;
} ipv6_esp_trl_t;

/**
 * @brief   Marks, Decrypts and returns pkt at next header. If the ipsec rules
             dictate tunnel mode, packet is consumed and processed.
 *
 * @param[in] pktsnip at ESP EXT header
 *
 * @return  processed ESP pkt at next header poisition
 * @return  NULL on tunnel mode
 */
gnrc_pktsnip_t *esp_header_process(gnrc_pktsnip_t *pkt, uint8_t protnum);

/**
 * @brief   Build ESP header for sending
 *
 * @param[in] pkt   head after IPv6 header build
 * @param[in] sa_entry   Database pointer to according Security
 *            Association (SA) entry
 * @param[in] ts   Pointer to Traffic Selector (TS) of pkt
 *
 * @return  pktsnip at IPv6 with ESP
 */
gnrc_pktsnip_t *esp_header_build(gnrc_pktsnip_t *pkt,
                                 const ipsec_sa_t *sa_entry,
                                 ipsec_ts_t *ts);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_ESP_H */
