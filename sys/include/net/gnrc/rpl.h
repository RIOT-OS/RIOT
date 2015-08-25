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

#ifndef GNRC_RPL_H_
#define GNRC_RPL_H_

#include <string.h>
#include <stdint.h>
#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/of_manager.h"
#include "net/fib.h"
#include "vtimer.h"
#include "trickle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the IPv6 thread
 */
#ifndef GNRC_RPL_STACK_SIZE
#define GNRC_RPL_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the RPL thread
 */
#ifndef GNRC_RPL_PRIO
#define GNRC_RPL_PRIO           (THREAD_PRIORITY_MAIN - 4)
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
 * @brief   Message type for trickle intervals
 */
#define GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL    (0x0901)

/**
 * @brief   Message type for trickle callbacks
 */
#define GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK    (0x0902)

/**
 * @brief   Message type for handling DAO sending
 */
#define GNRC_RPL_MSG_TYPE_DAO_HANDLE  (0x0903)

/**
 * @brief   Message type for handling DODAG cleanup
 */
#define GNRC_RPL_MSG_TYPE_CLEANUP_HANDLE  (0x0904)

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
#define GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE (256)

/**
 * @brief   Maximum rank increase
 */
#define GNRC_RPL_DEFAULT_MAX_RANK_INCREASE (0)

/**
 * @brief   Number of implemented Objective Functions
 */
#define GNRC_RPL_IMPLEMENTED_OFS_NUMOF (1)

/**
 * @brief   Default Objective Code Point (OF0)
 */
#define GNRC_RPL_DEFAULT_OCP (0)

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
#   define GNRC_RPL_DEFAULT_MOP GNRC_RPL_MOP_STORING_MODE_NO_MC
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
#define GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS (20)
#define GNRC_RPL_DEFAULT_DIO_INTERVAL_MIN (3)
#define GNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT (10)
/** @} */

/**
 * @name Default parent and route entry lifetime
 * default lifetime will be multiplied by the lifetime unit to obtain the resulting lifetime
 * @{
 */
#define GNRC_RPL_DEFAULT_LIFETIME (60)
#define GNRC_RPL_LIFETIME_UNIT (2)
/** @} */

/**
 * @brief Interval of the void _update_lifetime() function
 */
#define GNRC_RPL_LIFETIME_STEP (2)

/**
 * @brief Default prefix length for the DODAG id
 */
#define GNRC_RPL_DEFAULT_PREFIX_LEN   (64)

/**
 * @brief Default prefix valid and preferred time for the DODAG id
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
 * @{
 */
#define GNRC_RPL_DAO_SEND_RETRIES (4)
#define GNRC_RPL_DEFAULT_WAIT_FOR_DAO_ACK (3)
#define GNRC_RPL_REGULAR_DAO_INTERVAL (60)
#define GNRC_RPL_DEFAULT_DAO_DELAY (5)
/** @} */

/**
 * @brief Cleanup timeout in seconds
 */
#define GNRC_RPL_CLEANUP_TIME (30)

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
#define GNRC_RPL_ROOT_RANK (256)

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
 * @brief Initialization of the RPL thread.
 *
 * @param[in] if_pid            PID of the interface
 *
 * @return  The PID of the RPL thread, on success.
 * @return  a negative errno on error.
 */
kernel_pid_t gnrc_rpl_init(kernel_pid_t if_pid);

/**
 * @brief Initialization of a RPL DODAG as root node. Creates a new instance if necessary.
 *
 * @param[in] instance_id       Id of the instance
 * @param[in] dodag_id          Id of the DODAG
 *
 * @return  Pointer to the new DODAG, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_dodag_t *gnrc_rpl_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id);

/**
 * @brief   Send a DIO of the @p dodag to the @p destination.
 *
 * @param[in] dodag             Pointer to the DODAG.
 * @param[in] destination       IPv6 addres of the destination.
 */
void gnrc_rpl_send_DIO(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *destination);

/**
 * @brief   Send a DIS of the @p dodag to the @p destination.
 *
 * @param[in] dodag             Pointer to the DODAG, optional.
 * @param[in] destination       IPv6 addres of the destination.
 */
void gnrc_rpl_send_DIS(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *destination);

/**
 * @brief   Send a DAO of the @p dodag to the @p destination.
 *
 * @param[in] dodag             Pointer to the DODAG.
 * @param[in] destination       IPv6 addres of the destination.
 * @param[in] lifetime          Lifetime of the route to announce.
 */
void gnrc_rpl_send_DAO(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *destination, uint8_t lifetime);

/**
 * @brief   Send a DAO-ACK of the @p dodag to the @p destination.
 *
 * @param[in] dodag             Pointer to the DODAG, optional.
 * @param[in] destination       IPv6 addres of the destination.
 * @param[in] seq				Sequence number to be acknowledged.
 */
void gnrc_rpl_send_DAO_ACK(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *destination, uint8_t seq);

/**
 * @brief   Parse a DIS.
 *
 * @param[in] dis       Pointer to the DIS message.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] dst       Pointer to the destination address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DIS(gnrc_rpl_dis_t *dis, ipv6_addr_t *src, ipv6_addr_t *dst, uint16_t len);

/**
 * @brief   Parse a DIO.
 *
 * @param[in] dio       Pointer to the DIO message.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DIO(gnrc_rpl_dio_t *dio, ipv6_addr_t *src, uint16_t len);

/**
 * @brief   Parse a DAO.
 *
 * @param[in] dao       Pointer to the DAO message.
 * @param[in] src       Pointer to the source address of the IPv6 packet.
 * @param[in] len       Length of the IPv6 packet.
 */
void gnrc_rpl_recv_DAO(gnrc_rpl_dao_t *dao, ipv6_addr_t *src, uint16_t len);

/**
 * @brief   Parse a DAO-ACK.
 *
 * @param[in] dao_ack   Pointer to the DAO-ACK message.
 */
void gnrc_rpl_recv_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack);

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
#ifdef __cplusplus
}
#endif

#endif /* GNRC_RPL_H_ */
/** @} */
