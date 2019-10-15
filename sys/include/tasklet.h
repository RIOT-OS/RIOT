/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_tasklet Tasklets
 * @ingroup     sys
 * @brief       Support for kernel tasklets
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * A tasklet is a function that is scheduled to run in thread context
 * as soon as possible at a kernel-determined safe time.
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
 * The solution is to have a single thread which serializes
 * the interrupts of such driver modules and calls the functions of the driver
 * modules to handle the interrupts from its thread context. This is what
 * tasklets are made for.
 *
 * For this purpose, each driver module that wants to use tasklets for handling
 * IRQ has to define a tasklet of type #tasklet_t
 * for each of its interrupt sources. The tasklet contains a
 * reference to the function to be called to handle the interrupt.
 *
 * When an interrupt of the corresponding source occurs, the ISR of the
 * driver module registers only the tasklet associated with the
 * interrupt source with the #tasklet_schedule function on the handler. The
 * handler places the tasklet in an pending tasklet queue.
 *
 * Each tasklet can be registered on the handler only once. That is,
 * if the same interrupt occurs multiple times, only its first occurrence is
 * placed to the pending tasklets queue and is handled.
 *
 * When the tasklet thread gets the CPU, it processes all pending
 * tasklets events in the order they were scheduled before it yields.
 *
 * ## Usage
 *
 * Tasklets can be used not only for driver modules with bus access, but for
 * any interrupt handling that may trigger a blocking function and therefore
 * cannot be called in an interrupt context. It can also be used to process
 * low layer network events (RX process) or MAC events.
 *
 * @note All tasklets handled by the single tasklets handler thread are
 * serialized.
 *
 * To use tasklets, the used modules have to define a static
 * tasklet of type #tasklet_t for each of their events (e.g interrupt sources).
 * These static tasklets have to be initialized with the static
 * initializer #TASKLET_INIT. Furthermore, the task function
 * and optionally an argument, the context, have to be set.
 *
 * Once the tasklets have been initialized, they can be added to the
 * pending tasklets queue using the #tasklet_schedule function which
 * indicates that the given tasklets should be handled by the OS as soon as
 * possible
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *      #include "foo_device.h"
 *      #include "tasklet.h"
 *
 *      // tasklet structure with static initializer
 *      static tasklet_t _int_event = TASKLET_INIT;
 *
 *      ...
 *
 *      // non blocking ISR just adds the event and returns
 *      static void _int_request(void *arg)
 *      {
 *          tasklet_schedule(&_int_event);
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

#ifndef TASKLET_H
#define TASKLET_H

#include <stdbool.h>

#include "assert.h"
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default priority of the tasklets thread
 *
 * The priority of the taskletthread has to be high enough that all
 * pending tasklets are handled before other threads are executed.
 */
#ifndef TASKLET_HANDLER_PRIO
#define TASKLET_HANDLER_PRIO    0
#endif

/**
 * @brief   Tasklet handling function prototype
 *
 * Defines the prototype of the function that is registered together
 * with a taskletand to be called when the tasklet is handled.
 */
typedef void (*tasklet_task_t)(void *ctx);

/**
 * @brief   Tasklet structure
 *
 * Used modules have to define a structure of this type for each tasklet
 * used by the modules. Structures of this type are used to put
 * them in a pending tasklet queue indicating that a tasklet of the
 * corresponding source has been scheduled and needs to be handled.
 * Each tasklet can only be pending once.
 *
 * Tasklet structures have to be pre-allocated in order to use them.
 */
typedef struct {
    event_t event;          /**< Event structure */
    bool pending;           /**< Indicates whether the same tasklet
                                 is already pending */
    tasklet_task_t task;    /**< Function to be called to handle the tasklet */
    void *ctx;              /**< Context used by the function */
} tasklet_t;

/**
 * @brief Static initializer for #tasklet_t.
 */
#define TASKLET_INIT { \
                            .event.handler = NULL, \
                            .event.list_node.next = NULL, \
                            .pending = false, \
                            .task = NULL, \
                            .ctx = NULL, \
                       }

/**
 * @brief   Initialize a tasklet.
 *
 * Initializes the given tasklet structure.
 *
 * Only use this function for dynamically allocated tasklet
 * structures. For the initialization of static tasklet structures
 * use #TASKLET_INIT instead.
 *
 * @param[out]  tasklet Pre-allocated #tasklet_t structure, must not be NULL
 */

static inline void tasklet_init(tasklet_t *tasklet)
{
    assert(tasklet != NULL);
    tasklet_t tmp = TASKLET_INIT;
    *tasklet = tmp;
}

/**
 * @brief   Add a tasklet to the pending tasklet queue
 *
 * The tasklet given by parameter \p tasklet will be placed at the end of
 * the pending tasklet queue.
 *
 * Each tasklet can be added only once to the pending tasklet queue.
 * That is, if the same event that triggers the tasklet occurs multiple times,
 * only its first occurrence is placed to the pending tasklet queue and is handled.
 *
 * @param[in]   tasklet Preallocated tasklet
 *
 * @retval 0            on success
 * @retval -EALREADY    if the given tasklet is already pending
 */
int tasklet_schedule(tasklet_t *tasklet);

#ifdef __cplusplus
}
#endif

#endif /* TASKLET_H */
/** @} */
