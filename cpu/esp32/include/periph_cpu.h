/*
 * Copyright (C) 2022 Gunar Schorcht
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

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "hal/ledc_types.h"
#include "hal/spi_types.h"
#include "soc/ledc_struct.h"
#include "soc/periph_defs.h"
#include "soc/soc_caps.h"

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
#define PROVIDES_PM_LAYERED_OFF

/**
 * @brief   Number of usable low power modes
 */
#define PM_NUM_MODES            (3U)

/**
 * @name    Power modes
 * @{
 */
#define ESP_PM_MODEM_SLEEP      (2U)
#define ESP_PM_LIGHT_SLEEP      (1U)
#define ESP_PM_DEEP_SLEEP       (0U)
/** @} */

/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN               (6U)

/**
 * @name   GPIO configuration
 * @{
 */

/**
 * @brief   Override the default gpio_t type definition
 *
 * This is required here to have gpio_t defined in this file.
 * @{
 */
#define HAVE_GPIO_T
typedef unsigned int gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 * @{
 */
#define GPIO_UNDEF          (0xffffffff)
/** @} */

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 * @{
 */
#define GPIO_PIN(x, y)      ((x & 0) | y)
/** @} */

/**
 * @brief   Available GPIO ports on ESP32
 * @{
 */
#define PORT_GPIO           (0)
/** @} */

/**
 * @brief   Define CPU specific number of GPIO pins
 * @{
 */
#define GPIO_PIN_NUMOF      (SOC_GPIO_PIN_COUNT)
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override mode flank selection values
 *
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_NONE,
    GPIO_RISING,    /**< emit interrupt on rising flank  */
    GPIO_FALLING,   /**< emit interrupt on falling flank */
    GPIO_BOTH,      /**< emit interrupt on both flanks */
    GPIO_LOW,       /**< emit interrupt on low level */
    GPIO_HIGH       /**< emit interrupt on low level */
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
    GPIO_IN_OD_PU   /**< input and open-drain output with pull-up */
} gpio_mode_t;
/** @} */

#endif /* ndef DOXYGEN */
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
 * @name   ADC configuration
 *
 * ESP32 integrates two 12-bit ADCs (ADC1 and ADC2) capable of measuring up to
 * 18 analog signals in total. Most of these ADC channels are either connected
 * to a number of integrated sensors like a Hall sensors, touch sensors and a
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
 * used as ADC channels on the board. In this case, an empty list hast to be
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
 * @brief  Number of ADC channels that could be used at maximum
 *
 * @note GPIO37 and GPIO38 are usually not broken out on ESP32 modules and are
 * therefore not usable. The maximum number of ADC channels (ADC_NUMOF_MAX)
 * is therefore set to 16.
 */
#define ADC_NUMOF_MAX   (SOC_ADC_CHANNEL_NUM(0) + SOC_ADC_CHANNEL_NUM(1))

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
 * used as DAC channels on the board. In this case, an empty list hast to be
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
 * @brief  Number of DAC channels that could be used at maximum.
 */
#define DAC_NUMOF_MAX   (SOC_DAC_PERIPH_NUM)

/** @} */

/**
 * @name   I2C configuration
 *
 * ESP32 has two built-in I2C interfaces.
 *
 * The board-specific configuration of the I2C interface I2C_DEV(n) requires
 * the definition of
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

#ifndef DOXYGEN
/**
 * @brief    Override I2C clock speed values
 *
 * This is required here to have i2c_speed_t defined in this file.
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW = 0,      /**< 10 kbit/s */
    I2C_SPEED_NORMAL,       /**< 100 kbit/s */
    I2C_SPEED_FAST,         /**< 400 kbit/s */
    I2C_SPEED_FAST_PLUS,    /**< 1 Mbit/s */
    I2C_SPEED_HIGH,         /**< not supported */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C configuration structure type
 */
typedef struct {
    uint8_t module;         /**< I2C module identifier */
    i2c_speed_t speed;      /**< I2C bus speed */
    gpio_t scl;             /**< GPIO used as SCL pin */
    gpio_t sda;             /**< GPIO used as SDA pin */
    bool scl_pullup;        /**< Pullup enabled for SCL pin */
    bool sda_pullup;        /**< Pullup enabled for SDA pin */
} i2c_conf_t;

/**
 * @brief   Maximum number of I2C interfaces that can be used by board definitions
 */
#define I2C_NUMOF_MAX   (SOC_I2C_NUM)

#define PERIPH_I2C_NEED_READ_REG    /**< i2c_read_reg required */
#define PERIPH_I2C_NEED_READ_REGS   /**< i2c_read_regs required */
#define PERIPH_I2C_NEED_WRITE_REG   /**< i2c_write_reg required */
#define PERIPH_I2C_NEED_WRITE_REGS  /**< i2c_write_regs required */
/** @} */

/**
 * @name   PWM configuration
 *
 * The implementation of the PWM peripheral driver uses the LED PWM Controller
 * (LEDC) module of the ESP32x SoC. This LEDC module has one or two channel
 * groups with 6 or 8 channels each. The channels of each channel group can
 * use one of 4 timers as clock source. Thus, it is possible to define at
 * 4 or 8 virtual PWM devices in RIOT with different frequencies and
 * resolutions. Regardless of whether the LEDC module of the ESP32x SoC has
 * one or two channel groups, the PWM driver implementation allows to organize
 * the available channels into up to 4 virtual PWM devices.
 *
 * The assignment of the available channels to the virtual PWM devices is
 * done in the board-specific peripheral configuration by defining the
 * macros `PWM0_GPIOS`, `PWM1_GPIOS`, `PWM2_GPIOS` and `PWM3_GPIOS` These
 * macros specify the GPIOs that are used as channels for the available
 * virtual PWM devices PWM_DEV(0) ... PWM_DEV(3) in RIOT. The mapping of
 * these channels to the available channel groups and channel group timers
 * is done by the driver automatically as follows.
 *
 * <center>
 * Macro        | 1 Channel Group       | 2 Channel Groups       | Timer
 * -------------|-----------------------|------------------------|---------------
 * `PWM0_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_LOW_SPEED_MODE`  | `LEDC_TIMER_0`
 * `PWM1_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_HIGH_SPEED_MODE` | `LEDC_TIMER_1`
 * `PWM2_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_LOW_SPEED_MODE`  | `LEDC_TIMER_2`
 * `PWM3_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_HIGH_SPEED_MODE` | `LEDC_TIMER_3`
 *
 * </center>
 * For example, if the LEDC module of the ESP32x SoC has two channel groups,
 * two virtual PWM devices with 2 x 6/8 channels could be used by defining
 * 'PWM0_GPIOS' and 'PWM1_GPIOS' with 6/8 GPIOs each.
 *
 * @note
 * - The total number of channels defined for a channel group must not exceed
 *   #PWM_CH_NUMOF_MAX.
 * - The definition of `PWM0_GPIOS`, `PWM1_GPIOS`, `PWM2_GPIOS` and
 *   `PWM3_GPIOS` can be omitted. In this case the existing macros should
 *   be defined in ascending order, as the first defined macro is assigned
 *   to PWM_DEV(0), the second defined macro is assigned to PWM_DEV(1)
 *   and so on. So the minimal configuration would define all channels by
 *   `PWM0_GPIOS` as PWM_DEV(0).
 * - #PWM_NUMOF is determined automatically.
 * - The order of the GPIOs in these macros determines the mapping between
 *   RIOT's PWM channels and the GPIOs.
 * - As long as the GPIOs listed in `PWM0_GPIOS`, `PWM1_GPIOS`,
 *   `PWM2_GPIOS` and `PWM3_GPIOS` are not initialized as PWM channels with
 *   the #pwm_init function, they can be used for other purposes.
 *
 * @{
 */

/**
 * @brief   PWM configuration structure type
 *
 * The implementation of the PWM peripheral driver uses the LED PWM Controller
 * (LEDC) module of the ESP32x SoC. The LEDC module has up to 2 channel groups
 * with 6 or 8 channels each, which can use one of 4 timers.
 *
 * Based on these maximum 2 channel groups with 6 or 8 channels each and 4
 * timers, up to 4 PWM devices can be configured in RIOT. The configuration
 * structure defines static parameters for each virtual PWM device, i.e.
 * the channel group used, the timer used, the number of channels used and
 * the GPIOs assigned to the channels. The number of channels used by a PWM
 * device corresponds to the number of GPIOs assigned to this PWM device.
 */
typedef struct {
    uint8_t module;         /**< LEDC module identifier */
    ledc_mode_t group;      /**< LEDC channel group used (low/high speed) */
    ledc_timer_t timer;     /**< LEDC timer used by this device */
    uint8_t ch_numof;       /**< Number of channels used by this device */
    const gpio_t *gpios;    /**< GPIOs used as channels of this device */
} pwm_config_t;

/**
 * @brief   Maximum number of PWM devices
 */
#define PWM_NUMOF_MAX       (4)

/**
 * @brief   Maximum number of channels per PWM device.
 */
#define PWM_CH_NUMOF_MAX    (SOC_LEDC_CHANNEL_NUM)

/** @} */

/**
 * @name    RNG configuration
 * @{
 */

/**
 * @brief   The address of the register for accessing the hardware RNG.
 */
#define RNG_DATA_REG_ADDR   (WDEV_RND_REG)
/** @} */

/**
 * @name    RTT and RTC configuration
 * @{
 */

/**
 * @brief   RTT frequency definition
 *
 * The RTT frequency is always 32.768 kHz even if no external crystal is
 * connected. In this case the RTT value counted with the internal 150 kHz
 * RC oscillator is converted to a value for an RTT with 32.768 kHz.
 */
#define RTT_FREQUENCY       (32768UL)

/**
 * @brief   RTT is a 32-bit counter
 */
#define RTT_MAX_VALUE       (0xFFFFFFFFUL)

/** @} */

/**
 * @name   SPI configuration
 *
 * ESP32 has four SPI controllers:
 *
 * - controller SPI0 is reserved for caching the flash memory (CPSI)
 * - controller SPI1 is reserved for external memories like flash and PSRAM (FSPI)
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
 * the definition of
 *
 * - SPIn_CTRL, the SPI controller which is used for the interface (VSPI or HSPI),
 * - SPIn_SCK, the GPIO used as clock signal
 * - SPIn_MISO, the GPIO used as MISO signal
 * - SPIn_MOSI, the GPIO used as MOSI signal, and
 * - SPIn_CS0, the GPIO used as CS signal when the cs parameter in spi_aquire
 *   is GPIO_UNDEF,
 *
 * where n can be 0 or 1. If they are not defined, the according SPI interface
 * SPI_DEV(n) is not used.
 *
 * SPI_NUMOF is determined automatically from the board-specific peripheral
 * definitions of SPIn_*.
 *
 * @{
 */

#ifndef DOXYGEN
/**
 * @brief    Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100000,            /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400000,            /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000000,           /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = 5000000,           /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000           /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */
#endif /* !DOXYGEN */

/**
 * @brief   SPI controllers that can be used for peripheral interfaces
 */
typedef enum {
    HSPI = HSPI_HOST,       /**< HSPI interface controller */
    VSPI = VSPI_HOST,       /**< VSPI interface controller */
} spi_ctrl_t;

/**
 * @brief   SPI configuration structure type
 */
typedef struct {
    spi_ctrl_t ctrl;        /**< SPI controller used for the interface */
    gpio_t sck;             /**< GPIO used as SCK pin */
    gpio_t mosi;            /**< GPIO used as MOSI pin */
    gpio_t miso;            /**< GPIO used as MISO pin */
    gpio_t cs;              /**< GPIO used as CS0 pin */
} spi_conf_t;

/**
 * @brief   Maximum number of SPI interfaces that can be used by board definitions
 */
#define SPI_NUMOF_MAX   (SOC_SPI_PERIPH_NUM - 1)

#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  /**< requires function spi_transfer_byte */
#define PERIPH_SPI_NEEDS_TRANSFER_REG   /**< requires function spi_transfer_reg */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  /**< requires function spi_transfer_regs */
/** @} */

/**
 * @name   Timer configuration depending on which implementation is used
 *
 * Timers are MCU built-in feature and not board-specific. They are therefore
 * configured here.
 * @{
 */

#ifdef MODULE_ESP_HW_COUNTER
/** Hardware ccount/ccompare registers are used for timer implementation */
#define TIMER_NUMOF         (2)
#define TIMER_CHANNEL_NUMOF (1)
#else
/**
 * @brief Hardware timer modules are used for timer implementation (default)
 *
 * Since one timer is used for the system time, there is one timer less than
 * the total number of timers.
 */
#define TIMER_NUMOF         (SOC_TIMER_GROUP_TOTAL_TIMERS - 1)
#define TIMER_CHANNEL_NUMOF (1)
#endif

/** Timer group used for system time */
#define TIMER_SYSTEM_GROUP      TIMER_GROUP_0
/** Index of the timer in the timer timer group used for system time */
#define TIMER_SYSTEM_INDEX      TIMER_0
/** System time interrupt source */
#define TIMER_SYSTEM_INT_SRC    ETS_TG0_T0_LEVEL_INTR_SOURCE

/** @} */

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @name   UART configuration
 *
 * ESP32 supports up to three UART devices. UART_DEV(0) has a fixed pin
 * configuration and is always available. All ESP32 boards use it as standard
 * configuration for the console.
 *
 *      UART_DEV(0).TXD      GPIO1
 *      UART_DEV(0).RXD      GPIO3
 *
 * The pin configuration of UART_DEV(1) and UART_DEV(2) are defined in
 * board specific peripheral configuration by
 *
 * - UARTn_TXD, the GPIO used as TxD signal, and
 * - UARTn_RXD, the GPIO used as RxD signal,
 *
 * where n can be 1 or 2. If they are not defined, the according UART interface
 * UART_DEV(n) is not used.
 *
 * UART_NUMOF is determined automatically from the board-specific peripheral
 * definitions of UARTn_*.
 *
 * @{
 */

/**
 * @brief   UART configuration structure type
 */
typedef struct {
    gpio_t txd;             /**< GPIO used as TxD pin */
    gpio_t rxd;             /**< GPIO used as RxD pin */
} uart_conf_t;

/**
 * @brief   Maximum number of UART interfaces
 */
#define UART_NUMOF_MAX  (SOC_UART_NUM)
/** @} */

#ifdef MODULE_PERIPH_CAN
#include "can_esp.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
