/*
 * Copyright (C) 2015 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ppp gnrc ppp definitions
 * @ingroup     net_gnrc_ppp
 * @{
 *
 * @file
 * @brief  Definitions and interface of gnrc ppp
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#ifndef GNRC_PPP_H_
#define GNRC_PPP_H_

#include <inttypes.h>

#include "net/gnrc.h"
#include "net/ppp/hdr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "xtimer.h"
#include "thread.h"
#include "net/gnrc/ppp/opt.h"
#include "net/gnrc/ppp/prot.h"
#include "net/gnrc/ppp/lcp.h"
#include "net/gnrc/ppp/pap.h"
#include "net/gnrc/ppp/ipcp.h"
#include "sys/uio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GNRC_PPP_MSG_QUEUE 64

#define PPP_HDLC_ADDRESS (0xFF) /**< HDLC address field for PPP */
#define PPP_HDLC_CONTROL (0x03) /**< HDLC control field for PPP */


#define AUTH_PAP (1)            /**< Label of PAP authentication */

#define PPP_CONF_REQ (1)        /**< Code of Configure Request packet */
#define PPP_CONF_ACK (2)        /**< Code of Configure Ack packet */
#define PPP_CONF_NAK (3)        /**< Code of Configure NAK packet */
#define PPP_CONF_REJ (4)        /**< Code of Configure Reject packet */
#define PPP_TERM_REQ (5)        /**< Code of Temrminate Request packet */
#define PPP_TERM_ACK (6)        /**< Code of Terminate ACK packet */
#define PPP_CODE_REJ (7)        /**< Code of Code Reject packet */
#define PPP_PROT_REJ (8)        /**< Code of Protocol Reject packet */
#define PPP_ECHO_REQ (9)        /**< Code of Echo Request packet */
#define PPP_ECHO_REP (10)       /**< Code of Echo Reply packet */
#define PPP_DISC_REQ (11)       /**< Code of Discard Request packet */
#define PPP_IDENT (12)          /**< Code of Identification (not used yet) */
#define PPP_TIME_REM (13)       /**< Code of Time Remaining /not used yet) */
#define PPP_UNKNOWN_CODE (0)    /**< Code for Unknown Code packet (internal use)*/

#define BROADCAST_LCP (0xff)    /**< Shortcut to LCP message */
#define BROADCAST_NCP (0xfe)    /**< Broadcast message to al NCP available */


#define GNRC_PPP_MSG_QUEUE_SIZE (20)

#define PPPDEV_MSG_TYPE_EVENT (100)         /**< Messages for PPP device drive */
#define GNRC_PPPDEV_MSG_TYPE_EVENT (101)    /**< Messages for GNRC PPP layer */

#define DCP_MONITOR_INIT_DELAY (15000000)   /**< Time that the monitor should wait after the LCP initiation before monitoring */
#define DCP_MONITOR_TIMEOUT (10000000)      /**< time between LCP Echo request monitoriin */
#define DCP_DEAD_COUNTER (5)                /**< Number of failed LCP Echo Request responses before assuming the ppp device is dead */


/**
 * @brief list of events for gnrc_ppp
 */
typedef enum {
    PPP_LINKUP,         /**< link up event for a protocol */
    PPP_RECV,           /**< protocol received a packet */
    PPP_TIMEOUT,        /**< protocol received a timeout message */
    PPP_LINKDOWN,       /**< link down event for a protocol */
    PPP_UL_STARTED,     /**< upper layer of a protocol started */
    PPP_UL_FINISHED,    /**< upper layer of a protocol finished */
    PPP_MONITOR,        /**< Message for the monitor */
    PPP_LINK_ALIVE,     /**< Message from LCP to DCP indicating the link is alive */
    PPP_DIALUP          /**< Event for starting dialup process */
} ppp_dev_event_t;

/*Deprecated. Not used in gnrc_ppp*/
typedef enum {
    PPP_LINK_DEAD,
    PPP_LINK_ESTABLISHED,
    PPP_AUTHENTICATION,
    PPP_NETWORK,
    PPP_TERMINATION
} ppp_state_t;

typedef struct pppdev_t pppdev_t;

/**
 * @brief data type for handling driver representation of ppp device
 */
typedef struct pppdev_driver_t {
    int (*send)(pppdev_t *dev, const struct iovec *vector, int count);
    int (*recv)(pppdev_t *dev, char *buf, int len, void *info);
    void (*driver_ev)(pppdev_t *dev, uint8_t event);
    int (*init)(pppdev_t *dev);
    int (*set)(pppdev_t *dev, netopt_t opt, void *value, size_t value_len);
    int (*get)(pppdev_t *dev, netopt_t opt, void *value, size_t max_len);
    int (*dial_up)(pppdev_t *dev);
    int (*link_down)(pppdev_t *dev);
} pppdev_driver_t;


/**
 * @brief base class of a ppp device
 */
typedef struct pppdev_t {
    const pppdev_driver_t *driver; /**< pointer to driver representation */
} pppdev_t;


/**
 * @brief class of custom driver control protocol
 * @extends ppp_protocol_t
 *
 * @details the DCP is in charge of monitoring the link and exchanging messages with the ppp device
 */
typedef struct dcp_t {
    ppp_protocol_t prot;    /**< base ppp_protocol_t object */
    msg_t timer_msg;        /**< msg struct for handling timeouts messages */
    xtimer_t xtimer;        /**< xtimer struct for sending timeout messages */
    uint8_t dead_counter;   /**< when reaches zero, the link is assumed to be dead */
} dcp_t;


/**
 * @brief GNRC PPP main struct
 */
typedef struct gnrc_pppdev_t {
    ppp_protocol_t *protocol[NUM_OF_PROTS]; /**< array of PPP sub protocols */
    pppdev_t *netdev;                       /**< pointer to ppp device */
    uint8_t state;                          /**< State of gnrc_ppp. Unused and will be removed */
} gnrc_pppdev_t;



/**
 * @brief setup function for GNRC PPP
 *
 * @param dev pointer to gnrc ppp interface
 * @param pppdev pointer to pppdev interface
 *
 * @return 0 for the moment
 */
int gnrc_ppp_setup(gnrc_pppdev_t *dev, pppdev_t *pppdev);

gnrc_pktsnip_t *pkt_build(gnrc_nettype_t pkt_type, uint8_t code, uint8_t id, gnrc_pktsnip_t *payload);


/**
 * @brief send a PPP packet through an HDLC packet
 *
 * @param dev pointer to gnrc ppp interface
 * @param pkt packet to be sent
 *
 * @return -EBADMSG if the packet is not valid
 * @return 0 otherwise
 */
int gnrc_ppp_send(gnrc_pppdev_t *dev, gnrc_pktsnip_t *pkt);


/**
 * @brief main thread of GNRC PPP
 *
 * @param args arguments for the GNRC PPP thread
 */
void *_gnrc_ppp_thread(void *args);


/**
 * @brief init function for DCP
 *
 * @param ppp_dev pointer to gnrc ppp interface
 * @param dcp pointer to dcp protocol
 *
 * @return 0
 */
int dcp_init(gnrc_pppdev_t *ppp_dev, ppp_protocol_t *dcp);
ppp_protocol_t *dcp_get_static_pointer(void);


/**
 * @brief should be called from the driver when the ppp device is ready to init dial up
 *
 * @param msg pointer to msg structure
 * @param pid pid of driver
 */
void gnrc_ppp_link_up(msg_t *msg, kernel_pid_t pid);


/**
 * @brief should be called from the driver when the ppp device is down
 *
 * @param msg pointer to msg structure
 * @param pid pid of driver
 */
void gnrc_ppp_link_down(msg_t *msg, kernel_pid_t pid);


/**
 * @brief should be called from the driver when finishing the reception of a packet
 *
 * @param msg pointer to msg structure
 * @param pid pid of driver
 */
void gnrc_ppp_dispatch_pkt(msg_t *msg, kernel_pid_t pid);


/**
 * @brief Dial up ppp. This calls dial_up method from driver
 *
 * @param msg pointer to msg structure
 * @param pid pid of GNRC ppp
 */
void gnrc_ppp_dial_up(msg_t *msg, kernel_pid_t pid);

/**
 * @brief Disconnect ppp.
 *
 * @param msg pointer to msg structure
 * @param pid pid of GNRC ppp
 */
void gnrc_ppp_disconnect(msg_t *msg, kernel_pid_t pid);


/**
 * @brief set a net option for GNRC PPP or ppp device
 *
 * @param dev pointer to gnrc ppp interface
 * @param opt option to be set
 * @param value value of the option
 * @param value_len size of the option
 *
 * @return positive number with option result
 * @return negative number if there was an error
 */
int gnrc_ppp_set_opt(gnrc_pppdev_t *dev, netopt_t opt, void *value, size_t value_len);

/**
 * @brief init the GNRC PPP thread
 *
 * @param stack stack for GNRC PPP thread
 * @param stacksize size of the stack
 * @param priority priority of GNRC ppp stack
 * @param name name of the GNRC PPP stack
 * @param gnrc_pppdev pointer to gnrc ppp interface
 *
 * @return pid of GNRC PPP
 */
kernel_pid_t gnrc_pppdev_init(char *stack, int stacksize, char priority,
                              const char *name, gnrc_pppdev_t *gnrc_pppdev);


/**
 * @brief send a configure request packet
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param payload payload of configure request packet
 */
void send_configure_request(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *payload);

/**
 * @brief send a configure ack packet
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param opts ACK'd options
 */
void send_configure_ack(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts);

/**
 * @brief send a configure nak packet
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param opts NAK'd options
 */
void send_configure_nak(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts);

/**
 * @brief send configure reject
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param opts rejected options
 */
void send_configure_rej(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts);

/**
 * @brief
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 */
void send_terminate_req(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id);

/**
 * @brief send terminate ack
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param response response of terminate request
 */
void send_terminate_ack(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *response);

/**
 * @brief send code reject packet
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param rejected rejected packet including ppp header
 */
void send_code_rej(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *rejected);

/**
 * @brief send echo reply
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param data data of the echo reply
 */
void send_echo_reply(gnrc_pppdev_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *data);

/**
 * @brief send protocol reject packet
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param pkt rejected packet
 */
void send_protocol_reject(gnrc_pppdev_t *dev, uint8_t id, gnrc_pktsnip_t *pkt);

/**
 * @brief send PAP auth request
 *
 * @param dev pointer to gnrc ppp interface
 * @param protocol nettype of packet
 * @param id id of packet
 * @param credentials credentials of the PAP request
 */
void send_pap_request(gnrc_pppdev_t *dev, uint8_t id, gnrc_pktsnip_t *credentials);
#ifdef __cplusplus
}
#endif

#endif /* GNRC_PPP_H_ */
/** @} */
