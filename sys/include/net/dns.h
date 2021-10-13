/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dns DNS defines
 * @ingroup     net
 * @brief       Generic DNS values
 * @{
 *
 * @file
 * @brief   Generic DNS values
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_DNS_H
#define NET_DNS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name DNS defines
 * @{
 */
#define DNS_TYPE_A              (1)
#define DNS_TYPE_AAAA           (28)
#define DNS_CLASS_IN            (1)
/** @} */

/**
 * @name    Field lengths
 * @{
 */
#define RR_TYPE_LENGTH      (2U)
#define RR_CLASS_LENGTH     (2U)
#define RR_TTL_LENGTH       (4U)
#define RR_RDLENGTH_LENGTH  (2U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_DNS_H */
/** @} */
