/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_msp430_f2xx_g2xx
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef F2XX_G2XX_PERIPH_CPU_H
#define F2XX_G2XX_PERIPH_CPU_H

#include <stdbool.h>

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Override SPI mode selection values
 * @{
 */
#define HAVE_SPI_MODE_T     /**< MSP430 F2xx/G2xx has a custom spi_mode_t */
/**
 * @brief   Support SPI modes
 *
 * | Field  | Name                  |   Description                                         |
 * |:------ |:--------------------- |:----------------------------------------------------- |
 * | UCCKPH | Clock phase select    | 0 = capture on second edge, 1 = capture on first      |
 * | UCCKPL | Clock polarity select | 0 = clock is idle-low, 1 = clock is high idle-high    |
 */
typedef enum {
    SPI_MODE_0 = (UCCKPH),              /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0,                     /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (UCCKPL | UCCKPH),     /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (UCCKPL)               /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @name    Override SPI clock speed selection values
 * @{
 */
#define HAVE_SPI_CLK_T     /**< MSP430 F2xx/G2xx has a custom spi_clock_t */
/**
 * @brief   Support SPI clock frequencies
 */
typedef enum {
    SPI_CLK_100KHZ = 100000,        /**< 100 kHz */
    SPI_CLK_400KHZ = 400000,        /**< 400 kHz */
    SPI_CLK_1MHZ   = 1000000,       /**< 1 MHz */
    SPI_CLK_5MHZ   = 5000000,       /**< 5 MHz */
    SPI_CLK_10MHZ  = SPI_CLK_5MHZ,  /**< 10 MHz not supported, falling back to 5 MHz */
} spi_clk_t;
/** @} */

/**
 * @name declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS        /**< use shared spi_init_cs() */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  /**< use shared spi_transfer_byte() */
#define PERIPH_SPI_NEEDS_TRANSFER_REG   /**< use shared spi_transfer_reg() */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  /**< use shared spi_transfer_regs() */
/** @} */

/**
 * @brief   Identifiers for USCI instances
 *
 * This assigns the four USCI instances (A0, B0, A1, B1) numbers from 0 to 3.
 */
typedef enum {
#ifdef __MSP430_HAS_USCI_AB0__
    MSP430_USCI_ID_A0,      /**< USCI A0 */
    MSP430_USCI_ID_B0,      /**< USCI B0 */
#endif
#ifdef __MSP430_HAS_USCI_AB1__
    MSP430_USCI_ID_A1,      /**< USCI A1 */
    MSP430_USCI_ID_B1,      /**< USCI B1 */
#endif
    MSP430_USCI_ID_NUMOF    /**< Number of USCI IDs (also: number of USCI instances) */
} msp430_usci_id_t;

/**
 * @brief   MSP430 F2xx/G2xx USCI configuration
 *
 * @details This is intended to be stored in flash.
 */
typedef struct {
    msp430_usci_b_t *dev;       /**< The USCI device to use */

    REG8 *interrupt_enable;     /**< The interrupt enable register matching the USCI */
    REG8 *interrupt_flag;       /**< The interrupt flag register matching the USCI */
    uint8_t tx_irq_mask;        /**< The bitmask to enable the TX IRQ for this USCI*/
    uint8_t rx_irq_mask;        /**< The bitmask to enable the TX IRQ for this USCI */
    msp430_usci_id_t id;        /**< ID of the USCI */
} msp430_usci_params_t;

/**
 * @brief   MSP430 F2xx/G2xx USCI clock source
 */
typedef enum {
    USCI_CLK_UCLKI     = UCSSEL_UCLKI, /**< UCLKI clock source (not supported yet) */
    USCI_CLK_AUX       = UCSSEL_ACLK,  /**< auxiliary clock source */
    USCI_CLK_SUBMAIN   = UCSSEL_SMCLK, /**< sub-system master clock source */
} msp430_usci_clk_t;

/**
 * @brief   MSP430 F2xx/G2xx USCI prescaler configuration
 */
typedef struct {
    msp430_usci_clk_t clk_source;   /**< Clock source to use */
    uint8_t br0;                    /**< What to write in the BR0 register */
    uint8_t br1;                    /**< What to write in the BR1 register */
    uint8_t mctl;                   /**< USCI modulation control register */
} msp430_usci_prescaler_t;

/**
 * @brief   MSP430 F2xx/G2xx USCI configuration registers
 *
 * @details Unlike @ref msp430_usci_params_t this contains configuration that
 *          may depends on runtime settings
 */
typedef struct {
    msp430_usci_prescaler_t prescaler;  /**< Prescaler configuration */
    uint8_t ctl0;                       /**< USCI control register 0 */
} msp430_usci_conf_t;

/**
 * @brief   MSP430 F2xx/G2xx UART configuration, CPU level
 *
 * The MSP430 F2xx/G2xx has two USCI peripherals which both can be operated in
 * UART mode. Each is connected to a fixed GPIO for RXD and TXD, respectively.
 * Hence, there is not much left for the board to configure anyway, so we just
 * prepare UART configurations at CPU level for the board to refer to. The
 * unused configuration(s) will be garbage collected by the linker.
 */
typedef struct {
    msp430_usci_params_t usci_params; /**< The USCI params */
    gpio_t rxd;                     /**< RXD pin */
    gpio_t txd;                     /**< TXD pin */
} msp430_usci_uart_params_t;

/**
 * @brief   MSP430 F2xx/G2xx UART configuration, board level
 */
typedef struct {
    const msp430_usci_uart_params_t *uart; /**< The UART configuration to use */
} uart_conf_t;

/**
 * @brief   MSP430 F2xx/G2xx SPI configuration, CPU level
 *
 * The MSP430 F2xx/G2xx has two USCI peripherals which both can be operated in
 * SPI mode. Each is connected to a fixed GPIO for COPI (MOSI), CIPO (MISO),
 * and SCK, respectively. Hence, there is not much left for the board to
 * configure anyway, so we just prepare UART configurations at CPU level for
 * the board to refer to. The unused configuration(s) will be garbage collected
 * by the linker.
 */
typedef struct {
    msp430_usci_params_t usci_params;   /**< The USCI parameters */
    gpio_t miso;                        /**< CIPO (MISO) pin */
    gpio_t mosi;                        /**< COPI (MOSI) pin */
    gpio_t sck;                         /**< SCK pin */
} msp430_usci_spi_params_t;

/**
 * @brief   MSP430 F2xx/G2xx SPI configuration, board level
 */
typedef struct {
    const msp430_usci_spi_params_t *spi;    /**< The SPI configuration to use */
} spi_conf_t;

/**
 * @brief   Acquire and initialize USCI for use a SPI/UART peripheral
 *
 * @param   params      Parameter identifying the USCI to use
 * @param   conf        Configuration to initialize the USCI with
 *
 * @note    The USCI will be acquired and configured as specified in @p conf.
 *          However, it will still be held in software reset and all interrupts
 *          will be masked and all interrupt flags be cleared.
 * @warning You cannot enable IRQs while the USCI is still held under reset.
 * @details As currently only for UART USCI IRQs are actually needed, the
 *          ISR is implemented in the UART driver. If the SPI or I2C driver
 *          would start to make use of IRQs (other than polling for the IRQ
 *          flag to be set), the ISRs would need to be moved to the USCI
 *          driver and call into the UART/SPI/I2C driver, depending on what
 *          driver has currently acquired the USCI.
 */
void msp430_usci_acquire(const msp430_usci_params_t *params,
                         const msp430_usci_conf_t *conf);

/**
 * @brief   Release an USCI, so that it can be used to provide other peripherals
 *
 * This will also put the USCI in low power mode.
 */
void msp430_usci_release(const msp430_usci_params_t *params);

/**
 * @brief   Calculate prescaler settings for the given target frequency
 *
 * @param   target_hz   The clock frequency (in Hz) to generated with the
 *                      prescaler
 *
 * @return  The calculated prescaler settings
 *
 * @note    This will select the auxiliary clock source for well known UART
 *          symbol rates up to 9600 Bd, if that is running at 32,768 Hz.
 *          Otherwise the submain clock source is selected.
 */
msp430_usci_prescaler_t msp430_usci_prescale(uint32_t target_hz);

/**
 * @brief   MSP430 F2xx/G2xx USCI A0 in UART configuration
 */
extern const msp430_usci_uart_params_t usci_a0_as_uart;

/**
 * @brief   MSP430 F2xx/G2xx USCI A1 in UART configuration
 */
extern const msp430_usci_uart_params_t usci_a1_as_uart;

/**
 * @brief   MSP430 F2xx/G2xx USCI A0 in SPI configuration
 */
extern const msp430_usci_spi_params_t usci_a0_as_spi;

/**
 * @brief   MSP430 F2xx/G2xx USCI A1 in SPI configuration
 */
extern const msp430_usci_spi_params_t usci_a1_as_spi;

/**
 * @brief   MSP430 F2xx/G2xx USCI B0 in SPI configuration
 */
extern const msp430_usci_spi_params_t usci_b0_as_spi;

/**
 * @brief   MSP430 F2xx/G2xx USCI B1 in SPI configuration
 */
extern const msp430_usci_spi_params_t usci_b1_as_spi;

#ifdef __cplusplus
}
#endif

#endif /* F2XX_G2XX_PERIPH_CPU_H */
/** @} */
