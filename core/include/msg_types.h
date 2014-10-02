/*
 * Copyright (C) 2014 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_msg
 *
 * @{
 *
 * @file        msg_types.h
 * @brief       Global definitions of all message types
 *
 * This is the global list of all defined message types in RIOT.
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef __MSG_TYPES_H_
#define __MSG_TYPES_H_

/*
 * This list is sorted alphabetically by module name.
 * Within a module, message types are sorted alphabetically.
 * The message declaration format is MT_<MODULE>_<NAME>.
 */
typedef enum {
    /* core message type: */
    MT_MSG_UNDEF = 0,                   /**< undefined message type */

    /* core message type: */
    MT_MSG_MAX,                         /**< highest defined message type */
} msg_type_t;

#endif /* __MSG_TYPES_H_ */
/** @} */
