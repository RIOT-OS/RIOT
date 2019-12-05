/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef PERIPH_GPIO_EXT_H
#define PERIPH_GPIO_EXT_H

/**
 * @defgroup    drivers_periph_gpio_ext GPIO
 * @ingroup     drivers_periph
 * @brief       GPIO peripheral driver
 *
 * @note This is the new GPIO API which allows consistent access to any kind
 * of GPIO hardware. The GPIO ports of the MCU can thus be extended with GPIO
 * extenders and used in the same way. The new GPIO API defines a pin-oriented
 * high-level API and a port-oriented low-level API. The high-level API is
 * compatible with the legacy GPIO API (@ref drivers_periph_gpio).
 * However, it uses the functions of the new low-level API, which must also be
 * implemented by the MCU. The new GPIO API can therefore only be used if
 * the MCU implements the new low-level API.
 *
 * The interface provides capabilities to initialize a pin as output-,
 * input- and interrupt pin. With the API you can basically set/clear/toggle
 * the digital signal at the hardware pin when in output mode. Configured as
 * input you can read a digital value that is being applied to the pin
 * externally. When initializing an external interrupt pin, you can register
 * a callback function that is executed in interrupt context once the interrupt
 * condition applies to the pin. Usually you can react to rising or falling
 * signal flanks (or both).
 *
 * In addition the API provides to set standard input/output circuit modes
 * such as e.g. internal push-pull configurations.
 *
 * All modern micro controllers organize their GPIOs in some form of ports,
 * often named `PA`, `PB`, `PC`..., or `P0`, `P1`, `P2`..., or similar. Each
 * of these ports is then assigned a number of pins, often 8, 16, or 32. A
 * hardware pin can thus be described by its port/pin tuple. To access a pin,
 * the #GPIO_PIN(port, pin) macro should be used. For example: If your
 * platform has a pin PB22, it will be port=1 and pin=22.
 *
 * ## Implementation
 *
 * The GPIO interface is divided into a low-level API and a high-level API.
 *
 * ### Low-Level API
 *
 * The low-level API provides functions for port-oriented access to the GPIOs.
 * It has to be implemented by every hardware component that provides GPIOs.
 * The functions of the low-level API are called via a driver of type
 * #gpio_driver_t. This driver contains references to these functions of the
 * respective hardware component.
 *
 * Each MCU has to implement this low-level API, see #gpio_driver_t. The MCU
 * functions of the low-level API must follow a special naming scheme where
 * all function names have the prefix `gpio_cpu_*`. These functions are used
 * for the #gpio_cpu_driver, which provides the access to the GPIO ports of
 * the MCU.
 *
 * ### High Level API
 *
 * The high-level API is used by the application and provides a pin-oriented
 * access to the GPIO pins. It uses the functions of the low-level API for
 * this purpose.
 *
 * ### Ports and Port Table
 *
 * All existing ports of the MCU and the optional GPIO extenders are held
 * in a port table #gpio_ports. This table is created automatically from the
 * #GPIO_CPU_PORTS and `GPIO_EXT_PORTS` macros. The entries of the port table
 * are of type #gpio_port_table_t and contain a port definition of type
 * #gpio_port_t. The port definition can be either
 *
 * - the register address of a MCU port,
 * - the address of a device structure of a GPIO extender device or
 * - the port number that corresponds to its index in the port table.
 *
 * The respective implementation of the low-level API determines which
 * representation of a port is used.
 *
 * If GPIO extenders are enabled with module `extend_gpio`, each port table
 * entry also includes a reference to a driver of type #gpio_driver_t driver.
 * The driver contains the references to the low-level GPIO API functions of
 * the respective device and enables access to the GPIO port. In case of CPU
 * ports, the driver is set to #gpio_cpu_driver.
 *
 * ### GPIOs
 *
 * GPIO pins are of type #gpio_t and are defined as a tuple of the port
 * and the pin number of this port. The port is just the address of the
 * corresponding entry in the port table #gpio_ports.
 *
 * GPIOs of type #gpio_t are defined with the #GPIO_PIN(port,pin) macro.
 * The #GPIO_UNDEF macro defines a GPIO pin as undefined.
 *
 * The #GPIO_PORT(gpio) macro can be used to get the corresponding port of
 * type #gpio_port_t from the port table for a given GPIO pin of type @ref
 * gpio_t.
 *
 * ### GPIO extender
 *
 * The use of GPIO extenders is enabled by the `extend_gpio` module. If
 * the `extend_gpio` module is enabled, either the board definition or the
 * application must provide the configuration of the GPIO extenders in file
 * `gpio_ext_conf.h`. For this purpose the macro `GPIO_EXT_PORTS` has to be
 * defined.
 *
 * For example, if a GPIO extender device driver implements the low-level
 * functions `foo_gpio_*(foo_gpio_ext_t* dev, ...)`, it can be integrated by
 * defining the following configuration
 * ```
 * extern foo_gpio_ext_t    foo_ext;
 * extern foo_gpio_driver_t foo_ext_driver;
 *
 * #define GPIO_EXT_PORTS \
 *     { .port.dev = &foo_ext, .driver = &foo_ext_driver }, \
 * ```
 * See file `tests/periph_gpio_ext` as an example for defining GPIO extender
 * configurations.
 *
 * ## (Low-) Power Implications
 *
 * On almost all platforms, we can only control the peripheral power state of
 * full ports (i.e. groups of pins), but not for single GPIO pins. Together
 * with CPU specific alternate function handling for pins used by other
 * peripheral drivers, this can make it quite complex to keep track of pins
 * that are currently used at a certain moment. To simplify the implementations
 * (and ease the memory consumption), we expect ports to be powered on (e.g.
 * through peripheral clock gating) when first used and never be powered off
 * again.
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <limits.h>

#include "gpio_arch.h"  /* include architecture specific GPIO definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_GPIO_PIN_T
/**
 * @brief   GPIO pin number type
 */
typedef uint_fast8_t gpio_pin_t;
#endif

/**
 * @brief   Register address type for GPIO ports of the MCU
 */
#ifndef HAVE_GPIO_REG_T
typedef uint32_t gpio_reg_t;
#endif

/**
 * @brief   GPIO mask type that corresponds to the supported GPIO port width
 *
 * This type is used to mask the pins of a GPIO port in various low-level GPIO
 * functions. Its size must therefore be the maximum width of all different
 * GPIO ports used in the system. For this purpose, each component that
 * provides GPIO ports must activate the corresponding pseudomodule
 * `gpio_mask_8bit`, `gpio_mask_16bit` or `gpio_mask_32bit` that
 * specifies the width of its GPIO ports.
 */
#if defined(MODULE_GPIO_MASK_32BIT)
typedef uint32_t gpio_mask_t;
#elif defined(MODULE_GPIO_MASK_16BIT)
typedef uint16_t gpio_mask_t;
#else
typedef uint8_t gpio_mask_t;
#endif

/**
 * @brief   Convert (port, pin) tuple to gpio_t structure
 */
#define GPIO_PIN(x,y)       ((gpio_t){ .port = &gpio_ports[x], .pin = y })

/**
 * @brief   Convert GPIO pin to port structure of type gpio_port_t
 */
#define GPIO_PORT(x)        (x.port->port)

/**
 * @brief   GPIO pin not defined
 */
#define GPIO_PIN_UNDEF      ((gpio_pin_t)(UINT_FAST8_MAX))

/**
 * @brief   GPIO not defined
 */
#define GPIO_UNDEF          ((gpio_t){ .port = NULL, .pin = GPIO_PIN_UNDEF })

#if MODULE_EXTEND_GPIO || DOXYGEN
/**
 * @brief   Default initializer for driver in gpio_port_table_t
 */
#define GPIO_CPU_PORT(x)    ((gpio_port_table_t){ .port.reg = (gpio_reg_t)x, \
                                                  .driver = &gpio_cpu_driver })
#else
#define GPIO_CPU_PORT(x)    ((gpio_port_table_t){ .port.reg = (gpio_reg_t)x })
#endif

/**
 * @brief   Available GPIO modes
 *
 * Generally, a GPIO can be configured to be input or output. In output mode, a
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
                                 pull resistor */
    GPIO_OD_PU              /**< configure as output in open-drain mode with
                                 pull resistor enabled */
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
 * @brief   GPIO port type
 *
 * A GPIO port allows the access to a number of GPIO pins. It can be either
 *
 * - the base register address of a MCU GPIO port if all MCU GPIO ports
 *   follow a consistent address scheme,
 * - the port number that corresponds to the index of the entry in the GPIO port
 *   table #gpio_ports, or
 * - the pointer to a device of any type which provides the GPIO pin,
 *   e.g. a GPIO extender.
 *
 * Which representation is used is determined by the low-level implementation.
 */
typedef union {
    gpio_reg_t reg; /**< register address of a MCU GPIO port */
    uintptr_t num;  /**< port number (index in port table #gpio_ports) */
    void* dev;      /**< pointer to a device that provides the GPIO port */
} gpio_port_t;

/**
 * @name    Low-level GPIO API
 *
 * The following function prototypes define the low-level GPIO API
 * that have to be implemented by the driver of a GPIO hardware
 * device. They are used by high-level GPIO functions `gpio_*`.
 *
 * The GPIO device driver for a GPIO port contains references to these
 * low-level functions.
 *
 * @note Functions of the low-level API should only be called directly if
 * several pins of a GPIO port are to be changed simultaneously using the
 * definition of GPIO pin masks of type #gpio_mask_t.
 *
 * @{
 */

/**
 * @brief   Initialize the given pin as general purpose input or output
 *
 * @param[in] port  port of the GPIO pin to initialize
 * @param[in] pin   number of the GPIO pin to initialize
 * @param[in] mode  mode of the pin, see #gpio_mode_t
 *
 * @return    0 on success
 * @return    -1 on error
 *
 * @see gpio_init
 */
typedef int (*gpio_dev_init_t)(gpio_port_t port, uint8_t pin, gpio_mode_t mode);

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN

/**
 * @brief   Initialize a GPIO pin for external interrupt usage
 *
 * @param[in] port  port of the GPIO pin to initialize
 * @param[in] pin   number of the GPIO pin to initialize
 * @param[in] mode  mode of the pin, see #gpio_mode_t
 * @param[in] flank define the active flank(s)
 * @param[in] cb    callback that is called from interrupt context
 * @param[in] arg   optional argument passed to the callback
 *
 * @return    0 on success
 * @return    -1 on error
 *
 * @see gpio_init_int
 */
typedef int (*gpio_dev_init_int_t)(gpio_port_t port, uint8_t pin, gpio_mode_t mode,
                                   gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief   Enable GPIO pin interrupt if configured as interrupt source
 *
 * @param[in] port  port of the GPIO pin
 * @param[in] pin   number of the GPIO pin
 *
 * @see gpio_irq_enable
 */
typedef void (*gpio_dev_irq_enable_t)(gpio_port_t port, uint8_t pin);

/**
 * @brief   Disable GPIO pin interrupt if configured as interrupt source
 *
 * @param[in] port  port of the GPIO pin
 * @param[in] pin   number of the GPIO pin
 *
 * @see gpio_irq_disable
 */
typedef void (*gpio_dev_irq_disable_t)(gpio_port_t port, uint8_t pin);

#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

/**
 * @brief   Get current values of all pins of the given GPIO port
 *
 * @param[in] port  GPIO port
 *
 * @return    value of width gpio_mask_t where the bit positions
 *            represent the current value of the according pin
 *            (0 when pin is LOW and 1 when pin is HIGH)
 */
typedef gpio_mask_t (*gpio_dev_read_t)(gpio_port_t port);

/**
 * @brief   Set the pins of a port defined by the pin mask to HIGH
 *
 * @param[in] port  GPIO port
 * @param[in] pins  mask of the pins to set
 *
 * @see gpio_set
 */
typedef void (*gpio_dev_set_t)(gpio_port_t port, gpio_mask_t pins);

/**
 * @brief   Set the pins of a port defined by the pin mask to LOW
 *
 * @param[in] port  GPIO port
 * @param[in] pins  mask of the pins to clear
 *
 * @see gpio_set
 */
typedef void (*gpio_dev_clear_t)(gpio_port_t port, gpio_mask_t pins);

/**
 * @brief   Toggle the value the pins of a port defined by the pin mask
 *
 * @param[in] port  GPIO port
 * @param[in] pins  mask of the pins to toggle
 *
 * @see gpio_set
 */
typedef void (*gpio_dev_toggle_t)(gpio_port_t port, gpio_mask_t pins);

/**
 * @brief   Set the values of all pins of the given GPIO port
 *
 * @param[in] port  GPIO port
 * @param[in] pins  values of the pins (according bit is 0 for LOW and 1 for HIGH)
 *
 * @see gpio_write
 */
typedef void (*gpio_dev_write_t)(gpio_port_t port, gpio_mask_t values);

/**
 * @}
 */

/**
 * @brief   GPIO device driver type
 *
 * GPIO device drivers are used for port-oriented access to GPIO ports.
 * gpio_driver_t contains the references to the functions of the low-level
 * API, which are implemented by the driver of the hardware component that
 * provides a GPIO port.
 */
typedef struct {
    gpio_dev_init_t        init;         /**< see #gpio_dev_init_t  */
#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
    gpio_dev_init_int_t    init_int;     /**< see #gpio_dev_init_int_t */
    gpio_dev_irq_enable_t  irq_enable;   /**< see #gpio_dev_irq_enable_t */
    gpio_dev_irq_disable_t irq_disable;  /**< see #gpio_dev_irq_disable_t */
#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */
    gpio_dev_read_t        read;         /**< see #gpio_dev_read_t */
    gpio_dev_set_t         set;          /**< see #gpio_dev_set_t */
    gpio_dev_clear_t       clear;        /**< see #gpio_dev_clear_t */
    gpio_dev_toggle_t      toggle;       /**< see #gpio_dev_toggle_t */
    gpio_dev_write_t       write;        /**< see #gpio_dev_write_t */
} gpio_driver_t;

#if MODULE_EXTEND_GPIO || DOXYGEN
/**
 * @brief   GPIO device driver for MCU GPIO ports.
 *
 * The GPIO device driver gpio_cpu_driver contains the references to the
 * low-level functions `gpio_cpu_*` of the MCU implementation for accessing
 * GPIO pins of the MCU GPIO ports.
 */
extern const gpio_driver_t gpio_cpu_driver;
#endif

/**
 * @brief   GPIO port table entry type
 *
 * A GPIO port table entry contains the port of type gpio_port_t. If the
 * module `extend_gpio` is enabled, the port table entry additionally
 * includes a reference to the driver of the type #gpio_driver_t, which
 * implements the low-level GPIO API and realizes the access to the port.
 * If the `extend_gpio` module is not enabled, #gpio_cpu_driver is always
 * used and this additional reference to the driver is not needed.
 *
 * The GPIO port table is automatically created from the #GPIO_CPU_PORTS and
 * optionally the `GPIO_EXT_PORTS` macro. The #GPIO_CPU_PORTS macro is defined
 * by the MCU. The optional `GPIO_EXT_PORTS` macro is defined by the board
 * definition or the application according to the GPIO extenders used.
 */
typedef struct {
    gpio_port_t port;                /**< GPIO port */
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver;     /**< associated device driver */
#endif
} gpio_port_table_t;

/**
 * @brief   GPIO port table
 *
 * The GPIO port table includes an entry of type gpio_port_table_t for each
 * existing MCU and GPIO extender ports. It allows the access to a port via
 * an index, the GPIO port number. This port number is used in the definition
 * of a GPIO pin of type gpio_t.
 *
 * The GPIO port table is automatically created from the #GPIO_CPU_PORTS and
 * optionally the `GPIO_EXT_PORTS` macro. The #GPIO_CPU_PORTS macro is defined
 * by the MCU. The optional `GPIO_EXT_PORTS` macro is defined by the board
 * definition or the application according to the GPIO extenders used.
 *
 * The #GPIO_PORT(gpio) macro can be used to get the corresponding port of
 * type #gpio_port_t from the port table for a given GPIO pin of type @ref
 * gpio_t.
 */
extern const gpio_port_table_t gpio_ports[];

/**
 * @brief   GPIO pin type definition
 *
 * A GPIO pin is defined by a port that provides the access to the pin and
 * the pin number at this port. The port is given by entry in the port table
 * #gpio_ports.
 */
typedef struct {
    const gpio_port_table_t *port;  /**< pointer to the port table entry */
    gpio_pin_t pin;                 /**< pin number */
} gpio_t;

/**
 * @name    Low-level GPIO API functions for MCU GPIO pins
 *
 * The following functions correspond exactly to the function prototypes of
 * the low-level GPIO API, but follow a special naming scheme that is used to
 * automatically create gpio_cpu_driver. These functions have to be implemented
 * by each MCU in `cpu/.../periph/gpio.c` for GPIO ports.
 *
 * They should only be called directly if several pins of a GPIO port are
 * to be changed simultaneously using the definition of GPIO pin masks
 * of type gpio_mask_t.
 *
 * The GPIO device driver #gpio_cpu_driver contains references to these
 * low-level functions of the MCU implementation.
 *
 * @see See function prototypes in #gpio_driver_t for detailed information
 * about these functions.
 *
 * @{
 */
int  gpio_cpu_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode);
#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
int  gpio_cpu_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                       gpio_flank_t flank, gpio_cb_t cb, void *arg);
void gpio_cpu_irq_enable(gpio_port_t port, gpio_pin_t pin);
void gpio_cpu_irq_disable(gpio_port_t port, gpio_pin_t pin);
#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */
gpio_mask_t gpio_cpu_read(gpio_port_t port);
void gpio_cpu_set(gpio_port_t port, gpio_mask_t pins);
void gpio_cpu_clear(gpio_port_t port, gpio_mask_t pins);
void gpio_cpu_toggle(gpio_port_t port, gpio_mask_t pins);
void gpio_cpu_write(gpio_port_t port, gpio_mask_t values);
/** @} */

/**
 * @name    High-level GPIO API functions
 * @{
 */

#if MODULE_EXTEND_GPIO || DOXYGEN
/**
 * @brief   Get the GPIO port driver for a given GPIO
 *
 * @param[in] gpio  GPIO pin
 *
 * @return    GPIO port driver
 */
static inline const gpio_driver_t *gpio_driver_get(gpio_t gpio)
{
    return gpio.port->driver;
}
#endif

/**
 * @brief   Initialize the given pin as general purpose input or output
 *
 * When configured as output, the pin state after initialization is undefined.
 * The output pin's state **should** be untouched during the initialization.
 * This behavior can however **not be guaranteed** by every platform.
 *
 * @param[in] gpio  GPIO pin to initialize
 * @param[in] mode  mode of the pin, see #gpio_mode_t
 *
 * @return    0 on success
 * @return    -1 on error
 */
static inline int gpio_init(gpio_t gpio, gpio_mode_t mode)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    return driver->init(GPIO_PORT(gpio), gpio.pin, mode);
#else
    return gpio_cpu_init(GPIO_PORT(gpio), gpio.pin, mode);
#endif
}

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
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
 * @param[in] gpio  GPIO pin to initialize
 * @param[in] mode  mode of the pin, see #gpio_mode_t
 * @param[in] flank define the active flank(s)
 * @param[in] cb    callback that is called from interrupt context
 * @param[in] arg   optional argument passed to the callback
 *
 * @return    0 on success
 * @return   -1 on error
 */
static inline int gpio_init_int(gpio_t gpio, gpio_mode_t mode, gpio_flank_t flank,
                                gpio_cb_t cb, void *arg)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    return driver->init_int(GPIO_PORT(gpio), gpio.pin, mode, flank, cb, arg);
#else
    return gpio_cpu_init_int(GPIO_PORT(gpio), gpio.pin, mode, flank, cb, arg);
#endif
}

/**
 * @brief   Enable GPIO pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] gpio  GPIO pin to enable the interrupt for
 */
static inline void gpio_irq_enable(gpio_t gpio)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    driver->irq_enable(GPIO_PORT(gpio), gpio.pin);
#else
    gpio_cpu_irq_enable(GPIO_PORT(gpio), gpio.pin);
#endif
}

/**
 * @brief   Disable the GPIO pin interrupt if configured as interrupt source
 *
 * @note    You have to add the module `periph_gpio_irq` to your project to
 *          enable this function
 *
 * @param[in] gpio  GPIO pin to disable the interrupt for
 */
static inline void gpio_irq_disable(gpio_t gpio)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    driver->irq_disable(GPIO_PORT(gpio), gpio.pin);
#else
    gpio_cpu_irq_disable(GPIO_PORT(gpio), gpio.pin);
#endif
}

#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

/**
 * @brief   Get the current value of the given GPIO pin
 *
 * @param[in] gpio  GPIO pin to read
 *
 * @return    0 when pin is LOW
 * @return    >0 for HIGH
 */
static inline int gpio_read(gpio_t gpio)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    return driver->read(GPIO_PORT(gpio)) & (1 << gpio.pin);
#else
    return gpio_cpu_read(GPIO_PORT(gpio)) & (1 << gpio.pin);
#endif
}

/**
 * @brief   Set the given GPIO pin to HIGH
 *
 * @param[in] gpio  GPIO pin to set
 */
static inline void gpio_set(gpio_t gpio)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    driver->set(GPIO_PORT(gpio), (1 << gpio.pin));
#else
    gpio_cpu_set(GPIO_PORT(gpio), (1 << gpio.pin));
#endif
}

/**
 * @brief   Set the given GPIO pin to LOW
 *
 * @param[in] gpio  GPIO pin to clear
 */
static inline void gpio_clear(gpio_t gpio)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
   const gpio_driver_t *driver = gpio_driver_get(gpio);
   driver->clear(GPIO_PORT(gpio), (1 << gpio.pin));
#else
   gpio_cpu_clear(GPIO_PORT(gpio), (1 << gpio.pin));
#endif
}

/**
 * @brief   Toggle the value of the given GPIO pin
 *
 * @param[in] gpio  GPIO pin to toggle
 */
static inline void gpio_toggle(gpio_t gpio)
{
#if MODULE_EXTEND_GPIO || DOXYGEN
    const gpio_driver_t *driver = gpio_driver_get(gpio);
    driver->toggle(GPIO_PORT(gpio), (1 << gpio.pin));
#else
    gpio_cpu_toggle(GPIO_PORT(gpio), (1 << gpio.pin));
#endif
}

/**
 * @brief   Set the given GPIO pin to the given value
 *
 * @param[in] gpio  GPIO pin to set
 * @param[in] value value to set the pin to, 0 for LOW, HIGH otherwise
 */
static inline void gpio_write(gpio_t gpio, int value)
{
    if (value) {
        gpio_set(gpio);
    } else {
        gpio_clear(gpio);
    }
}

/**
 * @brief   Test if a GPIO pin is equal to another GPIO pin
 *
 * @param[in] gpio1 First GPIO pin to check
 * @param[in] gpio2 Second GPIO pin to check
 */
static inline int gpio_is_equal(gpio_t gpio1, gpio_t gpio2)
{
    return (gpio1.port == gpio2.port) && (gpio1.pin == gpio2.pin);
}

/**
 * @brief   Test if a GPIO pin is undefined
 *
 * @param[in] gpio GPIO pin to check
 */
static inline int gpio_is_undef(gpio_t gpio)
{
    return gpio_is_equal(gpio, GPIO_UNDEF);
}

/**
 * @brief   Returns the total number of GPIO ports (MCU and other GPIO ports)
 *
 * return   number of GPIO ports
 */
int gpio_port_numof(void);

/**
 * @brief   Returns the port number of a given GPIO pin (MCU and other
 *          GPIO ports)
 *
 * @param[in] gpio  given GPIO pin
 *
 * return     port number for the given GPIO pin
 */
int gpio_port_num(gpio_t gpio);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* PERIPH_GPIO_EXT_H */
