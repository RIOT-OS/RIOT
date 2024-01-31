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
 * @brief       ESP32-S2 specific peripheral configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_ESP32S2_H
#define PERIPH_CPU_ESP32S2_H

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Mapping configured ESP32-S2 default clock to CLOCK_CORECLOCK define */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)

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
#define GPIO20      (GPIO_PIN(PORT_GPIO, 20))
#define GPIO21      (GPIO_PIN(PORT_GPIO, 21))
/* GPIOs 22 ...25 are not available */
#define GPIO26      (GPIO_PIN(PORT_GPIO, 26))
#define GPIO27      (GPIO_PIN(PORT_GPIO, 27))
#define GPIO28      (GPIO_PIN(PORT_GPIO, 28))
#define GPIO29      (GPIO_PIN(PORT_GPIO, 29))
#define GPIO30      (GPIO_PIN(PORT_GPIO, 30))
#define GPIO31      (GPIO_PIN(PORT_GPIO, 31))
#define GPIO32      (GPIO_PIN(PORT_GPIO, 32))
#define GPIO33      (GPIO_PIN(PORT_GPIO, 33))
#define GPIO34      (GPIO_PIN(PORT_GPIO, 34))
#define GPIO35      (GPIO_PIN(PORT_GPIO, 35))
#define GPIO36      (GPIO_PIN(PORT_GPIO, 36))
#define GPIO37      (GPIO_PIN(PORT_GPIO, 37))
#define GPIO38      (GPIO_PIN(PORT_GPIO, 38))
#define GPIO39      (GPIO_PIN(PORT_GPIO, 39))
#define GPIO40      (GPIO_PIN(PORT_GPIO, 40))
#define GPIO41      (GPIO_PIN(PORT_GPIO, 41))
#define GPIO42      (GPIO_PIN(PORT_GPIO, 42))
#define GPIO43      (GPIO_PIN(PORT_GPIO, 43))
#define GPIO44      (GPIO_PIN(PORT_GPIO, 44))
#define GPIO45      (GPIO_PIN(PORT_GPIO, 45))
#define GPIO46      (GPIO_PIN(PORT_GPIO, 46))
/** @} */

/**
 * @name   ADC configuration
 *
 * ESP32-S2 integrates two 13-bit ADCs (ADC1 and ADC2) with 20 channels in
 * total:
 *
 * - **ADC1** supports 10 channels: GPIO1 ... GPIO10
 * - **ADC2** supports 10 channels: GPIO11 ... GPIO20
 *
 * @note
 * - ADC2 is also used by the WiFi module. The GPIOs connected to ADC2 are
 *   therefore not available as ADC channels if the modules `esp_wifi` or
 *   `esp_now` are used.
 * - Vref can be read with function #adc_line_vref_to_gpio at an ADC2 channel,
 *   that is at GPIO11 ... GPIO20.
 * - GPIO3 is a strapping pin und shouldn't be used as ADC channel
 */

/**
 * @name   I2C configuration
 *
 * ESP32-S2 has two built-in I2C interfaces.
 *
 * The following table shows the default configuration of I2C interfaces
 * used for ESP32-S2 boards. It can be overridden by
 * [application-specific configurations](#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device     | Signal | Pin    | Symbol        | Remarks
 * :----------|:-------|:-------|:--------------|:----------------
 * I2C_DEV(0) |        |        | `#I2C0_SPEED` | default is `I2C_SPEED_FAST`
 * I2C_DEV(0) | SCL    | GPIO9  | `#I2C0_SCL`   | -
 * I2C_DEV(0) | SDA    | GPIO8  | `#I2C0_SDA`   | -
 *
 * </center><br>
 */

/**
 * @name   PWM configuration
 *
 * The ESP32-S2 LEDC module has 1 channel group with 8 channels. Each of
 * these channels can be clocked by one of the 4 timers.
 */

/**
 * @name   SPI configuration
 *
 * ESP32-S2 has four SPI controllers where SPI0 and SPI1 share the same bus
 * and can only operate in memory mode while SPI2 and SPI3 can be used as general
 * purpose SPI:
 *
 * - controller SPI0 is reserved for external memories like Flash and PSRAM
 * - controller SPI1 is reserved for external memories like Flash and PSRAM
 * - controller SPI2 can be used for peripherals (also called FSPI)
 * - controller SPI3 can be used for peripherals
 *
 * Thus, SPI2 (`FSPI`) and SPI3 can be used as general purpose SPI in
 * RIOT as SPI_DEV(0) and SPI_DEV(1) by defining the symbols `SPI0_*`
 * and `SPI1_*`.
 *
 * The following table shows the pin configuration used by default, even
 * though it **can vary** from board to board.
 *
 * <center>
 *
 * Device (Alias)          | Signal | Pin    | Symbol      | Remarks
 * :-----------------------|:------:|:-------|:-----------:|:---------------------------
 * `SPI_HOST0`/`SPI_HOST1` | SPICS0 | GPIO29 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPICS1 | GPIO26 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPICLK | GPIO30 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPID   | GPIO32 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIQ   | GPIO31 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIHD  | GPIO27 | - | reserved for Flash and PSRAM (only in `qio` or `qout` mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIWP  | GPIO28 | - | reserved for Flash and PSRAM (only in `qio` or `qout` mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIIO4 | GPIO33 | - | reserved for Flash and PSRAM (only in octal mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIIO5 | GPIO34 | - | reserved for Flash and PSRAM (only in octal mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIIO6 | GPIO35 | - | reserved for Flash and PSRAM (only in octal mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIIO7 | GPIO36 | - | reserved for Flash and PSRAM (only in octal mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIDQA | GPIO37 | - | reserved for Flash and PSRAM (only in octal mode)
 * `SPI_HOST2` (`FSPI`)    | SCK    | GPIO36 |`#SPI0_SCK`  | can be used
 * `SPI_HOST2` (`FSPI`)    | MOSI   | GPIO35 |`#SPI0_MOSI` | can be used
 * `SPI_HOST2` (`FSPI`)    | MISO   | GPIO37 |`#SPI0_MISO` | can be used
 * `SPI_HOST2` (`FSPI`)    | CS0    | GPIO38 |`#SPI0_CS0`  | can be used
 * `SPI_HOST3` (`HSPI`)    | SCK    | -      |`#SPI1_SCK`  | can be used
 * `SPI_HOST3` (`HSPI`)    | MOSI   | -      |`#SPI1_MOSI` | can be used
 * `SPI_HOST3` (`HSPI`)    | MISO   | -      |`#SPI1_MISO` | can be used
 * `SPI_HOST3` (`HSPI`)    | CS0    | -      |`#SPI1_CS0`  | can be used
 *
 * </center><br>
 */

/**
 * @name   Timer configuration depending on which implementation is used
 *
 * Timers are MCU built-in feature and not board-specific. They are therefore
 * configured here.
 *
 * ESP32-S2 has two timer groups with two timers each, resulting in a total of
 * four timers. Since one timer is used as system timer, up to three timers
 * with one channel each can be used in RIOT as timer devices
 * TIMER_DEV(0) ... TIMER_DEV(2).
 *
 * Additionally ESP32-S2 has three CCOMPARE registers which can be used
 * alternatively as timer devices TIMER_DEV(0) ... TIMER_DEV(2) can be used
 * in RIOT if the module `esp_hw_counter` is enabled.
 */

#ifdef MODULE_ESP_HW_COUNTER
/** Hardware ccount/ccompare registers are used for timer implementation */
#define TIMER_NUMOF         (2)
#define TIMER_CHANNEL_NUMOF (1)
#endif

/**
 * @name   UART configuration
 *
 * ESP32-S2 integrates two UART interfaces. The following default pin
 * configuration of UART interfaces as used by a most boards can be overridden
 * by the application, see section [Application-Specific Configurations]
 * (#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device      |Signal|Pin     |Symbol      |Remarks
 * :-----------|:-----|:-------|:-----------|:----------------
 * UART_DEV(0) | TxD  | GPIO43 |`#UART0_TXD`| cannot be changed
 * UART_DEV(0) | RxD  | GPIO44 |`#UART0_RXD`| cannot be changed
 * UART_DEV(1) | TxD  | GPIO17 |`#UART1_TXD`| optional, can be overridden
 * UART_DEV(1) | RxD  | GPIO18 |`#UART1_RXD`| optional, can be overridden
 * UART_DEV(2) | TxD  | -      |`UART2_TXD` | optional, can be overridden (no direct I/O)
 * UART_DEV(2) | RxD  | -      |`UART2_RXD` | optional, can be overridden (no direct I/O)
 *
 * </center><br>
 */

/**
 * @name   USB device configuration
 *
 * ESP32x SoCs have:
 * - a bidirectional control endpoint EP0 IN and EP0 OUT
 * - six additional endpoints EP1 .. EP6 that can be configured as IN our OUT
 * - a maximum of five IN endpoints concurrently active at any time (including EP0 IN)
 * - all OUT endpoints share a single RX FIFO
 * - each IN endpoint has a dedicated TX FIFO
 *
 * To avoid a lot of special case handling, the maximum number of IN an OUT
 * endpoints including the control endpoint EP0 is 5.
 *
 * @{
 */

/**
 * @brief   Enable the USB OTG FS peripheral
 *
 * At the moment, only FS is supported on ESP32x SoCs.
 */
#define DWC2_USB_OTG_FS_ENABLED         1

/**
 * @brief   Number of USB OTG FS IN endpoints including the control endpoint
 */
#define DWC2_USB_OTG_FS_NUM_EP          (5)

/**
 * @brief   Size of the FIFO shared by all USB OTG FS OUT endpoints in 32-bit words
 */
#ifndef DWC2_USB_OTG_FS_RX_FIFO_SIZE
#define DWC2_USB_OTG_FS_RX_FIFO_SIZE    (128U)
#endif

/**
 * @brief   Total size of the FIFO in bytes
 */
#define DWC2_USB_OTG_FS_TOTAL_FIFO_SIZE (1024U)

/**
 * @brief   Buffers have to be word aligned for DMA
 */
#define USBDEV_CPU_DMA_ALIGNMENT        (4)

/**
 * @brief Number of USB IN and OUT endpoints available
 */
#define USBDEV_NUM_ENDPOINTS            DWC2_USB_OTG_FS_NUM_EP
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_ESP32S2_H */
/** @} */
