/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    aodvv2 AODVv2
 * @brief       The Ad-hoc On-demand Distance Vector routing protocol, version 2
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       Interface for the AODVv2 routing protocol
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_H_
#define AODVV2_H_

#include "common/netaddr.h"
#include "rfc5444/rfc5444_print.h"
#include "kernel_types.h"

#include "net/gnrc/aodvv2/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the AODVv2 routing protocol.
 *
 * @param interface   The interface AODVv2 operates on and finds routes for.
 */
void aodv_init(kernel_pid_t interface);

/**
 * @brief   Set the metric type. If metric_type does not match any known metric
 *          types, no changes will be made.
 *
 * @param[in] metric_type       type of new metric
 */
void aodv_set_metric_type(aodvv2_metric_t metric_type);

#ifdef  __cplusplus
}
#endif

#endif /* AODVV2_H_ */
/** @} */
