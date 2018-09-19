/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_gpio GPIO
 * @ingroup     drivers_periph
 * @brief       Low-level GPIO peripheral driver
 *
 * This is a basic GPIO (General-purpose input/output) interface to allow
 * platform independent access to a MCU's input/output pins. This interface is
 * intentionally designed to be as simple as possible, to allow for easy
 * implementation and maximum portability.
 *
 * The interface provides capabilities to initialize a pin as output-,
 * input- and interrupt pin. With the API you can basically set/clear/toggle the
 * digital signal at the hardware pin when in output mode. Configured as input you can
 * read a digital value that is being applied to the pin externally. When initializing
 * an external interrupt pin, you can register a callback function that is executed
 * in interrupt context once the interrupt condition applies to the pin. Usually you
 * can react to rising or falling signal flanks (or both).
 *
 * In addition the API provides to set standard input/output circuit modes such as
 * e.g. internal push-pull configurations.
 *
 * All modern micro controllers organize their GPIOs in some form of ports,
 * often named 'PA', 'PB', 'PC'..., or 'P0', 'P1', 'P2'..., or similar. Each of
 * these ports is then assigned a number of pins, often 8, 16, or 32. A hardware
 * pin can thus be described by its port/pin tuple. To access a pin, the
 * @p GPIO_PIN(port, pin) macro should be used. For example: If your platform has
 * a pin PB22, it will be port=1 and pin=22. The @p GPIO_PIN macro should be
 * overridden by a MCU, to allow for efficient encoding of the the port/pin tuple.
 * For example, on many platforms it is possible to `OR` the pin number with the
 * corresponding ports base register address. This allows for efficient decoding
 * of pin number and base address without the need of any address lookup.
 *
 * In case the driver does not define it, the below macro definition is used to
 * simply map the port/pin tuple to the pin value. In that case, predefined GPIO
 * definitions in `RIOT/boards/ * /include/periph_conf.h` will define the selected
 * GPIO pin.
 *
 * @{
 * @file
 * @brief       Low-level GPIO peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_GPIO_H
#define PERIPH_GPIO_H

#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_GPIO_T
/**
 * @brief   GPIO type identifier
 */
typedef unsigned int gpio_t;
#endif

#ifndef GPIO_PIN
/**
 * @brief   Convert (port, pin) tuple to @c gpio_t value
 */
/* Default GPIO macro maps port-pin tuples to the pin value */
#define GPIO_PIN(x,y)       ((gpio_t)((x & 0) | y))
#endif

#ifndef GPIO_EXT_DEV_LOC
/**
 * @brief   Set device ID location in gpio_t when using extensions
 */
#define GPIO_EXT_DEV_LOC    (8*sizeof(gpio_t) - 8)
#endif

#ifndef GPIO_EXT_THRESH
/**
 * @brief   Pin numbers greater than GPIO_EXT_THRESH use extensions
 */
#define GPIO_EXT_THRESH    ((gpio_t)(UINT_MAX) >> 1)
#endif

#ifndef GPIO_EXT_PIN
/**
 * @brief   Convert (device, pin) tuple to @c gpio_t value
 */
#define GPIO_EXT_PIN(x, y) \
    (gpio_t)(~GPIO_EXT_THRESH | (x << GPIO_EXT_DEV_LOC) | GPIO_PIN(0,y))
#endif

#ifndef GPIO_UNDEF
/**
 * @brief   GPIO pin not defined
 */
#define GPIO_UNDEF          (GPIO_EXT_THRESH)
#endif

/**
 * @brief   Available pin modes
 *
 * Generally, a pin can be configured to be input or output. In output mode, a
 * pin can further be put into push-pull or open drain configuration. Though
 * this is supported by most platforms, this is not always the case, so driver
 * implementations may return an error code if a mode is not supported.
 */
#ifndef HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN ,               /**< configure as input without pull resistor */
    GPIO_IN_PD,             /**< configure as input with pull-down resistor */
    GPIO_IN_PU,             /**< configure as input with pull-up resistor */
    GPIO_OUT,               /**< configure as output in push-pull mode */
    GPIO_OD,                /**< configure as output in open-drain mode without
                             *   pull resistor */
    GPIO_OD_PU              /**< configure as output in open-drain mode with
                             *   pull resistor enabled */
} gpio_mode_t;
#endif

/**
 * @brief   Definition of possible active flanks for external interrupt mode
 */
#ifndef HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 0,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 2           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*gpio_cb_t)(void *arg);

/**
 * @brief   Default interrupt context for GPIO pins
 */
#ifndef HAVE_GPIO_ISR_CTX_T
typedef struct {
    gpio_cb_t cb;           /**< interrupt callback */
    void *arg;              /**< optional argument */
} gpio_isr_ctx_t;
#endif

/**
 * @brief   Low-level versions of the GPIO functions
 *
 * These are for cpu gpio.c implementation and should not be called directly.
 * @{
 */
int gpio_init_ll(gpio_t pin, gpio_mode_t mode);
int gpio_init_int_ll(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                     gpio_cb_t cb, void *arg);
void gpio_irq_enable_ll(gpio_t pin);
void gpio_irq_disable_ll(gpio_t pin);
int gpio_read_ll(gpio_t pin);
void gpio_set_ll(gpio_t pin);
void gpio_clear_ll(gpio_t pin);
void gpio_toggle_ll(gpio_t pin);
void gpio_write_ll(gpio_t pin, int value);
/** @} */

/**
 * @brief   Redirecting versions of the GPIO functions
 *
 * These are for the extension interface and should not be called directly.
 * @{
 */
int gpio_init_redir(gpio_t pin, gpio_mode_t mode);
int gpio_init_int_redir(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                        gpio_cb_t cb, void *arg);
void gpio_irq_enable_redir(gpio_t pin);
void gpio_irq_disable_redir(gpio_t pin);
int gpio_read_redir(gpio_t pin);
void gpio_set_redir(gpio_t pin);
void gpio_clear_redir(gpio_t pin);
void gpio_toggle_redir(gpio_t pin);
void gpio_write_redir(gpio_t pin, int value);
/** @} */

/**
 * @brief   Initialize the given pin as general purpose input or output
 *
 * When configured as output, the pin state after initialization is undefined.
 * The output pin's state **should** be untouched during the initialization.
 * This behavior can however **not be guaranteed** by every platform.
 *
 * @param[in] pin       pin to initialize
 * @param[in] mode      mode of the pin, see @c gpio_mode_t
 *
 * @return              0 on success
 * @return              -1 on error
 */
static inline int gpio_init(gpio_t pin, gpio_mode_t mode)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        return gpio_init_redir(pin, mode);
    }
#endif

    return gpio_init_ll(pin, mode);
}

#if defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN)
/**
 * @brief   Initialize a GPIO pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every
 * time the defined flank(s) are detected.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       pin to initialize
 * @param[in] mode      mode of the pin, see @c gpio_mode_t
 * @param[in] flank     define the active flank(s)
 * @param[in] cb        callback that is called from interrupt context
 * @param[in] arg       optional argument passed to the callback
 *
 * @return              0 on success
 * @return              -1 on error
 */
static inline int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t
                                flank, gpio_cb_t cb, void *arg)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        return gpio_init_int_redir(pin, mode, flank, cb, arg);
    }
#endif

    return gpio_init_int_ll(pin, mode, flank, cb, arg);
}

/**
 * @brief   Enable pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       the pin to enable the interrupt for
 */
static inline void gpio_irq_enable(gpio_t pin)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        gpio_irq_enable_redir(pin);
        return;
    }
#endif

    gpio_irq_enable_ll(pin);
}

/**
 * @brief   Disable the pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       the pin to disable the interrupt for
 */
static inline void gpio_irq_disable(gpio_t pin)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        gpio_irq_disable_redir(pin);
        return;
    }
#endif

    gpio_irq_disable_ll(pin);
}

#endif /* defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN) */

/**
 * @brief   Get the current value of the given pin
 *
 * @param[in] pin       the pin to read
 *
 * @return              0 when pin is LOW
 * @return              >0 for HIGH
 */
static inline int gpio_read(gpio_t pin)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        return gpio_read_redir(pin);
    }
#endif

    return gpio_read_ll(pin);
}

/**
 * @brief   Set the given pin to HIGH
 *
 * @param[in] pin       the pin to set
 */
static inline void gpio_set(gpio_t pin)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        gpio_set_redir(pin);
        return;
    }
#endif

    gpio_set_ll(pin);
}

/**
 * @brief   Set the given pin to LOW
 *
 * @param[in] pin       the pin to clear
 */
static inline void gpio_clear(gpio_t pin)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        gpio_clear_redir(pin);
        return;
    }
#endif

    gpio_clear_ll(pin);
}

/**
 * @brief   Toggle the value of the given pin
 *
 * @param[in] pin       the pin to toggle
 */
static inline void gpio_toggle(gpio_t pin)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        gpio_toggle_redir(pin);
        return;
    }
#endif

    gpio_toggle_ll(pin);
}

/**
 * @brief   Set the given pin to the given value
 *
 * @param[in] pin       the pin to set
 * @param[in] value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
static inline void gpio_write(gpio_t pin, int value)
{
#ifdef MODULE_EXTEND_GPIO
    if (pin > GPIO_EXT_THRESH) {
        gpio_write_redir(pin, value);
        return;
    }
#endif

    gpio_write_ll(pin, value);
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_GPIO_H */
/** @} */
