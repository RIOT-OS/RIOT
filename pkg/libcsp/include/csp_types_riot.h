/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_libcsp
 * @{
 *
 * @file
 * @brief       LibCSP RIOT types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */
#ifndef CSP_RIOT_TYPES_H
#define CSP_RIOT_TYPES_H

#include <stdint.h>

#include "sema.h"
#include "mutex.h"
#include "sema.h"
#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CSP base type
 */
#define CSP_BASE_TYPE           uint32_t
/**
 * @brief CSP max timeout
 */
#define CSP_MAX_TIMEOUT         (UINT32_MAX)

/**
 * @brief CSP sempahore type
 */
typedef sema_t csp_bin_sem_t;

/**
 * @brief CSP tsrb based static queue type
 */
typedef struct csp_static_queue {
    tsrb_t tsrb;        /**< the ringbuffer type */
    mutex_t pop;        /**< lock to signal a empty queue */
    mutex_t push;       /**< lock to signal a full queue */
    size_t item_size;   /**< size of each queue item */
} csp_static_queue_t;

/**
 * @brief CSP queue handler definitions
 */
typedef csp_static_queue_t *csp_queue_handle_t;

#ifdef __cplusplus
}
#endif

#endif /* CSP_RIOT_TYPES_H */
