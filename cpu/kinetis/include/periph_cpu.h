/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 * Copyright (C) 2017-2018 Eistec AB
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
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PORT_PCR_MUX
#  define KINETIS_HAVE_PCR
#endif

#ifdef SIM_PINSEL_REG
#  define KINETIS_HAVE_PINSEL
#endif

#ifdef ADC_CFG1_MODE_MASK
#  define KINETIS_HAVE_ADC_K
#endif

#ifdef SPI_CTAR_CPHA_MASK
#  define KINETIS_HAVE_MK_SPI
#endif

#ifdef LPTMR_CSR_TEN_MASK
#  define KINETIS_HAVE_LPTMR
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

#ifdef SIM_UIDH_UID_MASK
/* Kinetis Cortex-M4 has a 128 bit SIM UID */
/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (&SIM->UIDH)

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)
#else /* defined(SIM_UIDH_UID_MASK) */
/* Kinetis Cortex-M0+ has a 96 bit SIM UID */
/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (&SIM->UIDMH)
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (12U)
#endif /* defined(SIM_UIDH_UID_MASK) */

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

#ifdef RTC
/* All Kinetis CPUs have exactly one RTC hardware module, except for the KL02
 * family which don't have an RTC at all */
/**
 * @name RTT and RTC configuration
 * @{
 */
#define RTT_FREQUENCY                (1)
#define RTT_MAX_VALUE                (0xffffffff)
/** @} */
#endif

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

#ifdef KINETIS_HAVE_PCR
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
#ifdef PORT_PCR_ODE_MASK
    GPIO_PCR_OD    = (PORT_PCR_ODE_MASK),   /**< open-drain mode */
#endif
    GPIO_PCR_PD    = (PORT_PCR_PE_MASK),    /**< enable pull-down */
    GPIO_PCR_PU    = (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK)  /**< enable PU */
} gpio_pcr_t;
#endif /* KINETIS_HAVE_PCR */

#ifndef DOXYGEN
/**
 * @name    GPIO flank configuration values
 * @{
 */
#ifdef KINETIS_HAVE_PCR
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING  = PORT_PCR_IRQC(0x9),  /**< emit interrupt on rising flank */
    GPIO_FALLING = PORT_PCR_IRQC(0xa),  /**< emit interrupt on falling flank */
    GPIO_BOTH    = PORT_PCR_IRQC(0xb),  /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* KINETIS_HAVE_PCR */
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
#ifdef KINETIS_HAVE_ADC_K
typedef enum {
    ADC_RES_6BIT  = (0xfe),             /**< not supported */
    ADC_RES_8BIT  = ADC_CFG1_MODE(0),   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CFG1_MODE(2),   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CFG1_MODE(1),   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0xff),             /**< ADC resolution: 14 bit */
    ADC_RES_16BIT = ADC_CFG1_MODE(3)    /**< ADC resolution: 16 bit */
} adc_res_t;
#endif /* KINETIS_HAVE_ADC_K */
/** @} */

#if defined(FTM_CnSC_MSB_MASK)
/**
 * @brief   Define the maximum number of PWM channels that can be configured
 */
#define PWM_CHAN_MAX        (4U)

/**
 * @name   PWM mode configuration
 * @{
 */
#define HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT   = (FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK),  /**< left aligned */
    PWM_RIGHT  = (FTM_CnSC_MSB_MASK | FTM_CnSC_ELSA_MASK),  /**< right aligned */
    PWM_CENTER = (FTM_CnSC_MSB_MASK)                        /**< center aligned */
} pwm_mode_t;
#endif /* defined(FTM_CnSC_MSB_MASK) */
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

#ifdef KINETIS_HAVE_MK_SPI
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                         /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = (SPI_CTAR_CPHA_MASK),                      /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (SPI_CTAR_CPOL_MASK),                      /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK)  /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */
#endif /* KINETIS_HAVE_MK_SPI */
#endif /* ndef DOXYGEN */

/**
 * @brief   CPU specific ADC configuration
 */
typedef struct {
    /**
     * @brief   ADC module
     */
    ADC_Type *dev;
    /**
     * @brief   pin to use
     *
     * Use GPIO_UNDEF non-muxed ADC pins, e.g. ADC0_DP, or for internal channels, e.g. Bandgap
     */
    gpio_t pin;
    /**
     * @brief   ADC channel
     *
     * Written as-is to ADCx_SC1x before conversion. This also decides
     * single-ended or differential sampling, see CPU reference manual for ADCx_SC1x
     */
    uint8_t chan;
    /**
     * @brief   Hardware averaging configuration
     *
     * Written as-is to ADCx_SC3 before conversion, use @ref ADC_AVG_NONE and
     * @ref ADC_AVG_MAX as a shorthand notation in the board configuration */
    uint8_t avg;
} adc_conf_t;

/**
 * @brief   Disable hardware averaging
 */
#define ADC_AVG_NONE    (0)
/**
 * @brief   Maximum hardware averaging (32 samples)
 */
#define ADC_AVG_MAX     (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3))

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

#ifdef KINETIS_HAVE_LPTMR
/**
 * @brief   CPU specific timer LPTMR module configuration
 */
typedef struct {
    /** LPTMR device base pointer */
    LPTMR_Type *dev;
    /** Input clock frequency */
    uint32_t base_freq;
    /** Clock source setting */
    uint8_t src;
    /** IRQn interrupt number */
    uint8_t irqn;
} lptmr_conf_t;
#endif /* KINETIS_HAVE_LPTMR */

#ifdef FTM_CnSC_MSB_MASK
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
#ifdef KINETIS_HAVE_PINSEL
    volatile uint32_t *pinsel;
    uint32_t pinsel_mask;
    uint32_t pinsel_val;
#endif
} pwm_conf_t;
#endif

#ifndef DOXYGEN
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       =   10000ul, /**< low speed mode:     ~10 kbit/s */
    I2C_SPEED_NORMAL    =  100000ul, /**< normal mode:       ~100 kbit/s */
    I2C_SPEED_FAST      =  400000ul, /**< fast mode:         ~400 kbit/s */
    I2C_SPEED_FAST_PLUS = 1000000ul, /**< fast plus mode:   ~1000 kbit/s */
    /* High speed is not supported without external hardware hacks */
    I2C_SPEED_HIGH      = 3400000ul, /**< high speed mode:  ~3400 kbit/s */
} i2c_speed_t;
/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */
#endif /* !defined(DOXYGEN) */

/**
 * @brief   I2C configuration structure
 */
typedef struct {
    I2C_Type *i2c;          /**< Pointer to hardware module registers */
    gpio_t scl_pin;         /**< SCL GPIO pin */
    gpio_t sda_pin;         /**< SDA GPIO pin */
    uint32_t freq;          /**< I2C module clock frequency, usually CLOCK_BUSCLOCK or CLOCK_CORECLOCK */
    i2c_speed_t speed;      /**< Configured bus speed, actual speed may be lower but never higher */
    IRQn_Type irqn;         /**< IRQ number for this module */
    uint32_t scl_pcr;       /**< PORT module PCR setting for the SCL pin */
    uint32_t sda_pcr;       /**< PORT module PCR setting for the SDA pin */
} i2c_conf_t;

/**
 * @brief   SPI module configuration options
 */
typedef struct {
    SPI_Type *dev;                      /**< SPI device to use */
    gpio_t pin_miso;                    /**< MISO pin used */
    gpio_t pin_mosi;                    /**< MOSI pin used */
    gpio_t pin_clk;                     /**< CLK pin used */
    gpio_t pin_cs[SPI_HWCS_NUMOF];      /**< pins used for HW cs lines */
#ifdef KINETIS_HAVE_PCR
    gpio_pcr_t pcr;                     /**< alternate pin function values */
#endif /* KINETIS_HAVE_PCR */
#ifdef KINETIS_HAVE_PINSEL
    volatile uint32_t *pinsel;
    uint32_t pinsel_mask;
    uint32_t pinsel_val;
#endif
    uint32_t simmask;                   /**< bit in the SIM register */
} spi_conf_t;

/**
 * @brief   Possible timer module types
 */
enum {
    TIMER_PIT,              /**< PIT */
#ifdef KINETIS_HAVE_LPTMR
    TIMER_LPTMR,            /**< LPTMR */
#endif /* KINETIS_HAVE_LPTMR */
};

/**
 * @name    Hardware timer type-specific device macros
 * @{
 */
/** @brief  Timers using PIT backend */
#define TIMER_PIT_DEV(x)   (TIMER_DEV(0 + (x)))
#ifdef KINETIS_HAVE_LPTMR
/** @brief  Timers using LPTMR backend */
#define TIMER_LPTMR_DEV(x) (TIMER_DEV(PIT_NUMOF + (x)))
#endif /* KINETIS_HAVE_LPTMR */
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
#ifdef KINETIS_HAVE_PCR
    uint32_t pcr_rx;              /**< Pin configuration register bits for RX */
    uint32_t pcr_tx;              /**< Pin configuration register bits for TX */
#endif
#ifdef KINETIS_HAVE_PINSEL
    volatile uint32_t *pinsel;
    uint32_t pinsel_mask;
    uint32_t pinsel_val;
#endif
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

#ifdef MODULE_PERIPH_MCG
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
 * @brief   Clock generation configuration flags
 *
 * @see "Clock distribution -> High-Level device clocking diagram" in every
 * Kinetis CPU reference manual
 */
typedef enum {
    /**
     * @brief   Turn on OSC0 oscillator
     *
     * - If this flag is set, the OSC0 oscillator expects a crystal between
     * the pins XTAL0 and EXTAL0, and the OSCCLK internal signal will be
     * provided by OSC0.
     * - If not set, the EXTAL0 pin will be used directly as the OSCCLK signal.
     */
    KINETIS_CLOCK_OSC0_EN           = (1 <<  0),
    /**
     * @brief   Turn on RTC oscillator
     *
     * - If this flag is set, the RTC oscillator expects a crystal between
     * the pins XTAL32 and EXTAL32.
     * - If not set, the EXTAL32 pin can be used as an external clock signal on
     * certain CPU models.
     */
    KINETIS_CLOCK_RTCOSC_EN         = (1 <<  1),
    /**
     * @brief   Use the fast internal reference clock as MCGIRCLK signal
     *
     * This flag corresponds to the IRCS bit in the MCG_C2 register.
     *
     * @note This flag affects the clock frequency of the CPU when using the MCG
     * in FBI, or BLPI clocking modes.
     *
     * - If this flag is set, the fast internal reference clock (up to 4 MHz,
     * depends on settings) will be routed to the MCGIRCLK internal clock signal.
     * - If not set, the slow internal reference clock (32 kHz) will be routed to
     * the MCGIRCLK internal clock signal. FBI and BLPI modes will clock the core
     * at 32 kHz.
     */
    KINETIS_CLOCK_USE_FAST_IRC      = (1 <<  2),
    /**
     * @brief   Enable MCGIRCLK internal clock signal
     *
     * This flag corresponds to the IRCLKEN bit in the MCG_C1 register.
     *
     * - If this flag is set, the MCG will provide MCGIRCLK for use by other
     * peripherals.
     */
    KINETIS_CLOCK_MCGIRCLK_EN       = (1 <<  3),
    /**
     * @brief   Enable MCGIRCLK signal during STOP modes
     *
     * This flag corresponds to the IREFSTEN bit in the MCG_SC register.
     *
     * - If this flag is set, MCGIRCLK internal clock signal will be available
     *   for clocking peripherals during CPU STOP modes.
     * - If not set, the MCGIRCLK internal clock signal will be stopped during
     *   CPU STOP modes.
     */
    KINETIS_CLOCK_MCGIRCLK_STOP_EN  = (1 <<  4),
} kinetis_clock_flags_t;

/**
 * @brief Clock configuration for Kinetis CPUs
 */
typedef struct {
    /**
     * @brief   Clock divider bitfield setting
     *
     * The value will be written to the SIM_CLKDIV1 hardware register without
     * any transformation. Use the SIM_CLKDIV1_OUTDIVx() macros to ensure the
     * proper bit shift for the chosen divider settings.
     *
     * @see CPU reference manual, SIM_CLKDIV1
     */
    uint32_t clkdiv1;
    /**
     * @brief   RTC oscillator Capacitor Load Configuration bits
     *
     * The bits will be passed directly to the RTC_CR register without any
     * transformation, i.e. the SC16P bit is (unintuitively) at bit position 10,
     * SC8P is at position 11, and so on (see details in the reference manual).
     * Use the RTC_CR_SCxP_MASK macros to avoid accidentally reversing the bits
     * here.
     *
     * @see CPU reference manual, RTC_CR[SCxP]
     */
    uint32_t rtc_clc;
    /**
     * @brief   ERCLK32K 32 kHz reference selection
     *
     * The bits will be passed directly to the SIM_SOPT1 register without any
     * transformation, use the SIM_SOPT1_OSC32KSEL() macro to ensure the proper
     * bit shift for the chosen setting.
     *
     * This signal is the input clock to the RTC module on some CPUs and an input
     * option for the LPTMRx modules. On other CPUs the RTC is clocked directly
     * by the RTC oscillator output without passing through this clock multiplexer.
     *
     * @see CPU reference manual, SIM_SOPT1[OSC32KSEL]
     */
    uint32_t osc32ksel;
    /**
     * @brief   Flags which will enable various clocking options at init
     *
     * @see @ref kinetis_clock_flags_t
     */
    unsigned int clock_flags;
    /**
     * @brief   MCG mode used after initialization
     *
     * @see @ref kinetis_mcg_mode_t
     */
    kinetis_mcg_mode_t default_mode;
    /**
     * @brief   ERC range setting
     *
     * @see @ref kinetis_mcg_erc_range_t
     */
    kinetis_mcg_erc_range_t erc_range;
    /**
     * @brief   OSC0 Capacitor Load Configuration bits
     *
     * The bits will be passed directly to the OSC_CR register without any
     * transformation, i.e. the SC16P bit is (unintuitively) the LSB, SC8P is
     * the next bit, and so on (see details in the reference manual). Use the
     * OSC_CR_SCxP_MASK macros to avoid accidentally reversing the bits here.
     *
     * @see CPU reference manual, OSC_CR[SCxP]
     */
    uint8_t osc_clc;
    /**
     * @brief   MCG external reference oscillator selection
     *
     * The bits will be passed directly to the MCG_C7 register without any
     * transformation, use the MCG_C7_OSCSEL() macro to ensure the proper bit
     * shift for the chosen setting.
     *
     * @see CPU reference manual, MCG_C7[OSCSEL]
     */
    uint8_t oscsel;
    /**
     * @brief   Fast internal reference clock divider
     *
     * The bits will be passed directly to the MCG_SC register without any
     * transformation, use the MCG_SC_FCRDIV() macro to ensure the proper bit
     * shift for the chosen setting.
     *
     * @see CPU reference manual, MCG_SC[FCRDIV]
     */
    uint8_t fcrdiv;
    /**
     * @brief   FLL ERC divider setting
     *
     * The bits will be passed directly to the MCG_C1 register without any
     * transformation, use the MCG_C1_FRDIV() macro to ensure the proper bit
     * shift for the chosen setting.
     *
     * @see CPU reference manual, MCG_C1[FRDIV]
     */
    uint8_t fll_frdiv;
    /**
     * @brief   FLL multiplier when running in FEI mode
     *
     * @see @ref kinetis_mcg_fll_t
     * @see CPU reference manual, MCG_C4[DMX32, DRST_DRS]
     */
    kinetis_mcg_fll_t fll_factor_fei;
    /**
     * @brief   FLL multiplier when running in FEE mode
     *
     * @see @ref kinetis_mcg_fll_t
     * @see CPU reference manual, MCG_C4[DMX32, DRST_DRS]
     */
    kinetis_mcg_fll_t fll_factor_fee;
#if KINETIS_HAVE_PLL
    /**
     * @brief   PLL ERC divider setting
     *
     * The bits will be passed directly to the MCG_C5 register without any
     * transformation, use the MCG_C5_PRDIV0() macro to ensure the proper bit
     * shift for the chosen setting.
     *
     * @see CPU reference manual, MCG_C5[PRDIV0]
     */
    uint8_t pll_prdiv;
    /**
     * @brief   PLL VCO divider setting
     *
     * The bits will be passed directly to the MCG_C6 register without any
     * transformation, use the MCG_C6_VDIV0() macro to ensure the proper bit
     * shift for the chosen setting.
     *
     * @see CPU reference manual, MCG_C6[VDIV0]
     */
    uint8_t pll_vdiv;
#endif /* KINETIS_HAVE_PLL */
} clock_config_t;
#endif /* MODULE_PERIPH_MCG */
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
