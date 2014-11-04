/**
 * RPL data structs
 *
 * Copyright (C) 2013, 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 * @file    rpl_config.h
 * @brief   RPL Config
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#ifndef RPL_CONFIG_H_INCLUDED
#define RPL_CONFIG_H_INCLUDED

/*  Default values */
#define RPL_NO_DOWNWARD_ROUTES  0x00
#define RPL_NON_STORING_MODE    0x01
#define RPL_STORING_MODE_NO_MC  0x02
#define RPL_STORING_MODE_MC     0x03

/* ICMP type */
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
#define RPL_OPT_PREFIX_INFO_LEN     30
#define RPL_OPT_SOLICITED_INFO_LEN  19
#define RPL_OPT_TARGET_LEN          18
#define RPL_OPT_TRANSIT_LEN         4

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
#define RPL_COUNTER_INIT                (RPL_COUNTER_MAX - RPL_COUNTER_SEQ_WINDOW + 1)

static inline uint8_t RPL_COUNTER_INCREMENT(uint8_t counter)
{
    return (counter > RPL_COUNTER_LOWER_REGION ? (counter == RPL_COUNTER_MAX ? counter=0 : ++counter) : (counter == RPL_COUNTER_LOWER_REGION ? counter=0 : ++counter));
}

static inline bool RPL_COUNTER_IS_INIT(uint8_t counter)
{
    return (counter > RPL_COUNTER_LOWER_REGION);
}

static inline bool RPL_COUNTER_GREATER_THAN_LOCAL(uint8_t A,uint8_t B)
{
    return (((A<B) && (RPL_COUNTER_LOWER_REGION + 1 - B + A < RPL_COUNTER_SEQ_WINDOW)) || ((A > B) && (A-B < RPL_COUNTER_SEQ_WINDOW)));
}

static inline bool RPL_COUNTER_GREATER_THAN(uint8_t A,uint8_t B)
{
    return ((A>RPL_COUNTER_LOWER_REGION) ? ((B > RPL_COUNTER_LOWER_REGION ) ? RPL_COUNTER_GREATER_THAN_LOCAL(A,B) : 0): (( B>RPL_COUNTER_LOWER_REGION ) ? 1: RPL_COUNTER_GREATER_THAN_LOCAL(A,B)));
}

/* Node Status */
#define NORMAL_NODE  0
#define ROOT_NODE    1
#define LEAF_NODE    2

/* Link Metric Type */
#define METRIC_ETX 1

/*  RPL Constants and Variables */

#define RPL_DEFAULT_MOP RPL_STORING_MODE_NO_MC
#define BASE_RANK 0
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
#define ROOT_RANK DEFAULT_MIN_HOP_RANK_INCREASE
/* DAO_DELAY is in seconds */
#define DEFAULT_DAO_DELAY 3
#define REGULAR_DAO_INTERVAL 300
#define DAO_SEND_RETRIES 4
#define DEFAULT_WAIT_FOR_DAO_ACK 15
#define RPL_DODAG_ID_LEN 16

/* others */

#define NUMBER_IMPLEMENTED_OFS 2
#define RPL_MAX_DODAGS 3
#define RPL_MAX_INSTANCES 1
#define RPL_MAX_PARENTS 5
#define RPL_MAX_ROUTING_ENTRIES 64 //128
#define RPL_ROOT_RANK 256
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

#endif
