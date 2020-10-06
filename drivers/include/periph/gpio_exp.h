/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_gpio_exp GPIO Expandable
 * @ingroup     drivers_periph
 * @brief       New expandable GPIO peripheral driver
 *
 * This API is the **new expandable GPIO API** that provides a consistent
 * access to GPIO pins of the MCU and GPIO expanders. It allows to expand
 * MCU GPIO pins by GPIO expanders. The API consists of
 *
 * - a **pin-oriented high-level API** which is compatible with the
 *   @ref drivers_periph_gpio "Legacy GPIO API" and
 * - a **port-oriented low-level API** that has to be implemented by drivers for
 *   each GPIO hardware component (MCU GPIO ports and GPIO expanders).
 *
 * @note Since the high-level API uses the functions of the
 * low-level API, the new expandable GPIO API can only be used if the
 * low-level API is also implemented by the MCU. This is automatically
 * detected when the file `periph/gpio.h` is included.
 *
 * The GPIO API provides capabilities to initialize a pin as output-,
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
 * The GPIO API is divided into a low-level API and a high-level API.
 *
 * ### High Level API
 *
 * The high-level API is used by the application and provides a pin-oriented
 * access to the GPIO pins. For this purpose it uses the functions of the
 * low-level API, whereby a distinction is made between two cases:
 *
 * 1. Default case without GPIO expanders (module `gpio_exp` is not enabled)<br>
 *    Only MCU GPIO ports are used. The functions of the high-level API can
 *    therefore directly call the low-level API functions `gpio_cpu_*`
 *    of the MCU implementation.
 * 2. GPIO expanders are used (module `gpio_exp` is enabled)<br>
 *    The functions of the high-level API always call the low-level API
 *    functions indirectly via a driver interface of type #gpio_driver_t.
 *    For GPIO expanders this driver is stored in an associated device
 *    structure of type #gpio_dev_t.
 *    For MCU GPIO Ports the driver interface #gpio_cpu_driver is used.
 *
 * ### Low-Level API
 *
 * The low-level API allows port-oriented access to the GPIOs. It has to be
 * implemented by every hardware driver that provides GPIOs, including the
 * MCU. The functions of the low-level API are called by the high-level API.
 *
 * @note The MCU functions of the low-level API must follow a special naming
 * scheme where all function names have the prefix `gpio_cpu_*`. These
 * functions are used to automatically create the driver interface
 * #gpio_cpu_driver for MCU GPIO ports.
 *
 * See @ref drivers_periph_gpio_exp_low_level_api_impl
 * "Low-Level API Implementation" for further information.
 *
 * ### GPIO Pins
 *
 * GPIO pins are of type #gpio_t and are defined as a tuple of a GPIO port
 * and the number of the pin on that GPIO port.
 *
 * GPIOs of type #gpio_t are defined with the #GPIO_PIN(port,pin) macro.
 * The #GPIO_UNDEF macro declares a GPIO pin as undefined.
 *
 * ### GPIO Ports
 *
 * A GPIO port is a hardware component that provides a certain number of
 * GPIO pins. Such hardware can be either a MCU GPIO port or a GPIO expander
 * port. The number of pins per GPIO port is defined by the width of
 * type #gpio_mask_t which in turn can be either
 *
 * - `uint32_t` if module `gpio_mask_32bit` is enabled,
 * - `uint16_t` if module `gpio_mask_16bit` is enabled or
 * - `uint8_t` if module `gpio_mask_8bit` is enabled.
 *
 * Each driver for GPIO ports including the MCU implementation has to enable
 * the according module. The width of type #gpio_mask_t is then the maximum
 * width of all different GPIO ports used in the system.
 *
 * The definition of a GPIO port is of type #gpio_port_t and can be either
 *
 * - the register address in case of a MCU GPIO port and if all MCU GPIO ports
 *   follow a consistent address scheme,
 * - the address of a device descriptor in case of a GPIO expander port or
 * - the port number.
 *
 * The respective implementation of the low-level API determines which
 * representation of a port is used.
 *
 * The port number is a sequential number that goes from 0 to `GPIO_EXP_PORT-1`
 * for MCU GPIO ports. For GPIO expanders, the port number is derived from
 * `GPIO_EXP_PORT` and their index in the GPIO device table #gpio_devs as
 * offset. Thus the port numbers for MCU ports and GPIO expander ports are
 * numbered consecutively and can be used with the GPIO_PIN macro in the
 * same way. The number of a port of type #gpio_port_t can be determined
 * using the #gpio_port_num function.
 *
 * In the case of GPIO expander ports, the port is defined as a reference to
 * a device descriptor of type #gpio_dev_t. This device descriptor
 * contains:
 *
 * 1. A reference to a device-specific descriptor which is used by the
 *    associated device driver to store the state and parameters
 *    of the device. The structure of the device-specific descriptor is
 *    defined by the device driver.
 * 2. A reference to a driver interface of type #gpio_driver_t. The driver
 *    interface contains the references to the low-level GPIO API functions
 *    of the associated device driver.
 *
 * Device descriptors of type #gpio_dev_t are stored for all existing
 * GPIO expander ports in a device table for GPIO Ports #gpio_devs, where each
 * entry contains a device descriptor for exactly one GPIO expander port.
 * If a GPIO expander provides multiple ports, each port has to be defined as
 * separate device of type #gpio_dev_t. Using the same GPIO expander for
 * different ports must then be implemented by the device driver using a
 * device-specific driver interface in combination with a device-specific
 * descriptor.
 *
 * The device table for GPIO Ports #gpio_devs is automatically generated from
 * the `GPIO_EXP_PORTS` macro definition which has to be defined either by
 * the board definition or by the application.
 *
 * ### Using GPIO Expander
 *
 * The use of GPIO expanders is enabled by the `gpio_exp` module. If
 * the `gpio_exp` module is enabled, either the board definition or the
 * application must provide the configuration of the GPIO expanders in file
 * `gpio_exp_conf.h`. This file has to contain the definition of
 * macro `GPIO_EXP_PORTS`.
 *
 * For example, if a GPIO expander device driver implements the low-level
 * GPIO API functions `foo_gpio_*(foo_gpio_exp_t* dev, ...)`, it can be
 * integrated by defining configuration as follows.
 * ```c
 * extern foo_gpio_exp_t    foo_gpio_exp;
 * extern foo_gpio_driver_t foo_gpio_exp_driver;
 *
 * #define GPIO_EXP_PORTS \
 *     { .port.dev = &foo_gpio_exp, .driver = &foo_gpio_exp_driver },
 * ```
 * ```c
 * foo_gpio_exp_t foo_gpio_exp = { ... };
 *
 * const gpio_driver_t foo_gpio_exp_driver = {
 *     .init = foo_gpio_exp_init,
 * #ifdef MODULE_PERIPH_GPIO_IRQ
 *     .init_int = foo_gpio_exp_init_int,
 *     .irq_enable = foo_gpio_exp_irq_enable,
 *     .irq_disable = foo_gpio_exp_irq_disable,
 * #endif
 *     .read = foo_gpio_exp_read,
 *     .set = foo_gpio_exp_set,
 *     .clear = foo_gpio_exp_clear,
 *     .toggle = foo_gpio_exp_toggle,
 *     .write = foo_gpio_exp_write,
 * };
 * ```
 *
 * If a GPIO expander provides multiple ports, each port has to be defined by a
 * separate device descriptor. Ports are then handled by the device driver
 * using a device-specific driver interface and a device-specific descriptor.
 *
 * For example, a driver for a multiple port GPIO expander would implement
 * functions `bar_*(bar_exp_t* dev, uint8_t port, ...)` using a device-specific
 * descriptor of type `bar_exp_t`. Low-level GPIO API functions
 * `bar_gpio_*(bar_gpio_exp_t* dev, ...)` would then have to be realized
 * as wrapper functions using descriptors of type `bar_gpio_exp_t` for each
 * port. See `tests/periph_gpio_exp/bar_gpio_exp.h` for an example.
 * ```c
 * extern bar_exp_t         bar_exp;
 * extern bar_gpio_exp_t    bar_gpio_exp1;
 * extern bar_gpio_exp_t    bar_gpio_exp2;
 * extern bar_gpio_driver_t bar_gpio_exp_driver;
 *
 * #define GPIO_EXP_PORTS \
 *     { .port.dev = &bar_gpio_exp1, .driver = &bar_gpio_exp_driver }, \
 *     { .port.dev = &bar_gpio_exp2, .driver = &bar_gpio_exp_driver },
 * ```
 * ```c
 * bar_exp_t bar_exp = { ... };
 * bar_gpio_exp_t bar_gpio_exp_1 = { .dev = &bar_exp, .port = 0 };
 * bar_gpio_exp_t bar_gpio_exp_2 = { .dev = &bar_exp, .port = 1 };
 * const bar_gpio_driver_t bar_gpio_exp_driver = { ... };
 * ```
 *
 * See `tests/periph_gpio_exp` as an example for defining GPIO expander
 * configurations. The example shows how to define and use GPIO expanders
 * with a single port and with multiple ports.
 *
 * @anchor  drivers_periph_gpio_exp_low_level_api_impl
 * ### Low-Level API Implementations
 *
 * Each GPIO hardware driver, i.e. each MCU and each GPIO expander driver,
 * has to implement the low-level API functions, see section
 * @ref drivers_periph_gpio_exp_low_level_api_functions
 * "Low-level API functions".
 *
 * @note The MCU functions of the low-level API must follow a special naming
 * scheme where all function names have the prefix `gpio_cpu_*`, see @ref
 * drivers_periph_gpio_cpu_low_level_api_impl
 * "Low-level API functions for MCU GPIO ports".
 *
 * If an MCU implementation uses register addresses for port definitions of
 * type #gpio_port_t, it has to define the #GPIO_CPU_PORT and
 * #GPIO_CPU_PORT_NUM macros. Otherwise, the port number is used by default
 * to define a #gpio_port_t port.
 * While #GPIO_CPU_PORT converts the port number to the corresponding register
 * address, #GPIO_CPU_PORT_NUM provides the port number for a specific register
 * address. Both macros are required by the GPIO API.
 *
 * If an MCU implementation uses register addresses and defines the
 * #GPIO_CPU_PORT and #GPIO_CPU_PORT_NUM macros, it has also to define the
 * GPIO port base register address #GPIO_CPU_PORT_BASE and a
 * #GPIO_CPU_PORT_MASK mask to uniquely distinguish a pointer to a memory
 * address in RAM or ROM from a register address. This is needed by macro
 * #GPIO_CPU_PORT_IS to clearly identify whether a port definition of type
 * #gpio_port_t is a register address or a pointer to the device structure
 * of type #gpio_dev_t, i.e. whether it is a MCU port or a GPIO expander port.
 * Alternatively, the macro #GPIO_CPU_PORT_IS can be overridden by the MCU
 * implementation with a more efficient implementation if possible.
 *
 * Thus the MCU implementation of the low-level API has to define
 *
 * - the width of its GPIO ports using the according `gpio_mask_*bit` module
 * - if register addresses are used as port definitions the macros
 *   - #GPIO_CPU_PORT and #GPIO_CPU_PORT_NUM
 *   - #GPIO_CPU_PORT_BASE and #GPIO_CPU_PORT_MASK or alternatively
 *     #GPIO_CPU_PORT_IS
 * - the low-level API functions with prefix `gpio_cpu_*`, see section
 *   @ref drivers_periph_gpio_exp_low_level_api_functions
 *   "Low-level API functions".
 * - the feature `periph_gpio_exp`
 *
 * ## (Low-) Power Implications
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_GPIO_EXP_H
#define PERIPH_GPIO_EXP_H

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
 *
 * The size of this type must match the size of a pointer to distinguish
 * between MCU GPIO register addresses and pointers on GPIO devices.
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
 * provides GPIO ports must activate the corresponding pseudo module that
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
 * @brief   Convert a MCU port number into its register address
 *
 * The macro has to be defined by the MCU if the low-level API implementation
 * of the MCU wants to use the port definitions directly as register addresses.
 */
#ifndef GPIO_CPU_PORT
#define GPIO_CPU_PORT(x)        (x)
#endif

/**
 * @brief   Convert a MCU port register address into its port number
 *
 * The macro has to be defined by the MCU if the low-level API implementation
 * of the MCU wants to use the port definitions directly as register addresses.
 */
#ifndef GPIO_CPU_PORT_NUM
#define GPIO_CPU_PORT_NUM(x)    (x)
#endif

/**
 * @brief   Mask for GPIO port register addresses
 *
 * The macro has to be defined by the MCU if the low-level API implementation
 * of the MCU wants to use the port definitions directly as register addresses.
 *
 * Otherwise the MCU ports are addressed via the port number and the ports of
 * the MCU are numbered from 0 to 31. The last 5 bits are then used as port
 * number in #gpio_port_t.
 *
 * The AND operation of the #GPIO_CPU_PORT_MASK mask with a port definition
 * must result in the base GPIO port register address #GPIO_CPU_PORT_BASE.
 * This is required to uniquely identify whether a port definition of type
 * #gpio_port_t is a register address or a pointer to the device structure
 * of type #gpio_dev_t, i.e. whether it is a MCU port or a GPIO expander port.
 */
#ifndef GPIO_CPU_PORT_MASK
#define GPIO_CPU_PORT_MASK      (0xffffffe0)
#endif

/**
 * @brief   Base GPIO port register addresses
 *
 * The macro has to be defined by the MCU if the low-level API implementation
 * of the MCU wants to use the port definitions directly as register addresses.
 *
 * Otherwise the MCU ports are addressed via the port number and the ports of
 * the MCU are numbered from 0 to 31. The last 5 bits are then used as port
 * number in #gpio_port_t.
 *
 * The AND operation of the #GPIO_CPU_PORT_MASK mask with a port definition
 * must result in the base GPIO port register address #GPIO_CPU_PORT_BASE.
 * This is required to uniquely identify whether a port definition of type
 * #gpio_port_t is a register address or a pointer to the device structure
 * of type #gpio_dev_t, i.e. whether it is a MCU port or a GPIO expander port.
 */
#ifndef GPIO_CPU_PORT_BASE
#define GPIO_CPU_PORT_BASE      (0)
#endif

/**
 * @brief   Determine whether a given port is a MCU GPIO port
 *
 * The macro defines a test to determine whether a given port is an MCU
 * GPIO port. By default this is done by masking the port register address
 * with #GPIO_CPU_PORT_MASK and comparing the result with #GPIO_CPU_PORT_BASE.
 *
 * The MCU implementation can override this macro by a more efficient
 * implementation if possible
 */
#ifndef GPIO_CPU_PORT_IS
#define GPIO_CPU_PORT_IS(x) ((x.reg & GPIO_CPU_PORT_MASK) == GPIO_CPU_PORT_BASE)
#endif

/**
 * @brief   Convert a (port, pin) tuple into @ref gpio_t structure
 */
#if MODULE_GPIO_EXP || DOXYGEN
#define GPIO_PIN(x, y)  ((gpio_t){ .port = { .dev = (x < GPIO_EXP_PORT ? (gpio_dev_t *)GPIO_CPU_PORT(x) \
                                                                       : &gpio_devs[x - GPIO_EXP_PORT]) }, \
                                   .pin = y })
#else
#define GPIO_PIN(x, y)  ((gpio_t){ .port = { .reg = GPIO_CPU_PORT(x) }, .pin = y })
#endif

/**
 * @brief   GPIO pin declared as not defined
 */
#define GPIO_PIN_UNDEF  ((gpio_pin_t)(UINT_FAST8_MAX))

/**
 * @brief   GPIO declared as not defined
 */
#define GPIO_UNDEF  ((gpio_t){ .port = { .dev = NULL }, .pin = GPIO_PIN_UNDEF })

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

/** forward declaration of GPIO device driver type */
struct gpio_driver;

/**
 * @brief   GPIO device type
 *
 * A GPIO device is a hardware component that provides a single GPIO port with
 * a number of GPIO pins, e.g. a single port GPIO expander. It is defined by
 * a device-specific descriptor that contains the state and parameters of
 * the device, as well as an associated device driver for using the device.
 *
 * @note The GPIO device type is only used for GPIO expander ports but
 * not for MCU GPIO ports.
 */
typedef struct {
    void *dev;                          /**< device descriptor */
    const struct gpio_driver *driver;   /**< associated device driver */
} gpio_dev_t;

/**
 * @brief   GPIO port type
 *
 * A GPIO port allows the access to a number of GPIO pins. It can be either
 *
 * - the register address in case of a MCU GPIO port and if all MCU GPIO ports
 *   follow a consistent address scheme,
 * - the address of a device descriptor in case of a GPIO expander port or
 * - the port number.
 *
 * Which representation is used is determined by the low-level implementation.
 *
 * @note To use the register address representation for MCU GPIO ports, the
 * MCU has to define the macros #GPIO_CPU_PORT and #GPIO_CPU_PORT_NUM.
 * Otherwise the port number is used as representation for MCU ports by
 * default. Furthermore, macros #GPIO_CPU_PORT_BASE and * #GPIO_CPU_PORT_MASK
 * or alternatively #GPIO_CPU_PORT_IS have to be defined.
 */
typedef union gpio_port {
    gpio_reg_t reg;        /**< register address of a MCU GPIO port */
    const gpio_dev_t* dev; /**< pointer to a device that provides the port */
    uintptr_t num;         /**< port number */
} gpio_port_t;

/**
 * @anchor  drivers_periph_gpio_exp_low_level_api_functions
 * @name    Low-level API functions
 *
 * The following function prototypes define the low-level GPIO API
 * that have to be implemented by each GPIO hardware driver. They are used
 * by high-level GPIO functions `gpio_*`.
 *
 * The GPIO device driver interface of type #gpio_driver_t for a GPIO port
 * contains references to these low-level API functions.
 *
 * @note Functions of the low-level API should only be called directly if
 * several pins of a GPIO port are to be changed simultaneously using the
 * definition of GPIO pin masks of type #gpio_mask_t.
 *
 * @{
 */

/**
 * @brief   Initialize the given pin as GPIO
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
typedef int (*gpio_dev_init_t)(gpio_port_t port, gpio_pin_t pin,
                               gpio_mode_t mode);

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
typedef int (*gpio_dev_init_int_t)(gpio_port_t port, gpio_pin_t pin,
                                   gpio_mode_t mode, gpio_flank_t flank,
                                   gpio_cb_t cb, void *arg);

/**
 * @brief   Enable GPIO pin interrupt if configured as interrupt source
 *
 * @param[in] port  port of the GPIO pin
 * @param[in] pin   number of the GPIO pin
 *
 * @see gpio_irq_enable
 */
typedef void (*gpio_dev_irq_enable_t)(gpio_port_t port, gpio_pin_t pin);

/**
 * @brief   Disable GPIO pin interrupt if configured as interrupt source
 *
 * @param[in] port  port of the GPIO pin
 * @param[in] pin   number of the GPIO pin
 *
 * @see gpio_irq_disable
 */
typedef void (*gpio_dev_irq_disable_t)(gpio_port_t port, gpio_pin_t pin);

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
 * @param[in] pins  values of the pins
 *                  (according bit is 0 for LOW and 1 for HIGH)
 *
 * @see gpio_write
 */
typedef void (*gpio_dev_write_t)(gpio_port_t port, gpio_mask_t values);

/**
 * @}
 */

/**
 * @brief   GPIO device driver interface type
 *
 * GPIO device drivers are used for port-oriented access to GPIO ports.
 * #gpio_driver_t contains the references to the functions of the low-level
 * API, which are implemented by the driver of the hardware component that
 * provides a GPIO port.
 */
typedef struct gpio_driver {
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

/**
 * @brief   GPIO pin type definition
 *
 * A GPIO pin is defined by a port that provides the access to the pin and
 * the pin number at this port.
 */
typedef struct {
    gpio_port_t port;   /**< GPIO port */
    gpio_pin_t pin;     /**< pin number */
} gpio_t;

#if MODULE_GPIO_EXP || DOXYGEN
/**
 * @brief   Device table of GPIO expander ports
 *
 * The table contains the device descriptors of type #gpio_dev_t for all
 * existing GPIO expander ports. Each GPIO expander port corresponds to one
 * entry. This table is created automatically from `GPIO_EXP_PORTS`.
 * `GPIO_EXP_PORTS` has to be defined in the GPIO expander configuration
 * file `gpio_exp_conf.h` by the board definition or the application.
 *
 * @see See file `tests/periph_gpio_exp` as an example for defining GPIO
 * expander configurations.
 *
 * @note The table is not used for MCU GPIO ports.
 */
extern const gpio_dev_t gpio_devs[];

/**
 * @brief   GPIO device driver interface for MCU GPIO ports.
 *
 * The GPIO device driver interface #gpio_cpu_driver contains the references
 * to the low-level functions `gpio_cpu_*` of the MCU implementation for
 * accessing GPIO pins of the MCU GPIO ports.
 */
extern const gpio_driver_t gpio_cpu_driver;
#endif

/**
 * @name    Low-level API functions for MCU GPIO ports
 * @anchor  drivers_periph_gpio_cpu_low_level_api_impl
 *
 * The following functions correspond exactly to the function prototypes of
 * the low-level GPIO API, but follow a special naming scheme. These functions
 * are called directly from the high-level GPIO API for MCU GPIO ports
 * must be implemented by each MCU in `cpu/.../periph/gpio.c`.
 *
 * They can also be called directly if several pins of a GPIO port are
 * to be changed simultaneously using the definition of a GPIO pin masks
 * of type gpio_mask_t.
 *
 * The GPIO device driver interface #gpio_cpu_driver contains references to
 * these low-level functions of the MCU implementation.
 *
 * @see See function prototypes in #gpio_driver_t for detailed information
 * about these functions.
 * @{
 */
int  gpio_cpu_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode);
int  gpio_cpu_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                       gpio_flank_t flank, gpio_cb_t cb, void *arg);
void gpio_cpu_irq_enable(gpio_port_t port, gpio_pin_t pin);
void gpio_cpu_irq_disable(gpio_port_t port, gpio_pin_t pin);

gpio_mask_t gpio_cpu_read(gpio_port_t port);
void gpio_cpu_set(gpio_port_t port, gpio_mask_t pins);
void gpio_cpu_clear(gpio_port_t port, gpio_mask_t pins);
void gpio_cpu_toggle(gpio_port_t port, gpio_mask_t pins);
void gpio_cpu_write(gpio_port_t port, gpio_mask_t values);
/** @} */

/**
 * @name    High-level API functions
 * @{
 */

#if MODULE_GPIO_EXP || DOXYGEN
/**
 * @brief    Get the driver interface for a GPIO port
 */
static inline const gpio_driver_t *gpio_driver_get(gpio_port_t port)
{
    if (GPIO_CPU_PORT_IS(port)) {
        return &gpio_cpu_driver;
    }
    else {
        assert(port.dev);
        assert(port.dev->driver);
        return port.dev->driver;
    }
}
#endif /* MODULE_GPIO_EXP || DOXYGEN */

/**
 * @brief   Initialize the given pin as GPIO
 *
 * When configured as output, the pin state after initialization is undefined.
 * The output pin's state **should** be untouched during the initialization.
 * This behavior can however **not be guaranteed** by every platform.
 *
 * @param[in] gpio  GPIO pin to initialize
 * @param[in] mode  mode of the pin, see @ref gpio_mode_t
 *
 * @return    0 on success
 * @return    -1 on error
 */
static inline int gpio_init(gpio_t gpio, gpio_mode_t mode)
{
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    return driver->init(gpio.port, gpio.pin, mode);
#else
    return gpio_cpu_init(gpio.port, gpio.pin, mode);
#endif
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
 * @param[in] gpio  GPIO pin to initialize
 * @param[in] mode  mode of the pin, see @ref gpio_mode_t
 * @param[in] flank define the active flank(s)
 * @param[in] cb    callback that is called from interrupt context
 * @param[in] arg   optional argument passed to the callback
 *
 * @return    0 on success
 * @return    -1 on error
 */
static inline int gpio_init_int(gpio_t gpio, gpio_mode_t mode,
                                gpio_flank_t flank,
                                gpio_cb_t cb, void *arg)
{
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    return driver->init_int(gpio.port, gpio.pin, mode, flank, cb, arg);
#else
    return gpio_cpu_init_int(gpio.port, gpio.pin, mode, flank, cb, arg);
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
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->irq_enable(gpio.port, gpio.pin);
#else
    gpio_cpu_irq_enable(gpio.port, gpio.pin);
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
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->irq_disable(gpio.port, gpio.pin);
#else
    gpio_cpu_irq_disable(gpio.port, gpio.pin);
#endif
}

#endif /* defined(MODULE_PERIPH_GPIO_IRQ) || defined(DOXYGEN) */

/**
 * @brief   Get the current value of the given GPIO pin
 *
 * @param[in] gpio  GPIO pin to read
 *
 * @return    0 when pin is LOW
 * @return    1 when pin is HIGH
 */
static inline int gpio_read(gpio_t gpio)
{
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    return driver->read(gpio.port) & (1 << gpio.pin);
#else
    return gpio_cpu_read(gpio.port) & (1 << gpio.pin);
#endif
}

/**
 * @brief   Set the given GPIO pin to HIGH
 *
 * @param[in] gpio  GPIO pin to set
 */
static inline void gpio_set(gpio_t gpio)
{
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->set(gpio.port, (1 << gpio.pin));
#else
    gpio_cpu_set(gpio.port, (1 << gpio.pin));
#endif
}

/**
 * @brief   Set the given GPIO pin to LOW
 *
 * @param[in] gpio  GPIO pin to clear
 */
static inline void gpio_clear(gpio_t gpio)
{
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->clear(gpio.port, (1 << gpio.pin));
#else
    gpio_cpu_clear(gpio.port, (1 << gpio.pin));
#endif
}

/**
 * @brief   Toggle the value of the given GPIO pin
 *
 * @param[in] gpio  GPIO pin to toggle
 */
static inline void gpio_toggle(gpio_t gpio)
{
#ifdef MODULE_GPIO_EXP
    const gpio_driver_t *driver = gpio_driver_get(gpio.port);
    driver->toggle(gpio.port, (1 << gpio.pin));
#else
    gpio_cpu_toggle(gpio.port, (1 << gpio.pin));
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
    return (gpio1.port.dev == gpio2.port.dev) && (gpio1.pin == gpio2.pin);
}

/**
 * @brief   Test if a GPIO pin is a valid pin and not declared as undefined
 *
 * @param[in] gpio GPIO pin to check
 */
static inline int gpio_is_valid(gpio_t gpio)
{
    return !gpio_is_equal(gpio, GPIO_UNDEF);
}

/**
 * @brief   Returns the total number of GPIO ports (MCU and other GPIO ports)
 *
 * @return  number of GPIO ports
 */
int gpio_port_numof(void);

/**
 * @brief   Returns the port number of a given GPIO pin (MCU and other
 *          GPIO ports)
 *
 * @param[in] gpio  given GPIO pin
 *
 * @return  port number for the given GPIO pin on success
 * @return  -1 on error
 */
int gpio_port_num(gpio_t gpio);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* PERIPH_GPIO_EXP_H */
