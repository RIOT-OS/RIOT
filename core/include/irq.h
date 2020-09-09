/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_irq     IRQ Handling
 * @ingroup     core
 * @brief       Provides an API to control interrupt processing
 * @{
 *
 * @file
 * @brief       IRQ driver interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 */

#ifndef IRQ_H
#define IRQ_H

#include <stdbool.h>
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IRQ_API_INLINED
#define MAYBE_INLINE static inline __attribute__((always_inline))
#else
#define MAYBE_INLINE
#endif /* IRQ_API_INLINED */

/**
 * @brief   This function sets the IRQ disable bit in the status register
 *
 * @return  Previous value of status register. The return value should not be
 *          interpreted as a boolean value. The actual value is only
 *          significant for irq_restore().
 *
 * @see     irq_restore
 */
MAYBE_INLINE unsigned irq_disable(void);

/**
 * @brief   This function clears the IRQ disable bit in the status register
 *
 * @return  Previous value of status register. The return value should not be
 *          interpreted as a boolean value. The actual value is only
 *          significant for irq_restore().
 *
 * @see     irq_restore
 */
MAYBE_INLINE unsigned irq_enable(void);

/**
 * @brief   This function restores the IRQ disable bit in the status register
 *          to the value contained within passed state
 *
 * @param[in] state   state to restore
 *
 * @see     irq_enable
 * @see     irq_disable
 */
MAYBE_INLINE void irq_restore(unsigned state);

/**
 * @brief   Check whether called from interrupt service routine
 * @return  true, if in interrupt service routine, false if not
 */
MAYBE_INLINE int irq_is_in(void);

#ifdef IRQ_API_INLINED
#include "irq_arch.h"
#endif /* IRQ_API_INLINED */

#ifdef __cplusplus
}
#endif

#endif /* IRQ_H */
/** @} */
