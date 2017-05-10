/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup conn
 * @{
 *
 * @file
 * @brief   Generic types for @ref conn.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef CONN_TYPES_H_
#define CONN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Events to signal to asynchronous callbacks.
 */
typedef conn_event {
    CONN_EVENT_RECV = 0,    /**< data received */
} conn_event_t;


#ifdef __cplusplus
}
#endif

#endif /* CONN_TYPES_H_ */
/** @} */
