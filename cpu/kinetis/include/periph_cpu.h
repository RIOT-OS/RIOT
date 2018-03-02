/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    CPU specific gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      (((x + 1) << 12) | (x << 6) | y)

/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (&SIM->UIDH)

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use the following bits to encode the pin mode:
 * - bit 0: 0 for pull-down or 1 for pull-up
 * - bit 1: pull resistor enable (as configured in bit 0)
 * - bit 5: OD enable
 * - bit 7: output or input mode
 */
#define GPIO_MODE(pu, pe, od, out)   (pu | (pe << 1) | (od << 5) | (out << 7))

/**
 * @brief   Define the maximum number of PWM channels that can be configured
 */
#define PWM_CHAN_MAX        (4U)

/**
 * @brief   Define a CPU specific SPI hardware chip select line macro
 *
 * We simply map the 5 hardware channels to the numbers [0-4], this still allows
 * us to differentiate between GPIP_PINs and SPI_HWSC lines.
 */
#define SPI_HWCS(x)         (x)

/**
 * @brief   Kinetis CPUs have a maximum number of 5 hardware chip select lines
 */
#define SPI_HWCS_NUMOF      (5)

/**
 * @name    This CPU makes use of the following shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  1
#define PERIPH_SPI_NEEDS_TRANSFER_REG   1
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  1
/** @} */

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   number of usable power modes
 */
#define PM_NUM_MODES    (1U)

#ifndef DOXYGEN
/**
 * @name    GPIO pin modes
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0, 0),     /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 1, 0, 0),     /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(1, 1, 0, 0),     /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(0, 0, 0, 1),     /**< OUT (push-pull) */
    GPIO_OD    = GPIO_MODE(1, 0, 1, 1),     /**< OD */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1, 1),     /**< OD with pull-up */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   PORT control register bitmasks
 *
 * To combine values just aggregate them using a logical OR.
 */
typedef enum {
    GPIO_AF_ANALOG = PORT_PCR_MUX(0),       /**< use pin as analog input */
    GPIO_AF_GPIO   = PORT_PCR_MUX(1),       /**< use pin as GPIO */
    GPIO_AF_2      = PORT_PCR_MUX(2),       /**< use alternate function 2 */
    GPIO_AF_3      = PORT_PCR_MUX(3),       /**< use alternate function 3 */
    GPIO_AF_4      = PORT_PCR_MUX(4),       /**< use alternate function 4 */
    GPIO_AF_5      = PORT_PCR_MUX(5),       /**< use alternate function 5 */
    GPIO_AF_6      = PORT_PCR_MUX(6),       /**< use alternate function 6 */
    GPIO_AF_7      = PORT_PCR_MUX(7),       /**< use alternate function 7 */
    GPIO_PCR_OD    = (PORT_PCR_ODE_MASK),   /**< open-drain mode */
    GPIO_PCR_PD    = (PORT_PCR_PE_MASK),    /**< enable pull-down */
    GPIO_PCR_PU    = (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK)  /**< enable PU */
} gpio_pcr_t;

#ifndef DOXYGEN
/**
 * @name    GPIO flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING  = PORT_PCR_IRQC(0x9),  /**< emit interrupt on rising flank */
    GPIO_FALLING = PORT_PCR_IRQC(0xa),  /**< emit interrupt on falling flank */
    GPIO_BOTH    = PORT_PCR_IRQC(0xb),  /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Available ports on the Kinetis family
 *
 * Not all CPUs have the full number of ports, see your CPU data sheet for pinout.
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    GPIO_PORTS_NUMOF        /**< overall number of available ports */
};

#ifndef DOXYGEN
/**
 * @name   ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (0xfe),             /**< not supported */
    ADC_RES_8BIT  = ADC_CFG1_MODE(0),   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CFG1_MODE(2),   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CFG1_MODE(1),   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0xff),             /**< ADC resolution: 14 bit */
    ADC_RES_16BIT = ADC_CFG1_MODE(3)    /**< ADC resolution: 16 bit */
} adc_res_t;
/** @} */

/**
 * @name    PWM mode configuration bits
 * @{
 */
#define HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT   = (FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK),  /**< left aligned */
    PWM_RIGHT  = (FTM_CnSC_MSB_MASK | FTM_CnSC_ELSA_MASK),  /**< right aligned */
    PWM_CENTER = (FTM_CnSC_MSB_MASK)                        /**< center aligned */
} pwm_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   UART transmission modes
 */
typedef enum {
    /** @brief 8 data bits, no parity, 1 stop bit */
    UART_MODE_8N1 = 0,
    /** @brief 8 data bits, even parity, 1 stop bit */
#if defined(UART_C1_M_MASK) || DOXYGEN
    /* LPUART and UART mode bits coincide, so the same setting for UART works on
     * the LPUART as well */
    UART_MODE_8E1 = (UART_C1_M_MASK | UART_C1_PE_MASK),
#elif defined(LPUART_CTRL_M_MASK)
    /* For CPUs which only have the LPUART */
    UART_MODE_8E1 = (LPUART_CTRL_M_MASK | LPUART_CTRL_PE_MASK),
#endif
    /** @brief 8 data bits, odd parity, 1 stop bit */
#if defined(UART_C1_M_MASK) || DOXYGEN
    UART_MODE_8O1 = (UART_C1_M_MASK | UART_C1_PE_MASK | UART_C1_PT_MASK),
#elif defined(LPUART_CTRL_M_MASK)
    /* For CPUs which only have the LPUART */
    UART_MODE_8O1 = (LPUART_CTRL_M_MASK | LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK),
#endif
} uart_mode_t;

#ifndef DOXYGEN
/**
 * @name    SPI mode bitmasks
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                         /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = (SPI_CTAR_CPHA_MASK),                      /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (SPI_CTAR_CPOL_MASK),                      /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK)  /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   CPU specific ADC configuration
 */
typedef struct {
    ADC_Type *dev;          /**< ADC device */
    gpio_t pin;             /**< pin to use, set to GPIO_UNDEF for internal
                             *   channels */
    uint8_t chan;           /**< ADC channel */
} adc_conf_t;

#if defined(DAC0_BASE) && (DAC0_BASE != This_symbol_has_been_deprecated)
/**
 * @brief   CPU specific DAC configuration
 */
typedef struct {
    DAC_Type *dev;                  /**< DAC device base pointer */
    volatile uint32_t *scgc_addr;   /**< Clock enable register, in SIM module */
    uint8_t scgc_bit;               /**< Clock enable bit, within the register */
} dac_conf_t;
#endif

/**
 * @brief   CPU specific timer PIT module configuration
 */
typedef struct {
    /** Prescaler channel */
    uint8_t prescaler_ch;
    /** Counting channel, will be linked to the prescaler channel */
    uint8_t count_ch;
} pit_conf_t;

/**
 * @brief   CPU specific timer LPTMR module configuration
 */
typedef struct {
    /** LPTMR device base pointer */
    LPTMR_Type *dev;
    /** IRQn interrupt number */
    uint8_t irqn;
} lptmr_conf_t;

/**
 * @brief   PWM configuration structure
 */
typedef struct {
    FTM_Type* ftm;          /**< used FTM */
    struct {
        gpio_t pin;         /**< GPIO pin used, set to GPIO_UNDEF */
        uint8_t af;         /**< alternate function mapping */
        uint8_t ftm_chan;   /**< the actual FTM channel used */
    } chan[PWM_CHAN_MAX];   /**< logical channel configuration */
    uint8_t chan_numof;     /**< number of actually configured channels */
    uint8_t ftm_num;        /**< FTM number used */
} pwm_conf_t;

/**
 * @brief   SPI module configuration options
 */
typedef struct {
    SPI_Type *dev;                      /**< SPI device to use */
    gpio_t pin_miso;                    /**< MISO pin used */
    gpio_t pin_mosi;                    /**< MOSI pin used */
    gpio_t pin_clk;                     /**< CLK pin used */
    gpio_t pin_cs[SPI_HWCS_NUMOF];      /**< pins used for HW cs lines */
    gpio_pcr_t pcr;                     /**< alternate pin function values */
    uint32_t simmask;                   /**< bit in the SIM register */
} spi_conf_t;

/**
 * @brief   Possible timer module types
 */
enum {
    TIMER_PIT,              /**< PIT */
    TIMER_LPTMR,            /**< LPTMR */
};

/**
 * @name    Hardware timer type-specific device macros
 * @{
 */
/** @brief  Timers using PIT backend */
#define TIMER_PIT_DEV(x)   (TIMER_DEV(0 + (x)))
/** @brief  Timers using LPTMR backend */
#define TIMER_LPTMR_DEV(x) (TIMER_DEV(PIT_NUMOF + (x)))
/** @} */

/**
 * @brief UART hardware module types
 */
typedef enum {
    KINETIS_UART,   /**< Kinetis UART module type */
    KINETIS_LPUART, /**< Kinetis Low-power UART (LPUART) module type */
} uart_type_t;

/**
 * @brief UART module configuration options
 */
typedef struct {
    void *dev;                    /**< Pointer to module hardware registers */
    uint32_t freq;                /**< Module clock frequency, usually CLOCK_CORECLOCK or CLOCK_BUSCLOCK */
    gpio_t pin_rx;                /**< RX pin, GPIO_UNDEF disables RX */
    gpio_t pin_tx;                /**< TX pin */
    uint32_t pcr_rx;              /**< Pin configuration register bits for RX */
    uint32_t pcr_tx;              /**< Pin configuration register bits for TX */
    IRQn_Type irqn;               /**< IRQ number for this module */
    volatile uint32_t *scgc_addr; /**< Clock enable register, in SIM module */
    uint8_t scgc_bit;             /**< Clock enable bit, within the register */
    uart_mode_t mode;             /**< UART mode: data bits, parity, stop bits */
    uart_type_t type;             /**< Hardware module type (KINETIS_UART or KINETIS_LPUART)*/
} uart_conf_t;

#if !defined(KINETIS_HAVE_PLL)
#if defined(MCG_C6_PLLS_MASK) || DOXYGEN
/**
 * @brief Defined to 1 if the MCG in this Kinetis CPU has a PLL
 */
#define KINETIS_HAVE_PLL 1
#else
#define KINETIS_HAVE_PLL 0
#endif
#endif /* !defined(KINETIS_HAVE_PLL) */

/**
 * @brief Kinetis possible MCG modes
 */
typedef enum kinetis_mcg_mode {
    KINETIS_MCG_MODE_FEI  = 0, /**< FLL Engaged Internal Mode */
    KINETIS_MCG_MODE_FEE  = 1, /**< FLL Engaged External Mode */
    KINETIS_MCG_MODE_FBI  = 2, /**< FLL Bypassed Internal Mode */
    KINETIS_MCG_MODE_FBE  = 3, /**< FLL Bypassed External Mode */
    KINETIS_MCG_MODE_BLPI = 4, /**< FLL Bypassed Low Power Internal Mode */
    KINETIS_MCG_MODE_BLPE = 5, /**< FLL Bypassed Low Power External Mode */
#if KINETIS_HAVE_PLL
    KINETIS_MCG_MODE_PBE  = 6, /**< PLL Bypassed External Mode */
    KINETIS_MCG_MODE_PEE  = 7, /**< PLL Engaged External Mode */
#endif
    KINETIS_MCG_MODE_NUMOF,    /**< Number of possible modes */
} kinetis_mcg_mode_t;

/**
 * @brief Kinetis MCG FLL multiplier settings
 */
typedef enum {
    /** FLL multiplier = 640 */
    KINETIS_MCG_FLL_FACTOR_640  = (MCG_C4_DRST_DRS(0)),
    /** FLL multiplier = 732 */
    KINETIS_MCG_FLL_FACTOR_732  = (MCG_C4_DRST_DRS(0) | MCG_C4_DMX32_MASK),
    /** FLL multiplier = 1280 */
    KINETIS_MCG_FLL_FACTOR_1280 = (MCG_C4_DRST_DRS(1)),
    /** FLL multiplier = 1464 */
    KINETIS_MCG_FLL_FACTOR_1464 = (MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK),
    /** FLL multiplier = 1920 */
    KINETIS_MCG_FLL_FACTOR_1920 = (MCG_C4_DRST_DRS(2)),
    /** FLL multiplier = 2197 */
    KINETIS_MCG_FLL_FACTOR_2197 = (MCG_C4_DRST_DRS(2) | MCG_C4_DMX32_MASK),
    /** FLL multiplier = 2560 */
    KINETIS_MCG_FLL_FACTOR_2560 = (MCG_C4_DRST_DRS(3)),
    /** FLL multiplier = 2929 */
    KINETIS_MCG_FLL_FACTOR_2929 = (MCG_C4_DRST_DRS(3) | MCG_C4_DMX32_MASK),
} kinetis_mcg_fll_t;

/**
 * @brief Kinetis FLL external reference clock range settings
 */
typedef enum {
    KINETIS_MCG_ERC_RANGE_LOW       = MCG_C2_RANGE0(0), /**< for 31.25-39.0625 kHz crystal */
    KINETIS_MCG_ERC_RANGE_HIGH      = MCG_C2_RANGE0(1), /**< for 3-8 MHz crystal */
    KINETIS_MCG_ERC_RANGE_VERY_HIGH = MCG_C2_RANGE0(2), /**< for 8-32 MHz crystal */
} kinetis_mcg_erc_range_t;

/**
 * @brief Clock configuration for Kinetis CPUs
 */
typedef struct {
    /** Clock divider bitfield setting, see reference manual for SIM_CLKDIV1 */
    uint32_t clkdiv1;
    /** MCG mode used after initialization, see kinetis_mcg_mode_t */
    kinetis_mcg_mode_t default_mode;
    /** ERC range setting, see kinetis_mcg_erc_range_t */
    kinetis_mcg_erc_range_t erc_range;
    /** Fast internal reference clock divider, see reference manual for MCG_SC[FCRDIV] */
    uint8_t fcrdiv;
    /** Oscillator selection, see reference manual for MCG_C7[OSCSEL] */
    uint8_t oscsel;
    /** Capacitor Load configuration bits, see reference manual for OSC_CR */
    uint8_t clc;
    /** FLL ERC divider setting, see reference manual for MCG_C1[FRDIV] */
    uint8_t fll_frdiv;
    /** FLL multiplier when running in FEI mode */
    kinetis_mcg_fll_t fll_factor_fei;
    /** FLL multiplier when running in FEE mode */
    kinetis_mcg_fll_t fll_factor_fee;
#if KINETIS_HAVE_PLL
    /** PLL ERC divider setting, see reference manual for MCG_C5[PRDIV] */
    uint8_t pll_prdiv;
    /** PLL VCO divider setting, see reference manual for MCG_C6[VDIV0] */
    uint8_t pll_vdiv;
#endif /* KINETIS_HAVE_PLL */
    /**
     * @brief External reference clock selection
     *
     * True: Use oscillator circuit with external crystal.
     * False: Use external clock signal directly.
     */
    bool enable_oscillator;
    /**
     * @brief Use fast internal reference clock for MCGIRCLK
     *
     * See reference manual for MCG module and MCG_C2[IRCS]
     */
    bool select_fast_irc;
    /**
     * @brief Enable MCGIRCLK output from MCG for use as alternate clock in some modules
     */
    bool enable_mcgirclk;
} clock_config_t;

/**
 * @brief   CPU internal function for initializing PORTs
 *
 * @param[in] pin       pin to initialize
 * @param[in] pcr       value for the PORT's PCR register
 */
void gpio_init_port(gpio_t pin, uint32_t pcr);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
