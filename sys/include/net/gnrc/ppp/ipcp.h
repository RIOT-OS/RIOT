/*
 * Copyright (C) 2016 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ppp Definitions of IPCP
 * @ingroup     net_gnrc_ppp
 * @{
 *
 * @file
 * @brief  Definitions and configuration of Internet Protocol Control Protocol
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef PPP_IPCP_H_
#define PPP_IPCP_H_

#include "net/gnrc/ppp/fsm.h"
#include "net/ipv4/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPCP_OPT_IP_ADDRESS (3)         /**< code of IP Address option */
#define IPCP_RESTART_TIMER (3000000U)   /**< restart time value for IPCP */


/**
 * @brief IPCP options
 */
typedef enum {
    IPCP_IPADDRESS, /**< IP address option */
    IPCP_NUMOPTS
} ipcp_options_t;

/**
 * @brief definition of IPCP protocol
 * @extends ppp_fsm_t
 */
typedef struct ipcp_t {
    ppp_fsm_t fsm;                      /**< base fsm class */
    ipv4_addr_t local_ip;               /**< local ip address obtained from ppp device */
    ipv4_addr_t ip;                     /**< ip address of ppp device */
    fsm_conf_t ipcp_opts[IPCP_NUMOPTS]; /**< configuration options for IPCP */
    int ip_id;                          /**< id of ip packet */
} ipcp_t;

/**
 * @brief definition of the PPP IPv4 encapsulator
 * @extends ppp_protocol_t
 *
 * @details since most mobile operators don't support IPv6, it's necessary to use a tunnel for transmitting data
 */
typedef struct ppp_ipv4_t {
    ppp_protocol_t prot;        /**< base ppp_protocol class */
    ipv4_addr_t tunnel_addr;    /**< IPv4 address of tunnel */
    uint16_t tunnel_port;       /**< UDP port of tunnel */
} ppp_ipv4_t;

struct ppp_fsm_t;

/**
 * @brief init function of IPCP
 *
 * @param ppp_dev pointer to gnrc ppp interface
 * @param ipcp pointer to ipcp protocol
 *
 * @return 0
 */
int ipcp_init(gnrc_netdev2_t *ppp_dev);


/**
 * @brief init function for ipv4
 *
 * @param ppp_dev pointer to gnrc ppp interface
 * @param ipv4 pointer to ppp ipv4 protocol
 * @param ipcp pointer to ipcp protocol
 *
 * @return 0
 */
int ppp_ipv4_init(gnrc_netdev2_t *ppp_dev);

/**
 * @brief send an encapsulated pkt
 *
 * @param ppp_dev pointer to to gnrc ppp interface
 * @param pkt packet to be sent
 *
 * @return negative value if there was an error
 * @return 0 otherwise
 */
int ppp_ipv4_send(gnrc_netdev2_t *ppp_dev, gnrc_pktsnip_t *pkt);


/**
 * @brief receive an encapsulated ipv4 packet for decapsulation
 *
 * @param ppp_dev pointer to gnrc ppp interface
 * @param pkt encapsulated packet
 *
 * @return
 */
gnrc_pktsnip_t *ppp_ipv4_recv(gnrc_netdev2_t *ppp_dev, gnrc_pktsnip_t *pkt);


/**
 * @brief get pointer of IPCP protocol
 *
 * @return pointer to IPCP protocol
 */
ppp_protocol_t *ipcp_get_static_pointer(void);



/**
 * @brief get pointer of PPP IPv4 protocol
 *
 * @return pointer to PPP IPv4 protocol
 */
ppp_protocol_t *ipv4_get_static_pointer(void);

#ifdef __cplusplus
}
#endif

#endif /* PPP_IPCP_H_ */
/** @} */
