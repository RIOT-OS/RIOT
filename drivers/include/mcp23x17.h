/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_mcp23x17 MCP23x17 I/O Expander
 * @ingroup     drivers_misc
 * @ingroup     drivers_saul
 * @brief       Device driver for Microchip MCP23x17 I/O expanders
 *
 * \section mcp23x17_driver Device driver for Microchip MCP23x17 I/O expanders
 *
 * ## Overview
 *
 * Microchip MCP23x17 I/O expanders provide general purpose I/O extension over
 * I2C or SPI. The driver supports the following MCP23x17 I/O expander and
 * interface variants:
 *
 * <center>
 * Expander | Type                | Interface | Pseudomodule to be used
 * :--------|:--------------------|:----------|:----------------------------
 * MCP23017 | 16-bit I/O expander | I2C       | `mcp23017` or `mcp23x17_i2c`
 * MCP23S17 | 16-bit I/O expander | SPI       | `mcp23s17` or `mcp23x17_spi`
 * </center>
 *
 * For each of the MCP23x17 interface variant, the driver defines a separate
 * pseudomodule. Multiple MCP23x17 I/O expanders and different interface
 * variants can be used simultaneously. The application has to specify used
 * MCP23x17 I/O expander or interface variants as a list of used pseudomodules.
 * For example, to use a MCP23017 with I2C interface and a MCP23S17 with SPI
 * interface at the same time, the make command would be:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USEMODULE="mcp23x17_i2c mcp23x17_spi" BOARD=... make -C tests/driver_mcp23x17
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * At least one MCP23x17 I/O expander or interface variant has to be specified.
 * The driver module `mcp23x17` is then enabled implicitly.
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
 * ## Defined pseudomodules
 *
 * The functionality of the driver is controlled by the use of pseudomodules.
 * The following pseudomodules are defined:
 * <center>
 * Pseudomoule           | Functionality
 * :---------------------|:---------------------
 * `mcp23017`            | support of MCP23017 with I2C interface
 * `mcp23x17_i2c`        | support of MCP23017 with I2C interface
 * `mcp23s17`            | support of MCP23S17 with SPI interface
 * `mcp23x17_spi`        | support of MCP23S17 with SPI interface
 * `mcp23x17_irq`        | support of interrupts enabled with medium event priority
 * `mcp23x17_irq_medium` | support of interrupts enabled with medium event priority
 * `mcp23x17_irq_highest`| support of interrupts enabled with highest event priority
 * `mcp23x17_reset`      | support of hardware reset (RESET pin is used)
 * </center><br>
 *
 * @note
 * - At least one of the modules `mcp23017`, `mcp23x17_i2c`, `mcp23s17` or
 *   `mcp23x17_spi` has to be used.
 * - Module `mcp23017` enables the `mcp23x17_i2c` module automatically.
 * - Module `mcp23s17` enables the `mcp23x17_spi` module automatically.
 * - Module `mcp23x17_irq` enables the `mcp23x17_irq_medium` module
 *   automatically if no other ``mcp23x17_irq_*` module is enabled.
 *
 * ## Expander GPIOs
 *
 * The MCP23x17 expander devices provide 16 bidirectional input/output (I/O)
 * pins. These pins are arranged in two ports A and B with 8 pins each.
 * Each expander I/O pin can be used as input or output. Weak pull-up resistors
 * can be enabled for input pins. Output pins are latched.
 *
 * The driver supports the following GPIO modes:
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
 * MCP23x17 expander I/O pins are in #GPIO_IN mode.
 *
 * The MCP23x17 expander I/O pins for each device can be addressed either
 * consecutively in the range of 0 ... 15 or by using the macro
 * #MCP23X17_GPIO_PIN with the tuple (port, pin) according to the
 * following scheme:
 *
 * <center>
 * MCP23x17 pin label | Expander Port | Expander Pin | RIOT symbol
 * ------------------ |:-------------:|:------------:|:-------------------------
 * GPA0               | 0             | 0            | `MCP23X17_GPIO_PIN(0, 0)`
 * GPA1               | 0             | 1            | `MCP23X17_GPIO_PIN(0, 1)`
 * ...                | ...           | ...          | ...
 * GPA7               | 0             | 7            | `MCP23X17_GPIO_PIN(0, 7)`
 * GPB0               | 1             | 0            | `MCP23X17_GPIO_PIN(1, 0)`
 * GPB1               | 1             | 1            | `MCP23X17_GPIO_PIN(1, 1)`
 * ...                | ...           | ...          | ...
 * GPB7               | 1             | 7            | `MCP23X17_GPIO_PIN(1, 7)`
 * </center>
 *
 * ## Expander Interfaces
 *
 * MCP23x17 I/O expanders can be connected either via I2C or via SPI. The
 * interface of the respective device is defined by the configuration
 * parameter mcp23x17_if_params_t::type, which can be either
 * #MCP23X17_SPI or #MCP23X17_I2C. To use these interface types, the
 * corresponding modules `mcp23x17_spi` and/or `mcp23x17_i2c` have to be
 * enabled.
 *
 * Each MCP23x17 device requires an address which is configured via the
 * hardware address pins A0 ... A2 of the MCP23x17 device. The address is in
 * the range from 0 to 7 and is used internally by the driver as an offset
 * to the base address #MCP23X17_BASE_ADDR to derive the full device address.
 *
 * @note The driver uses hardware addressing with MCP23x17 pins A0 ... A2
 * also for MCP23x17 SPI devices. This allows the use of up to eight SPI
 * devices with the same CS signal.
 *
 * In addition to the type and the address of the interface, a number of
 * interface specific parameters have to be configured for each device:
 *
 * <center>
 * |Interface | Parameter          | Parameter                              |
 * |:--------:|:-------------------|:---------------------------------------|
 * | SPI      | Device Identifier  | \ref mcp23x17_spi_params_t::dev "mcp23x17_if_params_t::if_params.spi.dev" |
 * | SPI      | Chip Select GPIO   | \ref mcp23x17_spi_params_t::cs  "mcp23x17_if_params_t::if_params.spi.cs"  |
 * | SPI      | Clock Rate         | \ref mcp23x17_spi_params_t::clk "mcp23x17_if_params_t::if_params.spi.clk" |
 * | SPI      | Address Offset     | \ref mcp23x17_params_t::addr "mcp23x17_params_t::addr" |
 * |          |                    |                                        |
 * | I2C      | Device Identifier  | \ref mcp23x17_i2c_params_t::dev "mcp23x17_if_params_t::if_params.i2c.dev" |
 * | I2C      | Address Offset     | \ref mcp23x17_params_t::addr "mcp23x17_params_t::addr" |
 * </center>
 *
 * ## Hardware Reset
 *
 * MCP23x17 I/O expanders have a low-active `RESET` pin. If module
 * `mcp23x17_reset` is used, a hardware reset is executed when the expander
 * device is initialized with function #mcp23x17_init. Otherwise, only power
 * on reset configuration is restored.
 *
 * If the hardware reset is used by enabling module `mcp23x17_reset`, the
 * configuration parameter mcp23x17_params_t::reset_pin has to define the
 * MCU GPIO pin that is connected to the `RESET` pin of MCP23x17 devices.
 *
 * ## Interrupts
 *
 * MCP23x17 expanders have two interrupt pins `INTA` and `INTB`, one for each
 * port. These interrupt pins are internally connected (mirrored) by the driver
 * so that an interrupt on either port will cause both pins to activate. Thus,
 * interrupts on either port can be handled using a single interrupt pin
 * connected to the MCU.
 *
 * The configuration parameter mcp23x17_params_t::int_pin is used to
 * configure the MCU pin that is connected to one of the interrupt pins
 * `INTA` or `INTB`. Each MCP23x17 expander device must use its own GPIO pin
 * for its combined `INTA`/`INTB` signal.
 *
 * An interrupt callback function can be attached to an expander input pin with
 * the #mcp23x17_gpio_init_int function. This interrupt callback function is
 * then called on any rising and/or falling edge of the expander input pin.
 *
 * @note The interrupt callback function is called in the thread context,
 * so there are no restrictions on execution time or bus access.
 *
 * To be able to handle interrupts in thread context, a separate event
 * thread is used, see section [The Interrupt Context Problem]
 * (#mcp23x17_interrupt_context_problem).
 * Therefore, enabling interrupts requires more RAM and interrupts have to
 * be explicitly enabled with the module `mcp23x17_irq_<priority>`.
 * `priority` can be one `medium` or `highest`, which correspond
 * to the priority of the event thread that processes the interrupts.
 * For more information on the priorities check @ref sys_event module.
 *
 * @note Interrupt support can also be enabled by using module `mxp23x17_irq`
 * without specifying the priority. In this case, module `mcp23x17_irq_medium`
 * is enabled automatically and the interrupt support is enabled with a
 * medium priority of the event thread.
 *
 * Furthermore, the GPIO pin to which the combined MCP23x17 `INTA`/`INTB` signal
 * is connected has to be defined by the parameter mcp23x17_params_t::int_pin.
 * The default hardware configuration as defined in `mcp23x17_params.h`
 * defines by
 *
 * - #MCP23X17_PARAM_I2C_INT the GPIO pin for the interrupt signal of the
 *   default MCP23017 device, and
 * - #MCP23X17_PARAM_SPI_INT he GPIO pin for the interrupt signal of the
 *   default MCP23S17 device.
 *
 * For more information about the default hardware configuration, see section
 * [Default Hardware Configuration](#mcp23x17_default_hardware_configuration).
 *
 * This default configuration could be overridden at make command line,
 * for example to use a MCP23017 with I2C interface and interrupt support with
 * high priority:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DMCP23X17_PARAM_I2C_INT=GPIO_PIN\(0,6\)" \
 * USEMODULE="mcp23x17_i2c mcp23x17_irq_highest" BOARD=... make -C tests/driver_mcp23x17
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## The Interrupt Context Problem {#mcp23x17_interrupt_context_problem}
 *
 * Handling an interrupt of a MCP23x17 expander requires the driver to access
 * the device directly via I2Cor SPI. However, the mutex-based synchronization
 * of I2C and SPI accesses do not work in the interrupt context. Therefore the
 * ISR must not access the MCP23x17 expander device directly. Rather, the ISR
 * must only indicate the occurrence of the interrupt which has to be handled
 * asynchronously in the thread context.
 *
 * For this purpose an event thread module is used when interrupts are
 * enabled by the module `mcp23x17_irq_<priority>`. The driver then
 * handles the interrupts in the context of the event thread with given
 * `priority`. For more information on the priorities check
 * the @ref sys_event module.
 *
 * ## SAUL Capabilities
 *
 * The driver provides SAUL capabilities that are compatible to the
 * SAUL capabilities of MCU GPIOs. Each MCP23x17 expander I/O pin can
 * be mapped directly to SAUL by defining an according entry in
 * #MCP23X17_SAUL_GPIO_PARAMS. Please refer file `mcp23x17_params.h` for
 * an example.
 *
 * @note Module `saul_gpio` has to be added to the
 * project to enable SAUL capabilities of the MCP23x17 driver, e.g.,
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USEMODULE="mcp23x17_i2c saul_gpio" BOARD=... make -C tests/saul
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Using Multiple Devices
 *
 * It is possible to use multiple devices and different interface variants of
 * MCP23x17 I/O expanders simultaneously. The application has to specify used
 * interface variants by a list of pseudomodules. For example, to use MCP23017
 * and MCP23S17 I/O expanders simultaneously, the make command would be:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USEMODULE="mcp23x17_i2c mcp23x17_spi" BOARD=... make -C tests/driver_mcp23x17
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Furthermore, used devices have to be configured by defining the hardware
 * configuration parameter array `mcp23x17_params` of type #mcp23x17_params_t.
 * A default hardware configuration for one device of each interface variant is
 * already defined in `mcp23x17_params.h`.
 *
 * The application can override it by placing a `mcp23x17_params.h` *
 * in the application directory `$(APPDIR)`. For example, the definition
 * of the hardware configuration parameter array for two devices with SPI
 * interface and two devices with I2C interface could be:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *      static const mcp23x17_params_t mcp23x17_params[] = {
 *          {
 *              .addr = 0,
 *              .int_pin = GPIO_PIN(0,1),
 *              .reset_pin = GPIO_UNDEF,
 *              .if_params.type = MCP23X17_SPI,
 *              .if_params.spi.dev = SPI_DEV(0),
 *              .if_params.spi.cs = GPIO_PIN(0,0),
 *              .if_params.spi.clk = SPI_CLK_10MHZ,
 *          },
 *          {
 *              .addr = 1,
 *              .int_pin = GPIO_PIN(0,2),
 *              .reset_pin = GPIO_UNDEF,
 *              .if_params.type = MCP23X17_SPI,
 *              .if_params.spi.dev = SPI_DEV(0),
 *              .if_params.spi.cs = GPIO_PIN(0,0),
 *              .if_params.spi.clk = SPI_CLK_10MHZ,
 *          },
 *          {
 *              .addr = 0,
 *              .int_pin = GPIO_PIN(0,3),
 *              .reset_pin = GPIO_UNDEF,
 *              .if_params.type = MCP23X17_I2C,
 *              .if_params.i2c.dev = I2C_DEV(0),
 *          },
 *          {
 *              .addr = 1,
 *              .int_pin = GPIO_PIN(0,4),
 *              .reset_pin = GPIO_UNDEF,
 *              .if_params.type = MCP23X17_I2C,
 *              .if_params.i2c.dev = SPI_DEV(0),
 *          }
 *      };
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Default Hardware Configuration {#mcp23x17_default_hardware_configuration}
 *
 * The default hardware configuration is defined in file `mcp23x17_params.h`
 * using the following defines:
 *
 * <center>
 * | Hardware configuration        | Driver name               | Default Value |
 * |:------------------------------|:--------------------------|:--------------|
 * | SPI Address Offset            | #MCP23X17_PARAM_SPI_ADDR  | 0             |
 * | SPI Device Identifier         | #MCP23X17_PARAM_SPI_DEV   | SPI_DEV(0)    |
 * | SPI Clock rRate               | #MCP23X17_PARAM_SPI_CLK   | SPI_CLK_10MHZ |
 * | SPI Chip Select GPIO          | #MCP23X17_PARAM_SPI_CS    | GPIO_PIN(0,0) |
 * | SPI Device `INTA`/`INTB` GPIO | #MCP23X17_PARAM_SPI_INT   | GPIO_PIN(0,1) |
 * |                               |                           |               |
 * | I2C Address Offset            | #MCP23X17_PARAM_I2C_ADDR  | 0             |
 * | I2C Device Identifier         | #MCP23X17_PARAM_I2C_DEV   | I2C_DEV(0)    |
 * | I2C Device `INTA`/`INTB` GPIO | #MCP23X17_PARAM_I2C_INT   | GPIO_PIN(0,2) |
 * |                               |                           |               |
 * | `RESET` GPIO for all devices  | #MCP23X17_PARAM_RESET_PIN | GPIO_UNDEF    |
 * </center><br>
 *
 * These default hardware configuration parameters can be overridden either by
 * the board definition or by defining them in the `CFLAGS` variable in the make
 * command, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DMCP23X17_PARAM_I2C_ADDR=2 -DMCP23X17_PARAM_RESET_PIN=GPIO_PIN\(0,7\)" \
 * USEMODULE='mcp23x17_i2c mcp23x17_reset' BOARD=... make -C tests/driver_mcp23x17
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @{
 * @file
 * @brief       Device driver interface for Microchip MCP23x17 I/O expanders
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "modules.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"

#if IS_USED(MODULE_SAUL_GPIO) || DOXYGEN
#include "saul/periph.h"
#endif /* MODULE_SAUL_GPIO */

#if !IS_USED(MODULE_MCP23X17_I2C) && !IS_USED(MODULE_MCP23X17_SPI)
#error "Please provide the MCP23x17 variants used by the application."
#error "At least one variant has to be specified (mcp23017 and/or mcp23s17)."
#endif

#if IS_USED(MODULE_MCP23X17_IRQ) || DOXYGEN

#include "event.h"

#endif /* MODULE_MCP23X17_IRQ */

/**
 * @brief   MCP23x17 device base address
 *
 * The address of a MCP23x17 device, both for devices with I2C interface and
 * for devices with SPI interface, is defined as the offset to a base address.
 * The address is in the range from 0 to 7 and is defined for the respective
 * MCP23x17 device by its hardware address pins A0 ... A2.
 *
 * @note The base address MCP23X17_BASE_ADDR is for internal use only. In the
 * device parameters only the offset to the base address is used as address.
 */
#define MCP23X17_BASE_ADDR          (0x20)

/**
 * @brief   MCP23x17 has 16 I/O pins
 */
#define MCP23X17_GPIO_PIN_NUM       (16)

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
#define MCP23X17_GPIO_PIN(port, pin) ((gpio_t)((port << 3) | pin))

/**
 * @brief   Named MCP23x17 driver error codes
 */
typedef enum {
    MCP23X17_OK,                    /**< success */
    MCP23X17_ERROR_I2C,             /**< I2C communication error */
    MCP23X17_ERROR_SPI,             /**< SPI communication error */
    MCP23X17_ERROR_NO_DEV,          /**< no MCP23x17 I/O expander device */
    MCP23X17_ERROR_INV_MODE,        /**< invalid pin mode */
    MCP23X17_ERROR_INV_FLANK,       /**< invalid interrupt flank */
    MCP23X17_ERROR_GPIO,            /**< GPIO pin error */
    MCP23X17_ERROR_INT_PIN,         /**< `INTA`/`INTB` pin error */
    MCP23X17_ERROR_RESET_PIN,       /**< `RESET` pin error */
} mcp23x17_error_codes_t;

/**
 * @brief   MCP23x17 interface types
 */
typedef enum {
#if IS_USED(MODULE_MCP23X17_I2C) || DOXYGEN
    MCP23X17_I2C,                   /**< I2C interface used */
#endif
#if IS_USED(MODULE_MCP23X17_SPI) || DOXYGEN
    MCP23X17_SPI,                   /**< SPI interface used */
#endif
} mcp23x17_if_t;

/**
 * @brief   MCP23017 I2C parameters
 */
#if IS_USED(MODULE_MCP23X17_I2C) || DOXYGEN
typedef struct {
    i2c_t dev;                      /**< I2C device used */
} mcp23x17_i2c_params_t;
#endif

/**
 * @brief   MCP23S17 SPI parameters
 */
#if IS_USED(MODULE_MCP23X17_SPI) || DOXYGEN
typedef struct {
    spi_t     dev;                  /**< SPI device used */
    spi_clk_t clk;                  /**< SPI clock speed */
    gpio_t    cs;                   /**< SPI chip Select pin */
} mcp23x17_spi_params_t;
#endif

/**
 * @brief   MCP23x17 Hardware interface parameters union
 */
typedef struct {
    mcp23x17_if_t type;             /**< I2C/SPI interface type selector */
    union {
#if IS_USED(MODULE_MCP23X17_I2C) || DOXYGEN
        mcp23x17_i2c_params_t i2c;  /**< I2C specific interface parameters */
#endif
#if IS_USED(MODULE_MCP23X17_SPI) || DOXYGEN
        mcp23x17_spi_params_t spi;  /**< SPI specific interface parameters */
#endif
    };
} mcp23x17_if_params_t;

/**
 * @brief   Struct containing the peripheral configuration
 */
typedef struct {
    /**
     * @brief  MCP2317 device address.
     *
     * The MCP2317 device address is the address configured via the hardware
     * address pins A0 ... A2 of the MCP23x17 device. It is in the range
     * from 0 to 7. The address is used internally by the driver as an offset
     * to the base address MCP23X17_BASE_ADDR to derive the complete device
     * address.
     *
     * @note The driver uses hardware addressing with MCP23x17
     * pins A0 ... A2 also for MCP23x17 SPI devices. The use of hardware
     * addressing also for SPI devices allows the use of up to eight
     * SPI devices with the same CS signal.
     */
    uint8_t addr;
    gpio_t int_pin;     /**< GPIO pin used for combined `INTA`/`INTB` signal.
                             Each device must use its own GPIO pin for its
                             combined `INTA`/`INTB` signal. */
    gpio_t reset_pin;   /**< GPIO pin used for `RESET` signal */
    mcp23x17_if_params_t if_params; /**< specific I2C/SPI interface parameters */
} mcp23x17_params_t;

#if IS_USED(MODULE_MCP23X17_IRQ) || DOXYGEN
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
 * structure to indicate that an interrupt for the device occurred. Since there
 * is only one interrupt source, only one interrupt can be pending per device.
 * Thus, only one object of this type per device is required.
 */
typedef struct {
    event_t event;      /**< Super event data structure */
    void *dev;          /**< MCP23x17 device reference */
} mcp23x17_irq_event_t;

#endif /* MODULE_MCP23X17_IRQ */

/**
 * @brief   Device descriptor for MCP23x17 I/O expanders
 */
typedef struct {
    mcp23x17_params_t params;   /**< Device initialization parameters */

    uint16_t od_pins;           /**< Pins defined as GPIO_OD or GPIO_OD_PU */

#if IS_USED(MODULE_MCP23X17_IRQ) || DOXYGEN
    gpio_isr_ctx_t isr[MCP23X17_GPIO_PIN_NUM];  /**< ISR with arg for each expander pin */
    gpio_flank_t flank[MCP23X17_GPIO_PIN_NUM];  /**< interrupt flank for each expander pin */

    mcp23x17_irq_event_t irq_event;             /**< IRQ event object used for the device */
#endif /* MODULE_MCP23X17_IRQ */

} mcp23x17_t;

#if IS_USED(MODULE_SAUL_GPIO) || DOXYGEN
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
    uint8_t dev;                /**< MCP23x17 device index */
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

#if IS_USED(MODULE_MCP23X17_IRQ) || DOXYGEN
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
 * - Module `mcp23x17_irq` has to be added to the project to enable this
 *   function.
 * - The GPIO pin connected to the MCP23x17 combined `INTA`/`INTB` signal has
 *   to be defined by parameter mcp23x17_params_t::int_pin.
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
#endif /* MODULE_MCP23X17_IRQ || DOXYGEN */

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

#if IS_USED(MODULE_MCP23X17_IRQ) || DOXYGEN
/**
 * @brief   Enable pin interrupt
 *
 * @note
 * - Module `mcp23x17_irq` has to be added to the project to enable this
 *   function.
 * - The GPIO pin connected to the MCP23x17 combined `INTA`/`INTB` signal has
 *   to be defined by parameter mcp23x17_params_t::int_pin.
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to enable the interrupt for
 */
void mcp23x17_gpio_irq_enable(mcp23x17_t *dev, gpio_t pin);

/**
 * @brief   Disable pin interrupt
 *
 * @note
 * - Module `mcp23x17_irq` has to be added to the project to enable this
 *   function.
 * - The GPIO pin connected to the MCP23x17 combined `INTA`/`INTB` signal has
 *   to be defined by parameter mcp23x17_params_t::int_pin.
 *
 * @param[in]   dev     Descriptor of MCP23x17 I/O expander device
 * @param[in]   pin     pin to enable the interrupt for
 */
void mcp23x17_gpio_irq_disable(mcp23x17_t *dev, gpio_t pin);

#endif /* MODULE_MCP23X17_IRQ || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
