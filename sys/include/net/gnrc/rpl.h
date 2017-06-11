/*
 * Copyright (C) 2013 - 2014  INRIA.
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_rpl  RPL
 * @ingroup     net_gnrc
 * @brief       RPL implementation for GNRC
 *
 * Configuration
 * =============
 *
 * USEMODULE
 * ---------
 *
 * - RPL (Storing Mode)
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += gnrc_rpl
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - RPL auto-initialization on interface
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   USEMODULE += auto_init_gnrc_rpl
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Auto-Initialization
 * -------------------
 *
 * If the application defines only one interface (`GNRC_NETIF_NUMOF == 1`),
 * then RPL will be initialized on this interface.
 *
 * If the application defines several interfaces (`GNRC_NETIF_NUMOF > 1`),
 * then RPL will be initialized on the interface `GNRC_RPL_DEFAULT_NETIF`.
 * Your application is responsible for setting `GNRC_RPL_DEFAULT_NETIF` to a
 * valid interface PID, e.g. via `CFLAGS`.
 *
 * Initializing RPL on multiple interfaces automatically is currently not supported.
 * Call `gnrc_rpl_init()` manually from your application for the desired interfaces in this case.
 *
 * CFLAGS
 * ------
 *
 * - Exclude Prefix Information Options from DIOs
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   CFLAGS += -DGNRC_RPL_WITHOUT_PIO
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - Modify trickle parameters
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   CFLAGS += -DGNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS=20
 *   CFLAGS += -DGNRC_RPL_DEFAULT_DIO_INTERVAL_MIN=3
 *   CFLAGS += -DGNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT=10
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - Make reception of DODAG_CONF optional when joining a DODAG.
 *   This will use the default trickle parameters until a
 *   DODAG_CONF is received from the parent. The DODAG_CONF is
 *   requested once from the parent while joining the DODAG.
 *   The standard behaviour is to request a DODAG_CONF and join
 *   only a DODAG once a DODAG_CONF is received.
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   CFLAGS += -DGNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - Set interface for auto-initialization if more than one
 *   interface exists (`GNRC_NETIF_NUMOF > 1`)
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   CFLAGS += -DGNRC_RPL_DEFAULT_NETIF=6
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - By default, all incoming control messages get checked for validation.
 *   This validation can be disabled in case the involved RPL implementations
 *   are known to produce valid messages.
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *   CFLAGS += -DGNRC_RPL_WITHOUT_VALIDATION
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       RPL header. Declaration of global variables and functions needed for
 *              core functionality of RPL.
 *
 * Header which includes all core RPL-functions.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#ifndef NET_GNRC_RPL_H
#define NET_GNRC_RPL_H

#include <string.h>
#include <stdint.h>
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/of_manager.h"
#include "net/fib.h"
#include "xtimer.h"
#include "trickle.h"

#ifdef MODULE_NETSTATS_RPL
#include "net/rpl/rpl_netstats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the RPL thread
 */
#ifndef GNRC_RPL_STACK_SIZE
#define GNRC_RPL_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the RPL thread
 */
#ifndef GNRC_RPL_PRIO
#define GNRC_RPL_PRIO           (GNRC_IPV6_PRIO + 1)
#endif

/**
 * @brief   Default message queue size to use for the RPL thread.
 */
#ifndef GNRC_RPL_MSG_QUEUE_SIZE
#define GNRC_RPL_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief   Static initializer for the all-RPL-nodes multicast IPv6
 *          address (ff02::1a)
 *
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6">
 *          RFC 6550, section 6
 *      </a>
 */
#define GNRC_RPL_ALL_NODES_ADDR {{ 0xff, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1a }}

/**
 * @brief   Message type for lifetime updates
 */
#define GNRC_RPL_MSG_TYPE_LIFETIME_UPDATE     (0x0900)

/**
 * @brief   Message type for trickle
 */
#define GNRC_RPL_MSG_TYPE_TRICKLE_MSG         (0x0901)

/**
 * @brief   Message type for handling DAO sending
 */
#define GNRC_RPL_MSG_TYPE_DAO_HANDLE  (0x0903)

/**
 * @brief   Infinite rank
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-17">
 *          RFC 6550, section 17
 *      </a>
 */
#define GNRC_RPL_INFINITE_RANK (0xFFFF)

/**
 * @brief   Default minimum hop rank increase
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-17">
 *          RFC 6550, section 17
 *      </a>
 */
#ifndef GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE
#define GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE (256)
#endif

/**
 * @brief   Maximum rank increase
 */
#ifndef GNRC_RPL_DEFAULT_MAX_RANK_INCREASE
#define GNRC_RPL_DEFAULT_MAX_RANK_INCREASE (0)
#endif

/**
 * @brief   Number of implemented Objective Functions
 */
#define GNRC_RPL_IMPLEMENTED_OFS_NUMOF (1)

/**
 * @brief   Default Objective Code Point (OF0)
 */
#define GNRC_RPL_DEFAULT_OCP (0)

/**
 * @brief   Default Instance ID
 */
#ifndef GNRC_RPL_DEFAULT_INSTANCE
#define GNRC_RPL_DEFAULT_INSTANCE (0)
#endif

/**
 * @name RPL Mode of Operations
 * @{
 */
#define GNRC_RPL_MOP_NO_DOWNWARD_ROUTES  (0x00)
#define GNRC_RPL_MOP_NON_STORING_MODE    (0x01)
#define GNRC_RPL_MOP_STORING_MODE_NO_MC  (0x02)
#define GNRC_RPL_MOP_STORING_MODE_MC     (0x03)

/** default MOP set on compile time */
#ifndef GNRC_RPL_DEFAULT_MOP
#define GNRC_RPL_DEFAULT_MOP GNRC_RPL_MOP_STORING_MODE_NO_MC
#endif
/** @} */

/**
 * @name Sequence counter handling
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-7.2">
 *          Sequence Counter Operation
 *      </a>
 * @{
 */
#define GNRC_RPL_COUNTER_MAX                 (255)
#define GNRC_RPL_COUNTER_LOWER_REGION        (127)
#define GNRC_RPL_COUNTER_SEQ_WINDOW          (16)
#define GNRC_RPL_COUNTER_INIT                (GNRC_RPL_COUNTER_MAX - GNRC_RPL_COUNTER_SEQ_WINDOW + 1)

static inline uint8_t GNRC_RPL_COUNTER_INCREMENT(uint8_t counter)
{
    return ((counter > GNRC_RPL_COUNTER_LOWER_REGION) ?
            ((counter == GNRC_RPL_COUNTER_MAX) ? counter = 0 : ++counter) :
            ((counter == GNRC_RPL_COUNTER_LOWER_REGION) ? counter = 0 : ++counter));
}

static inline bool GNRC_RPL_COUNTER_IS_INIT(uint8_t counter)
{
    return (counter > GNRC_RPL_COUNTER_LOWER_REGION);
}

static inline bool GNRC_RPL_COUNTER_GREATER_THAN_LOCAL(uint8_t A, uint8_t B)
{
    return (((A < B) && (GNRC_RPL_COUNTER_LOWER_REGION + 1 - B + A < GNRC_RPL_COUNTER_SEQ_WINDOW))
            || ((A > B) && (A - B < GNRC_RPL_COUNTER_SEQ_WINDOW)));
}

static inline bool GNRC_RPL_COUNTER_GREATER_THAN(uint8_t A, uint8_t B)
{
    return ((A > GNRC_RPL_COUNTER_LOWER_REGION) ? ((B > GNRC_RPL_COUNTER_LOWER_REGION) ?
                GNRC_RPL_COUNTER_GREATER_THAN_LOCAL(A, B) : 0) :
            ((B > GNRC_RPL_COUNTER_LOWER_REGION) ? 1 : GNRC_RPL_COUNTER_GREATER_THAN_LOCAL(A, B)));
}
/** @} */

/**
 * @name Trickle parameters
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-8.3.1">
 *          Trickle Parameters
 *      </a>
 * @{
 */
#ifndef GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS
#define GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS (20)
#endif

#ifndef GNRC_RPL_DEFAULT_DIO_INTERVAL_MIN
#define GNRC_RPL_DEFAULT_DIO_INTERVAL_MIN (3)
#endif

#ifndef GNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT
#define GNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT (10)
#endif
/** @} */

/**
 * @name Default parent and route entry lifetime
 * default lifetime will be multiplied by the lifetime unit to obtain the resulting lifetime
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.6">
            DODAG Configuration
        </a>
 * @{
 */
#ifndef GNRC_RPL_DEFAULT_LIFETIME
#define GNRC_RPL_DEFAULT_LIFETIME (5)
#endif
#ifndef GNRC_RPL_LIFETIME_UNIT
#define GNRC_RPL_LIFETIME_UNIT (60)
#endif
/** @} */

/**
 * @brief Default prefix length for the DODAG id
 */
#define GNRC_RPL_DEFAULT_PREFIX_LEN   (64)

/**
 * @brief Default prefix valid and preferred time for the DODAG id
 * @note Currently not used, but needed for RIOs
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.5">
 *          Route Information
 *      </a>
 */
#define GNRC_RPL_DEFAULT_PREFIX_LIFETIME  (0xFFFFFFFF)

/**
 * @brief A DODAG can be grounded or floating
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-3.2.4">
 *          Grounded and Floating DODAGs
 *      </a>
 */
#define GNRC_RPL_GROUNDED (1)

/**
 * @name Parameters used for DAO handling
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-17">
            RPL Constants and Variables
        </a>
 * @{
 */
#ifndef GNRC_RPL_DAO_SEND_RETRIES
#define GNRC_RPL_DAO_SEND_RETRIES (4)
#endif
#ifndef GNRC_RPL_DEFAULT_WAIT_FOR_DAO_ACK
#define GNRC_RPL_DEFAULT_WAIT_FOR_DAO_ACK (3)
#endif
#ifndef GNRC_RPL_REGULAR_DAO_INTERVAL
#define GNRC_RPL_REGULAR_DAO_INTERVAL (60)
#endif
#ifndef GNRC_RPL_DEFAULT_DAO_DELAY
#define GNRC_RPL_DEFAULT_DAO_DELAY (1)
#endif
/** @} */

/**
 * @brief Cleanup timeout in seconds
 */
#define GNRC_RPL_CLEANUP_TIME (5)

/**
 * @name Node Status
 * @{
 */
#define GNRC_RPL_NORMAL_NODE (0)
#define GNRC_RPL_ROOT_NODE (1)
#define GNRC_RPL_LEAF_NODE (2)
/** @} */


/**
 * @name RPL Control Message Options
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7">
 *          RPL Control Message Options
 *      </a>
 * @{
 */
#define GNRC_RPL_OPT_PAD1                 (0)
#define GNRC_RPL_OPT_PADN                 (1)
#define GNRC_RPL_OPT_DAG_METRIC_CONTAINER (2)
#define GNRC_RPL_OPT_ROUTE_INFO           (3)
#define GNRC_RPL_OPT_DODAG_CONF           (4)
#define GNRC_RPL_OPT_TARGET               (5)
#define GNRC_RPL_OPT_TRANSIT              (6)
#define GNRC_RPL_OPT_SOLICITED_INFO       (7)
#define GNRC_RPL_OPT_PREFIX_INFO          (8)
#define GNRC_RPL_OPT_TARGET_DESC          (9)
/** @} */

/**
 * @brief Rank of the root node
 */
#define GNRC_RPL_ROOT_RANK (GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE)

/**
 *  @brief  DIS ICMPv6 code
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-6">
 *          RFC 6550, section 6
 *      </a>
 */
#define GNRC_RPL_ICMPV6_CODE_DIS (0x00)

/**
 *  @brief  DIO ICMPv6 code
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-6">
 *          RFC 6550, section 6
 *      </a>
 */
#define GNRC_RPL_ICMPV6_CODE_DIO (0x01)

/**
 *  @brief  DAO ICMPv6 code
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-6">
 *          RFC 6550, section 6
 *      </a>
 */
#define GNRC_RPL_ICMPV6_CODE_DAO (0x02)

/**
 *  @brief  DAO ACK ICMPv6 code
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-6">
 *          RFC 6550, section 6
 *      </a>
 */
#define GNRC_RPL_ICMPV6_CODE_DAO_ACK (0x03)

/**
 * @brief Update interval of the lifetime update function
 */
#define GNRC_RPL_LIFETIME_UPDATE_STEP (2)

/**
 *  @brief Rank part of the DODAG
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-3.5.1">
 *          RFC 6550, section 3.5.1, Rank Comparison (DAGRank())
 *      </a>
 */
#define DAGRANK(rank,mhri)   (rank/mhri)

/**
 *  @name   Global / Local instance id masks
 *  @see <a href="https://tools.ietf.org/html/rfc6550#section-5.1">
 *          RFC 6550, section 5.1, RPL Instance ID
 *      </a>
 *  @{
 */
#define GNRC_RPL_INSTANCE_ID_MSB      (1 << 7)
#define GNRC_RPL_GLOBAL_INSTANCE_MASK (0x7F)
#define GNRC_RPL_LOCAL_INSTANCE_MASK  (0x3F)
#define GNRC_RPL_INSTANCE_D_FLAG_MASK (1 << 6)
/** @} */

/**
 * @brief PID of the RPL thread.
 */
extern kernel_pid_t gnrc_rpl_pid;

/**
 * @brief @see @ref GNRC_RPL_ALL_NODES_ADDR
 */
extern const ipv6_addr_t ipv6_addr_all_rpl_nodes;

#ifdef MODULE_NETSTATS_RPL
/**
 * @brief Statistics for RPL control messages
 */
extern netstats_rpl_t gnrc_rpl_netstats;
#endif

/**
 * @brief Initialization of the RPL thread.
 *
 * @param[in] if_pid            PID of the interface
 *
 * @return  The PID of the RPL thread, on success.
 * @return  a negative errno on error.
 */
kernel_pid_t gnrc_rpl_init(kernel_pid_t if_pid);

/**
 * @brief Initialization of a node as root.
 *
 * @param[in] instance_id       Id of the instance
 * @param[in] dodag_id          Id of the DODAG
 * @param[in] gen_inst_id       Flag indicating whether to generate an instance id.
 *                              If true, @p instance_id will be ignored
 * @param[in] local_inst_id     Flag indicating whether a local or global instance id
 *                              should be generatad
 *
 * @return  Pointer to the new RPL Instance, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_instance_t *gnrc_rpl_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id,
                                        bool gen_inst_id, bool local_inst_id);

/**
 * @brief   Send a DIO of the @p instance to the @p destination.
 *
 * @param[in] instance          Pointer to the RPL instance.
 * @param[in] destination       IPv6 addres of the destination.
 */
void gnrc_rpl_send_DIO(gnrc_rpl_instance_t *instance, ipv6_addr_t *destination);

/**
 * @brief   Send a DIS of the @p instace to the @p destination.
 *
 * @param[in] instance          Pointer to the RPL instance, optional.
 * @param[in] destination       IPv6 addres of the destination.
 */
void gnrc_rpl_send_DIS(gnrc_rpl_instance_t *instance, ipv6_addr_t *destination);

/**
 * @brief   Send a DAO of the @p dodag to the @p destination.
 *
 * @param[in] instance          Pointer to the instance.
 * @param[in] destination       IPv6 addres of the destination.
 * @param[in] lifetime          Lifetime of the route to announce.
 */
void gnrc_rpl_send_DAO(gnrc_rpl_instance_t *instance, ipv6_addr_t *destination, uint8_t lifetime);

/**
 * @brief   Send a DAO-ACK of the @p instance to the @p destination.
 *
 * @param[in] instance          Pointer to the RPL instance.
 * @param[in] destination       IPv6 addres of the destination.
 * @param[in] seq				Sequence number to be acknowledged.
 */
void gnrc_rpl_send_DAO_ACK(gnrc_rpl_instance_t *instance, ipv6_addr_t *destination, uint8_t seq);

/**
 * @brief   Parse a DIS.
 *
 * @param[in] dis       Pointer to the DIS message.
 * @param[in] iface     Interface PID of the incoming DIS.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] dst       Pointer to the destination address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DIS(gnrc_rpl_dis_t *dis, kernel_pid_t iface, ipv6_addr_t *src,
                       ipv6_addr_t *dst, uint16_t len);

/**
 * @brief   Parse a DIO.
 *
 * @param[in] dio       Pointer to the DIO message.
 * @param[in] iface     Interface PID of the incoming DIO.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] dst       Pointer to the destination address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DIO(gnrc_rpl_dio_t *dio, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                       uint16_t len);

/**
 * @brief   Parse a DAO.
 *
 * @param[in] dao       Pointer to the DAO message.
 * @param[in] iface     Interface PID of the incoming DIO.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] dst       Pointer to the destination address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DAO(gnrc_rpl_dao_t *dao, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                       uint16_t len);

/**
 * @brief   Parse a DAO-ACK.
 *
 * @param[in] dao_ack   Pointer to the DAO-ACK message.
 * @param[in] iface     Interface PID of the incoming DIO.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] dst       Pointer to the destination address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack, kernel_pid_t iface, ipv6_addr_t *src,
                           ipv6_addr_t *dst, uint16_t len);

/**
 * @brief   Delay the DAO sending interval
 *
 * @param[in] dodag     The DODAG of the DAO
 */
void gnrc_rpl_delay_dao(gnrc_rpl_dodag_t *dodag);

/**
 * @brief   Long delay the DAO sending interval
 *
 * @param[in] dodag     The DODAG of the DAO
 */
void gnrc_rpl_long_delay_dao(gnrc_rpl_dodag_t *dodag);

/**
 * @brief Create a new RPL instance and RPL DODAG.
 *
 * @param[in] instance_id       Id of the instance
 * @param[in] dodag_id          Id of the DODAG
 * @param[in] mop               Mode of Operation
 *
 * @return  Pointer to the new RPL instance, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_instance_t *gnrc_rpl_root_instance_init(uint8_t instance_id, ipv6_addr_t *dodag_id,
                                                 uint8_t mop);

/**
 * @brief Send a control message
 *
 * @param[in] pkt               gnrc_pktnsip_t to send
 * @param[in] iface             Interface PID to send this pkt to
 * @param[in] src               Source address of the packet
 * @param[in] dst               Destination address of the packet
 * @param[in] dodag_id          Id of the DODAG
 */
void gnrc_rpl_send(gnrc_pktsnip_t *pkt, kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                   ipv6_addr_t *dodag_id);

/**
 * @brief Generate a local or global instance id
 *
 * @param[in] local             flag to indicate whether a local or global instance id is requested
 *
 * @return  Local instance id, if @p local is true
 * @return  Global instance id, otherwise.
 */
uint8_t gnrc_rpl_gen_instance_id(bool local);

#ifndef GNRC_RPL_WITHOUT_PIO
/**
 * @brief (De-)Activate the transmission of Prefix Information Options within DIOs
 *        for a particular DODAG
 *
 * @param[in] dodag             Pointer to the DODAG
 * @param[in] status            true for activating PIOs and false for deactivating them
 */
static inline void gnrc_rpl_config_pio(gnrc_rpl_dodag_t *dodag, bool status)
{
    dodag->dio_opts = (dodag->dio_opts & ~GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO) |
                      (status << GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO_SHIFT);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_RPL_H */
/** @} */
