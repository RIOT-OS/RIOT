/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup     sys_bhp Bottom Half Processor
 * @ingroup      sys

 * @brief        Base implementation of Bottom Half Processor module for generic
 *               IRQ offloading.
 *
 * This module provides a generic mechanism to schedule an offload request
 * (Top Half) from interrupt context and run the IRQ handler in thread context.
 * A Bottom Half Processor interface stores a pointer to the device IRQ handler
 * and context, which allows device agnostic IRQ offloading.
 *
 * A user of this module can either use the Bottom Half Processor interface directly
 * or use an existing implementation of a Bottom Half Processor (see
 * @ref sys_bhp_event)
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef BHP_H
#define BHP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the Bottom Half Processor descriptor
 */
typedef struct bhp bhp_t;

/**
 * @brief A Bottom Half Processor callback
 *
 * @param[in] arg   Context of the callback
 */
typedef void (*bhp_cb_t)(void *arg);

/**
 * @brief Bottom Half Processor descriptor
 */
struct bhp {
    bhp_cb_t irq_handler;   /**< Bottom Half Processor IRQ handler */
    void *ctx;              /**< Context of the IRQ handler */
};

/**
 * @brief Call the IRQ handler associated to a Bottom Half Processor descriptor
 *
 * @note It is possible, although not recommended, to call this function on ISR. This
 * can be done e.g when it is required to process the handler during ISR.
 *
 * @param[in] bhp   Pointer to the Bottom Half Processor descriptor
 */
static inline void bhp_irq_handler(bhp_t *bhp)
{
    bhp->irq_handler(bhp->ctx);
}

/**
 * @brief Set the callback for a Bottom Half Processor
 *
 * This function should be called inside the init function of a device that requires
 * ISR offloading.
 *
 * @param[in] bhp   Pointer to the Bottom Half Processor
 * @param[in] cb    IRQ handler of the Bottom Half Processor
 * @param[in] ctx   Context of the IRQ handler
 */
static inline void bhp_set_cb(bhp_t *bhp, bhp_cb_t cb, void *ctx)
{
    bhp->irq_handler = cb;
    bhp->ctx = ctx;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BHP_H */
