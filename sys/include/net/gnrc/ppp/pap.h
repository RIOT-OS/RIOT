/*
 * Copyright (C) 2016 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ppp Definitions of PAP authentication
 * @ingroup     net_gnrc_ppp
 * @{
 *
 * @file
 * @brief  Definitions and configuration of Password Authentication Protocol
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef GNRC_PPP_PAP_H
#define GNRC_PPP_PAP_H

#include "net/gnrc/ppp/prot.h"
#include "xtimer.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief definition of PAP protocol
 * @extends ppp_protocol_t
 */
typedef struct gnrc_ppp_pap {
    gnrc_ppp_protocol_t prot;    /**< base ppp_protocol class */
    char username[20];      /**< user name of PAP */
    size_t user_size;       /**< user size */
    char password[20];      /**< password of PAP */
    size_t pass_size;       /**< password size */
    uint8_t counter;        /**< attempts counter of PAP */
    uint8_t id;             /**< last configure request id of PAP packet */
    xtimer_t xtimer;        /**< xtimer structure for timeouts */
    msg_t timer_msg;        /**< msg structure for timeout messages */
} gnrc_ppp_pap_t;

/**
 * @brief init PAP procotol
 *
 * @param ppp_dev pointer to gnrc ppp interface
 * @param pap pointer to pap protocol
 *
 * @return 0
 */
int pap_init(gnrc_netdev2_t *ppp_dev);
gnrc_ppp_protocol_t *pap_get_static_pointer(void);
void pap_recv(gnrc_ppp_protocol_t *protocol, gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif
#endif /* GNRC_PPP_PAP_H */
/**
 * @}
 */
