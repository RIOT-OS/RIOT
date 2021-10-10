/*
* Copyright (C) 2021 Jens Wetterich <jens@wetterich-net.de>
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/
/**
* @ingroup core_irq
* @{
* @file
* @brief   Provides a C++ RAI based API to control interrupt processing
* @author  Jens Wetterich <jens@wetterich-net.de>
*
*/
#ifndef IRQ_HPP
#define IRQ_HPP
#include "irq.h"

namespace riot {
/**
 * @brief   RAII based IRQ lock
 * @details While this object is on the stack IRQ is disabled.
 *          During destruction it will be restored to the previous state.
 */
class irq_lock {
public:
    /**
     * @brief   Test whether IRQs are currently enabled.
     * @return  IRQ enabled
     */
    static inline bool is_locked() noexcept {
        return irq_is_enabled() == 0;
    }

    /**
     * @brief   Check whether called from interrupt service routine.
     * @return  in ISR context
     */
    static inline bool is_isr() noexcept {
        return irq_is_in() != 0;
    }

    /**
     * @brief   This sets the IRQ disable bit in the status register.
     */
    inline irq_lock() : state(irq_disable()) {
    }

    /**
     * @brief   This restores the IRQ disable bit in the status register
     *          to the value saved during construction of the object
     * @see     irq_disable
     */
    inline ~irq_lock() {
        irq_restore(state);
    }

    irq_lock(irq_lock const& irq) = delete;
    irq_lock(irq_lock const&& irq) = delete;

private:
    unsigned int state;
};
}// namespace riot
#endif /* IRQ_HPP */
/** @} */
