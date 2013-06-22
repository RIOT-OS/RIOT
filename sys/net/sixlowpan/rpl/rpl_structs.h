/**
 * RPL data structs 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl 
 * @{
 * @file    rpl_structs.h
 * @brief   RPL data structs
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include "sys/net/sixlowpan/sixlowip.h"

#ifndef RPL_STRUCTS_H_INCLUDED
#define RPL_STRUCTS_H_INCLUDED
/* Modes of Operation */

#define NO_DOWNWARD_ROUTES  0x00
#define NON_STORING_MODE    0x01
#define STORING_MODE_NO_MC  0x02
#define STORING_MODE_MC     0x03

/* ICMP type */
#define ICMP_RPL_CONTROL            155
#define RPL_SEQUENCE_WINDOW         16
#define ICMP_CODE_DIS               0x00
#define ICMP_CODE_DIO               0x01
#define ICMP_CODE_DAO               0x02
#define ICMP_CODE_DAO_ACK           0x03
/* packet base lengths */
#define DIO_BASE_LEN                24
#define DIS_BASE_LEN                2
#define DAO_BASE_LEN                4
#define DAO_D_LEN                   24
#define DAO_ACK_LEN                 4
#define DAO_ACK_D_LEN               24
#define RPL_OPT_LEN                 2
#define RPL_OPT_DODAG_CONF_LEN      14
#define RPL_OPT_PREFIX_INFO_LEN		30
#define RPL_OPT_SOLICITED_INFO_LEN	19
#define RPL_OPT_TARGET_LEN			18
#define RPL_OPT_TRANSIT_LEN			4

/* message options */
#define RPL_OPT_PAD1                 0
#define RPL_OPT_PADN                 1
#define RPL_OPT_DAG_METRIC_CONTAINER 2
#define RPL_OPT_ROUTE_INFO           3
#define RPL_OPT_DODAG_CONF           4
#define RPL_OPT_TARGET               5
#define RPL_OPT_TRANSIT              6
#define RPL_OPT_SOLICITED_INFO       7
#define RPL_OPT_PREFIX_INFO          8
#define RPL_OPT_TARGET_DESC          9

/* Counters */
#define RPL_COUNTER_MAX                 255
#define RPL_COUNTER_LOWER_REGION        127
#define RPL_COUNTER_SEQ_WINDOW          16
#define RPL_COUNTER_INIT                RPL_COUNTER_MAX - RPL_COUNTER_SEQ_WINDOW + 1
#define RPL_COUNTER_INCREMENT(counter)  (counter > RPL_COUNTER_LOWER_REGION ? (counter == RPL_COUNTER_MAX ? counter=0 : ++counter) : (counter == RPL_COUNTER_LOWER_REGION ? counter=0 : ++counter))
#define RPL_COUNTER_IS_INIT(counter)    (counter > RPL_COUNTER_LOWER_REGION)
#define RPL_COUNTER_GREATER_THAN_LOCAL(A,B) (((A<B) && (RPL_COUNTER_LOWER_REGION + 1 - B + A < RPL_COUNTER_SEQ_WINDOW)) || ((A > B) && (A-B < RPL_COUNTER_SEQ_WINDOW)))
#define RPL_COUNTER_GREATER_THAN(A,B)   ((A>RPL_COUNTER_LOWER_REGION) ? ((B > RPL_COUNTER_LOWER_REGION ) ? RPL_COUNTER_GREATER_THAN_LOCAL(A,B) : 0): (( B>RPL_COUNTER_LOWER_REGION ) ? 1: RPL_COUNTER_GREATER_THAN_LOCAL(A,B)))

/*  Default values */

#define RPL_DEFAULT_MOP STORING_MODE_NO_MC

/*  RPL Constants and Variables */

#define BASE_RANK 0
#define ROOT_RANK 1
#define INFINITE_RANK 0xFFFF
#define RPL_DEFAULT_INSTANCE 0
#define DEFAULT_PATH_CONTROL_SIZE 0
#define DEFAULT_DIO_INTERVAL_MIN 11
/* standard value: */
/* #define DEFAULT_DIO_INTERVAL_MIN 3 */
#define DEFAULT_DIO_INTERVAL_DOUBLINGS 7
/* standard value: */
/* #define DEFAULT_DIO_INTERVAL_DOUBLINGS 20 */
#define DEFAULT_DIO_REDUNDANCY_CONSTANT 10
#define DEFAULT_MIN_HOP_RANK_INCREASE 256
/* DAO_DELAY is in seconds */
#define DEFAULT_DAO_DELAY 3
#define REGULAR_DAO_INTERVAL 300
#define DAO_SEND_RETRIES 4
#define DEFAULT_WAIT_FOR_DAO_ACK 15
#define RPL_DODAG_ID_LEN 16

/* others */

#define NUMBER_IMPLEMENTED_OFS 1
#define RPL_MAX_DODAGS 3
#define RPL_MAX_INSTANCES 1
#define RPL_MAX_PARENTS 5
#define RPL_MAX_ROUTING_ENTRIES 128
#define RPL_ROOT_RANK 1
#define RPL_DEFAULT_LIFETIME 0xff
#define RPL_LIFETIME_UNIT 2
#define RPL_GROUNDED 1
#define RPL_PRF_MASK 0x7
#define RPL_MOP_SHIFT 3
#define RPL_SHIFTED_MOP_MASK 0x7
#define RPL_DIS_V_MASK 0x80
#define RPL_DIS_I_MASK 0x40
#define RPL_DIS_D_MASK 0x20
#define RPL_GROUNDED_SHIFT 7
#define RPL_DEFAULT_OCP 0

struct __attribute__((packed)) rpl_dio_t {
    uint8_t rpl_instanceid;
    uint8_t version_number;
    uint16_t rank;
    uint8_t g_mop_prf;
    uint8_t dtsn;
    uint8_t flags;
    uint8_t reserved;
    ipv6_addr_t dodagid;
};

struct __attribute__((packed)) rpl_dis_t {
    uint8_t flags;
    uint8_t reserved;
};

struct __attribute__((packed)) rpl_dao_t {
    uint8_t rpl_instanceid;
    uint8_t k_d_flags;
    uint8_t reserved;
    uint8_t dao_sequence;
};

struct __attribute__((packed)) rpl_dao_ack_t {
    uint8_t rpl_instanceid;
    uint8_t d_reserved;
    uint8_t dao_sequence;
    uint8_t status;
};

/* may be present in dao or dao_ack packets */
struct __attribute__((packed)) dodag_id_t {
    ipv6_addr_t dodagid;
};

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
} rpl_opt_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t flags_a_pcs;
    uint8_t DIOIntDoubl;
    uint8_t DIOIntMin;
    uint8_t DIORedun;
    uint16_t MaxRankIncrease;
    uint16_t MinHopRankIncrease;
    uint16_t ocp;
    uint8_t reserved;
    uint8_t default_lifetime;
    uint16_t lifetime_unit;
} rpl_opt_dodag_conf_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t rplinstanceid;
    uint8_t VID_Flags;
    ipv6_addr_t dodagid;
    uint8_t version;
} rpl_opt_solicited_t;

/* ipv6_addr_t target may be replaced by a target prefix of variable length */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t flags;
    uint8_t prefix_length;
    ipv6_addr_t target;
} rpl_opt_target_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t e_flags;
    uint8_t path_control;
    uint8_t path_sequence;
    uint8_t path_lifetime;
} rpl_opt_transit_t;

struct rpl_dodag_t;

typedef struct {
    ipv6_addr_t addr;
    uint16_t rank;
    uint8_t dtsn;
    struct rpl_dodag_t *dodag;
    uint16_t lifetime;
    uint8_t used;
} rpl_parent_t;

struct rpl_of_t;

typedef struct {
    uint8_t id;
    uint8_t used;
    uint8_t joined;

} rpl_instance_t;

typedef struct {
    rpl_instance_t *instance;
    ipv6_addr_t dodag_id;
    uint8_t used;
    uint8_t mop;
    uint8_t dtsn;
    uint8_t prf;
    uint8_t dio_interval_doubling;
    uint8_t dio_min;
    uint8_t dio_redundancy;
    uint16_t maxrankincrease;
    uint16_t minhoprankincrease;
    uint8_t default_lifetime;
    uint16_t lifetime_unit;
    uint8_t version;
    uint8_t grounded;
    uint16_t my_rank;
    uint8_t dao_seq;
    uint16_t min_rank;
    uint8_t joined;
    rpl_parent_t *my_preferred_parent;
    struct rpl_of_t *of;
} rpl_dodag_t;

typedef struct {
    uint16_t ocp;
    uint16_t (*calc_rank)(rpl_parent_t *, uint16_t);
    rpl_parent_t *(*which_parent)(rpl_parent_t *, rpl_parent_t *);
    rpl_dodag_t *(*which_dodag)(rpl_dodag_t *, rpl_dodag_t *);
    void (*reset)(struct rpl_dodag_t *);
    void (*parent_state_callback)(rpl_parent_t *, int, int);
} rpl_of_t;

typedef struct {
    uint8_t used;
    ipv6_addr_t address;
    ipv6_addr_t next_hop;
    uint16_t lifetime;

} rpl_routing_entry_t;

#endif
