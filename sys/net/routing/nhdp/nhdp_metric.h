/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Macros for NHDP metric computation
 *
 * The used infrastructure for exchanging metric values is based on the rules defined in the RFC
 * of <a href="https://tools.ietf.org/html/rfc7181">OLSRv2</a>. The calculations for the
 * Directional Airtime Metric (DAT) are based on
 * <a href="https://tools.ietf.org/html/draft-ietf-manet-olsrv2-dat-metric-05">DAT Draft v5</a>.
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef NHDP_METRIC_H_
#define NHDP_METRIC_H_

#include "rfc5444/rfc5444.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    NHDP protocol macros
 *
 * @{
 */
/** @brief Hop Count metric extension value for metric TLV extension field */
#define NHDP_LMT_HOP_COUNT          (163)
/** @brief DAT metric extension value for metric TLV extension field */
#define NHDP_LMT_DAT                (165)

/** @brief Used metric (change to switch to another metric type) */
#define NHDP_METRIC                 (NHDP_LMT_HOP_COUNT)

/** @brief Randomly chosen number for NHDP's metric timer event */
#define NHDP_METRIC_TIMER           (5445)
/** @brief Macro controlling the start of a periodic timer event for matric computation */
#define NHDP_METRIC_NEEDS_TIMER     (NHDP_METRIC == NHDP_LMT_DAT)

#define NHDP_METRIC_UNKNOWN         (0)
#define NHDP_METRIC_MINIMUM         (RFC5444_METRIC_MIN)
#define NHDP_METRIC_MAXIMUM         (RFC5444_METRIC_MAX)

/** @brief Default queue size for metrics using a queue to determine link loss */
#define NHDP_Q_MEM_LENGTH           (64)
/** @brief Restart detection value for packet sequence number based link loss computation */
#define NHDP_SEQNO_RESTART_DETECT   (256)

/** @brief Encoding for an incoming link metric value in metric TLV */
#define NHDP_KD_LM_INC              (0x8000)
/** @brief Encoding for an outgoing link metric value in metric TLV */
#define NHDP_KD_LM_OUT              (0x4000)
/** @brief Encoding for an incoming neighbor metric value in metric TLV */
#define NHDP_KD_NM_INC              (0x2000)
/** @brief Encoding for an outgoing neighbor metric value in metric TLV */
#define NHDP_KD_NM_OUT              (0x1000)
/** @} */

/**
 * @name    DAT metric specific macros
 *
 * @{
 */
/** @brief Length of RCVD and TOTAL queue of DAT metric */
#define DAT_MEMORY_LENGTH           (NHDP_Q_MEM_LENGTH)
/** @brief Time between DAT metric refreshal */
#define DAT_REFRESH_INTERVAL        (1)
/** @brief Factor to spread HELLO interval */
#define DAT_HELLO_TIMEOUT_FACTOR    (1.2)
/** @brief Minimal supported bit rate in bps (default value for new links) */
#define DAT_MINIMUM_BITRATE         (1000)
/** @brief Maximum allowed loss in expected/rcvd HELLOs (should not be changed) */
#define DAT_MAXIMUM_LOSS            (8)
/** @brief Constant value needed for DAT metric computation (should not be changed) */
#define DAT_CONSTANT                (16777216)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NHDP_METRIC_H_ */
