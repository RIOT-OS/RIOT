/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    gnrc_sixlowpan_nd_border_router Border router part of 6LoWPAN-ND
 * @ingroup     gnrc_sixlowpan_nd
 * @brief       Border router part of 6LoWPAN-ND
 * @{
 *
 * @file
 * @brief   Border router definitions for 6LoWPAN.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SIXLOWPAN_BORDER_ROUTER_H_
#define GNRC_SIXLOWPAN_BORDER_ROUTER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default lifetime in minutes for 6LoWPAN border router information.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4.3">
 *          RFC 6775, section 4.3
 *      </a>
 */
#ifndef GNRC_SIXLOWPAN_ND_BORDER_ROUTER_DEFAULT_LTIME
#define GNRC_SIXLOWPAN_ND_BORDER_ROUTER_DEFAULT_LTIME   (10000U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_BORDER_ROUTER_H_ */
/** @} */
