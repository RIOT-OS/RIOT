/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_msp430_x1xx
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include <stdbool.h>

#include "compiler_hints.h"
#include "periph_cpu_common.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   All MSP430 x1xx MCUs have two USART peripherals
 */
#define USART_NUMOF         2

/**
 * @brief   The clock divider of the UASRT (BR register) must be at least 2
 *          when in SPI mode
 */
#define USART_MIN_BR_SPI    2

/**
 * @brief   The clock divider of the UASRT (BR register) must be at least 3
 *          when in UART mode
 */
#define USART_MIN_BR_UART   3

/**
 * @name    Override SPI mode selection values
 * @{
 */
#define HAVE_SPI_MODE_T     /**< MSP430 x1xx has a custom spi_mode_t */
/**
 * @brief   Support SPI modes
 *
 * | Field  | Name                  |   Description                                         |
 * |:------ |:--------------------- |:----------------------------------------------------- |
 * | CKPH   | Clock phase select    | 0 = capture on second edge, 1 = capture on first      |
 * | CKPL   | Clock polarity select | 0 = clock is idle-low, 1 = clock is high idle-high    |
 */
typedef enum {
    SPI_MODE_0 = (CKPH),                /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0,                     /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (CKPL | CKPH),         /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (CKPL)                 /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @name    Override SPI clock speed selection values
 * @{
 */
#define HAVE_SPI_CLK_T     /**< MSP430 x1xx has a custom spi_clock_t */
/**
 * @brief   Support SPI clock frequencies
 */
typedef enum {
    SPI_CLK_100KHZ = KHZ(100),      /**< 100 kHz */
    SPI_CLK_400KHZ = KHZ(400),      /**< 400 kHz */
    SPI_CLK_1MHZ   = MHZ(1),        /**< 1 MHz */
    SPI_CLK_5MHZ   = MHZ(5),        /**< 5 MHz */
    SPI_CLK_10MHZ  = SPI_CLK_5MHZ,  /**< 10 MHz not supported, falling back to 5 MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   MSP430 x1xx USART configuration
 *
 * @details This is intended to be stored in flash.
 */
typedef struct {
    msp430_usart_t *dev;        /**< The USART device to use */
    msp430_usart_sfr_t *sfr;    /**< The corresponding SFR registers */
    uint8_t tx_irq_mask;        /**< The bitmask to enable the TX IRQ for this
                                     USART */
    uint8_t rx_irq_mask;        /**< The bitmask to enable the TX IRQ for this
                                     USART */
    uint8_t num;                /**< Number of the USART */
} msp430_usart_params_t;

/**
 * @brief   MSP430 x1xx USART clock source
 *
 * @details The UC
 */
typedef enum {
    USART_CLK_UCLKI     = UXTCTL_SSEL_UCLKI,    /**< UCLKI clock source (not supported yet) */
    USART_CLK_AUX       = UXTCTL_SSEL_ACLK,     /**< auxiliary clock source */
    USART_CLK_SUBMAIN   = UXTCTL_SSEL_SMCLK,    /**< sub-system master clock source */
} msp430_usart_clk_t;

/**
 * @brief   MSP430 x1xx USART prescaler configuration
 */
typedef struct {
    msp430_usart_clk_t clk_source; /**< Clock source to use */
    uint8_t br0;                /**< What to write in the BR0 register */
    uint8_t br1;                /**< What to write in the BR1 register */
    uint8_t mctl;               /**< USART modulation control register */
} msp430_usart_prescaler_t;

/**
 * @brief   MSP430 x1xx USART configuration registers
 *
 * @details Unlike @ref msp430_usart_params_t this contains configuration that
 *          may depends on runtime settings
 */
typedef struct {
    msp430_usart_prescaler_t prescaler; /**< Prescaler configuration */
    uint8_t ctl;                        /**< USART control register */
} msp430_usart_conf_t;

/**
 * @brief   MSP430 x1xx UART configuration, CPU level
 *
 * The MSP430 x1xx has two USART peripherals which both can be operated in
 * UART mode. Each is connected to a fixed GPIO for RXD and TXD, respectively.
 * Hence, there is not much left for the board to configure anyway, so we just
 * prepare UART configurations at CPU level for the board to refer to. The
 * unused configuration(s) will be garbage collected by the linker.
 */
typedef struct {
    msp430_usart_params_t usart_params; /**< The USART params */
    /**
     * @brief   The bitmask to write to the SFR register to enable the USART
     *          device in UART mode with TX enabled
     *
     * @note    Sadly, the layout of the SFR registers corresponding to the
     *          USART peripherals differs.
     */
    uint8_t tx_enable_mask;
    /**
     * @brief   The bitmask to write to the SFR register to enable the USART
     *          device in UART mode with RX+TX enabled
     *
     * @details The RX and TX part of the UART can be enabled individually.
     *          But since there is no way to enable only RX with RIOT's UART
     *          API and this is an internal structure anyway, we optimize this
     *          to avoid an OR function to compile `tx_enable_mask` and
     *          `rx_enable_mask`.
     */
    uint8_t rxtx_enable_mask;
    gpio_t rxd;                     /**< RXD pin */
    gpio_t txd;                     /**< TXD pin */
} msp430_usart_uart_params_t;

/**
 * @brief   MSP430 x1xx UART configuration, board level
 */
typedef struct {
    const msp430_usart_uart_params_t *uart; /**< The UART configuration to use */
} uart_conf_t;

/**
 * @brief   MSP430 x1xx SPI configuration, CPU level
 *
 * The MSP430 x1xx has two USART peripherals which both can be operated in
 * SPI mode. Each is connected to a fixed GPIO for COPI (MOSI), CIPO (MISO),
 * and SCK, respectively. Hence, there is not much left for the board to
 * configure anyway, so we just prepare UART configurations at CPU level for
 * the board to refer to. The unused configuration(s) will be garbage collected
 * by the linker.
 */
typedef struct {
    msp430_usart_params_t usart_params;    /**< The USART parameters */
    /**
     * @brief   The bitmask to write to the SFR register to enable the USART
     *          device in SPI mode
     *
     * @note    Sadly, the layout of the SFR registers corresponding to the
     *          USART peripherals differs.
     */
    uint8_t enable_mask;
    gpio_t miso;                    /**< CIPO (MISO) pin */
    gpio_t mosi;                    /**< COPI (MOSI) pin */
    gpio_t sck;                     /**< SCK pin */
} msp430_usart_spi_params_t;

/**
 * @brief   MSP430 x1xx SPI configuration, board level
 */
typedef struct {
    const msp430_usart_spi_params_t *spi;    /**< The SPI configuration to use */
} spi_conf_t;

/**
 * @brief   MSP430 x1xx USART0 in UART configuration
 */
extern const msp430_usart_uart_params_t usart0_as_uart;

/**
 * @brief   MSP430 x1xx USART1 in UART configuration
 */
extern const msp430_usart_uart_params_t usart1_as_uart;

/**
 * @brief   MSP430 x1xx USART0 in SPI configuration
 */
extern const msp430_usart_spi_params_t usart0_as_spi;

/**
 * @brief   MSP430 x1xx USART1 in SPI configuration
 */
extern const msp430_usart_spi_params_t usart1_as_spi;

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
 * @brief       Enable the TX interrupt on the given USART
 * @param[in]   usart_conf  USART config of the USART to enable the TX IRQ on
 *
 * @warning     This will not work while the peripheral is still under reset,
 *              as the IRQ configuration is constantly reset while the software
 *              reset bit is set.
 */
static inline void msp430_usart_enable_tx_irq(const msp430_usart_params_t *usart_conf)
{
    usart_conf->sfr->IE |= usart_conf->tx_irq_mask;
}

/**
 * @brief       Enable the RX interrupt on the given USART
 * @param[in]   usart_conf  USART config of the USART to enable the RX IRQ on
 *
 * @warning     This will not work while the peripheral is still under reset,
 *              as the IRQ configuration is constantly reset while the software
 *              reset bit is set.
 */
static inline void msp430_usart_enable_rx_irq(const msp430_usart_params_t *usart_conf)
{
    usart_conf->sfr->IE |= usart_conf->rx_irq_mask;
}

/**
 * @brief       Disable the TX interrupt on the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the TX IRQ on
 *
 * @warning     This will not work while the peripheral is still under reset,
 *              as the IRQ configuration is constantly reset while the software
 *              reset bit is set.
 */
static inline void msp430_usart_disable_tx_irq(const msp430_usart_params_t *usart_conf)
{
    usart_conf->sfr->IE &= ~(usart_conf->tx_irq_mask);
}

/**
 * @brief       Disable the RX interrupt on the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the RX IRQ on
 *
 * @warning     This will not work while the peripheral is still under reset,
 *              as the IRQ configuration is constantly reset while the software
 *              reset bit is set.
 */
static inline void msp430_usart_disable_rx_irq(const msp430_usart_params_t *usart_conf)
{
    usart_conf->sfr->IE &= ~(usart_conf->rx_irq_mask);
}

/**
 * @brief       Get the TX IRQ flag of the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the RX IRQ on
 * @retval      true        The interrupt flag is set
 * @retval      false       The interrupt flag is **NOT** set
 */
static inline bool msp430_usart_get_tx_irq_flag(const msp430_usart_params_t *usart_conf)
{
    return usart_conf->sfr->IFG & usart_conf->tx_irq_mask;
}

/**
 * @brief       Get the RX IRQ flag of the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the RX IRQ on
 * @retval      true        The interrupt flag is set
 * @retval      false       The interrupt flag is **NOT** set
 */
static inline bool msp430_usart_get_rx_irq_flag(const msp430_usart_params_t *usart_conf)
{
    return usart_conf->sfr->IFG & usart_conf->rx_irq_mask;
}

/**
 * @brief       Check if both TX *and* RX IRQ flags are set on the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the RX IRQ on
 * @retval      true        The interrupt flag *both* TX and RX is set
 * @retval      false       Either TX IRQ flag *not* set, or RX *not* set, or *neither* set
 */
static inline bool msp430_usart_are_both_irq_flags_set(const msp430_usart_params_t *usart_conf)
{
    const uint8_t mask = usart_conf->tx_irq_mask | usart_conf->rx_irq_mask;
    return (usart_conf->sfr->IFG & mask) == mask;
}

/**
 * @brief       Clear the TX IRQ flag of the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the RX IRQ on
 */
static inline void msp430_usart_clear_tx_irq_flag(const msp430_usart_params_t *usart_conf)
{
    usart_conf->sfr->IFG &= ~(usart_conf->tx_irq_mask);
}

/**
 * @brief       Clear the RX IRQ flag of the given USART
 * @param[in]   usart_conf  USART config of the USART to disable the RX IRQ on
 */
static inline void msp430_usart_clear_rx_irq_flag(const msp430_usart_params_t *usart_conf)
{
    usart_conf->sfr->IFG &= ~(usart_conf->rx_irq_mask);
}

/**
 * @brief       Get exclusive access to an USART peripheral and initialize it
 *              for operation as configured
 * @param[in]   params          Static USART configuration
 * @param[in]   conf            Dynamic USART configuration
 * @param[in]   enable_mask Bitmask to write to the SFR register to enable the
 *              peripheral in the intended mode.
 *
 * @warning     The freshly acquired USART will still be placed in software
 *              reset when this function returns. The caller is expected to
 *              finish configuration such as configuring GPIOs connected to
 *              the USART suitably, clearing error flags, setting up IRQ
 *              handlers etc. and finally releasing the USART from reset.
 */
void msp430_usart_acquire(const msp430_usart_params_t *params,
                       const msp430_usart_conf_t *conf,
                       uint8_t enable_mask);

/**
 * @brief       Release exclusive access to an USART peripheral and turn it
 *              off again
 * @param[in]   params  Configuration specifying the USART to release
 */
void msp430_usart_release(const msp430_usart_params_t *params);

/**
 * @brief       Compute a suitable prescaler
 *
 * @param[in]   clock       SPI clock in Hz or UART symbol rate in Baud
 * @param[in]   min_br      Smallest BR0/BR1 value supported by hardware
 *                          (@ref USART_MIN_BR_SPI or @ref USART_MIN_BR_UART)
 * @return      Suitable prescaler config
 * @note        If the board has an auxiliary clock generated from a
 *              32,678 Hz watch crystal with no clock divider, the auxiliary
 *              clock is used for symbol rates of 9,600 Bd, 4,800 Bd, 2,400 Bd,
 *              and 1,200 Bd. For every other symbol rate the subsystem main
 *              clock is used instead.
 * @warning     This will compute the prescaler generating the frequency
 *              *closest* to @p clock. It may generate a frequency higher than
 *              requested, if this is closer to the target frequency than the
 *              next lower frequency. This makes bit-timings better in UART
 * @warning     The mctl `struct` field in the result may be non-zero. Callers
 *              must clear this, if the intend to use the USART in modes
 *              other than UART.
 */
msp430_usart_prescaler_t msp430_usart_prescale(uint32_t clock, uint16_t min_br);
#ifdef __cplusplus
}
#endif

/** @} */
