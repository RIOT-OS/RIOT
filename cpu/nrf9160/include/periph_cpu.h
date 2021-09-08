/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf9160
 * @{
 *
 * @file
 * @brief           nRF9160 specific definitions for handling peripherals
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 64MHz for all NRF9160 CPUs
 */
#define CLOCK_CORECLOCK     MHZ(64)

/**
 * @brief    Peripheral clock speed (fixed to 16MHz for nRF9160 based CPUs)
 */
#define PERIPH_CLOCK        MHZ(16)

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    NRF_UARTE_Type *dev;    /**< UART with EasyDMA device base
                             * register address */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t rts_pin;         /**< RTS pin */
    gpio_t cts_pin;         /**< CTS pin */
#endif
    uint8_t irqn;           /**< IRQ channel */
} uart_conf_t;

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE    (64)
#endif

#ifndef DOXYGEN
/**
 * @brief Wrapper to fix differences between nRF9160 and
  *       nRF52 vendor files
  */
#define UART_BAUDRATE_BAUDRATE_Baud1200 UARTE_BAUDRATE_BAUDRATE_Baud1200
#define UART_BAUDRATE_BAUDRATE_Baud2400 UARTE_BAUDRATE_BAUDRATE_Baud2400
#define UART_BAUDRATE_BAUDRATE_Baud4800 UARTE_BAUDRATE_BAUDRATE_Baud4800
#define UART_BAUDRATE_BAUDRATE_Baud9600 UARTE_BAUDRATE_BAUDRATE_Baud9600
#define UART_BAUDRATE_BAUDRATE_Baud14400 UARTE_BAUDRATE_BAUDRATE_Baud14400
#define UART_BAUDRATE_BAUDRATE_Baud19200 UARTE_BAUDRATE_BAUDRATE_Baud19200
#define UART_BAUDRATE_BAUDRATE_Baud28800 UARTE_BAUDRATE_BAUDRATE_Baud28800
#define UART_BAUDRATE_BAUDRATE_Baud31250 UARTE_BAUDRATE_BAUDRATE_Baud31250
#define UART_BAUDRATE_BAUDRATE_Baud38400 UARTE_BAUDRATE_BAUDRATE_Baud38400
#define UART_BAUDRATE_BAUDRATE_Baud56000 UARTE_BAUDRATE_BAUDRATE_Baud56000
#define UART_BAUDRATE_BAUDRATE_Baud57600 UARTE_BAUDRATE_BAUDRATE_Baud57600
#define UART_BAUDRATE_BAUDRATE_Baud76800 UARTE_BAUDRATE_BAUDRATE_Baud76800
#define UART_BAUDRATE_BAUDRATE_Baud115200 UARTE_BAUDRATE_BAUDRATE_Baud115200
#define UART_BAUDRATE_BAUDRATE_Baud230400 UARTE_BAUDRATE_BAUDRATE_Baud230400
#define UART_BAUDRATE_BAUDRATE_Baud250000 UARTE_BAUDRATE_BAUDRATE_Baud250000
#define UART_BAUDRATE_BAUDRATE_Baud460800 UARTE_BAUDRATE_BAUDRATE_Baud460800
#define UART_BAUDRATE_BAUDRATE_Baud921600 UARTE_BAUDRATE_BAUDRATE_Baud921600
#define UART_BAUDRATE_BAUDRATE_Baud1M UARTE_BAUDRATE_BAUDRATE_Baud1M

/**
 * @brief   Override I2C speed settings
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 0xff,                         /**< not supported */
    I2C_SPEED_NORMAL    = TWIM_FREQUENCY_FREQUENCY_K100,    /**< 100kbit/s */
    I2C_SPEED_FAST      = TWIM_FREQUENCY_FREQUENCY_K400,    /**< 400kbit/s */
    I2C_SPEED_FAST_PLUS = 0xfe,                         /**< not supported */
    I2C_SPEED_HIGH      = 0xfd,                         /**< not supported */
} i2c_speed_t;
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @brief   I2C (TWI) configuration options
 */
typedef struct {
    NRF_TWIM_Type *dev;         /**< TWIM hardware device */
    gpio_t scl;                 /**< SCL pin */
    gpio_t sda;                 /**< SDA pin */
    i2c_speed_t speed;          /**< Bus speed */
} i2c_conf_t;
/** @} */

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_WRITE_REG
/** @} */

/**
 * @name    Define macros for sda and scl pin to be able to reinitialize them
 * @{
 */
#define i2c_pin_sda(dev) i2c_config[dev].sda
#define i2c_pin_scl(dev) i2c_config[dev].scl
/** @} */

/**
 * @brief   Defines macros for SPI pins initialization
 * @{
 */
#define SPI_SCKSEL          (dev(bus)->PSEL.SCK)
#define SPI_MOSISEL         (dev(bus)->PSEL.MOSI)
#define SPI_MISOSEL         (dev(bus)->PSEL.MISO)
/** @} */

/**
 * @brief   SPI temporary buffer size for storing const data in RAM before
 *          initiating DMA transfer
 */
#ifndef CONFIG_SPI_MBUF_SIZE
#define CONFIG_SPI_MBUF_SIZE    64
#endif

/**
 * @brief  SPI configuration values
 */
typedef struct {
    NRF_SPIM_Type *dev; /**< SPI device used */
    gpio_t sclk;        /**< CLK pin */
    gpio_t mosi;        /**< MOSI pin */
    gpio_t miso;        /**< MISO pin */
} spi_conf_t;

#ifndef DOXYGEN
/**
 * @brief   Override SPI mode values
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                             /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = SPIM_CONFIG_CPHA_Msk,                           /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = SPIM_CONFIG_CPOL_Msk,                           /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPIM_CONFIG_CPOL_Msk | SPIM_CONFIG_CPHA_Msk)    /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override SPI clock values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = SPIM_FREQUENCY_FREQUENCY_K125,  /**< 100KHz */
    SPI_CLK_400KHZ = SPIM_FREQUENCY_FREQUENCY_K500,  /**< 400KHz */
    SPI_CLK_1MHZ   = SPIM_FREQUENCY_FREQUENCY_M1,    /**< 1MHz */
    SPI_CLK_5MHZ   = SPIM_FREQUENCY_FREQUENCY_M4,    /**< 5MHz */
    SPI_CLK_10MHZ  = SPIM_FREQUENCY_FREQUENCY_M8     /**< 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief Common SPI/I2C interrupt callback
 *
 * @param   arg     Opaque context pointer
 */
typedef void (*spi_twi_irq_cb_t)(void *arg);

/**
 * @brief Reqister a SPI IRQ handler for a shared I2C/SPI irq vector
 *
 * @param   bus bus to register the IRQ handler on
 * @param   cb  callback to call on IRQ
 * @param   arg Argument to pass to the handler
 */
void spi_twi_irq_register_spi(NRF_SPIM_Type *bus,
                              spi_twi_irq_cb_t cb, void *arg);

/**
 * @brief Reqister a I2C IRQ handler for a shared I2C/SPI irq vector
 *
 * @param   bus bus to register the IRQ handler on
 * @param   cb  callback to call on IRQ
 * @param   arg Argument to pass to the handler
 */
void spi_twi_irq_register_i2c(NRF_TWIM_Type *bus,
                              spi_twi_irq_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
