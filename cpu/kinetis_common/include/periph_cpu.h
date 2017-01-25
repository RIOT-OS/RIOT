/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_kinetis_common
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

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Overwrite the default gpio_t type definition
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
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use the following bits to encode the pin mode:
 * - bit 0: 0 for pull-down or 1 for pull-up
 * - bit 1: pull register enable (as configured in bit 0)
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
 * @brief   This CPU makes use of the following shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override GPIO modes
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
 * @brief   Define a condensed set of PORT PCR values
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
 * @brief   Override flank configuration values
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
 * @todo    This is not equal for all members of the Kinetis family, right?
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
 * @brief   Override default ADC resolution values
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
 * @brief   Override default PWM mode configuration
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

#ifndef DOXYGEN
/**
 * @brief   Override default ADC resolution values
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

/**
 * @brief   CPU specific DAC configuration
 */
typedef struct {
    /** DAC device base pointer */
    DAC_Type *dev;
    /** Pointer to module clock gate bit in bitband region, use BITBAND_REGADDR() */
    uint32_t volatile *clk_gate;
} dac_conf_t;

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
    /** Pointer to module clock gate bit in bitband region, use BITBAND_REGADDR() */
    uint32_t volatile *clk_gate;
    /** LPTMR device index */
    uint8_t index;
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
 * @brief   Hardware timer type-specific device macros
 * @{
 */
#define TIMER_PIT_DEV(x)   (TIMER_DEV(0 + (x)))
#define TIMER_LPTMR_DEV(x) (TIMER_DEV(PIT_NUMOF + (x)))
/** @} */

/**
 * @brief   CPU internal function for initializing PORTs
 *
 * @param[in] pin       pin to initialize
 * @param[in] pcr       value for the PORT's PCR register
 */
void gpio_init_port(gpio_t pin, uint32_t pcr);

/**
 * @brief   define number of usable power modes
 */
#define PM_NUM_MODES    (1U)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
