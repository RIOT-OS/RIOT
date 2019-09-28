/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *               2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_extend_gpio GPIO extension
 * @ingroup     drivers_extend
 * @brief       GPIO peripheral extension handling
 *
 * The GPIO extension interface makes handling of non-CPU GPIO devices invisible
 * to code using the GPIO peripheral API (periph/gpio.h). This is accomplished by
 * reserving part of the range of values of @ref gpio_t. When a call to the
 * GPIO API uses a pin that falls within this range, it is parsed into a device
 * number that is looked up in a GPIO extension device list and the call is
 * redirected to the corresponding device.
 *
 * The GPIO extension interface uses @ref gpio_t values of following structure:
 *
 * ```
 *  MSB                                                                          LSB
 * +-----+---+------------------------------------------+---------------------------+
 * |     | 1 |            device number                 |          pin number       |
 * +-----+---+------------------------------------------+---------------------------+
 *         ^
 *         | |<-- (GPIO_EXT_BIT - GPIO_EXT_PIN_BITS) -->|<-- (GPIO_EXT_PIN_BITS) -->|
 *         |
 *   GPIO_EXT_BIT
 * ```
 *
 * The @ref GPIO_EXT_PIN is set to one to identify the @ref gpio_t value as GPIO
 * extension pin. @ref GPIO_EXT_PIN_BITS defines the number of bits that are
 * used to address the pin of a GPIO extension device in a @ref gpio_t value.
 * It also defines the number of bits by which the GPIO extension device
 * number is shifted to the left in a @ref gpio_t value. Thus, the resulting
 * number of bits that can be used to address a GPIO extension device is
 * `(GPIO_EXT_BIT - GPIO_EXT_PIN_BITS)`.
 *
 * The default value of @ref GPIO_EXT_PIN is `sizeof(gpio_t)*8 -1`. That is,
 * the MSB is used. The default value of @ref GPIO_EXT_PIN_BITS is 5 to be
 * able to use @ref gpio_t values with a size of 8 bit. Thus, the number of
 * pins that can be addressed for one GPIO extension device is 32. If a board
 * uses GPIO extension devices with more than 32 pins, its board definition has
 * to override at least @ref GPIO_EXT_PIN_BITS and @ref GPIO_EXT_PIN_MASK.
 *
 * The following requirements have to be met in order to use the GPIO
 * extension interface
 *
 * - The board has to provide a list of GPIO extension devices
 *   of type @ref gpio_ext_dev_t as a configuration in file
 *   `boards/.../include/gpio_ext_conf.h`. Please refer gpio_ext_conf.h
 *   for more information and @ref gpio_ext as an example.
 *
 * - GPIO extension device drivers for GPIO extender hardware modules have to
 *   implement a set of functions to be compatible with the GPIO extension
 *   interface. Please refer @ref gpio_ext_driver for more information.
 *
 * @{
 *
 * @file
 * @brief       GPIO extension interface definitions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef EXTEND_GPIO_H
#define EXTEND_GPIO_H

#include <limits.h>
#include <stdint.h>

#include "assert.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GPIO_EXT_BIT
/**
 * @brief   The bit that identifies a @ref gpio_t value as GPIO extension pin
 *
 * @ref GPIO_EXT_BIT also defines the lowest @ref gpio_t value for GPIO
 * extension pins, see @ref GPIO_EXT_BASE.
 *
 * By default, the MSB of @ref gpio_t used.
 */
#define GPIO_EXT_BIT        ((8 * sizeof(gpio_t)) - 1)
#endif

#ifndef GPIO_EXT_PIN_BITS
/**
 * @brief   Number of bits in @ref gpio_t values used to address the pin of
 *          a GPIO extension device
 *
 * Defines the number of bits that are used to address the pin of a GPIO
 * extension device in @ref gpio_t values. It also defines the number of bits
 * by which the GPIO extension device number is shifted to the left in
 * @ref gpio_t values. Thus, the resulting number of bits that can be used
 * to address a GPIO extension device is `(GPIO_EXT_BIT - GPIO_EXT_PIN_BITS)`
 *
 * @note    @ref GPIO_EXT_PIN_BITS has to be equal to the length of the
 *          right-aligned 1-bit sequence in @ref GPIO_EXT_PIN_MASK.
 */
#define GPIO_EXT_PIN_BITS  (5)
#endif

#ifndef GPIO_EXT_PIN_MASK
/**
 * @brief   Mask for GPIO extension pin number in @ref gpio_t values
 *
 * @note    The length of the right-aligned 1-bit sequence has to be equal
 *          to @ref GPIO_EXT_PIN_BITS.
 */
#define GPIO_EXT_PIN_MASK   (0x1f)
#endif

/**
 * @brief   Lowest @ref gpio_t value of GPIO extension pins
 *
 * GPIO pins greater than or equal to GPIO_EXT_BASE are GPIO extension pins.
 */
#define GPIO_EXT_BASE       ((gpio_t)(1 << GPIO_EXT_BIT))

/**
 * @brief   Test whether a @ref gpio_t value refers a GPIO extension pin
 * @param   pin     pin value of type @ref gpio_t
 */
#define GPIO_EXT_IS(pin)    (pin & GPIO_EXT_BASE)

/**
 * @brief   Convert the (dev, num) tuple of a GPIO extension pin to
 *          corresponding @ref gpio_t value
 * @param   dev     GPIO extension device number
 * @param   num     Pin of the GPIO extension device
 */
#define GPIO_EXT_PIN(dev, num)  \
                            ((gpio_t)(GPIO_EXT_BASE | \
                                     ((dev << GPIO_EXT_DEV_SHIFT) & GPIO_EXT_DEV_MASK) | \
                                      (num & GPIO_EXT_PIN_MASK)))

/**
 * @brief   Shift of GPIO extension device number in @ref gpio_t values
 *
 * Defines the number of bits by which the GPIO extension device number
 * is shifted to the left in the @ref gpio_t value. It is equal to the
 * number of bits used to address a pin of a GPIO extension device, see
 * @ref GPIO_EXT_PIN_BITS.
 */
#define GPIO_EXT_DEV_SHIFT      (GPIO_EXT_PIN_BITS)

/**
 * @brief   Mask for GPIO extension device number in @ref gpio_t values
 */
#define GPIO_EXT_DEV_MASK       (~GPIO_EXT_BASE & ~GPIO_EXT_PIN_MASK)

/**
 * @brief   Extract the GPIO extension device number from given @ref gpio_t value
 * @param   pin value of type @ref gpio_t
 */
#define GPIO_EXT_PIN_DEV(pin)   ((gpio_t)((pin & GPIO_EXT_DEV_MASK) >> GPIO_EXT_DEV_SHIFT))

/**
 * @brief   Extract the GPIO extension pin number from given @ref gpio_t value
 * @param   pin value of type @ref gpio_t
 */
#define GPIO_EXT_PIN_NUM(pin)   ((gpio_t)((pin & GPIO_EXT_PIN_MASK)))

/**
 * @brief   Get GPIO extension device of type @ref gpio_ext_dev_t for given
 *          @ref gpio_t value
 * @param   pin value of type @ref gpio_t
 */
#define GPIO_EXT_DEV(pin)       (gpio_ext[GPIO_EXT_PIN_DEV(pin)])

/**
 * @brief   Number of configured GPIO extension devices
 */
#define GPIO_EXT_DEV_NUMOF      (ARRAY_SIZE(gpio_ext))

/**
 * @name    GPIO extension device driver functions
 *
 * GPIO extension device drivers have to implement the following functions
 * to be compatible with the GPIO extension interface.
 *
 * @{
 */

/**
 * @brief   Callback typedef for @ref gpio_init
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_init
 */
typedef int (*gpio_ext_init_t)(void *dev, gpio_t pin, gpio_mode_t mode);

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN

/**
 * @brief   Callback typedef for @ref gpio_init_int
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_init_int
 */
typedef int (*gpio_ext_init_int_t)(void *dev, gpio_t pin, gpio_mode_t mode,
                                   gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief   Callback typedef for @ref gpio_irq_enable
 * @param   dev Pointer to extension device descriptor
 * @see @ref gpio_irq_enable
 */
typedef void (*gpio_ext_irq_enable_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for @ref gpio_irq_disable
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_irq_disable
 */
typedef void (*gpio_ext_irq_disable_t)(void *dev, gpio_t pin);

#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

/**
 * @brief   Callback typedef for @ref gpio_read
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_read
 */
typedef int (*gpio_ext_read_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for @ref gpio_set
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_set
 */
typedef void (*gpio_ext_set_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for @ref gpio_clear
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_clear
 */
typedef void (*gpio_ext_clear_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for @ref gpio_toggle
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_toggle
 */
typedef void (*gpio_ext_toggle_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for @ref gpio_write
 * @param   dev Pointer to extension device descriptor
 * @see     @ref gpio_write
 */
typedef void (*gpio_ext_write_t)(void *dev, gpio_t pin, int value);

/** @} */

/**
 * @brief   GPIO extension driver type
 */
typedef struct gpio_ext_driver {
    gpio_ext_init_t init;                 /**< callback for @ref gpio_init */
#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
    gpio_ext_init_int_t init_int;         /**< callback for @ref gpio_init_int */
    gpio_ext_irq_enable_t irq_enable;     /**< callback for @ref gpio_irq_enable */
    gpio_ext_irq_disable_t irq_disable;   /**< callback for @ref gpio_irq_disable */
#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */
    gpio_ext_read_t read;                 /**< callback for @ref gpio_read */
    gpio_ext_set_t set;                   /**< callback for @ref gpio_set */
    gpio_ext_clear_t clear;               /**< callback for @ref gpio_clear */
    gpio_ext_toggle_t toggle;             /**< callback for @ref gpio_toggle */
    gpio_ext_write_t write;               /**< callback for @ref gpio_write */
} gpio_ext_driver_t;

/**
 * @brief   GPIO extension device type
 */
typedef struct gpio_ext_dev {
    const gpio_ext_driver_t *driver;    /**< pointer to extension device driver */
    void *dev;                          /**< pointer to extension device descriptor */
} gpio_ext_dev_t;

#include "gpio_ext_conf.h"

/**
 * @name    GPIO API functions realized by the GPIO extension interface.
 * @{
 */

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_init
 */
static inline int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /*
     * Check that the GPIO_EXT_BIT is inside the gpio_t size.
     */
    _Static_assert(GPIO_EXT_BIT < (sizeof(gpio_t) * 8),
                   "GPIO_EXT_BIT has to be less than sizeof(gpio_t)*8");
    /*
     * Check that the gpio_t value has at least one bit left for the device
     * number to address GPIO extension devices.
     */
    _Static_assert(GPIO_EXT_PIN_BITS < GPIO_EXT_BIT,
                   "GPIO_EXT_PIN_BITS has to be less than GPIO_EXT_BIT");
    /*
     * Check that the number of bits used for the device number is large enough
     * to address all extension devices in the device list.
     */
    _Static_assert((1 << (GPIO_EXT_BIT - GPIO_EXT_PIN_BITS)) >= GPIO_EXT_DEV_NUMOF,
                   "Number of bits for GPIO extension device number is too small");

    if (GPIO_EXT_IS(pin)) {
       assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
       return GPIO_EXT_DEV(pin).driver->init(GPIO_EXT_DEV(pin).dev,
                                             GPIO_EXT_PIN_NUM(pin), mode);
    }
#if MODULE_PERIPH_GPIO
    return gpio_cpu_init(pin, mode);
#endif
    return -1;
}

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_init_int
 */
static inline int gpio_init_int(gpio_t pin, gpio_mode_t mode,
                                gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    /*
     * Check that the GPIO_EXT_BIT is inside the gpio_t size.
     */
    _Static_assert(GPIO_EXT_BIT < (sizeof(gpio_t) * 8),
                   "GPIO_EXT_BIT has to be less than sizeof(gpio_t)*8");
    /*
     * Check that the gpio_t value has at least one bit left for the device
     * number to address GPIO extension devices.
     */
    _Static_assert(GPIO_EXT_PIN_BITS < GPIO_EXT_BIT,
                   "GPIO_EXT_PIN_BITS has to be less than GPIO_EXT_BIT");
    /*
     * Check that the number of bits used for the device number is large enough
     * to address all extension devices in the device list.
     */
    _Static_assert((1 << (GPIO_EXT_BIT - GPIO_EXT_PIN_BITS)) >= GPIO_EXT_DEV_NUMOF,
                   "Number of bits for GPIO extension device number is too small");

    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        return GPIO_EXT_DEV(pin).driver->init_int(GPIO_EXT_DEV(pin).dev,
                                                  GPIO_EXT_PIN_NUM(pin),
                                                  mode, flank, cb, arg);
    }
#ifdef MODULE_PERIPH_GPIO
    return gpio_cpu_init_int(pin, mode, flank, cb, arg);
#endif
    return -1;
}

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_irq_enable
 */
static inline void gpio_irq_enable(gpio_t pin)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        GPIO_EXT_DEV(pin).driver->irq_enable(GPIO_EXT_DEV(pin).dev,
                                             GPIO_EXT_PIN_NUM(pin));
        return;
    }
#ifdef MODULE_PERIPH_GPIO
    gpio_cpu_irq_enable(pin);
#endif
}

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_irq_disable
 */
static inline void gpio_irq_disable(gpio_t pin)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        GPIO_EXT_DEV(pin).driver->irq_disable(GPIO_EXT_DEV(pin).dev,
                                              GPIO_EXT_PIN_NUM(pin));
        return;
    }
#ifdef MODULE_PERIPH_GPIO
    gpio_cpu_irq_disable(pin);
#endif
}

#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_read
 */
static inline int gpio_read(gpio_t pin)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        return GPIO_EXT_DEV(pin).driver->read(GPIO_EXT_DEV(pin).dev,
                                              GPIO_EXT_PIN_NUM(pin));
    }
#ifdef MODULE_PERIPH_GPIO
    return gpio_cpu_read(pin);
#endif
    return 0;
}

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_set
 */
static inline void gpio_set(gpio_t pin)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        GPIO_EXT_DEV(pin).driver->set(GPIO_EXT_DEV(pin).dev,
                                      GPIO_EXT_PIN_NUM(pin));
        return;
    }
#ifdef MODULE_PERIPH_GPIO
    gpio_cpu_set(pin);
#endif
}

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_clear
 */
static inline void gpio_clear(gpio_t pin)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        GPIO_EXT_DEV(pin).driver->clear(GPIO_EXT_DEV(pin).dev,
                                        GPIO_EXT_PIN_NUM(pin));
        return;
    }
#ifdef MODULE_PERIPH_GPIO
    gpio_cpu_clear(pin);
#endif
}

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_toggle
 */
static inline void gpio_toggle(gpio_t pin)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        GPIO_EXT_DEV(pin).driver->toggle(GPIO_EXT_DEV(pin).dev,
                                         GPIO_EXT_PIN_NUM(pin));
        return;
    }
#ifdef MODULE_PERIPH_GPIO
    gpio_toggle(pin);
#endif
}

/**
 * @brief   Implementation by the GPIO extension interface, see @ref gpio_write
 */
static inline void gpio_write(gpio_t pin, int value)
{
    if (GPIO_EXT_IS(pin)) {
        assert(GPIO_EXT_PIN_DEV(pin) < ARRAY_SIZE(gpio_ext));
        GPIO_EXT_DEV(pin).driver->write(GPIO_EXT_DEV(pin).dev,
                                        GPIO_EXT_PIN_NUM(pin), value);
        return;
    }
#ifdef MODULE_PERIPH_GPIO
    gpio_cpu_write(pin, value);
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EXTEND_GPIO_H */
/** @} */
