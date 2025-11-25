/*
 * Copyright (C) 2020 Gunar Schorcht
 *               2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_gpio_ll_irq  IRQ Support in Peripheral GPIO Low-Level API
 * @ingroup     drivers_periph_gpio_ll
 * @brief       IRQ Support in Peripheral GPIO Low-Level API
 *
 * @warning     This API is not stable yet and intended for internal use only
 *              as of now.
 * @note        This API can only be used, if the features `periph_gpio_ll_irq`
 *              is used. Not every MCU will implement the features required for
 *              this API or will have a suitable driver
 *              available.
 *
 * # Motivation
 *
 * The IRQ related GPIO Low-Level API is very similar to the one of @ref
 * drivers_periph_gpio - however two aspects have been changed (in addition to
 * using separate port and pin parameters): First, level triggered IRQs that
 * many MCUs support are now exposed. (Note: They can also be trivially be
 * emulated, see the STM32 implementation as example.) Second, the
 * configuration of the GPIO pin and the associated IRQ have been split into
 * two functions. This allows reconfiguring the trigger of an IRQ with less
 * overhead. This can result in reconfiguring the IRQ trigger being less effort
 * than filtering out unneeded events and hence consume power due to longer
 * power saving phases.
 *
 * @{
 * @file
 * @brief       IRQ Support in Peripheral GPIO Low-Level API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @warning     This API is not stable yet and intended for internal use only
 *              as of now.
 */

#ifndef PERIPH_GPIO_LL_IRQ_H
#define PERIPH_GPIO_LL_IRQ_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(HAVE_GPIO_IRQ_TRIG_T) || defined(DOXYGEN)
/**
 * @brief   Definition of possible IRQ triggers
 */
typedef enum {
    GPIO_TRIGGER_EDGE_FALLING,  /**< edge triggered IRQ on falling flanks only
                                  */
    GPIO_TRIGGER_EDGE_RISING,   /**< edge triggered IRQ on rising flanks only */
    GPIO_TRIGGER_EDGE_BOTH,     /**< edge triggered IRQ on falling *AND* rising
                                     flanks */
    GPIO_TRIGGER_LEVEL_HIGH,    /**< level triggered IRQ on high input */
    GPIO_TRIGGER_LEVEL_LOW      /**< level triggered IRQ on low input */
} gpio_irq_trig_t;
#endif

/**
 * @brief   Signature of event callback function that is called on IRQs
 * @param[in] arg       optional context for the callback
 * @warning This function is called in IRQ context
 */
typedef void (*gpio_ll_cb_t)(void *arg);

/**
 * @brief   Set up an IRQ for the given GPIO pin and activate it
 *
 * @param[in] port      port the pin belongs to
 * @param[in] pin       number of the pin to setup IRQs on
 * @param[in] trig      define what triggers the IRQ
 * @param[in] cb        callback that is called from interrupt context
 * @param[in] arg       argument to be passed to the callback
 *
 * @pre     The given GPIO pin has been initialized using
 *          @ref gpio_ll_init prior to this call
 * @pre     The trigger in @p trig is supported by the MCU
 *
 * @retval  0           Success
 * @retval  -ENOTSUP    No external IRQs supported for @p port and @p pin
 * @retval  -EBUSY      All hardware entities that monitor GPIOs and issue IRQs
 *                      are busy monitoring other GPIOs. Call
 *                      @ref gpio_ll_irq_off on one of those and retry.
 */
int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig,
                gpio_ll_cb_t cb, void *arg);

/**
 * @brief   Mask IRQs on the given GPIO pin
 *
 * @param[in] port      port the pin belongs to
 * @param[in] pin       number of the pin to disable IRQs on
 *
 * @note    IRQs can be re-enabled without reconfiguration using
 *          @ref gpio_ll_irq_unmask
 *
 * If IRQs are not needed for a longer term, consider using @ref
 * gpio_ll_irq_off instead.
 */
void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin);

#if MODULE_PERIPH_GPIO_LL_IRQ_UNMASK || defined(DOXYGEN)
/**
 * @brief   Unmask IRQs on the given GPIO pin
 *
 * Same as @ref gpio_ll_irq_unmask_and_clear except that IRQs that came in while
 * masked are not lost.
 *
 * @warning On some MCUs (most notably STM32) this is impossible to implement.
 *          The feature `periph_gpio_ll_irq_unmask` is provided, if this
 *          function is available.
 *
 * @ref gpio_ll_irq_unmask_and_clear is a portable alternative that might suit your
 * use case equally well.
 *
 * @param[in] port      port the pin belongs to
 * @param[in] pin       number of the pin to unmask IRQs on
 */
void gpio_ll_irq_unmask(gpio_port_t port, uint8_t pin);
#endif

/**
 * @brief   Unmask IRQs on the given GPIO pin and clear pending IRQs
 *
 * @pre     IRQs on the given pin have been previously configured via
 *          @ref gpio_ll_irq
 * @post    IRQs will be acted upon again from now on. If one or more IRQs came
 *          in while masked, this IRQs are silently lost.
 *
 * @param[in] port      port the pin belongs to
 * @param[in] pin       number of the pin to unmask IRQs on
 */
void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin);

/**
 * @brief   Disable IRQs on the given GPIO pin
 *
 * @post    Signals on the specified pin will no longer triggers IRQs until an
 *          IRQ is reconfigured using @ref gpio_ll_irq
 *
 * @param[in] port      port the pin belongs to
 * @param[in] pin       number of the pin disable IRQs on
 *
 * @note    The implementation should consume power by disabling trigger
 *          detection on the given pin.
 *
 * Unlike @ref gpio_ll_irq_mask IRQs cannot be re-enabled with a call to
 * @ref gpio_ll_irq_unmask.
 */
void gpio_ll_irq_off(gpio_port_t port, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_GPIO_LL_IRQ_H */
/** @} */
