/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief           nRF5x common definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Hugues Larrive <hugues.larrive@pm.me>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Compatibility wrapper for nRF9160
 */
#ifdef NRF_FICR_S
#define NRF_FICR NRF_FICR_S
#endif
/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_OFF
/** @} */

/**
 * @brief   Starting offset of CPU_ID
 */
#ifdef FICR_INFO_DEVICEID_DEVICEID_Msk
#define CPUID_ADDR          (&NRF_FICR->INFO.DEVICEID[0])
#else
#define CPUID_ADDR          (&NRF_FICR->DEVICEID[0])
#endif
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @name    Override macro for defining GPIO pins
 *
 * The port definition is used (and zeroed) to suppress compiler warnings
 */
#if GPIO_COUNT > 1
#define GPIO_PIN(x,y)       ((x << 5) | y)
#else
#define GPIO_PIN(x,y)       ((x & 0) | y)
#endif

/**
 * @brief   Override GPIO_UNDEF value
 */
/* The precise value matters where GPIO_UNDEF is set in registers like
 * PWM.PSEL.OUT where it is used in sign-extended form to get a UINT32_MAX */
#define GPIO_UNDEF          (UINT8_MAX)

/**
 * @brief   Wrapper around GPIOTE ISR
 *
 * @note    nRF53 has two GPIOTE instances available on Application Core
 *          but we always use the first one.
 */
#ifdef NRF_GPIOTE0_S
#define ISR_GPIOTE isr_gpiote0
#else
#define ISR_GPIOTE isr_gpiote
#endif

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 4 bit to encode the pin mode:
 * - bit      0: output enable
 * - bit      1: input connect
 * - bit    2+3: pull resistor configuration
 * - bit 8+9+10: drive configuration
 */
#define GPIO_MODE(oe, ic, pr, dr)   (oe | (ic << 1) | (pr << 2) | (dr << 8))

#ifndef DOXYGEN /* BEGIN: GPIO LL overwrites */
#define HAVE_GPIO_SLEW_T
typedef enum {
    GPIO_SLEW_SLOWEST = 0,
    GPIO_SLEW_SLOW = 0,
    GPIO_SLEW_FAST = 0,
    GPIO_SLEW_FASTEST = 0,
} gpio_slew_t;

#define HAVE_GPIO_PULL_STRENGTH_T
typedef enum {
    GPIO_PULL_WEAKEST = 0,
    GPIO_PULL_WEAK = 0,
    GPIO_PULL_STRONG = 0,
    GPIO_PULL_STRONGEST = 0
} gpio_pull_strength_t;

#define HAVE_GPIO_DRIVE_STRENGTH_T
typedef enum {
    GPIO_DRIVE_WEAKEST = 0,
    GPIO_DRIVE_WEAK = 0,
    GPIO_DRIVE_STRONG = 1,
    GPIO_DRIVE_STRONGEST = 1
} gpio_drive_strength_t;

#define HAVE_GPIO_IRQ_TRIG_T
typedef enum {
    GPIO_TRIGGER_EDGE_RISING    = GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos,
    GPIO_TRIGGER_EDGE_FALLING   = GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos,
    GPIO_TRIGGER_EDGE_BOTH      = GPIO_TRIGGER_EDGE_RISING | GPIO_TRIGGER_EDGE_FALLING,
    GPIO_TRIGGER_LEVEL_HIGH     = 0, /**< unsupported */
    GPIO_TRIGGER_LEVEL_LOW      = 0, /**< unsupported */
} gpio_irq_trig_t;

#define HAVE_GPIO_PULL_T
typedef enum {
    GPIO_FLOATING = 0,
    GPIO_PULL_UP = GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos,
    GPIO_PULL_DOWN = GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos,
    /* GPIO_PULL_KEEP is not supported by, gpio_ll_init() returns -ENOTSUP */
    GPIO_PULL_KEEP = 0xff
} gpio_pull_t;
#endif /* END: GPIO LL overwrites */

/**
 * @brief   No support for HW chip select...
 */
#define SPI_HWCS(x)         (SPI_CS_UNDEF)

/**
 * @brief   Declare needed shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint8_t gpio_t;
/** @} */

/**
 * @brief   Override GPIO modes
 *
 * We use 4 bit to encode the pin mode:
 * - bit   0: output enable
 * - bit   1: input connect
 * - bit 2+3: pull resistor configuration
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN       = GPIO_MODE(0, 0, 0, 0), /**< IN */
    GPIO_IN_PD    = GPIO_MODE(0, 0, 1, 0), /**< IN with pull-down */
    GPIO_IN_PU    = GPIO_MODE(0, 0, 3, 0), /**< IN with pull-up */
    GPIO_IN_OD_PU = GPIO_MODE(0, 0, 3, 6), /**< IN with pull-up and open drain output */
    GPIO_OUT      = GPIO_MODE(1, 1, 0, 0), /**< OUT (push-pull) */
    GPIO_OD       = (0xff),                /**< not supported by HW */
    GPIO_OD_PU    = (0xfe)                 /**< not supported by HW */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override GPIO active flank values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING  = 1,       /**< emit interrupt on rising flank */
    GPIO_BOTH    = 3        /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Timer configuration options
 */
typedef struct {
    NRF_TIMER_Type *dev;    /**< timer device */
    /**
     * @brief   number of hardware channels ***minus one***
     *
     * The last hardware channels is implicitly used by timer_read() and not
     * available to the user. This value, hence, is the number of channels
     * available to the user.
     */
    uint8_t channels;
    uint8_t bitmode;        /**< counter width */
    uint8_t irqn;           /**< IRQ number of the timer device */
} timer_conf_t;

/**
 * @brief   The nRF5x periph_timer implements timer_set()
 */
#define PERIPH_TIMER_PROVIDES_SET   1

#ifndef DOXYGEN
/**
 * @brief   Override SPI mode values
 * @{
 */
#if !defined(CPU_FAM_NRF9160) && !defined(CPU_FAM_NRF53)
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                             /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = SPI_CONFIG_CPHA_Msk,                           /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = SPI_CONFIG_CPOL_Msk,                           /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CONFIG_CPOL_Msk | SPI_CONFIG_CPHA_Msk)    /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */
#endif /* ndef CPU_FAM_NRF9160 */

/**
 * @brief   Override SPI clock configuration
 * @{
 */
#define HAVE_SPI_CLK_T
typedef struct {
    uint32_t frequency;
    int err;
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT          (1)
/* Set upper limit to the maximum possible value that could go in CRV register */
#define NWDT_TIME_UPPER_LIMIT          ((UINT32_MAX >> 15) * US_PER_MS + 1)
/** @} */

/**
 * @brief Quadrature decoder configuration struct
 */
typedef struct {
    gpio_t a_pin;          /**< GPIO Pin for phase A */
    gpio_t b_pin;          /**< GPIO Pin for phase B */
    gpio_t led_pin;        /**< LED GPIO, GPIO_UNDEF to disable */
    uint8_t sample_period; /**< Sample period used, e.g. QDEC_SAMPLEPER_SAMPLEPER_128us */
    bool debounce_filter;  /**< Enable/disable debounce filter */
} qdec_conf_t;

/**
 * @brief Retrieve the exti(GPIOTE) channel associated with a gpio
 *
 * @param   pin     GPIO pin to retrieve the channel for
 *
 * @return          the channel number
 * @return          0xff if no channel is found
 */
uint8_t gpio_int_get_exti(gpio_t pin);

#if !defined(CPU_MODEL_NRF52832XXAA) && !defined(CPU_FAM_NRF51)
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

#endif /* ndef CPU_MODEL_NRF52832XXAA && ndef CPU_FAM_NRF51 */

/**
 * @brief USBDEV buffers must be word aligned because of DMA restrictions
 */
#define USBDEV_CPU_DMA_ALIGNMENT       (4)

/**
 * @brief USBDEV buffer instantiation requirement
 */
#define USBDEV_CPU_DMA_REQUIREMENTS    __attribute__((aligned(USBDEV_CPU_DMA_ALIGNMENT)))

#if !defined(CPU_FAM_NRF51) && !defined(DOXYGEN)
/**
 * @brief    The PWM unit on the nRF52, nRF53 and nRF9160
 *          supports 4 channels per device
 */
#define PWM_CHANNELS        (4U)

/**
 * @brief    Generate PWM mode values
 *
 * To encode the PWM mode, we use two bit:
 * - bit  0: select up or up-and-down counting
 * - bit 15: select polarity
 */
#define PWM_MODE(ud, pol)   (ud | (pol << 15))

/**
 * @brief   Override the PWM mode definitions
 */
#define HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT       = PWM_MODE(0, 1),    /**< left aligned PWM */
    PWM_RIGHT      = PWM_MODE(0, 0),    /**< right aligned PWM */
    PWM_CENTER     = PWM_MODE(1, 1),    /**< not supported */
    PWM_CENTER_INV = PWM_MODE(1, 0)     /**< not supported */
} pwm_mode_t;

/**
 * @brief   PWM configuration options
 *
 * Each device supports up to 4 channels. If you want to use less than 4
 * channels, just set the unused pins to GPIO_UNDEF.
 *
 * @note    define unused pins only from right to left, so the defined channels
 *          always start with channel 0 to x and the undefined ones are from x+1
 *          to PWM_CHANNELS.
 *
 * @warning All the channels not in active use must be set to GPIO_UNDEF; just
 *          initializing fewer members of pin would insert a 0 value, which
 *          would be interpreted as the P0.00 pin that's then driven.
 */
#if defined(PWM_PRESENT)
typedef struct {
    NRF_PWM_Type *dev;                  /**< PWM device descriptor */
    gpio_t pin[PWM_CHANNELS];           /**< PWM out pins */
} pwm_conf_t;
#endif
#endif /* ndef CPU_FAM_NRF51 */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
