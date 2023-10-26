/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup pkg_libschc_gnrc
 * @{
 *
 * @file
 * @brief   Type definitions for libSCHC with GNRC
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef LIBSCHC_GNRC_TYPES_H
#define LIBSCHC_GNRC_TYPES_H

#include "schc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   libSCHC interface descriptor
 */
typedef struct {
    uint32_t dev_id;                    /**< SCHC device ID */
    reliability_mode upstream_mode;     /**< Mode for upstream fragmentation */
} libschc_gnrc_netif_t;

#ifdef __cplusplus
}
#endif

#endif /* LIBSCHC_GNRC_TYPES_H */
/** @} */
