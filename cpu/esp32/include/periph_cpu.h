/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       CPU specific definitions and functions for peripheral handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
#define PROVIDES_PM_RESTART
#define PROVIDES_PM_OFF
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (7U)

/**
 * @brief   Available ports on the ESP32
 * @{
 */
#define PORT_GPIO 0       /**< port GPIO */
/** @} */

/**
 * @brief   Define CPU specific number of GPIO pins
 * @{
 */
#define GPIO_PIN_NUMOF  40
#ifndef GPIO_PIN_COUNT
#define GPIO_PIN_COUNT  GPIO_PIN_NUMOF
#endif
/** @} */

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO,0))
#define GPIO1       (GPIO_PIN(PORT_GPIO,1))
#define GPIO2       (GPIO_PIN(PORT_GPIO,2))
#define GPIO3       (GPIO_PIN(PORT_GPIO,3))
#define GPIO4       (GPIO_PIN(PORT_GPIO,4))
#define GPIO5       (GPIO_PIN(PORT_GPIO,5))
#define GPIO6       (GPIO_PIN(PORT_GPIO,6))
#define GPIO7       (GPIO_PIN(PORT_GPIO,7))
#define GPIO8       (GPIO_PIN(PORT_GPIO,8))
#define GPIO9       (GPIO_PIN(PORT_GPIO,9))
#define GPIO10      (GPIO_PIN(PORT_GPIO,10))
#define GPIO11      (GPIO_PIN(PORT_GPIO,11))
#define GPIO12      (GPIO_PIN(PORT_GPIO,12))
#define GPIO13      (GPIO_PIN(PORT_GPIO,13))
#define GPIO14      (GPIO_PIN(PORT_GPIO,14))
#define GPIO15      (GPIO_PIN(PORT_GPIO,15))
#define GPIO16      (GPIO_PIN(PORT_GPIO,16))
#define GPIO17      (GPIO_PIN(PORT_GPIO,17))
#define GPIO18      (GPIO_PIN(PORT_GPIO,18))
#define GPIO19      (GPIO_PIN(PORT_GPIO,19))
/* GPIO 20 is not available */
#define GPIO21      (GPIO_PIN(PORT_GPIO,21))
#define GPIO22      (GPIO_PIN(PORT_GPIO,22))
#define GPIO23      (GPIO_PIN(PORT_GPIO,23))
/* GPIO 24 is not available */
#define GPIO25      (GPIO_PIN(PORT_GPIO,25))
#define GPIO26      (GPIO_PIN(PORT_GPIO,26))
#define GPIO27      (GPIO_PIN(PORT_GPIO,27))
/* GPIOs 28 ...32 are not available */
#define GPIO32      (GPIO_PIN(PORT_GPIO,32))
#define GPIO33      (GPIO_PIN(PORT_GPIO,33))
/* GPIOs 34 ... 39 can only be used as inputs and do not have pullups/pulldowns */
#define GPIO34      (GPIO_PIN(PORT_GPIO,34))
#define GPIO35      (GPIO_PIN(PORT_GPIO,35))
#define GPIO36      (GPIO_PIN(PORT_GPIO,36))
#define GPIO37      (GPIO_PIN(PORT_GPIO,37))
#define GPIO38      (GPIO_PIN(PORT_GPIO,38))
#define GPIO39      (GPIO_PIN(PORT_GPIO,39))
/** @} */

/**
 * @brief   Override mode flank selection values
 *
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_NONE    = 0,
    GPIO_RISING  = 1,        /**< emit interrupt on rising flank  */
    GPIO_FALLING = 2,        /**< emit interrupt on falling flank */
    GPIO_BOTH    = 3,        /**< emit interrupt on both flanks   */
    GPIO_LOW     = 4,        /**< emit interrupt on low level     */
    GPIO_HIGH    = 5         /**< emit interrupt on low level     */
} gpio_flank_t;

/** @} */

/**
 * @brief   Override GPIO modes
 *
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN,        /**< input */
    GPIO_IN_PD,     /**< input with pull-down */
    GPIO_IN_PU,     /**< input with pull-up */
    GPIO_OUT,       /**< output */
    GPIO_OD,        /**< open-drain output */
    GPIO_OD_PU,     /**< open-drain output with pull-up */
    GPIO_IN_OUT,    /**< input and output */
    GPIO_IN_OD,     /**< input and open-drain output */
    GPIO_IN_OD_PU   /**< input and open-drain output */
} gpio_mode_t;
/** @} */

/**
 * @name   ADC configuration
 *
 * ESP32 integrates two 12-bit ADCs (ADC1 and ADC2) capable of measuring up to
 * 18 analog signals in total. Most of these ADC channels are either connected
 * to a number of intergrated sensors like a Hall sensors, touch sensors and a
 * temperature sensor or can be connected with certain GPIOs. Integrated
 * sensors are disabled in RIOT's implementation and are not accessible. Thus,
 * up to 18 GPIOs, the RTC GPIOs, can be used as ADC inputs:
 *
 * - ADC1 supports 8 channels: GPIO 32-39
 * - ADC2 supports 10 channels: GPIO 0, 2, 4, 12-15, 25-27
 *
 * For each ADC line, an attenuation of the input signal can be defined
 * separately unsing function *adc_set_attenuation*, see file
 * ```$RIOTBASE/cpu/esp32/include/adc_arch.h```.
 * This results in different full ranges of the measurable voltage
 * at the input. The attenuation can be set to 0 dB, 3 dB, 6 dB and 11 dB,
 * with 11 dB being the standard attenuation. Since an ADC input is measured
 * against a reference voltage Vref of 1.1 V, approximately the following
 * measurement ranges are given when using a corresponding attenuation:
 *
 * <center>
 *
 * Attenuation     | Voltage Range     | Symbol
 * ----------------|-------------------|----------------------
 *  0 dB           | 0 ... 1.1V (Vref) | ADC_ATTENUATION_0_DB
 *  3 dB           | 0 ... 1.5V        | ADC_ATTENUATION_3_DB
 *  6 dB           | 0 ... 2.2V        | ADC_ATTENUATION_6_DB
 * 11 dB (default) | 0 ... 3.3V        | ADC_ATTENUATION_11_DB
 *
 * </center>
 *
 * Please note: The reference voltage Vref can vary from device to device in
 * the range of 1.0V and 1.2V. The Vref of a device can be read with the
 * function *adc_vref_to_gpio25* at the pin GPIO 25, see file
 * ```$RIOTBASE/cpu/esp32/include/adc_arch.h```. The results of the ADC
 * input can then be adjusted accordingly.
 *
 * ADC_GPIOS in the board-specific peripheral configuration defines a list of
 * GPIOs that can be used as ADC channels. The order of the listed GPIOs
 * determines the mapping between the RIOT's ADC lines and the GPIOs.
 *
 * @note ADC_GPIOS must be defined even if there are no GPIOs that could be
 * used as ADC channels on the board. In this case, an empy list hast to be
 * defined which just contains the curly braces.
 *
 * ADC_NUMOF is determined automatically from the ADC_GPIOS definition.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized
 * as ADC channels with the *adc_init* function, they can be used for other
 * purposes.
 *
 * @{
 */
/**
 * @brief   Possible ADC resolution settings
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xf0,   /**< ADC resolution: 6 bit is not supported */
    ADC_RES_8BIT  = 0xf1,   /**< ADC resolution: 8 bit is not supported  */
    ADC_RES_9BIT  = 0,      /**< ADC resolution: 9 bit */
    ADC_RES_10BIT = 1,      /**< ADC resolution: 10 bit */
    ADC_RES_11BIT = 2,      /**< ADC resolution: 11 bit */
    ADC_RES_12BIT = 3,      /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xf2,   /**< ADC resolution: 14 bit is not supported */
    ADC_RES_16BIT = 0xf3,   /**< ADC resolution: 16 bit is not supported */
} adc_res_t;
/** @} */

/**
 * @brief  Number of ADC cahnnels that could be used at maximum
 *
 * @note GPIO37 and GPIO38 are usually not broken out on ESP32 modules and are
 * therefore not usable. The maximum number of ADC channels (ADC_NUMOF_MAX)
 * is therefore set to 16.
 */
#define ADC_NUMOF_MAX   16

/** Number of ADC channels determined from ADC_GPIOS */
extern const unsigned adc_chn_num;

/** @} */

/**
 * @name   DAC configuration
 *
 * ESP32 supports 2 DAC lines at GPIO25 and GPIO26. These DACs have a width of
 * 8 bits and produce voltages in the range from 0 V to 3.3 V (VDD_A). The 16
 * bits DAC values given as parameter of function *dac_set* are down-scaled
 * to 8 bit.
 *
 * DAC_GPIOS in the board-specific peripheral configuration defines a list of
 * GPIOs that can be used as DAC channels. The order of the listed GPIOs
 * determines the mapping between the RIOT's DAC lines and the GPIOs.
 *
 * @note DAC_GPIOS must be defined even if there are no GPIOs that could be
 * used as DAC channels on the board. In this case, an empy list hast to be
 * defined which just contains the curly braces.
 *
 * DAC_NUMOF is determined automatically from the DAC_GPIOS definition.
 *
 * @note As long as the GPIOs listed in DAC_GPIOS are not initialized
 * as DAC channels with the *dac_init* function, they can be used for other
 * purposes.
 * @{
 */

/**
 * @brief  Number of DAC cahnnels that could be used at maximum.
 */
#define DAC_NUMOF_MAX   2

/** Number of DAC channels determined from DAC_GPIOS */
extern const unsigned dac_chn_num;

/** @} */

/**
 * @name   I2C configuration
 *
 * ESP32 has two built-in I2C interfaces.
 *
 * The board-specific configuration of the I2C interface I2C_DEV(n) requires
 * the defintion of
 *
 * I2Cn_SPEED, the bus speed,
 * I2Cn_SCL, the GPIO used as SCL signal, and
 * I2Cn_SDA, the GPIO used as SDA signal,
 *
 * where n can be 0 or 1. If they are not defined, the I2C interface
 * I2C_DEV(n) is not used.
 *
 * @note The configuration of the I2C interfaces I2C_DEV(n) must be in
 * continuous ascending order of n.
 *
 * I2C_NUMOF is determined automatically from board-specific peripheral
 * definitions of I2Cn_SPEED, I2Cn_SCK, and I2Cn_SDA.
 *
 * @{
 */

/** Number of I2C interfaces determined from I2Cn_* definitions */
extern const unsigned i2c_bus_num;

#define PERIPH_I2C_NEED_READ_REG    /**< i2c_read_reg required */
#define PERIPH_I2C_NEED_READ_REGS   /**< i2c_read_regs required */
#define PERIPH_I2C_NEED_WRITE_REG   /**< i2c_write_reg required */
#define PERIPH_I2C_NEED_WRITE_REGS  /**< i2c_write_regs required */
/** @} */

/**
 * @name   PWM configuration
 * @{
 *
 * PWM implementation uses ESP32's high-speed MCPWM modules. ESP32 has 2 such
 * modules, each with up to 6 channels (PWM_CHANNEL_NUM_DEV_MAX). Thus, the
 * maximum number of PWM devices is 2 and the maximum total number of PWM
 * channels is 12.
 *
 * PWM0_GPIOS and PWM1_GPIOS in the board-specific peripheral configuration
 * each define a list of GPIOs that can be used with the respective PWM
 * devices as PWM channels. The order of the listed GPIOs determines the
 * association between the RIOT PWM channels and the GPIOs.
 *
 * @note The definition of PWM0_GPIOS and PWM1_GPIOS can be omitted or
 * empty. In the latter case, they must at least contain the curly braces.
 * The corresponding PWM device can not be used in this case.
 *
 * PWM_NUMOF is determined automatically from the PWM0_GPIOS and PWM1_GPIOS
 * definitions.
 *
 * @note As long as the GPIOs listed in PWM0_GPIOS and PMW1_GPIOS are not
 * initialized as PWM channels with the *pwm_init* function, they can be used
 * other purposes.
 */

/**
 * @brief   Maximum number of channels per PWM device.
 */
#define PWM_CHANNEL_NUM_DEV_MAX (6)

/** Number of PWM devices determined from PWM0_GPIOS and PWM1_GPIOS. */
extern const unsigned pwm_dev_num;

/** @} */

/**
 * @name   SPI configuration
 *
 * ESP32 has four SPI controllers:
 *
 * - controller SPI0 is reserved for caching the flash memory
 * - controller SPI1 is reserved for external memories like flash and PSRAM
 * - controller SPI2 realizes interface HSPI that can be used for peripherals
 * - controller SPI3 realizes interface VSPI that can be used for peripherals
 *
 * Thus, a maximum of two SPI controllers can be used as peripheral interfaces:
 *
 * - VSPI
 * - HSPI
 *
 * SPI interfaces could be used in quad SPI mode, but RIOT's low level
 * device driver doesn't support it.
 *
 * The board-specific configuration of the SPI interface SPI_DEV(n) requires
 * the defintion of
 *
 * - SPIn_CTRL, the SPI controller which is used for the interface (VSPI or HSPI),
 * - SPIn_SCK, the GPIO used as clock signal
 * - SPIn_MISO, the GPIO used as MISO signal
 * - SPIn_MOSI, the GPIO used as MOSI signal, and
 * - SPIn_CS0, the GPIO used as CS signal when the cs parameter in spi_aquire
 *   is GPIO_UNDEF,
 *
 * where n can be 0 or 1.
 *
 * @note The configuration of the SPI interfaces SPI_DEV(n) should be in
 * continuous ascending order of n.
 *
 * SPI_NUMOF is determined automatically from the board-specific peripheral
 * definitions of SPIn_*.
 */

/** Number of SPI interfaces determined from SPI_* definitions */
extern const unsigned spi_bus_num;

#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  /**< requires function spi_transfer_byte */
#define PERIPH_SPI_NEEDS_TRANSFER_REG   /**< requires function spi_transfer_reg */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  /**< requires function spi_transfer_regs */
/** @} */


/**
 * @name   Timer configuration depenend on which implementation is used
 *
 * Timers are MCU built-in feature and not board-specific. They are therefore
 * configured here.
 * @{
 */
#ifdef MODULE_ESP_HW_COUNTER
/** hardware ccount/ccompare registers are used for timer implementation */
#define TIMER_NUMOF     (2)
#define TIMER_CHANNELS  (1)
#else
/** hardware timer modules are used for timer implementation (default) */
#define TIMER_NUMOF     (3)
#define TIMER_CHANNELS  (1)
#endif

/** Timer used for system time */
#define TIMER_SYSTEM    TIMERG0.hw_timer[0]

/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 supports up to three UART devices. UART_DEV(0) has a fixed pin
 * configuration and is always available. All ESP32 boards use it as standard
 * configuration for the console.
 *
 * UART_DEV(0).TXD      GPIO1
 * UART_DEV(0).RXD      GPIO3
 *
 * The pin configuration of UART_DEV(1) and UART_DEV(2) are defined in
 * board specific peripheral configuration by
 *
 * UARTn_TXD, the GPIO used as TxD signal, and
 * UARTn_RXD, the GPIO used as RxD signal,
 *
 * where n can be 2 or 3. If they are not defined, the UART interface
 * UART_DEV(n) is not used.
 *
 * UART_NUMOF is determined automatically from the board-specific peripheral
 * definitions of UARTn_TXD and UARTn_RXD.
 *
 * @{
 */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
