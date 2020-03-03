/*
 * Copyright (C) 2017 HAW Hamburg
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mcp23x17 MCP23x17 I/O expander
 * @ingroup     drivers_extend_gpio
 * @ingroup     drivers_saul
 * @brief       Device driver for Microchip MCP23x17 I/O expanders
 *
 * ## Overview
 *
 * Microchip MCP23x17 I/O expanders provide general purpose I/O extension over
 * I2C or SPI. The driver supports the following MCP23x17 I/O expander variants:
 *
 * <center>
 * Expander | Type                | Interface | Pseudomodule
 * :--------|:--------------------|:----------|:------------
 * MCP23017 | 16-bit I/O expander | I2C       | `mcp23017`
 * MCP23S17 | 16-bit I/O expander | SPI       | `mcp23s17`
 * </center><br>
 *
 * For each of the MCP23x17 I/O expanders variants, the driver defines
 * a separate pseudomodule. Multiple MCP23x17 I/O expanders and different
 * variants can be used simultaneously. The application has to specify used
 * MCP23x17 I/O expander variants as a list of used pseudomodules. For
 * example, to use MCP23017 and MCP23S17 I/O expanders at the same time, the
 * make command would be:
 *
 *      USEMODULE="mcp23017 mcp23s17" make -C tests/driver_mcp23x17 BOARD=...
 *
 * At least one MCP23x17 I/O expander variant has to be specified. The
 * driver module `mcp23x17` is then enabled implicitly.
 *
 * ## Driver Interface
 *
 * The driver interface is kept as compatible as possible with the peripheral
 * GPIO interface. The only differences are that
 *
 * - functions have the prefix `mcp23x17_` and
 * - functions require an additional parameter, the pointer to the expander
 *   device of type #mcp23x17_t.
 *
 * The driver also supports the GPIO extension API so that MCP23x17
 * I/O expanders can be used as GPIO extension with the standard peripheral
 * GPIO interface. Please refer the test application in `tests/driver_mcp23x17`
 * for more information on how to use the driver either directly or with the
 * GPIO extension interface.
 * ## Expander GPIOs
 *
 * The MCP23x17 expander devices provide 16 bidirectional input/output (I/O)
 * pins. These pins are arranged in two ports A and B with 8 pins each.
 * Each expander I/O pin can be used as input or output.
 * Output pins are latched. The driver supports the following GPIO modes
 *
 * <center>
 * GPIO mode   | Remarks
 * :---------- |:--------
 * #GPIO_IN    | supported by the MCP27x17 expander device
 * #GPIO_IN_PU | supported by the MCP27x17 expander device
 * #GPIO_IN_PD | not supported
 * #GPIO_OUT   | supported by the MCP27x17 expander device
 * #GPIO_OD    | emulated by the driver
 * #GPIO_OD_PU | emulated by the driver
 * </center><br>
 *
 * After the initialization with function #mcp23x17_init, all
 * MCP23X17 expander I/O pins are in #GPIO_IN mode.
 *
 * The MCP23X17 expander I/O pins can be addressed either consecutively in
 * the range of 0 ... 15 or by using the macro #MCP23X17_GPIO_PIN with
 * the tuple (port, pin) according to the following scheme:
 *
 * <center>
 * MCP23x17 pin label | Port | Pin | RIOT symbol
 * ------------------ |:----:|:---:|:-------------------------
 * GPA0               | 0    | 0   | `MCP23X17_GPIO_PIN(0, 0)`
 * GPA1               | 0    | 1   | `MCP23X17_GPIO_PIN(0, 1)`
 * ...                | ...  | ... | ...
 * GPA7               | 0    | 7   | `MCP23X17_GPIO_PIN(0, 7)`
 * GPB0               | 1    | 0   | `MCP23X17_GPIO_PIN(1, 0)`
 * GPB1               | 1    | 1   | `MCP23X17_GPIO_PIN(1, 1)`
 * ...                | ...  | ... | ...
 * GPB7               | 1    | 7   | `MCP23X17_GPIO_PIN(1, 7)`
 * </center><br>
 *
 * ## Expander Interfaces
 *
 * MCP23x17 I/O expanders can be connected either via I2C or via SPI. Which
 * interface is used is determined by the configuration parameter
 * mcp23x17_params_t::cs, which defines the pin used as the CS signal for SPI.
 * If this parameter is defined, the SPI interface is used. In this case, the
 * SPI clock speed can be defined using the configuration parameter
 * mcp23x17_params_t::clk. If mcp23x17_params_t::cs is #GPIO_UNDEF, I2C will
 * be used instead. mcp23x17_params_t::clk is not used in this case.
 *
 * ## Hardware Reset
 *
 * MCP23x17 I/O expanders have a low-active RESET pin. Configuration
 * parameter mcp23x17_params_t::reset_pin can be used to define a MCU pin that
 * is connected to the RESET pin. If mcp23x17_params_t::reset_pin is defined,
 * a hardware reset is executed when the expander device is initialized
 * with function #mcp23x17_init. Otherwise, only power on reset configuration
 * is restored.
 *
 * ## Interrupts
 *
 * MCP23x17 expanders have two interrupt pins INTA and INTB, one for each
 * port. These interrupt pins can be internally connected (mirrored) so that
 * an interrupt on either port will cause both pins to activate. Thus,
 * interrupts on either port can be handled using a single interrupt pin
 * connected to the MCU.
 *
 * The configuration parameter mcp23x17_params_t::int_pin can be used to
 * configure the MCU pin that is connected to one of the interrupt pins
 * INTA or INTB. If mcp23x17_params_t::int_pin is defined, the driver uses
 * the mirrored interrupt pins in low-active push-pull configuration. If
 * mcp23x17_params_t::int_pin is #GPIO_UNDEF, interrupts are not available.
 *
 * An interrupt service function can be attached to an expander input pin with
 * the #mcp23x17_gpio_init_int function. This interrupt service function is
 * then called on any rising and/or falling edge of the expander input pin.
 *
 * @note To use interrupts, the `periph_gpio_irq` module has to be added to
 * the project. Furthermore the MCU pin to which the MCP23x17 INTA or INTB
 * pin is connected has to be defined by the parameter
 * mcp23x17_params_t::int_pin either in the parameter file or at the command
 * line, e.g.
 *
 *      CFLAGS="-DMCP23X17_PARAM_INT_PIN=\(GPIO\(0,6\)\)" \
 *      USEMODULE="mcp23017 periph_gpio_irq" make -C tests/driver_mcp23x17 BOARD=...
 *
 * ## The Interrupt Context Problem
 *
 * Handling an interrupt of a MCP23x17 expander requires direct access to the
 * device by the driver over I2C/SPI within the ISR. However, the mutex
 * based synchronization of I2C/SPI accesses does not work in the interrupt
 * context. Accessing I2C/SPI within an ISR could therefore interfere with an
 * existing I2C/SPI access. Therefore, the ISR must not access the MCP23x17
 * expander device. Rather, the ISR has only to indicate the occurrence
 * of the interrupt. The interrupt is then handled asynchronously by a thread.
 *
 * Therefore, the driver creates its own thread to handle the interrupts when
 * interrupts are enabled by the `periph_gpio_irq` module. This thread has to
 * have a priority which is high enough to handle the interrupts of MCP23x17
 * expander devices with almost no delay. The thread priority is defined by
 * #MCP23X17_IRQ_THREAD_PRIO and is 1 by default.
 *
 * ## SAUL Capabilities
 *
 * The driver provides SAUL capabilities that are compatible to the SAUL
 * capabilities of peripheral GPIOs. Each MCP23x17 expander I/O pin can be
 * mapped directly to SAUL by defining an according entry in
 * \c MCP23X17_SAUL_GPIO_PARAMS. Please refer file
 * `$RIOTBASE/drivers/mcp23x17/include/mcp23x17_params.h` for an example.
 *
 * @note Module `saul_gpio` has to be added to the
 * project to enable SAUL capabilities of the MCP23x17 driver, e.g.,
 *
 *      USEMODULE="mcp23017 saul_gpio" make -C tests/saul BOARD=...
 *
 * ## Using Multiple Devices
 *
 * It is possible to used multiple devices and different variants of MCP23x17
 * I/O expanders at the same time. The application has to specify used MCP23x17
 * I/O expander variants by a list of pseudomodules. For example, to use
 * one MCP23017 and one MCP23S17 I/O expander the make command would be:
 *
 *      USEMODULE="mcp23017 mcp23s17" make -C tests/driver_mcp23x17 BOARD=...
 *
 * Furthermore, used devices have to be configured by defining the
 * configuration parameter array `mcp23x17_params` of type #mcp23x17_params_t.
 * The default configuration for one device is defined in
 * `drivers/mcp23x17/mcp23x17_params.h`. The application can override it by by
 * placing a file `mcp23x17_params.h` in the application directory `$(APPDIR)`.
 * For example, the definition of the configuration parameter array for the
 * two devices above could be:
 * ```c
 *      static const mcp23x17_params_t mcp23x17_params[] = {
 *          {
 *              .dev = I2C_DEV(0),
 *              .addr = 0,
 *              .cs = GPIO_UNDEF
 *              .int_pin = GPIO(0,1),
 *              .rest_pin = GPIO_UNDEF,
 *          },
 *          {
 *              .dev = I2C_DEV(0),
 *              .addr = 1,
 *              .cs = GPIO_UNDEF
 *              .int_pin = GPIO(0,2),
 *              .rest_pin = GPIO_UNDEF,
 *          },
 *      };
 * ```
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @{
 * @file
 * @brief       Device driver interface for Microchip MCP23x17 I/O expanders
 */

#ifndef MCP23X17_H
#define MCP23X17_H

#ifdef __cplusplus
extern "C" {
#endif

#if !MODULE_MCP23017 && !MODULE_MCP23S17
#error "Pleae provide the MCP23x17 variants used by the application."
#error "At least one variant has to be specified by mcp23017 and/or mcp23s17."
#endif

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"

#if MODULE_SAUL_GPIO || DOXYGEN
#include "saul/periph.h"
#endif /* MODULE_SAUL_GPIO */

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN

#include "event.h"

/**
 * @brief Interrupt handler thread priority.
 *
 * Has to be high enough to handle interrupts of MCP23x17 expanders
 * with almost no delay.
 */
#ifndef MCP23X17_IRQ_THREAD_PRIO
#define MCP23X17_IRQ_THREAD_PRIO        (1)
#endif

/**
 * @brief Interrupt handler thread stack size.
 */
#ifndef MCP23X17_IRQ_THREAD_STACKSIZE
#define MCP23X17_IRQ_THREAD_STACKSIZE   (THREAD_STACKSIZE_DEFAULT)
#endif

#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   MCP23x17 slave addresses
 *
 * MCP23x17 slave addresses are defined as an offset to a base address.
 * The address offset is in the range of 0 to 7.
 */
#define MCP23X17_BASE_ADDR              (0x20)

/**
 * @brief   MCP23x17 has 16 I/O pins
 */
#define MCP23X17_GPIO_PIN_NUM           (16)

/**
 * @brief   Conversion of (port x : pin y) to a pin number
 *
 * MCP23x17 expanders have 16 pins arranged in 2 ports with 8 pins each.
 * #MCP23X17_GPIO_PIN can either be used
 *
 * - #MCP23X17_GPIO_PIN(0, 0...15) or
 * - #MCP23X17_GPIO_PIN(0, 0...7) and MCP23X17_GPIO_PIN(1, 0...7)
 *
 * to address the 16 expander pins.
 */
#define MCP23X17_GPIO_PIN(x,y)          ((gpio_t)((x << 3) | y))

/**
 * @brief   Named MCP23x17 driver error codes
 */
typedef enum {
    MCP23X17_OK,                /**< success */
    MCP23X17_ERROR_I2C,         /**< I2C communication error */
    MCP23X17_ERROR_SPI,         /**< SPI communication error */
    MCP23X17_ERROR_NO_DEV,      /**< no MCP23x17 I/O expander device */
    MCP23X17_ERROR_INV_MODE,    /**< invalid pin mode */
    MCP23X17_ERROR_INV_FLANK,   /**< invalid interrupt flank */
    MCP23X17_ERROR_GPIO,        /**< GPIO pin error */
    MCP23X17_ERROR_INT_PIN,     /**< INT pin error */
    MCP23X17_ERROR_RESET_PIN,   /**< RESET pin error */
} mcp23x17_error_codes_t;

/**
 * @brief   Struct containing the needed peripheral configuration
 */
typedef struct {

    unsigned dev;       /**< I2C/SPI device used */
    uint8_t addr;       /**< I2C/SPI device address */

    spi_clk_t clk;      /**< Clock speed if SPI is used */
    gpio_t cs;          /**< GPIO pin used as CS signal. If defined, SPI is
                             used. If it is #GPIO_UNDEF, I2C is used. */
    gpio_t int_pin;     /**< GPIO pin used for INTA and INTB signals */
    gpio_t reset_pin;   /**< GPIO pin used for RESET signal */

} mcp23x17_params_t;

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
/**
 * @brief   IRQ event type
 *
 * Handling an interrupt of a MCP23x17 expander requires direct access to the
 * device by the driver over I2C/SPI within the ISR. However, the mutex
 * based synchronization of I2C/SPI accesses does not work in the interrupt
 * context. Accessing I2C/SPI within an ISR could therefore interfere with an
 * existing I2C/SPI access. Therefore, the ISR must not access the MCP23x17
 * expander device. Rather, the ISR has only to indicate the occurrence
 * of the interrupt. The interrupt is then handled asynchronously by a thread.
 *
 * The type defines the data structure which is part of each device data
 * structure to indicate that an interrupt for the device occured. Since there
 * is only one interrupt source, only one interrupt can be pending per device.
 * Thus, only one object of this type per device is required.
 */
typedef struct {
    event_t event;      /**< Super event data structure */
    void *dev;          /**< MCP23x17 device reference */
} mcp23x17_irq_event_t;

#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Device descriptor for MCP23x17 I/O expanders
 */
typedef struct {
    mcp23x17_params_t params;   /**< Device initialization parameters */

    uint16_t od_pins;           /**< Pins defined as GPIO_OD or GPIO_OD_PU */

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
    gpio_isr_ctx_t isr[MCP23X17_GPIO_PIN_NUM];  /**< ISR with arg for each expander pin */
    gpio_flank_t flank[MCP23X17_GPIO_PIN_NUM];  /**< interrupt flank for each expander pin */

    mcp23x17_irq_event_t irq_event;             /**< IRQ event object used for the device */
#endif /* MODULE_PERIPH_GPIO_IRQ */

} mcp23x17_t;

#if MODULE_SAUL_GPIO || DOXYGEN
/**
 * @brief   MCP23x17 configuration structure for mapping expander pins to SAUL
 *
 * This data structure is an extension of the GPIO configuration structure for
 * mapping GPIOs to SAUL. The only additional information required is a
 * reference to the according MCP23x17 device.
 *
 * @note To use MCP23x17 with SAUL, module `saul_gpio` has to be added to the
 * project.
 */
typedef struct {
    unsigned int dev;           /**< MCP23x17 device index */
    saul_gpio_params_t gpio;    /**< GPIO configuration for mapping to SAUL */
} mcp23x17_saul_gpio_params_t;
#endif

/**
 * @brief   Initialize the MCP23x17 I/O expander
 *
 * All expander pins are set to be input and are pulled up.
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   params  Configuration parameters, see #mcp23x17_params_t
 *
 * @retval  MCP23X17_OK      on success
 * @retval  MCP23X17_ERROR_* on error, a negative error code,
 *                           see #mcp23x17_error_codes_t
 */
int mcp23x17_init(mcp23x17_t *dev, const mcp23x17_params_t *params);

/**
 * @brief   Initialize a MCP23x17 pin
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     Pin to initialize, use MCP23X17_GPIO_PIN(x,y) to specify
 * @param[in]   mode    Mode of the pin, see #gpio_t
 *
 * @retval  MCP23X17_OK      on success
 * @retval  MCP23X17_ERROR_* on error, a negative error code,
 *                           see #mcp23x17_error_codes_t
 */
int mcp23x17_gpio_init(mcp23x17_t *dev, gpio_t pin, gpio_mode_t mode);

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
/**
 * @brief   Initialize a MCP23x17 pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every
 * time the defined flank(s) are detected. Therefore, it MUST NOT be blocking
 * or time-consuming.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @note
 * - Module `periph_gpio_irq` has to be added to the project to enable this
 *   function.
 * - The MCU interrupt pin connected to the MCP23x17 INTA and INTB signals have
 *   to be defined by parameters mcp23x17_params_t::int_a_pin and
 *   mcp23x17_params_t::int_b_pin.
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to initialize, use MCP23X17_GPIO_PIN(x,y) to specify
 * @param[in]   mode    mode of the pin, see #gpio_t
 * @param[in]   flank   define the active flanks, see #gpio_flank_t
 * @param[in]   isr     ISR that is called back from interrupt context
 * @param[in]   arg     optional argument passed to the callback
 *
 * @retval  MCP23X17_OK      on success
 * @retval  MCP23X17_ERROR_* on error, a negative error code,
 *                           see #mcp23x17_error_codes_t
 */
int mcp23x17_gpio_init_int(mcp23x17_t *dev, gpio_t pin,
                                          gpio_mode_t mode,
                                          gpio_flank_t flank,
                                          gpio_cb_t isr,
                                          void *arg);
#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

/**
 * @brief   Get the value from MCP23x17 input pin
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to read, use MCP23X17_GPIO_PIN(x,y) to specify
 *
 * @retval  0                on LOW signal
 * @retval  1                on HIGH signal
 * @retval  MCP23X17_ERROR_* on error, a negative error code,
 *                           see #mcp23x17_error_codes_t
 */
int mcp23x17_gpio_read(mcp23x17_t *dev, gpio_t pin);

/**
 * @brief   Write the value to MCP23x17 input pin
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to write, use MCP23X17_GPIO_PIN(x,y) to specify
 * @param[in]   value   value to write
 */
void mcp23x17_gpio_write(mcp23x17_t *dev, gpio_t pin, int value);

/**
 * @brief   Clear the MCP23x17 output pin
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to clear, use MCP23X17_GPIO_PIN(x,y) to specify
 */
void mcp23x17_gpio_clear(mcp23x17_t *dev, gpio_t pin);

/**
 * @brief   Set the MCP23x17 output pin
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to set, use MCP23X17_GPIO_PIN(x,y) to specify
 */
void mcp23x17_gpio_set(mcp23x17_t *dev, gpio_t pin);

/**
 * @brief   Toggle the value of the MCP23x17 output pin
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to toggle, use MCP23X17_GPIO_PIN(x,y) to specify
 */
void mcp23x17_gpio_toggle(mcp23x17_t *dev, gpio_t pin);

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN
/**
 * @brief   Enable pin interrupt
 *
 * @note
 * - Module `periph_gpio_irq` has to be added to the project to enable this
 *   function.
 * - The MCU interrupt pin connected to the MCP23x17 INT signal has to be
 *   defined by parameter mcp23x17_params_t::int_pin.
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to enable the interrupt for
 */
void mcp23x17_gpio_irq_enable(mcp23x17_t *dev, gpio_t pin);

/**
 * @brief   Disable pin interrupt
 *
 * @note
 * - Module `periph_gpio_irq` has to be added to the project to enable this
 *   function.
 * - The MCU interrupt pin connected to the MCP23x17 INT signal has to be
 *   defined by parameter mcp23x17_params_t::int_pin.
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to enable the interrupt for
 */
void mcp23x17_gpio_irq_disable(mcp23x17_t *dev, gpio_t pin);

#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* MCP23X17_H */
/** @} */
