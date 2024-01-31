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
 * @brief       ESP32 specific peripheral configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_ESP32_H
#define PERIPH_CPU_ESP32_H

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Mapping configured ESP32 default clock to CLOCK_CORECLOCK define */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)

/**
 * @brief   CPU cycles per busy wait loop
 */
#define CPU_CYCLES_PER_LOOP (6)

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO, 0))
#define GPIO1       (GPIO_PIN(PORT_GPIO, 1))
#define GPIO2       (GPIO_PIN(PORT_GPIO, 2))
#define GPIO3       (GPIO_PIN(PORT_GPIO, 3))
#define GPIO4       (GPIO_PIN(PORT_GPIO, 4))
#define GPIO5       (GPIO_PIN(PORT_GPIO, 5))
#define GPIO6       (GPIO_PIN(PORT_GPIO, 6))
#define GPIO7       (GPIO_PIN(PORT_GPIO, 7))
#define GPIO8       (GPIO_PIN(PORT_GPIO, 8))
#define GPIO9       (GPIO_PIN(PORT_GPIO, 9))
#define GPIO10      (GPIO_PIN(PORT_GPIO, 10))
#define GPIO11      (GPIO_PIN(PORT_GPIO, 11))
#define GPIO12      (GPIO_PIN(PORT_GPIO, 12))
#define GPIO13      (GPIO_PIN(PORT_GPIO, 13))
#define GPIO14      (GPIO_PIN(PORT_GPIO, 14))
#define GPIO15      (GPIO_PIN(PORT_GPIO, 15))
#define GPIO16      (GPIO_PIN(PORT_GPIO, 16))
#define GPIO17      (GPIO_PIN(PORT_GPIO, 17))
#define GPIO18      (GPIO_PIN(PORT_GPIO, 18))
#define GPIO19      (GPIO_PIN(PORT_GPIO, 19))
/* GPIO 20 is not available */
#define GPIO21      (GPIO_PIN(PORT_GPIO, 21))
#define GPIO22      (GPIO_PIN(PORT_GPIO, 22))
#define GPIO23      (GPIO_PIN(PORT_GPIO, 23))
/* GPIO 24 is not available */
#define GPIO25      (GPIO_PIN(PORT_GPIO, 25))
#define GPIO26      (GPIO_PIN(PORT_GPIO, 26))
#define GPIO27      (GPIO_PIN(PORT_GPIO, 27))
/* GPIOs 28 ...32 are not available */
#define GPIO32      (GPIO_PIN(PORT_GPIO, 32))
#define GPIO33      (GPIO_PIN(PORT_GPIO, 33))
/* GPIOs 34 ... 39 can only be used as inputs and do not have pullups/pulldowns */
#define GPIO34      (GPIO_PIN(PORT_GPIO, 34))
#define GPIO35      (GPIO_PIN(PORT_GPIO, 35))
#define GPIO36      (GPIO_PIN(PORT_GPIO, 36))
#define GPIO37      (GPIO_PIN(PORT_GPIO, 37))
#define GPIO38      (GPIO_PIN(PORT_GPIO, 38))
#define GPIO39      (GPIO_PIN(PORT_GPIO, 39))
/** @} */

/**
 * @name   ADC configuration
 *
 * ESP32 integrates two 12-bit ADCs (ADC1 and ADC2) with 18 channels in total:
 *
 * - **ADC1** supports 8 channels: GPIOs 32-39
 * - **ADC2** supports 10 channels: GPIOs 0, 2, 4, 12-15, 25-27
 *
 * Most of these ADC channels are either connected to a number of integrated
 * sensors like a Hall sensors, touch sensors and a temperature sensor or can
 * be connected with certain GPIOs. Integrated sensors are disabled in RIOT's
 * implementation and are not accessible. Thus, up to 18 GPIOs can be used as
 * ADC channels:
 *
 * @note
 * - GPIO37 and GPIO38 are usually not broken out on ESP32 boards and can not
 *   be used therefore.
 * - ADC2 is also used by the WiFi module. The GPIOs connected to ADC2 are
 *   therefore not available as ADC channels if the modules `esp_wifi` or
 *   `esp_now` are used.
 * - Vref can be read with function #adc_line_vref_to_gpio at GPIO25.
 */

/**
 * @name   DAC configuration
 *
 * ESP32 supports 2 DAC lines at GPIO25 and GPIO26.
 */

/**
 * @name   I2C configuration
 *
 * ESP32 has two built-in I2C interfaces.
 *
 * The following table shows the default configuration of I2C interfaces
 * used for a large number of ESP32 boards. It can be overridden by
 * [application-specific configurations](#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device     | Signal | Pin    | Symbol        | Remarks
 * :----------|:-------|:-------|:--------------|:----------------
 * I2C_DEV(0) |        |        | `#I2C0_SPEED` | default is `I2C_SPEED_FAST`
 * I2C_DEV(0) | SCL    | GPIO22 | `#I2C0_SCL`   | -
 * I2C_DEV(0) | SDA    | GPIO21 | `#I2C0_SDA`   | -
 *
 * </center><br>
 */

/**
 * @name   PWM configuration
 *
 * The ESP32 LEDC module has 2 channel groups with 8 channels each. Each of
 * these channels can be clocked by one of the 4 timers.
 */

/**
 * @name   SDMMC configuration
 */

/**
 * @name   SPI configuration
 *
 * ESP32 has four SPI controllers where SPI0 and SPI1 share the same bus and
 * are used as interface for external memory:
 *
 * - Controller SPI0 is reserved for caching external memory like Flash
 * - Controller SPI1 is reserved for external memoriy like PSRAM
 * - Controller SPI2 can be used as general purpose SPI (also called HSPI)
 * - Controller SPI3 can be used as general purpose SPI (also called VSPI)
 *
 * Thus, only SPI2 (HSPI) and SPI3 (VSPI) can be used as general purpose SPI
 * in RIOT as SPI_DEV(0) and SPI_DEV(1) in arbitrary order.
 *
 * The following table shows the pin configuration used for most boards, even
 * though it **can vary** from board to board.
 *
 * <center>
 *
 * Device                  |Signal| Pin    | Symbol      | Remarks
 * :-----------------------|:----:|:-------|:-----------:|:---------------------------
 * `SPI_HOST0`/`SPI_HOST1` | SCK  | GPIO6  |- | reserved for flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | CMD  | GPIO11 |- | reserved for flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SD0  | GPIO7  |- | reserved for flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SD1  | GPIO8  |- | reserved for flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SD2  | GPIO9  |- | reserved for flash and PSRAM (only in `qio` or `qout` mode)
 * `SPI_HOST0`/`SPI_HOST1` | SD3  | GPIO10 |- | reserved for flash and PSRAM (only in `qio` or `qout` mode)
 * `SPI_HOST2` (`HSPI`)    | SCK  | GPIO14 |`#SPI1_SCK`  | can be used
 * `SPI_HOST2` (`HSPI`)    | MISO | GPIO12 |`#SPI1_MISO` | can be used
 * `SPI_HOST2` (`HSPI`)    | MOSI | GPIO13 |`#SPI1_MOSI` | can be used
 * `SPI_HOST2` (`HSPI`)    | CS0  | GPIO15 |`#SPI1_CS0`  | can be used
 * `SPI_HOST3` (`VSPI`)    | SCK  | GPIO18 |`#SPI0_SCK`  | can be used
 * `SPI_HOST3` (`VSPI`)    | MISO | GPIO19 |`#SPI0_MISO` | can be used
 * `SPI_HOST3` (`VSPI`)    | MOSI | GPIO23 |`#SPI0_MOSI` | can be used
 * `SPI_HOST3` (`VSPI`)    | CS0  | GPIO18 |`#SPI0_CS0`  | can be used
 *
 * </center><br>
 *
 * Some boards use the `HSPI` (`SPI_HOST2`) as SD-Card interface (SDIO) in 4-bit SD mode.
 *
 * <center>
 *
 * Device|Pin            | SD 4-bit mode | SPI mode
 * :---------------------|:------:|:-------------:|:----------:
 * `SPI_HOST2` (`HSPI`)  | GPIO14 | CLK           | SCK
 * `SPI_HOST2` (`HSPI`)  | GPIO15 | CMD           | CS0
 * `SPI_HOST2` (`HSPI`)  | GPIO2  | DAT0          | MISO
 * `SPI_HOST2` (`HSPI`)  | GPIO4  | DAT1          | -
 * `SPI_HOST2` (`HSPI`)  | GPIO12 | DAT2          | -
 * `SPI_HOST2` (`HSPI`)  | GPIO13 | DAT3          | MOSI
 *
 * </center><br>
 *
 * @note On these boards, all these signals are pulled up. This may cause
 *       flashing problems due to the bootstrap function of the GPIO2 pin,
 *       see section [GPIO pins](#esp32_gpio_pins_esp32). Therefore, it can
 *       be necessary to either press the **Boot** button, to remove the SD card
 *       or to remove the peripheral hardware to flash RIOT.
 */

/**
 * @name   Timer configuration depending on which implementation is used
 *
 * Timers are MCU built-in feature and not board-specific. They are therefore
 * configured here.
 *
 * ESP32 has two timer groups with two timers each, resulting in a total of
 * four timers. Since one timer is used as system timer, up to three timers
 * with one channel each can be used in RIOT as timer devices
 * TIMER_DEV(0) ... TIMER_DEV(2).
 *
 * Additionally ESP32 has three CCOMPARE registers which can be used
 * alternatively as timer devices TIMER_DEV(0) ... TIMER_DEV(2) can be used
 * in RIOT if the module `esp_hw_counter` is enabled.
 *
 * @{
 */

#ifdef MODULE_ESP_HW_COUNTER
/** Hardware ccount/ccompare registers are used for timer implementation */
#define TIMER_NUMOF         (2)
#define TIMER_CHANNEL_NUMOF (1)
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 integrates three UART interfaces. The following default pin
 * configuration of UART interfaces as used by a most boards can be overridden
 * by the application, see section [Application-Specific Configurations]
 * (#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device      |Signal|Pin     |Symbol      |Remarks
 * :-----------|:-----|:-------|:-----------|:----------------
 * UART_DEV(0) | TxD  | GPIO1  |`#UART0_TXD`| cannot be changed
 * UART_DEV(0) | RxD  | GPIO3  |`#UART0_RXD`| cannot be changed
 * UART_DEV(1) | TxD  | GPIO10 |`#UART1_TXD`| optional, can be overridden
 * UART_DEV(1) | RxD  | GPIO9  |`#UART1_RXD`| optional, can be overridden
 * UART_DEV(2) | TxD  | -      |`UART2_TXD` | optional, can be overridden
 * UART_DEV(2) | RxD  | -      |`UART2_RXD` | optional, can be overridden
 *
 * </center><br>
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_ESP32_H */
/** @} */
