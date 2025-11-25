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
 * @warning The scalar GPIO pin type `gpio_t` is deprecated and will be
 * replaced by a structured GPIO pin type in a future GPIO API. Therefore,
 * don't use the direct comparison of GPIO pins anymore. Instead, use the
 * inline comparison functions @ref gpio_is_equal and @ref gpio_is_valid.
 *
 * # (Low-) Power Implications
 *
 * On almost all platforms, we can only control the peripheral power state of
 * full ports (i.e. groups of pins), but not for single GPIO pins. Together with
 * CPU specific alternate function handling for pins used by other peripheral
 * drivers, this can make it quite complex to keep track of pins that are
 * currently used at a certain moment. To simplify the implementations (and ease
 * the memory consumption), we expect ports to be powered on (e.g. through
 * peripheral clock gating) when first used and never be powered off again.
 *
 * GPIO driver implementations **should** power on the corresponding port during
 * gpio_init() and gpio_init_int().
 *
 * For external interrupts to work, some platforms may need to block certain
 * power modes (although this is not very likely). This should be done during
 * gpio_init_int().
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

#ifndef GPIO_UNDEF
/**
 * @brief   GPIO pin not defined
 */
#define GPIO_UNDEF          ((gpio_t)(UINT_MAX))
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
int gpio_init(gpio_t pin, gpio_mode_t mode);

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
 * @pre     @p cb must not be NULL
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
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg);

/**
 * @brief   Enable pin interrupt if configured as interrupt source
 *
 *          Interrupts that would have occurred after @see gpio_irq_disable
 *          was called will be discarded.
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       the pin to enable the interrupt for
 */
void gpio_irq_enable(gpio_t pin);

/**
 * @brief   Disable the pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] pin       the pin to disable the interrupt for
 */
void gpio_irq_disable(gpio_t pin);

#endif /* defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN) */

/**
 * @brief   Get the current value of the given pin
 *
 * @param[in] pin       the pin to read
 *
 * @return              0 when pin is LOW
 * @return              >0 for HIGH
 */
int gpio_read(gpio_t pin);

/**
 * @brief   Set the given pin to HIGH
 *
 * @param[in] pin       the pin to set
 */
void gpio_set(gpio_t pin);

/**
 * @brief   Set the given pin to LOW
 *
 * @param[in] pin       the pin to clear
 */
void gpio_clear(gpio_t pin);

/**
 * @brief   Toggle the value of the given pin
 *
 * @param[in] pin       the pin to toggle
 */
void gpio_toggle(gpio_t pin);

/**
 * @brief   Set the given pin to the given value
 *
 * @param[in] pin       the pin to set
 * @param[in] value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
void gpio_write(gpio_t pin, int value);

/**
 * @brief   Test if a GPIO pin is equal to another GPIO pin
 *
 * @param[in] gpio1 First GPIO pin to check
 * @param[in] gpio2 Second GPIO pin to check
 */
static inline int gpio_is_equal(gpio_t gpio1, gpio_t gpio2)
{
    return (gpio1 == gpio2);
}

/**
 * @brief   Test if a GPIO pin is a valid pin and not declared as undefined.
 *
 * @param[in] gpio GPIO pin to check
 */
static inline int gpio_is_valid(gpio_t gpio)
{
    return (gpio != GPIO_UNDEF);
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_GPIO_H */
/** @} */
