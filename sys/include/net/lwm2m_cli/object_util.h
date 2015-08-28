/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_lwm2m_cli
 * @{
 *
 * @file
 * @brief       Object handling utility functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifndef LWM2M_CLI_OBJECT_UTIL_H
#define LWM2M_CLI_OBJECT_UTIL_H

#include <stdint.h>

#include "net/ipv6/addr.h"
#include "liblwm2m.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Find an object
 *
 * @param[in] ctx       LWM2M context
 * @param[in] id        object ID of the object to find
 *
 * @return      pointer to the object of interest
 * @return      NULL if object could not be found
 */
lwm2m_object_t *lwm2m_cli_obj_find(lwm2m_context_t *ctx, uint16_t id);

/**
 * @brief   Initialize the given object store
 *
 * @todo    Remove once object store has been made static
 *
 * @param[in] obj_store pointer to the object stores memory
 * @param[in] len       number of objects that fit into the object store
 */
void lwm2m_cli_obj_init(lwm2m_object_t **obj_store, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLI_OBJECT_UTIL_H */
/** @} */
