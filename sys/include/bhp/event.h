/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup     sys_bhp_event Event based implementation of Bottom Half Processor
 * @ingroup      sys_bhp

 * @brief        Bottom Half Processor module for generic IRQ offloading.
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include "bhp.h"
#include <event.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event based Bottom Half Processor descriptor
 */
typedef struct {
    bhp_t bhp;          /**< Bottom Half Processor descriptor */
    event_queue_t *evq; /**< Pointer to the event queue */
    event_t ev;         /**< Event holding the Bottom Half Processor handler */
} bhp_event_t;

/**
 * @brief Init a Bottom Half Processor to be used with events
 *
 * @param[in] bhp_ev    The Event based Bottom Half Processor descriptor
 * @param[in] evq       The target event queue to process the Bottom Half Processor
 * @param[in] cb        Bottom Half Processor callback
 * @param[in] ctx       Context of @p cb
 */
void bhp_event_init(bhp_event_t *bhp_ev, event_queue_t *evq, bhp_cb_t cb, void *ctx);

/**
 * @brief Event based Bottom Half Processor ISR callback
 *        To be called from ISR in order to trigger the Bottom Half Processor.
 *
 * @param[in] bhp_event_ctx     Context (@ref bhp_event_t) of the event based
 *                              Bottom Half Processor.
 */
void bhp_event_isr_cb(void *bhp_event_ctx);

#ifdef __cplusplus
}
#endif

/** @} */
