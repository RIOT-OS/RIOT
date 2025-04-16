/*
 * Copyright (C) 2013,2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef IRQ_H
#define IRQ_H

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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdbool.h>
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IRQ_API_INLINED
#  define MAYBE_INLINE static inline __attribute__((always_inline))
#else
#  define MAYBE_INLINE
#endif /* IRQ_API_INLINED */

#ifndef IRQ_API_INLINED
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
 * @warning This function is here primarily for internal use, and for
 *          compatibility with the Arduino environment (which lacks the
 *          "disable / restore" concept. Enabling interrupts when a different
 *          component disabled them can easily cause unintended behavior there.
 *
 *          Use @ref irq_restore instead.
 */
MAYBE_INLINE unsigned irq_enable(void);

/**
 * @brief   This function restores the IRQ disable bit in the status register
 *          to the value contained within passed state
 *
 * @param[in] state   state to restore
 *
 * @see     irq_disable
 */
MAYBE_INLINE void irq_restore(unsigned state);

/**
 * @brief   Test if IRQs are currently enabled
 *
 * @warning Use this function from thread context only. When used in interrupt
 *          context the returned state may be incorrect.
 *
 * @return  false if IRQs are currently disabled
 * @return  true if IRQs are currently enabled
 */
MAYBE_INLINE bool irq_is_enabled(void);

/**
 * @brief   Check whether called from interrupt service routine
 * @return  true, if in interrupt service routine, false if not
 */
MAYBE_INLINE bool irq_is_in(void);

#else
#  include "irq_arch.h" /* IWYU pragma: export */
#endif                  /* IRQ_API_INLINED */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* IRQ_H */
