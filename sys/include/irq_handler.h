/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_irq_handler Interrupt handler thread
 * @ingroup     sys
 * @brief       Single thread for handling interrupts that may trigger blocking
 *              functions and therefore may only be called in thread context.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * ## Interrupt Context Problem
 *
 * There are many devices connected to the host CPU via a bus system such as
 * I2C or SPI. Such devices are, for example, sensors and actuators. Since
 * these devices share the bus system, their access to the bus system has
 * to be synchronized.
 *
 * In addition, such devices often use interrupts to trigger the execution
 * of certain functions, such as reading the sensor values. That is, when an
 * interrupt occurs, the driver requires often access to the bus system, for
 * example, to read the status registers.
 *
 * The access to SPI and I2C interfaces is synchronized by mutual exclusion
 * using mutexes. If one thread tries to access such an interface that is
 * already being used by another thread, it will be blocked until the interface
 * becomes available. Although this synchronization works in the thread
 * context, it does not work in the interrupt context. Accessing such an
 * interface within an ISR would interfere with an already existing interface
 * access. This problem is called [interrupt context problem]
 * (http://api.riot-os.org/group__drivers__netdev__api.html).
 *
 * The only solution to this problem is *not to call any function that
 * interacts with a device directly from interrupt context*. Rather, the ISR
 * should only indicate the occurrence of the interrupt. The interrupt is
 * then handled asynchronously by a normal function within a thread context.
 *
 * The problem now is that driver modules usually do not use their own thread,
 * but run in the context of the calling thread. However, it can not be left
 * to the application thread to handle the interrupts of driver modules. The
 * only solution would be to have a separate interrupt handler thread for each
 * driver module that uses interrupts along with SPI or I2C interfaces.
 * However, as the number of such modules increases, many resources (thread
 * contexts, including their thread stacks) are allocated only for interrupt
 * handling.
 *
 * ## Solution
 *
 * The solution is to have a single interrupt handler thread which serializes
 * the interrupts of such driver modules and calls the functions of the driver
 * modules to handle the interrupts from its thread context.
 *
 * For this purpose, each driver module that wants to use this interrupt
 * handler thread has to define an interrupt event of type #irq_event_t
 * for each of its interrupt sources. The interrupt event contains a
 * reference to the function to be called to handle the interrupt.
 *
 * When an interrupt of the corresponding source occurs, the ISR of the
 * driver module registers only the interrupt event associated with the
 * interrupt source with the #irq_event_add function on the handler. The
 * handler places the interrupt event in an pending interrupt queue.
 *
 * Each interrupt event can be registered on the handler only once. That is,
 * if the same interrupt occurs multiple times, only its first occurrence is
 * placed to the pending interrupt queue and is handled.
 *
 * When the interrupt handler thread gets the CPU, it processes all pending
 * interrupt events in the order of their occurrence before it yields.
 *
 * ## Usage
 *
 * The single-interrupt handler thread can be used not only for driver
 * modules with bus access, but for any interrupt handling that may
 * trigger a blocking function and therefore cannot be called in an
 * interrupt context.
 *
 * @note All interrupts handled by the single interrupt handler thread are
 * serialized.
 *
 * To use the interrupt handler thread, using modules have to define a static
 * interrupt event of type #irq_event_t for each of their interrupt sources.
 * These static interrupt events have to be initialized with the static
 * initializer #IRQ_EVENT_INIT. Furthermore, the interrupt handling function
 * and optionally an argument, the context, have to be set.
 *
 * Once the interrupt events have been initialized, they can be added to the
 * pending interrupts queue of the interrupt handler thread by an ISR using
 * the #irq_event_add function which indicates that an interrupt has occurred
 * and needs to be handled.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *      #include "foo_device.h"
 *      #include "irq_handler.h"
 *
 *      // interrupt event structure with static initializer
 *      static irq_event_t _int_event = IRQ_EVENT_INIT;
 *
 *      ...
 *
 *      // non blocking ISR just adds the event and returns
 *      static void _int_request(void *arg)
 *      {
 *          irq_event_add(&_int_event);
 *      }
 *
 *      // example handler for the interrupt including blocking functions
 *      static void _int_handler(void *ctx)
 *      {
 *          foo_device_t dev = (foo_device_t*)ctx;
 *          uint8_t status;
 *
 *          // blocking access to the I2C
 *          i2c_aquire(dev->i2c_device);
 *          i2c_read_reg(dev->i2c_device, FOO_DEVICE_REG_STATUS, &status, 1);
 *          i2c_release(dev->i2c_device);
 *
 *          // application thread callbacks
 *          switch (status) {
 *              case FOO_INT_TYPE1:  dev->int_type1.isr(dev->int_type1.arg);
 *                                   break;
 *              case FOO_INT_TYPE2:  dev->int_type2.isr(dev->int_type2.arg);
 *                                   break;
 *              ...
 *          }
 *          ...
 *      }
 *
 *      ...
 *
 *      int foo_device_init(foo_device_t *dev, foo_device_params_t *params)
 *      {
 *          // set the handler for the interrupt
 *          _int_event.isr = _int_handler;
 *          _int_event.ctx = (void*)dev;
 *
 *          // initialize the GPIO with ISR
 *          gpio_init_int(GPIO_PIN(0, 1), GPIO_IN, GPIO_BOTH, int_request, 0);
 *          ...
 *      }
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 * @file
 */

#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

#include <stdbool.h>

#include "assert.h"
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default priority of the interrupt handler thread
 *
 * The priority of the interrupt handler thread has to be high enough that all
 * pending interrupts are handled before other threads are executed.
 */
#ifndef IRQ_HANDLER_PRIO
#define IRQ_HANDLER_PRIO    0
#endif

/**
 * @brief   Interrupt handling function prototype
 *
 * Defines the prototype of the function that is registered together
 * with an interrupt event and to be called when the interrupt is handled.
 */
typedef void (*irq_isr_t)(void *ctx);

/**
 * @brief   Interrupt event structure
 *
 * Using modules have to define a structure of this type for each interrupt
 * source used by the modules. Structures of this type are used to put
 * them in a pending interrupt queue indicating that an interrupt of the
 * corresponding source has occurred and needs to be handled. Each interrupt
 * event can only be pending once.
 *
 * Interrupt event structures have to be pre-allocated to use them.
 */
typedef struct {
    event_t event;      /**< Event structure */
    bool pending;       /**< Indicates whether the same interrupt request event
                             is already pending */
    irq_isr_t isr;      /**< Function to be called to handle the interrupt */
    void *ctx;          /**< Context used by the function */
} irq_event_t;

/**
 * @brief Static initializer for #irq_event_t.
 */
#define IRQ_EVENT_INIT { \
                            .event.handler = NULL, \
                            .event.list_node.next = NULL, \
                            .pending = false, \
                            .isr = NULL, \
                            .ctx = NULL, \
                       }

/**
 * @brief   Initialize an interrupt event
 *
 * Initializes the given interrupt event structure.
 *
 * Only use this function for dynamically allocated interrupt event
 * structures. For the initialization of static interrupt event structures
 * use #IRQ_EVENT_INIT instead.
 *
 * @param[out]  irq     Pre-allocated #irq_event_t structure, must not be NULL
 */

static inline void irq_event_init(irq_event_t *irq)
{
    assert(irq != NULL);
    irq_event_t tmp = IRQ_EVENT_INIT;
    *irq = tmp;
}

/**
 * @brief   Add an interrupt event to the pending interrupt queue
 *
 * The interrupt event given by parameter \p irq will be placed at the end of
 * the pending interrupt queue.
 *
 * Each interrupt event can be added only once to the pending interrupt queue.
 * That is, if the same interrupt occurs multiple times, only its first
 * occurrence is placed to the pending interrupt queue and is handled.
 *
 * @param[in]   irq     Preallocated interrupt event
 *
 * @retval 0            on success
 * @retval -EALREADY    if the given interrupt event is already pending
 */
int irq_event_add(irq_event_t *irq);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_HANDLER_H */
/** @} */
