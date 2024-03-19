/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_bhp
 * @{
 *
 * @file
 * @brief       Event based Bottom Half Processor implementation
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>
#include "bhp/event.h"

static void _event_handler(event_t *event)
{
    bhp_event_t *bhp_event = container_of(event, bhp_event_t, ev);
    bhp_irq_handler(&bhp_event->bhp);
}

void bhp_event_init(bhp_event_t *bhp_ev, event_queue_t *evq, bhp_cb_t cb, void *ctx)
{
    bhp_set_cb(&bhp_ev->bhp, cb, ctx);
    bhp_ev->evq = evq;
    bhp_ev->ev.handler = _event_handler;
}

void bhp_event_isr_cb(void *bhp_event_ctx)
{
    bhp_event_t *bhp_event = bhp_event_ctx;
    event_post(bhp_event->evq, &bhp_event->ev);
}
