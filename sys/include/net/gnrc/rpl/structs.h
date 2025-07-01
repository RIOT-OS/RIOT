/*
 * Copyright (C) 2017       HAW Hamburg
 * Copyright (C) 2015–2018  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 * Copyright (C) 2013       INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       RPL data structs
 *
 * Header file, which defines all structs used by RPL.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "byteorder.h"
#include "net/ipv6/addr.h"
#include "evtimer.h"
#include "evtimer_msg.h"
#include "trickle.h"

/**
 * @name Option lengths
 * @{
 */
#define GNRC_RPL_OPT_DODAG_CONF_LEN         (14)
#define GNRC_RPL_OPT_PREFIX_INFO_LEN        (30)
#define GNRC_RPL_OPT_TARGET_LEN             (18)
#define GNRC_RPL_OPT_TRANSIT_INFO_LEN       (4)
/** @} */

/**
 * @name DAO flag macros
 * @{
 */
#define GNRC_RPL_DAO_D_BIT                  (1 << 6)
#define GNRC_RPL_DAO_K_BIT                  (1 << 7)
/** @} */

/**
 * @name DAO-ACK flag macros
 * @{
 */
#define GNRC_RPL_DAO_ACK_D_BIT              (1 << 7)
/** @} */

/**
 * @anchor GNRC_RPL_REQ_DIO_OPTS
 * @name DIO Options for gnrc_rpl_dodag_t::dio_opts
 * @{
 */
#define GNRC_RPL_REQ_DIO_OPT_DODAG_CONF_SHIFT       (0)
#define GNRC_RPL_REQ_DIO_OPT_DODAG_CONF             (1 << GNRC_RPL_REQ_DIO_OPT_DODAG_CONF_SHIFT)
#define GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO_SHIFT      (1)
#define GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO            (1 << GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO_SHIFT)
/** @} */

/**
 * @brief RPL-Option Generic Format
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.1">
 *          RFC6550, section 6.7.1, RPL Control Message Option Generic Format
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;       /**< Option Type */
    uint8_t length;     /**< Option Length, does not include the first two byte */
} gnrc_rpl_opt_t;

/**
 * @brief DIO Base Object
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.3.1">
 *          RFC6550, section 6.3.1, Format of the DIO Base Object
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t instance_id;        /**< id of the instance */
    uint8_t version_number;     /**< version number of the DODAG */
    network_uint16_t rank;      /**< rank of the parent emitting the DIO */
    uint8_t g_mop_prf;          /**< grounded, MOP, preferred flags */
    uint8_t dtsn;               /**< Destination Advertisement Trigger Sequence Number */
    uint8_t flags;              /**< unused */
    uint8_t reserved;           /**< reserved */
    ipv6_addr_t dodag_id;       /**< id of the dodag */
} gnrc_rpl_dio_t;

/**
 * @brief DODAG Configuration Option
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.6">
 *          RFC6550, section 6.7.6, DODAG Configuration
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                       /**< Option Type: 0x04 */
    uint8_t length;                     /**< length of option, not including first two bytes */
    uint8_t flags_a_pcs;                /**< flags */
    uint8_t dio_int_doubl;              /**< trickle Imax parameter */
    uint8_t dio_int_min;                /**< trickle Imin parameter */
    uint8_t dio_redun;                  /**< trickle k parameter */
    network_uint16_t max_rank_inc;      /**< allowable increase in rank */
    network_uint16_t min_hop_rank_inc;  /**< DAGRank(rank) = floor(rank/MinHopRankIncrease) */
    network_uint16_t ocp;               /**< Objective Code Point */
    uint8_t reserved;                   /**< reserved */
    uint8_t default_lifetime;           /**< lifetime of RPL routes (lifetime * lifetime_unit) */
    network_uint16_t lifetime_unit;     /**< unit in seconds */
} gnrc_rpl_opt_dodag_conf_t;

/**
 * @brief DODAG Information Solicitation
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.2">
 *          RFC6550, section 6.2, DODAG Information Solicitation
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t flags;      /**< unused */
    uint8_t reserved;   /**< reserved */
} gnrc_rpl_dis_t;

/**
 * @brief DIS Solicited Information option
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.9">
 *          RFC6550, section 6.7.9, Solicited Information
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< Option Type: 0x07 */
    uint8_t length;             /**< Option Length: 19 bytes*/
    uint8_t instance_id;        /**< id of the instance */
    uint8_t VID_flags;          /**< V|I|D predicate options followed by 5 bit unused flags */
    ipv6_addr_t dodag_id;       /**< DODAG ID predicate */
    uint8_t version_number;     /**< version number of the DODAG */
} gnrc_rpl_opt_dis_solicited_t;

/**
 * @brief Destination Advertisement Object
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.4">
 *          RFC6550, section 6.4, Destination Advertisement Object
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t instance_id;        /**< id of the instance */
    uint8_t k_d_flags;          /**< K and D flags */
    uint8_t reserved;           /**< reserved */
    uint8_t dao_sequence;       /**< sequence of the DAO, needs to be used for DAO-ACK */
} gnrc_rpl_dao_t;

/**
 * @brief Destination Advertisement Object Acknowledgement
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.5">
 *          RFC6550, section 6.5, Destination Advertisement Object Acknowledgement
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t instance_id;        /**< id of the instance */
    uint8_t d_reserved;         /**< if set, indicates that the DODAG id should be included */
    uint8_t dao_sequence;       /**< sequence must be equal to the sequence from the DAO object */
    uint8_t status;             /**< indicates completion */
} gnrc_rpl_dao_ack_t;

/**
 * @brief Target Option
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.7">
 *          RFC6550, section 6.7.7, RPL Target
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< option type */
    uint8_t length;             /**< option length without the first two bytes */
    uint8_t flags;              /**< unused */
    uint8_t prefix_length;      /**< number of valid leading bits in the IPv6 prefix */
    ipv6_addr_t target;         /**< IPv6 prefix, address or multicast group */
} gnrc_rpl_opt_target_t;

/**
 * @brief Transit Option
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.8">
 *          RFC6550, section 6.7.8, Transit Information
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< option type */
    uint8_t length;             /**< option length without the first two bytes */
    uint8_t e_flags;            /**< external flag indicates external routes */
    uint8_t path_control;       /**< limits the number of DAO parents */
    uint8_t path_sequence;      /**< increased value for route updates */
    uint8_t path_lifetime;      /**< lifetime of routes */
} gnrc_rpl_opt_transit_t;

/**
 * @brief Prefix Information Option
 * @see <a href="https://tools.ietf.org/html/rfc6550#section-6.7.10">
 *          RFC6550, section 6.7.10, Prefix Information
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                       /**< option type */
    uint8_t length;                     /**< option length without the first
                                         *   two bytes */
    uint8_t prefix_len;                 /**< prefix length */
    uint8_t LAR_flags;                  /**< flags and resereved */
    network_uint32_t valid_lifetime;    /**< valid lifetime */
    network_uint32_t pref_lifetime;     /**< preferred lifetime */
    uint32_t reserved;                  /**< reserved */
    ipv6_addr_t prefix;                 /**< prefix used for Stateless Address
                                         *   Autoconfiguration */
} gnrc_rpl_opt_prefix_info_t;

/**
 * @brief DODAG representation
 */
typedef struct gnrc_rpl_dodag gnrc_rpl_dodag_t;

/**
 * @brief Parent representation
 */
typedef struct gnrc_rpl_parent gnrc_rpl_parent_t;

/**
 * @brief Instance representation
 */
typedef struct gnrc_rpl_instance gnrc_rpl_instance_t;

/**
 * @cond INTERNAL */
struct gnrc_rpl_parent {
    gnrc_rpl_parent_t *next;        /**< pointer to the next parent */
    uint8_t state;                  /**< see @ref gnrc_rpl_parent_states */
    ipv6_addr_t addr;               /**< link-local IPv6 address of this parent */
    uint8_t dtsn;                   /**< last seen dtsn of this parent */
    uint16_t rank;                  /**< rank of the parent */
    gnrc_rpl_dodag_t *dodag;        /**< DODAG the parent belongs to */
    double link_metric;             /**< metric of the link */
    uint8_t link_metric_type;       /**< type of the metric */
    /**
     * @brief Parent timeout events (see @ref GNRC_RPL_MSG_TYPE_PARENT_TIMEOUT)
     */
    evtimer_msg_event_t timeout_event;
};
/**
 * @endcond
 */

/**
 * @brief Objective function representation
 */
typedef struct {
    uint16_t ocp;   /**< objective code point */

    /**
     * @brief Calculate the rank of this node.
     *
     * @param[in]   dodag       RPL DODAG to calculate rank from.
     * @param[in]   base_rank   BASE_RANK parameter as in rfc6550
     *
     * @return      RPL Rank of this node.
     * @return      GNRC_RPL_INFINITE_RANK, if no rank calculation is possible.
     */
    uint16_t (*calc_rank)(gnrc_rpl_dodag_t *dodag, uint16_t base_rank);

    /**
     * @brief   Compare two @ref gnrc_rpl_parent_t.
     *
     * Compares two parents based on the rank calculated by the objective
     * function. This function is used to determine the parent list order. The
     * parents are ordered from the preferred parent to the least preferred
     * parent.
     *
     * @param[in] parent1 First parent to compare.
     * @param[in] parent2 Second parent to compare.
     *
     * @return      Zero if the parents are of equal preference.
     * @return      Positive, if the second parent is preferred.
     * @return      Negative, if the first parent is preferred.
     */
    int (*parent_cmp)(gnrc_rpl_parent_t *parent1, gnrc_rpl_parent_t *parent2);
    int (*which_dodag)(gnrc_rpl_dodag_t *, gnrc_rpl_dio_t *); /**< compare for dodags */

    /**
     * @brief Reset the state of the objective function.
     *
     * @param[in]   dodag   RPL dodag object.
     */
    void (*reset)(gnrc_rpl_dodag_t *dodag);
    void (*parent_state_callback)(gnrc_rpl_parent_t *, int, int); /**< retrieves the state of a parent*/

    /**
     * @brief Initialize the objective function.
     *
     * @param[in]   dodag   RPL dodag object.
     */
    void (*init)(gnrc_rpl_dodag_t *dodag);
    void (*process_dio)(void);  /**< DIO processing callback (acc. to OF0 spec, chpt 5) */
} gnrc_rpl_of_t;

/**
 * @cond INTERNAL
 */
struct gnrc_rpl_dodag {
    ipv6_addr_t dodag_id;           /**< id of the DODAG */
    gnrc_rpl_parent_t *parents;     /**< pointer to the parents list of this DODAG */
    gnrc_rpl_instance_t *instance;  /**< pointer to the instance that this dodag is part of */
    uint8_t dtsn;                   /**< DAO Trigger Sequence Number */
    uint8_t prf;                    /**< preferred flag */
    uint8_t dio_interval_doubl;     /**< trickle Imax parameter */
    uint8_t dio_min;                /**< trickle Imin parameter */
    uint8_t dio_redun;              /**< trickle k parameter */
    uint8_t default_lifetime;       /**< lifetime of routes (lifetime * unit) */
    uint16_t lifetime_unit;         /**< unit in seconds of the lifetime */
    kernel_pid_t iface;             /**< interface PID this DODAG operates on */
    uint8_t version;                /**< version of this DODAG */
    uint8_t grounded;               /**< grounded flag */
    uint16_t my_rank;               /**< rank/position in the DODAG */
    uint8_t node_status;            /**< leaf, normal, or root node */
    uint8_t dao_seq;                /**< dao sequence number */
    uint8_t dao_counter;            /**< amount of retried DAOs */
    bool dao_ack_received;          /**< flag to check for DAO-ACK */
    uint8_t dio_opts;               /**< options in the next DIO
                                         (see @ref GNRC_RPL_REQ_DIO_OPTS "DIO Options") */
    evtimer_msg_event_t dao_event;  /**< DAO TX events (see @ref GNRC_RPL_MSG_TYPE_DODAG_DAO_TX) */
    trickle_t trickle;              /**< trickle representation */
};

struct gnrc_rpl_instance {
    uint8_t id;                     /**< id of the instance */
    uint8_t state;                  /**< 0 for unused, 1 for used */
    gnrc_rpl_dodag_t dodag;         /**< DODAG of this instance */
    uint8_t mop;                    /**< configured Mode of Operation */
    gnrc_rpl_of_t *of;              /**< configured Objective Function */
    uint16_t min_hop_rank_inc;      /**< minimum hop rank increase */
    uint16_t max_rank_inc;          /**< max increase in the rank */
    /**
     * @brief Instance cleanup events (see @ref GNRC_RPL_MSG_TYPE_INSTANCE_CLEANUP)
     */
    evtimer_msg_event_t cleanup_event;
};
/**
 * @endcond
 */

/**
 * @brief internal unpacked struct type for option insertion
 */
typedef struct {
    uint8_t type;       /**< Option Type */
    uint8_t length;     /**< Option Length, does not include the first two byte */
} gnrc_rpl_internal_opt_t;

/**
 * @brief internal unpacked struct type for DIS solicited option insertion
 */
typedef struct  {
    uint8_t type;                /**< Option Type: 0x07 */
    uint8_t length;              /**< Option Length: 19 bytes*/
    uint8_t instance_id;         /**< id of the instance */
    uint8_t VID_flags;           /**< V|I|D predicate options followed by 5 bit unused flags */
    ipv6_addr_t dodag_id;        /**< DODAG ID predicate */
    uint8_t version_number;      /**< version number of the DODAG */
} gnrc_rpl_internal_opt_dis_solicited_t;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
