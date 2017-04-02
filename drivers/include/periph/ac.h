/*
 * Copyright (C) 2017 RWTH-Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_ac Analog Comparator
 * @ingroup     drivers_periph
 * @brief       Low-level Analog Comparator peripheral driver interface
 *
 *
 * @{
 * @file
 * @brief       Low-level Analog Comparator peripheral driver interface definitions
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef AC_H
#define AC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default ac_t type definition
 * @{
 */
#ifndef HAVE_ac_T
typedef unsigned int ac_t;
#endif

typedef enum {
	AC_COMPARE_OUTPUT = 0, /* Normal mode of operation, output in register  */
	AC_INPUT_CAPTURE, /*Input Capture used, Output is forwarded to a Timer */
}ac_output_mode_t;

typedef enum {
	AC_IRQ_DISABLED = 0,
	AC_IRQ_TOGGLE,
	AC_IRQ_FALLING_EDGE,
	AC_IRQ_RISING_EDGE,
}ac_irq_mode_t;


/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 * @param[in] dev		the device which received the interrupt
 */
typedef void (*ac_cb_t)(void *arg, uint8_t dev);

/**
 * @brief   Default interrupt context entry holding callback and argument
 * @{
 */
typedef struct {
    ac_cb_t cb;          /** callback executed from comparator interrupt */
    void *arg;              /** optional argument given to that callback */
} ac_isr_ctx_t;
/** @} */

/**
 * @brief Initialize the analog comparator, especially the interrupts
 *
 * @param[in] dev           	comparator to use
 * @param[in] output_mode		Select behaviour of the output
 * @param[in] irq_mode			Select when to call the irq
 */
int ac_init(ac_t dev, ac_output_mode_t output_mode, ac_irq_mode_t irq_mode, ac_isr_ctx_t ctx);


/**
 * @brief gets actual compare value
 *
 * @param[in] dev           comparator to use
 * @return		the actual compare value (0 or 1)
 */
uint8_t ac_read(ac_t dev);

/**
 * @brief Enable the interrupts for the given comparator
 *
 * @param[in] dev           comparator to enable interrupts for
 */
void ac_irq_enable(ac_t dev);

/**
 * @brief Disable interrupts for the given comparator
 *
 * @param[in] dev           comparator to disable interrupts for
 */
void ac_irq_disable(ac_t dev);

/**
 * @brief Powers on The analog Comparator
 *
 * @param[in] dev			the device thats being powered on
 */
void ac_poweron(ac_t dev);

/**
 * @brief Powers off The analog Comparator
 *
 * @param[in] dev			the device thats being powered off
 */
void ac_poweroff(ac_t dev);

#ifdef __cplusplus
}
#endif

#endif /* AC_H */
/** @} */
