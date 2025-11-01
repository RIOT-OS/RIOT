/*
 * SPDX-FileCopyrightText: 2020 Gunar Schorcht
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_periph_gpio_ll GPIO Low-Level API
 * @ingroup     drivers_periph
 * @brief       Peripheral GPIO Low-Level API
 *
 * @warning     This API is not stable yet and intended for internal use only
 *              as of now.
 *
 * # General
 *
 * Ports and pin numbers are not always internally defined as the same numbers
 * that you would expect them to be. Therefore it is highly recommended to call
 * the low level functions like gpio_ll_init() or gpio_ll_query_conf() with the
 * gpio_get_port() and gpio_get_pin_num() functions instead of directly setting
 * port or pin numbers. Using the helper functions will assure reliable
 * operation on all platforms and GPIO banks.
 *
 * # Design Goals
 *
 * This API aims to provide low-level access to GPIOs with as little
 * abstraction and overhead in place as possible for the hot code paths, while
 * providing a relatively high-level and feature complete API for the
 * configuration of GPIO pins. The former is to enable sophisticated use cases
 * such at bit-banging parallel protocols, bit-banging at high data rates,
 * bit-banging with strict timing requirements, or any combination of these.
 * The latter is to expose as much of the features the (arguably) most
 * important peripheral of the MCU as possible.
 *
 * It is possible to implement the high level pin-based GPIO API of RIOT, @ref
 * drivers_periph_gpio, on top of this API. It is expected that for many use
 * cases the high level API will still remain the API of choice, since it is
 * more concise an easier to use.
 *
 * Note that this API is likely to be faster moving than the high level GPIO
 * API, as it intents to match the hardware features more closely. Hence, new
 * support for new MCUs are more likely to result in API changes here. This is
 * another reason why only users interested in the low level access to GPIOs
 * should be using the this low level API, while the high level API will cater
 * all the LED blinking and button sensing use cases with a more convenient
 * and more very stable interface.
 *
 * # Thread Safety
 *
 * All functions provided by this API have to be implemented in a thread-safe
 * manner, except for @ref gpio_ll_prepare_write and
 * @ref gpio_ll_prepare_write_all_outputs. If the read-modify-write operations
 * @ref gpio_ll_set, @ref gpio_ll_clear, and @ref gpio_ll_toggle can be done
 * atomically in hardware with predictable timing, this must be used in the
 * implementation. Otherwise IRQs have to be disabled during read-modify-write
 * sequences. Calls to @ref gpio_ll_write are inherently thread-safe and
 * lock-less, but sharing pins on the same port between threads is still
 * requires a lock between the calls to @ref gpio_ll_prepare_write and
 * @ref gpio_ll_write in the general case.
 *
 * Under no circumstances two threads may call @ref gpio_ll_init on the same
 * port / pin combination concurrently.
 *
 * @{
 * @file
 * @brief       Peripheral GPIO Low-Level API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @warning     This API is not stable yet and intended for internal use only as
 *              of now.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "architecture.h"
#include "periph/gpio.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO port type
 */
typedef uintptr_t gpio_port_t;

#if !defined(GPIO_PORT_UNDEF) || defined(DOXYGEN)
/**
 * @brief   Magic "undefined GPIO port" value
 */
#define GPIO_PORT_UNDEF         UINTPTR_MAX
#endif

#if defined(DOXYGEN)
/**
 * @brief   Indicates whether GPIO ports are enumerated alphabetically (`1`)
 *          or numerically (`0`).
 *
 * @note    You can use both @ref GPIO_PORT_A and @ref GPIO_PORT_0 to refer
 *          to the first GPIO port in RIOT, regardless of the naming scheme
 *          used by the MCU the app is compiled for. This macro is useful
 *          e.g. for pretty-printing.
 */
#  define GPIO_PORT_NUMBERING_ALPHABETIC    implementation_specific
#endif

#ifdef DOXYGEN
/**
 * @brief   Get the @ref gpio_port_t value of the port labeled 0.
 *
 * @note    For MCUs that use letters instead of numbers, this will be an alias
 *          for @ref GPIO_PORT_A
 * @note    Some MCUs will not start with Port 0 / Port A, but rather with
 *          Port 1 (e.g. MSP430) or Port B (e.g. ATmega328P). It will be
 *          undefined when unavailable
 * @note    There will also be `GPIO_PORT_1`, `GPIO_PORT_2`, etc. when there
 *          are corresponding GPIO ports in hardware.
 */
#define GPIO_PORT_0     implementation_specific

/**
 * @brief       Get the @ref gpio_port_t value of the port number @p num
 * @param[in]   num     The number of the port to get
 * @pre         @p num is a valid GPIO port number. An implementation may
 *              follow the "garbage in, garbage out" philosophy.
 *
 * @note        If the MCU uses an alphabetic naming scheme, number 0 refers
 *              to port A.
 * @warning     This may involve accessing a lookup table, prefer e.g. using
 *              `GPIO_PORT_0` over `gpio_port(0)` if the port number is known
 *              at compile time.
 */
gpio_port_t gpio_port(uword_t num);

/**
 * @brief       Get the number of the GPIO port @p port refers to
 * @param[in]   port    The port to get the number of
 *
 * @pre         @p port is a valid GPIO port. An implementation may follow the
 *              "garbage in, garbage out" philosophy.
 * @warning     This may involve iterating over a lookup table, prefer using
 *              e.g. `0` instead of `gpio_port_num(GPIO_PORT_0)` if the port
 *              number is known at compile time.
 *
 * In other words `n == gpio_port_num(gpio_port(n))` for every `n` that is
 * a valid port number.
 */
uword_t gpio_port_num(gpio_port_t port);
#endif /* DOXYGEN */

#if !defined(HAVE_GPIO_STATE_T) || defined(DOXYGEN)
/**
 * @brief   Enumeration of GPIO states (direction)
 */
typedef enum {
    /**
     * @brief   Use pin as output in push-pull configuration
     *
     * @note    This is supported and implemented on all MCUs.
     *
     * | Logical Value  | Electrical Behavior               |
     * |:-------------- |:--------------------------------- |
     * | `0`            | Low                               |
     * | `1`            | High                              |
     */
    GPIO_OUTPUT_PUSH_PULL,
    /**
     * @brief   Use pin as output in open collector configuration
     *
     * @note    The feature `periph_gpio_ll_open_drain` is used to indicate
     *          support for this GPIO mode. However, it may not be available on
     *          all pins.
     *
     * | Logical Value  | Electrical Behavior               |
     * |:-------------- |:--------------------------------- |
     * | `0`            | Low                               |
     * | `1`            | High Impedance (Disconnected)     |
     */
    GPIO_OUTPUT_OPEN_DRAIN,
    /**
     * @brief   Use pin as output in open emitter configuration
     *
     * @note    The feature `periph_gpio_ll_open_source` is used to indicate
     *          support for this GPIO mode. However, it may not be available on
     *          all pins.
     *
     * | Logical Value  | Electrical Behavior               |
     * |:-------------- |:--------------------------------- |
     * | `0`            | High Impedance (Disconnected)     |
     * | `1`            | High                              |
     */
    GPIO_OUTPUT_OPEN_SOURCE,
    /**
     * @brief   Use pin as input
     *
     * @note    This is supported and implemented on all MCUs.
     */
    GPIO_INPUT,
    /**
     * @brief   The GPIO pin is used by a peripheral
     *
     * Note that calling @ref gpio_ll_init with this state is implementation
     * defined behavior, as implementation specific additional details
     * (such as which peripheral to connect the pin to) are needed. An
     * implementation may choose to not support this at all.
     *
     * However, it is *strongly* encouraged that @ref gpio_ll_query_conf uses
     * this state to indicate a GPIO pin is currently used by a peripheral (e.g.
     * as ADC input, I2C data/clock pin, etc.).
     */
    GPIO_USED_BY_PERIPHERAL,
    /**
     * @brief   Disconnect pin from all peripherals
     *
     * @note    This may be an alias for GPIO_INPUT when the MCU does not
     *          support support disconnecting GPIO pins from the GPIO
     *          peripheral. The feature `periph_gpio_ll_disconnect` indicates
     *          that disconnecting pins from all peripherals (including the
     *          GPIO peripheral) is supported.
     *
     * The implementation should aim to reduce power consumption of the pin
     * when this state is entered, if this is feasible. For pins where it is
     * possible and sensible, this should electrically disconnect them
     * (high impedance).
     *
     * @warning If a given pin will not only be disconnected from peripherals
     *          but also enter an high impedance state is implementation
     *          defined and may differ from pin to pin.
     *
     * @note    Pull resistors can still be requested in this mode. This can be
     *          useful e.g. for keeping an UART TXD pin from emitting noise
     *          while the UART peripheral is powered off. But not every
     *          implementation will support this.
     *
     * @details Once all GPIOs of a GPIO port are disconnected, the
     *          implementation is allowed to power off the whole GPIO port again
     *          to conserve power.
     */
    GPIO_DISCONNECT,
} gpio_state_t;
#endif

#if !defined(HAVE_GPIO_PULL_T) || defined(DOXYGEN)
/**
 * @brief   Enumeration of pull resistor configurations
 */
typedef enum {
    GPIO_FLOATING,  /**< No pull ups nor pull downs enabled */
    GPIO_PULL_UP,   /**< Pull up resistor enabled */
    GPIO_PULL_DOWN, /**< Pull down resistor enabled */
    GPIO_PULL_KEEP, /**< Keep the signal at current logic level with pull
                         up/down resistors */
} gpio_pull_t;
#endif

#if !defined(HAVE_GPIO_PULL_STRENGTH_T) || defined(DOXYGEN)
/**
 * @brief   Enumeration of pull resistor values
 *
 * @note    Depending on the implementation, some (or even all!) constants can
 *          have the same numeric value if less than four pull resistors per
 *          direction are provided. For obvious reasons, only neighboring values
 *          are allowed to have the same numeric value.
 */
typedef enum {
    GPIO_PULL_WEAKEST,      /**< Use the weakest (highest Ohm value) resistor */
    GPIO_PULL_WEAK,         /**< Use a weak pull resistor */
    GPIO_PULL_STRONG,       /**< Use a strong pull resistor */
    GPIO_PULL_STRONGEST,    /**< Use the strongest pull resistor */
} gpio_pull_strength_t;
#endif

/**
 * @brief   The number of distinct supported pull resistor strengths
 *
 * This equals the number of pull resistor strengths actually supported and can
 * be less than four, if one or more enumeration values in @ref
 * gpio_pull_strength_t have the same numeric value.  Note that: a) some pins
 * might have more options than others and b) it could be possible that there
 * are e.g. two pull up resistors to pick from, but only one pull down
 * resistor.
 */
#define GPIO_PULL_NUMOF (1U + (GPIO_PULL_WEAKEST != GPIO_PULL_WEAK) \
                            + (GPIO_PULL_WEAK != GPIO_PULL_STRONG) \
                            + (GPIO_PULL_STRONG != GPIO_PULL_STRONGEST))

#if !defined(HAVE_GPIO_DRIVE_STRENGTH_T) || defined(DOXYGEN)
/**
 * @brief   Enumeration of drive strength options
 *
 * @note    Depending on the implementation, some (or even all!) constants can
 *          have the same numeric value if less than four drive strength options
 *          to pick from. For obvious reasons, only neighboring values are
 *          allowed to have the same numeric value.
 */
typedef enum {
    GPIO_DRIVE_WEAKEST,     /**< Use the weakest drive strength */
    GPIO_DRIVE_WEAK,        /**< Use a weak drive strength */
    GPIO_DRIVE_STRONG,      /**< Use a strong drive strength */
    GPIO_DRIVE_STRONGEST,   /**< Use the strongest drive strength */
} gpio_drive_strength_t;
#endif

/**
 * @brief   The number of distinct supported drive strengths
 *
 * This equals the number of drive strengths actually supported and can be less
 * than four, if one or more enumeration values in @ref gpio_drive_strength_t
 * have the same numeric value.  Note that some pins might have more options
 * than others.
 */
#define GPIO_DRIVE_NUMOF (1U + (GPIO_DRIVE_WEAKEST != GPIO_DRIVE_WEAK) \
                             + (GPIO_DRIVE_WEAK != GPIO_DRIVE_STRONG) \
                             + (GPIO_DRIVE_STRONG != GPIO_DRIVE_STRONGEST))

#if !defined(HAVE_GPIO_SLEW_T) || defined(DOXYGEN)
/**
 * @brief   Enumeration of slew rate settings
 *
 * Reducing the slew rate can be useful to limit the high frequency noise
 * emitted by a GPIO pin.  On the other hand, a high frequency signal cannot be
 * generated if the slew rate is too slow.
 *
 * @warning The numeric values are implementation defined and multiple
 *          constants can have the same numeric value, if an implementation
 *          supports fewer slew rates. An implementation only supporting a
 *          single slew rate can have all constants set to a value of zero.
 */
typedef enum {
    GPIO_SLEW_SLOWEST,  /**< let the output voltage level rise/fall as slow as
                             possible */
    GPIO_SLEW_SLOW,     /**< let the output voltage level rise/fall slowly */
    GPIO_SLEW_FAST,     /**< let the output voltage level rise/fall fast */
    GPIO_SLEW_FASTEST,  /**< let the output voltage level rise/fall as fast as
                             possible */
} gpio_slew_t;
#endif

/**
 * @brief   The number of distinct supported slew rates
 *
 * This equals the number of slew rates actually supported and can be less than
 * four, if one or more enumeration values in @ref gpio_drive_strength_t have
 * the same numeric value.  Note that some pins might have more options than
 * others.
 */
#define GPIO_SLEW_NUMOF (1U + (GPIO_SLEW_SLOWEST != GPIO_SLEW_SLOW) \
                            + (GPIO_SLEW_SLOW != GPIO_SLEW_FAST) \
                            + (GPIO_SLEW_FAST != GPIO_SLEW_FASTEST))

/**
 * @brief   Public members of `gpio_conf_t`
 *
 * The type `gpio_conf_t` is implementation specific, but will in any case
 * be a union of an unsigned integer of implementation defined width named
 * `bits` (to access all bits of the configuration in one read/write), and
 * an anonymous `struct` to access the actual configuration via its members.
 *
 * The members given here have to be present in every implementation. Make sure
 * to use the `enum` names to assign them, as the actual numeric representation
 * again is implementation specific.
 *
 * It is explicitly intended that code makes use of implementation specific
 * extensions to `gpio_conf_t`. However, portable code should be restricted
 * to only use the members detailed here and make sure that all other bits
 * are initialized with zero.
 */
union gpio_conf_minimal {
    uint8_t bits;           /**< The raw bits of the configuration */
    struct {
        /**
         * @brief   State of the pin
         */
        gpio_state_t state              : 3;
        /**
         * @brief   Pull resistor configuration
         */
        gpio_pull_t pull                : 2;
        /**
         * @brief   Initial value of the output
         *
         * Ignored if @ref gpio_conf_minimal::state is set to @ref GPIO_INPUT or
         * @ref GPIO_DISCONNECT. If the pin was previously in a high impedance
         * state, it is guaranteed to directly transition to the given initial
         * value.
         *
         * @ref gpio_ll_query_conf will write the current value of the specified
         * pin here, which is read from the input register when the state is
         * @ref GPIO_INPUT, otherwise the state from the output register is
         * consulted.
         */
        bool initial_value              : 1;
        uint8_t                         : 2; /**< padding */
    };
};

#if !defined(HAVE_GPIO_CONF_T) && !defined(DOXYGEN)
typedef union gpio_conf_minimal gpio_conf_t;
#endif

#ifdef DOXYGEN
/**
 * @brief   GPIO pin configuration
 *
 * @warning The layout of this structure is implementation dependent and
 *          additional implementation specific fields might be present. For this
 *          reason, this structure must be initialized using designated
 *          initializers or zeroing out the whole contents using `memset()
 *          before initializing the individual fields.
 *
 * See @ref gpio_conf_minimal for the minimal structure fields to expect.
 */
typedef /* implementation specific */ gpio_conf_t;
#endif

#ifndef __cplusplus
/**
 * @name    Commonly used GPIO LL configuration presets
 *
 * @warning These are not available in C++
 *
 * C++ requires initializers to be provided in declaration order and contain
 * explicit initialization for each and every field. However, the actual
 * layout and the number of members of `gpio_conf_t` depends on the
 * implementation, so that implementations can expose advanced features such
 * as pull strength, driver strength, skew rate, mux settings, etc. The
 * API mandates that those extra fields will have a sane default value when
 * implicitly initialized with 0, as done here in C.
 *
 * This doesn't work in C++, unless multiplying the maintenance burden by
 * the number of implementations by having each implementation provide this
 * by hand. This is not acceptable.
 *
 * @{
 */
/**
 * @brief   A standard configuration for a generic floating input pin
 */
static const gpio_conf_t gpio_ll_in = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_FLOATING,
};

/**
 * @brief   A standard configuration for a generic input pin with pull down
 *          resistor
 */
static const gpio_conf_t gpio_ll_in_pd = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_PULL_DOWN,
};

/**
 * @brief   A standard configuration for a generic input pin with pull up
 *          resistor
 */
static const gpio_conf_t gpio_ll_in_pu = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_PULL_UP,
};

/**
 * @brief   A standard configuration for a generic input pin with pull
 *          resistor to keep signal at bus level
 *
 * This means, when the input reaches a 0, a pull down resistor is applied. If
 * input reaches 1, a pull up is applied instead.
 *
 * @note    This is a rather uncommon feature. MCUs that support this are
 *          RP2040.
 */
static const gpio_conf_t gpio_ll_in_pk = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_PULL_KEEP,
};

/**
 * @brief   A standard configuration for a generic push-pull output pin
 *
 * @note    The pin will have an initial value of 0.
 */
static const gpio_conf_t gpio_ll_out = {
    .state              = GPIO_OUTPUT_PUSH_PULL,
    .initial_value      = false,
};

/**
 * @brief   A standard configuration for a generic floating open drain output
 *
 * @note    The pin will have an initial value of 1 (which in absence of an
 *          external pull up resistor will be high impedance).
 */
static const gpio_conf_t gpio_ll_od = {
    .state              = GPIO_OUTPUT_OPEN_DRAIN,
    .pull               = GPIO_FLOATING,
    .initial_value      = true,
};

/**
 * @brief   A standard configuration for a generic open drain output with pull
 *          up
 *
 * @note    The pin will have an initial value of 1 (so that the pull up will
 *          pill the line high).
 */
static const gpio_conf_t gpio_ll_od_pu = {
    .state              = GPIO_OUTPUT_OPEN_DRAIN,
    .pull               = GPIO_PULL_UP,
    .initial_value      = true,
};
/** @} */
#endif

/**
 * @brief   Check if the given number is a valid argument for @ref gpio_port
 *
 * @param[in]       num     port number to check
 * @retval          true    the MCU used has a GPIO port with that number
 * @retval          false   the MCU used has ***NO*** GPIO port with that number
 */
static inline bool is_gpio_port_num_valid(uint_fast8_t num);

/**
 * @brief   Initialize the given GPIO pin as specified
 *
 * @param[in]       port    port the pin to initialize belongs to
 * @param[in]       pin     number of the pin to initialize
 * @param[in]       conf    configuration to apply
 *
 * @retval  0           success
 * @retval  -ENOTSUP    GPIO state or pull resistor configuration not supported
 *
 * @warning If the configuration of the Schmitt trigger, the drive strength, or
 *          the pull resistor strength are not supported, the closest supported
 *          value will be chosen instead and `0` is returned.
 * @warning Note that hardware GPIO peripherals may have shared building
 *          blocks. Those *SHOULD* be handed out by the implementation in
 *          first-come-first-served fashion. (E.g. if there is only one pull up
 *          resistor per port that can be connected to any pin of that port,
 *          typically the first pin on the port configured as pull up will
 *          succeed and subsequent configuration as pull ups for other pins on
 *          that port will get an `-ENOTSUP`.) For that reason, an application
 *          might need to optimize the order in which it configures GPIO pins
 *          to get the most suitable overall configuration supported by the
 *          hardware.
 * @note    An application having strict requirements can use
 *          @ref gpio_ll_query_conf afterwards to verify that the used
 *          configuration is indeed within spec. It is often sensible to omit
 *          these checks if `DEVELHELP` is disabled. An application can rely on
 *          that the configuration of pin @p pin at port @p port will not be
 *          changed as side-effect of operations performed on other pins. That
 *          is, once @ref gpio_ll_init returns the configuration details are
 *          settled and may only change due to subsequent calls to
 *          @ref gpio_ll_init with the same values for @p port and @p pin.
 * @pre     No concurrent context is calling this function for the same
 *          combination of @p port and @p pin - concurrent initialization of
 *          different pins on the same port is supported. The underlying
 *          implementation might perform locking where needed.
 */
int gpio_ll_init(gpio_port_t port, uint8_t pin, gpio_conf_t conf);

/**
 * @brief   Retrieve the current configuration of a GPIO pin
 *
 * @param[in]       port    GPIO port the pin to query is located at
 * @param[in]       pin     Number of the pin to query within @p port
 * @return                  The current config of the given GPIO here
 *
 * @pre     @p port and @p pin refer to an existing GPIO pin and @p dest can
 *          be written to. Expect blowing assertions otherwise.
 *
 * @note    @ref gpio_conf_minimal::initial_value should be set to the current
 *          value of the pin, so that no shadow log of the initial value is
 *          needed to consult.
 */
gpio_conf_t gpio_ll_query_conf(gpio_port_t port, uint8_t pin);

/**
 * @brief   INTERNAL, use @ref gpio_ll_print_conf instead
 *
 * This function prints the public API part of @ref gpio_conf_t to stdio. The
 * intention is that implementations that extend @ref gpio_conf_t to contain
 * more members overwrite @ref gpio_ll_print_conf and call this function to
 * print the common members
 */
void gpio_ll_print_conf_common(const gpio_conf_t conf);

/**
 * @brief   Utility function to print a given GPIO configuration to stdio
 * @param[in]       conf    Configuration to print
 */
void gpio_ll_print_conf(const gpio_conf_t conf);

/**
 * @brief   Get the current input value of all GPIO pins of the given port as
 *          bitmask
 *
 * @param[in] port      port to read
 *
 * @return              The current value of the input register of the given
 *                      GPIO port
 *
 * @note    The value of unconfigured pins and pins configured as @ref
 *          GPIO_DISCONNECT or @ref GPIO_OUTPUT_PUSH_PULL is implementation
 *          defined.
 * @details Unless technically impossible, this must be implemented as a single
 *          read instruction.
 */
static inline uword_t gpio_ll_read(gpio_port_t port);

/**
 * @brief   Get the current output value of all GPIO pins of the given port as
 *          bitmask
 *
 * @param[in] port      port to read
 *
 * @return              The current value of the output register of the given
 *                      GPIO port
 *
 * @note    The value of unconfigured pins and pins configured as @ref
 *          GPIO_INPUT or @ref GPIO_OUTPUT_PUSH_PULL is implementation
 *          defined.
 * @details Unless technically impossible, this must be implemented as a single
 *          read instruction.
 */
static inline uword_t gpio_ll_read_output(gpio_port_t port);

/**
 * @brief   Perform an `reg |= mask` operation on the I/O register of the port
 *
 * @note    The behavior regarding pins not configured as
 *          @ref GPIO_OUTPUT_PUSH_PULL or as
 *          @ref GPIO_OUTPUT_OPEN_DRAIN is implementation defined.
 * @warning Portable code must set the bits in @p mask that do not correspond to
 *          pins configured as output to zero.
 * @details On hardware that supports implementing this as a single write
 *          instruction, this must be implemented as such. Otherwise this
 *          read-modify-write will disable IRQs to still behave atomically.
 *
 * @param[in] port      port to modify
 * @param[in] mask      bitmask containing the pins to set
 */
static inline void gpio_ll_set(gpio_port_t port, uword_t mask);

/**
 * @brief   Perform an `reg &= ~mask` operation on the I/O register of the port
 *
 * @note    The behavior regarding pins not configured as
 *          @ref GPIO_OUTPUT_PUSH_PULL or as
 *          @ref GPIO_OUTPUT_OPEN_DRAIN is implementation defined.
 * @warning Portable code must set the bits in @p mask that do not correspond to
 *          pins configured as output to zero.
 * @details On hardware that supports implementing this as a single write
 *          instruction, this must be implemented as such. Otherwise this
 *          read-modify-write will disable IRQs to still behave atomically.
 *
 * @param[in] port      port to modify
 * @param[in] mask      bitmask containing the pins to clear
 */
static inline void gpio_ll_clear(gpio_port_t port, uword_t mask);

/**
 * @brief   Perform an `reg ^= mask` operation on the I/O register of the port
 *
 * @note    The behavior regarding pins not configured as
 *          @ref GPIO_OUTPUT_PUSH_PULL or as
 *          @ref GPIO_OUTPUT_OPEN_DRAIN is implementation defined.
 * @warning Portable code must set the bits in @p mask that do not correspond to
 *          pins configured as output to zero.
 * @details On hardware that supports implementing this as a single write
 *          instruction, this must be implemented as such. Otherwise this
 *          read-modify-write will disable IRQs to still behave atomically.
 *
 * @param[in] port      port to modify
 * @param[in] mask      bitmask containing the pins to toggle
 */
static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask);

#if defined(DOXYGEN) || !defined(HAVE_GPIO_LL_PREPARE_WRITE_ALL_PINS)
/**
 * @brief       Same as `gpio_ll_prepare_write(port, UWORD_MAX, value)`, but
 *              faster
 * @param[in]   port    port that should be written to
 * @param[in]   value   value to write to port
 * @return      Value to call @ref gpio_ll_write with
 *
 * @details     On most platforms this function will just pass @p value through
 *              unmodified, becoming a no-op (and costing neither CPU cycles,
 *              nor RAM, nor ROM). Hence, this function will only cost you
 *              if your platform really requires preparation of any sorts.
 *
 * @note        If all pins on @p port are known to be configured as output,
 *              calls to this functions can be omitted.
 * @details     The caller needs to make sure that no concurrent changes
 *              (this includes configuration changes, writing, clearing, setting
 *              or toggling GPIO pins) are performed.
 *
 * This function can be used to prevent side-effects on non-output pins of a
 * port when writing to it, e.g. when the output buffer is multiplexed with
 * the pull configuration for input pins (such as on ATmega MCUs).
 */
static inline uword_t gpio_ll_prepare_write_all_outputs(gpio_port_t port,
                                                        uword_t value)
{
    (void)port;
    return value;
}
#endif

#if defined(DOXYGEN) || !defined(HAVE_GPIO_LL_PREPARE_WRITE)
/**
 * @brief       Helper to use @ref gpio_ll_write side-effect free
 * @param[in]   port    port that should be written to
 * @param[in]   mask    bitmask of the pins to write to
 * @param[in]   value   value to write to port
 * @return      Value to call @ref gpio_ll_write with
 *
 * @details     The caller needs to make sure that no concurrent changes
 *              (this includes configuration changes, writing, clearing, setting
 *              or toggling GPIO pins) are performed.
 *
 * See @ref gpio_ll_write on how to use this function
 */
static inline uword_t gpio_ll_prepare_write(gpio_port_t port, uword_t mask,
                                            uword_t value)
{
    return value | (gpio_ll_read_output(port) & (~mask));
}
#endif

#if defined(DOXYGEN) || !defined(HAVE_GPIO_LL_PREPARE_SWITCH_DIR)
/**
 * @brief       Prepare bitmask for use with @ref gpio_ll_switch_dir_output
 *              and @ref gpio_ll_switch_dir_input
 * @param[in]   mask    bitmask specifying the pins to switch the direction of
 *
 * @return      Value to use in @ref gpio_ll_switch_dir_output or
 *              @ref gpio_ll_switch_dir_input
 */
static inline uword_t gpio_ll_prepare_switch_dir(uword_t mask)
{
    return mask;
}
#endif

/**
 * @brief       Turn GPIO pins specified by @p pins (obtained from
 *              @ref gpio_ll_prepare_switch_dir) to outputs
 *
 * @param[in]   port        GPIO port to modify
 * @param[in]   pins        Output of @ref gpio_ll_prepare_switch_dir
 * @pre         The feature `gpio_ll_switch_dir` is available
 * @pre         Each affected GPIO pin is either configured as input or as
 *              push-pull output.
 */
static inline void gpio_ll_switch_dir_output(gpio_port_t port, uword_t pins);

/**
 * @brief       Turn GPIO pins specified by @p pins (obtained from
 *              @ref gpio_ll_prepare_switch_dir) to inputs
 *
 * @param[in]   port        GPIO port to modify
 * @param[in]   pins        Output of @ref gpio_ll_prepare_switch_dir
 * @pre         The feature `gpio_ll_switch_dir` is available
 * @pre         Each affected GPIO pin is either configured as input or as
 *              push-pull output.
 *
 * @warning     The state of the output register may be intermixed with the
 *              input configuration. Specifically, on AVR the output register
 *              enables/disables the internal pull up, on SAM0 MCUs the output
 *              register controls the pull resistor direction (if the pull
 *              resistor is enabled). Hence, the bits in the output
 *              register of the pins switched to input should be restored
 *              just after this call.
 */
static inline void gpio_ll_switch_dir_input(gpio_port_t port, uword_t pins);

/**
 * @brief   Perform a masked write operation on the I/O register of the port
 *
 *  Some platforms multiplex the "write" I/O register with additional
 *          functions e.g. for input pin configuration. To prevent unintentional
 *          side effects prepare a value using @ref gpio_ll_prepare_write that
 *          will set the bits of non-output pins as needed to not have side
 *          effects on the state the GPIO port had when calling
 *          @ref gpio_ll_prepare_write .
 *
 * @param[in] port      port to modify
 * @param[in] state     Opaque value produced by @ref gpio_ll_prepare_write
 *
 * Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *  // emit a square wave from two pins with a phase shift of pi between the
 *  // waves, e.g.:
 *  // pin1: _⎍_⎍_⎍_⎍_⎍_⎍_⎍_⎍_⎍
 *  // pin2: ⎍_⎍_⎍_⎍_⎍_⎍_⎍_⎍_⎍_
 *  void square_wave(gpio_port_t port, uint8_t pin1, uint8_t pin2)
 *  {
 *      uword_t mask = (1U << pin1) | (1U << pin2);
 *      uword_t state1 = gpio_ll_prepare_write(port, mask, 1U << pin1);
 *      uword_t state2 = gpio_ll_prepare_write(port, mask, 1U << pin2);
 *      while (1) {
 *          gpio_ll_write(port, state1);
 *          gpio_ll_write(port, state2);
 *      }
 *  }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note    If the configuration of the used port changes between the calls
 *          of @ref gpio_ll_prepare_write and @ref gpio_ll_write the write will
 *          still have undesired side effects on the configuration of input pins
 *          if the platform multiplexes the write register with configuration
 *          for input pins. It is expected that the user of the API either
 *          exclusively uses a GPIO port or synchronizes with other users to
 *          update the prepared value on configuration changes.
 * @details Unless technically impossible, this must be implemented as a single
 *          write instruction.
 */
static inline void gpio_ll_write(gpio_port_t port, uword_t state);

/**
 * @brief   Extract the `gpio_port_t` from a `gpio_t`
 *
 * CPU specific implementation to return the GPIO port for a given
 * GPIO pin.
 *
 * @param[in] pin GPIO pin structure, usually defined with the GPIO_PIN macro
 *
 * @return  GPIO port
 *
 */
static inline gpio_port_t gpio_get_port(gpio_t pin);

/**
 * @brief   Extract the pin number from a `gpio_t`
 *
 * CPU specific implementation to return the internal pin number for a
 * given GPIO pin.
 *
 * @note    The actual, internal pin numbers are not always directly related to
 *          their name. For example on some microcontrollers as the nRF52,
 *          the distinction between P0 and P1 pins is made by a bit that is set
 *          in the pin numbers.
 *
 * @param[in] pin GPIO pin structure, usually defined with the GPIO_PIN macro
 *
 * @return  Internal pin number
 */
static inline uint8_t gpio_get_pin_num(gpio_t pin);

/**
 * @brief   Pack a pointer into a @ref gpio_port_t
 *
 * @pre     The address in @p addr is not `NULL` ***and*** points to a regular
 *          memory region (e.g. anywhere in `.data`, `.rodata`, `.bss` or an
 *          address returned by `malloc()` and friends)
 *
 * @return  A value that is distinguishable from any valid port reference and
 *          from which @p addr can be reconstructed by calling
 *          @ref gpio_port_unpack_addr
 */
static inline gpio_port_t gpio_port_pack_addr(void *addr);

/**
 * @brief   Extract a data pointer that was packed by @ref gpio_port_pack_addr
 *
 * @return  The exact address previously packed by @ref gpio_port_pack_addr
 * @retval  NULL    @p port is a valid GPIO port
 *
 * The motivation is that a high level API can multiplex peripheral GPIOs and
 * GPIOs provided by port extenders. That high level API could pack pointers to
 * the device descriptors of port extenders into a @ref gpio_port_t and use this
 * function to check if the port is a peripheral port (the return value is
 * `NULL`), or retrieve the device descriptor.
 */
static inline void * gpio_port_unpack_addr(gpio_port_t port);

#ifndef DOXYGEN
#if !MODULE_PERIPH_GPIO_LL_SWITCH_DIR
static inline void gpio_ll_switch_dir_output(gpio_port_t port, uword_t outputs)
{
    (void)port;
    (void)outputs;
    /* Hack: If this function is only used guarded by some
     *
     *      if (IS_USED(MODULE_PERIPH_GPIO_LL_SWITCH_DIR)) {
     *          ...
     *      }
     *
     * as intended, all calls to the following fake function will be optimized
     * due to the elimination of dead branches. If used incorrectly, a linking
     * failure will be the result. The error message will not be ideal, but a
     * compile time error is much better than a runtime error.
     */
    extern void gpio_ll_switch_dir_output_used_but_feature_gpio_ll_switch_dir_is_not_provided(void);
    gpio_ll_switch_dir_output_used_but_feature_gpio_ll_switch_dir_is_not_provided();
}

static inline void gpio_ll_switch_dir_input(gpio_port_t port, uword_t inputs)
{
    (void)port;
    (void)inputs;
    /* Same hack as above */
    extern void gpio_ll_switch_dir_input_used_but_feature_gpio_ll_switch_dir_is_not_provided(void);
    gpio_ll_switch_dir_input_used_but_feature_gpio_ll_switch_dir_is_not_provided();
}

#endif /* !MODULE_PERIPH_GPIO_LL_SWITCH_DIR */
#endif /* !DOXYGEN */

#ifdef __cplusplus
}
#endif

/* the hardware specific implementation relies on the types such as gpio_port_t
 * to be provided */
#include "gpio_ll_arch.h" /* IWYU pragma: export */

#if !defined(DOXYGEN) && !defined(GPIO_PORT_NUMBERING_ALPHABETIC)
#  define GPIO_PORT_NUMBERING_ALPHABETIC    0
#endif

/**
 * @name    GPIO port aliases for alphabetic enumeration
 * @{
 */
#if defined(GPIO_PORT_0) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_0`
 */
#  define GPIO_PORT_A   GPIO_PORT_0
#endif
#if defined(GPIO_PORT_1) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_1`
 */
#  define GPIO_PORT_B   GPIO_PORT_1
#endif
#if defined(GPIO_PORT_2) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_2`
 */
#  define GPIO_PORT_C   GPIO_PORT_2
#endif
#if defined(GPIO_PORT_3) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_3`
 */
#  define GPIO_PORT_D   GPIO_PORT_3
#endif
#if defined(GPIO_PORT_4) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_4`
 */
#  define GPIO_PORT_E   GPIO_PORT_4
#endif
#if defined(GPIO_PORT_5) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_5`
 */
#  define GPIO_PORT_F   GPIO_PORT_5
#endif
#if defined(GPIO_PORT_6) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_6`
 */
#  define GPIO_PORT_G   GPIO_PORT_6
#endif
#if defined(GPIO_PORT_7) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_7`
 */
#  define GPIO_PORT_H   GPIO_PORT_7
#endif
#if defined(GPIO_PORT_8) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_8`
 */
#  define GPIO_PORT_I   GPIO_PORT_8
#endif
#if defined(GPIO_PORT_9) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_9`
 */
#  define GPIO_PORT_J   GPIO_PORT_9
#endif
#if defined(GPIO_PORT_10) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_10`
 */
#  define GPIO_PORT_K   GPIO_PORT_10
#endif
#if defined(GPIO_PORT_11) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_11`
 */
#  define GPIO_PORT_L   GPIO_PORT_11
#endif
#if defined(GPIO_PORT_12) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_12`
 */
#  define GPIO_PORT_M   GPIO_PORT_12
#endif
#if defined(GPIO_PORT_13) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_13`
 */
#  define GPIO_PORT_N   GPIO_PORT_13
#endif
#if defined(GPIO_PORT_14) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_14`
 */
#  define GPIO_PORT_O   GPIO_PORT_14
#endif
#if defined(GPIO_PORT_15) || defined(DOXYGEN)
/**
 * @brief   Alias of `ref GPIO_PORT_15`
 */
#  define GPIO_PORT_P   GPIO_PORT_15
#endif
/** @} */

/** @} */
