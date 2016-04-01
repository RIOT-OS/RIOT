/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_lwmac
 * @file
 * @brief       Timeout handling.
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#ifndef LWMAC_TIMEOUT_H
#define LWMAC_TIMEOUT_H

#include <stdint.h>
#include <stdbool.h>
#include <msg.h>
#include <xtimer.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Foward declaration */
typedef struct lwmac lwmac_t;

typedef enum {
    TIMEOUT_DISABLED = 0,
    TIMEOUT_WR,
    TIMEOUT_NO_RESPONSE,
    TIMEOUT_WA,
    TIMEOUT_DATA,
    TIMEOUT_WAIT_FOR_DEST_WAKEUP,
    TIMEOUT_WAKEUP_PERIOD,
    TIMEOUT_NEXT_BROADCAST,
    TIMEOUT_BROADCAST_END,
} lwmac_timeout_type_t;

typedef struct {
    xtimer_t timer;
    msg_t msg;
    /* If type != DISABLED, this indicates if timeout has expired */
    bool expired;
    lwmac_timeout_type_t type;
} lwmac_timeout_t;
#define LWMAC_TIMEOUT_INIT  { {}, {}, false, TIMEOUT_DISABLED }

void lwmac_set_timeout(lwmac_t* lwmac, lwmac_timeout_type_t type, uint32_t offset);

void lwmac_clear_timeout(lwmac_t* lwmac, lwmac_timeout_type_t type);

bool lwmac_timeout_is_running(lwmac_t* lwmac, lwmac_timeout_type_t type);

bool lwmac_timeout_is_expired(lwmac_t* lwmac, lwmac_timeout_type_t type);

void lwmac_reset_timeouts(lwmac_t* lwmac);

void lwmac_timeout_make_expire(lwmac_timeout_t* timeout);

#ifdef __cplusplus
}
#endif

#endif /* LWMAC_TIMEOUT_H */
