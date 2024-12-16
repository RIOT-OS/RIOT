/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam0_common
 * @brief           Common CPU specific definitions for all SAMx21 based CPUs
 * @{
 *
 * @file
 * @brief           Common CPU specific definitions for all SAMx21 based CPUs
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"
#include "exti_config.h"
#include "timer_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Use shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#ifndef MODULE_PERIPH_DMA
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
#endif
/** @} */

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

/**
 * @brief   Maximum bytes per frame for I2C operations
 */
#define PERIPH_I2C_MAX_BYTES_PER_FRAME 256

/**
 * @brief   Override GPIO type
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Macro for accessing GPIO pins
 * @{
 */
#ifdef MODULE_PERIPH_GPIO_FAST_READ
#ifdef PORT_IOBUS_SEC
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT_IOBUS_SEC->Group[x])) | y)
#else /* Use IOBUS access when available */
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT_IOBUS->Group[x])) | y)
#endif /* PORT_IOBUS_SEC */
#else
#ifdef PORT_SEC
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT_SEC->Group[x])) | y)
#else
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT->Group[x])) | y)
#endif /* PORT_IOBUS_SEC */
#endif

/**
 * @brief   Available ports on the SAMD21 & SAML21
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
    PD = 3,                 /**< port D */
};

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: PD(0) or PU(1)
 * - bit 1: input enable
 * - bit 2: pull enable
 */
#define GPIO_MODE(pr, ie, pe)   (pr | (ie << 1) | (pe << 2))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO modes
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 1, 1),    /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(1, 1, 1),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(0, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = 0xfe,                  /**< not supported by HW */
    GPIO_OD_PU = 0xff                   /**< not supported by HW */
} gpio_mode_t;

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

#define HAVE_GPIO_PULL_T
typedef enum {
    GPIO_FLOATING,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN,
    GPIO_PULL_KEEP,
} gpio_pull_t;

#define HAVE_GPIO_STATE_T
typedef enum {
    GPIO_OUTPUT_PUSH_PULL,
    GPIO_OUTPUT_OPEN_DRAIN,
    GPIO_OUTPUT_OPEN_SOURCE,
    GPIO_INPUT,
    GPIO_USED_BY_PERIPHERAL,
    GPIO_DISCONNECT,
} gpio_state_t;

#define HAVE_GPIO_IRQ_TRIG_T
typedef enum {
    GPIO_TRIGGER_EDGE_RISING    = EIC_CONFIG_SENSE0_RISE_Val,
    GPIO_TRIGGER_EDGE_FALLING   = EIC_CONFIG_SENSE0_FALL_Val,
    GPIO_TRIGGER_EDGE_BOTH      = EIC_CONFIG_SENSE0_BOTH_Val,
    GPIO_TRIGGER_LEVEL_HIGH     = EIC_CONFIG_SENSE0_HIGH_Val,
    GPIO_TRIGGER_LEVEL_LOW      = EIC_CONFIG_SENSE0_LOW_Val,
} gpio_irq_trig_t;

#define HAVE_GPIO_CONF_T
typedef union gpio_conf_sam0 gpio_conf_t;

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief       GPIO pin configuration for SAM0 MCUs
 * @ingroup     drivers_periph_gpio_ll
 */
union gpio_conf_sam0 {
    uint8_t bits;  /**< the raw bits */
    struct {
        /**
         * @brief   State of the pin
         */
        gpio_state_t state                      : 3;
        /**
         * @brief   Pull resistor configuration
         */
        gpio_pull_t pull                        : 2;
        /**
         * @brief   Drive strength of the GPIO
         *
         * @warning If the requested drive strength is not available, the
         *          closest fit supported will be configured instead.
         *
         * This value is ignored when @ref gpio_conf_sam0::state is configured
         * to @ref GPIO_INPUT or @ref GPIO_DISCONNECT.
         */
        gpio_drive_strength_t drive_strength    : 1;
        /**
         * @brief   Initial value of the output
         *
         * Ignored if @ref gpio_conf_sam0::state is set to @ref GPIO_INPUT or
         * @ref GPIO_DISCONNECT. If the pin was previously in a high impedance
         * state, it is guaranteed to directly transition to the given initial
         * value.
         *
         * @ref gpio_ll_query_conf will write the current value of the specified
         * pin here, which is read from the input register when the state is
         * @ref GPIO_INPUT, otherwise the state from the output register is
         * consulted.
         */
        bool initial_value                      : 1;
        uint8_t                                 : 1; /*< padding */
    };
};

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
#ifndef SAM_MUX_T
typedef enum {
    GPIO_MUX_A = 0x0,       /**< select peripheral function A */
    GPIO_MUX_B = 0x1,       /**< select peripheral function B */
    GPIO_MUX_C = 0x2,       /**< select peripheral function C */
    GPIO_MUX_D = 0x3,       /**< select peripheral function D */
    GPIO_MUX_E = 0x4,       /**< select peripheral function E */
    GPIO_MUX_F = 0x5,       /**< select peripheral function F */
    GPIO_MUX_G = 0x6,       /**< select peripheral function G */
    GPIO_MUX_H = 0x7,       /**< select peripheral function H */
    GPIO_MUX_I = 0x8,       /**< select peripheral function I */
    GPIO_MUX_J = 0x9,       /**< select peripheral function J */
    GPIO_MUX_K = 0xa,       /**< select peripheral function K */
    GPIO_MUX_L = 0xb,       /**< select peripheral function L */
    GPIO_MUX_M = 0xc,       /**< select peripheral function M */
    GPIO_MUX_N = 0xd,       /**< select peripheral function N */
    GPIO_MUX_DISABLED = 0xff, /**< Disable  */
} gpio_mux_t;
#endif

/**
 * @brief   Available values for SERCOM UART RX pad selection
 */
typedef enum {
    UART_PAD_RX_0 = 0x0,    /**< use pad 0 for RX line */
    UART_PAD_RX_1 = 0x1,    /**< select pad 1 */
    UART_PAD_RX_2 = 0x2,    /**< select pad 2 */
    UART_PAD_RX_3 = 0x3,    /**< select pad 3 */
} uart_rxpad_t;

/**
 * @brief   Available values for SERCOM UART TX pad selection
 */
typedef enum {
    UART_PAD_TX_0             = 0x0,    /**< select pad 0 */
    UART_PAD_TX_2             = 0x1,    /**< select pad 2 */
    UART_PAD_TX_0_RTS_2_CTS_3 = 0x2,    /**< TX is pad 0, on top RTS on pad 2
                                         *   and CTS on pad 3 */
} uart_txpad_t;

/**
 * @brief   Available SERCOM UART flag selections
 */
typedef enum {
    UART_FLAG_NONE            = 0x0,    /**< No flags set */
    UART_FLAG_RUN_STANDBY     = 0x1,    /**< run SERCOM in standby mode */
    UART_FLAG_WAKEUP          = 0x2,    /**< wake from sleep on receive */
    UART_FLAG_TX_ONDEMAND     = 0x4,    /**< Only enable TX pin on demand */
} uart_flag_t;

#ifndef DOXYGEN
/**
 * @brief   Available SERCOM UART data size selections
 *
 * 9 bit UART mode is currently unavailable as it is not supported by the common
 * RIOT UART peripheral API.
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = 0x5,   /**< 5 data bits */
    UART_DATA_BITS_6 = 0x6,   /**< 6 data bits */
    UART_DATA_BITS_7 = 0x7,   /**< 7 data bits */
    UART_DATA_BITS_8 = 0x0,   /**< 8 data bits */
} uart_data_bits_t;
/** @} */

/**
 * @brief   UART pin getters
 * @{
 */
#define uart_pin_rx(dev) uart_config[dev].rx_pin
#define uart_pin_tx(dev) uart_config[dev].tx_pin
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @name    Common SERCOM Interface
 * @{
 */
/**
 * @brief   Identifier of a SERCOM
 */
typedef uint8_t sercom_t;

/**
 * @brief   IRQ callback to call on IRQs triggered by the SERCOM
 *
 * @param[in]   arg         The argument provided on @ref sercom_acquire
 */
typedef void (*sercom_irq_cb_t)(void *arg);

/**
 * @brief   Acquire exclusive access to the given SERCOM, enable it, and
 *          configure it as specified
 * @param[in]   sercom      The SERCOM to acquire
 * @param[in]   gclk        Generator clock
 * @param[in]   irq_cb      Function to call on IRQ or `NULL`
 * @param[in]   irq_arg     Argument to pass to @p irq_cb
 *
 * @note    This function may block if the SERCOM is already in use to provide
 *          another serial interface until it is release.
 * @warning Beware: UART buses are typically kept acquired and never released.
 *
 * @pre     The pins the caller wants to route to the SERCOM are not routed to
 *          it prior to the call (as the SERCOM may still be active). Only
 *          after this function returns, pins may be routed to the SERCOM.
 * @post    The SERCOM is exclusively held by the caller
 * @post    The SERCOM is configured as asked for in @p conf
 * @post    If @p irq_cb was `NULL`, the corresponding IRQ(s) is/are disabled
 *          in the NIVC. Otherwise they are enabled but masked.
 */
void sercom_acquire(sercom_t sercom, uint8_t gclk,
                    sercom_irq_cb_t irq_cb, void *irq_arg);

/**
 * @brief   Enable the SERCOM APB clock
 * @param[in]   sercom  The SERCOM to enable the clock of
 */
static inline void sercom_apb_enable(sercom_t sercom);

/**
 * @brief   Disable the SERCOM APB clock
 * @param[in]   sercom  The SERCOM to disable the clock of
 */
static inline void sercom_apb_disable(sercom_t sercom);

/**
 * @brief   Enable GCLK clock signal to the given SEROM
 * @param[in]   sercom  The SERCOM to clock
 * @param[in] gclk      Generator clock
 */
static inline void sercom_gclk_enable(sercom_t sercom, uint8_t gclk);

/**
 * @brief   Release exclusive access to the given SERCOM and disable it
 *          (low power mode)
 * @param[in]   sercom  The SERCOM to release
 *
 * @pre     The pins that were routed to this SERCOM are no longer routed to it,
 *          so that other pins can be routed to it right after the SERCOM is
 *          release to provide other serial buses
 */
void sercom_release(sercom_t sercom);
/** @} */

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE    (64)
#endif

/**
 * @brief   UART device configuration
 *
 *          The frequency f() of the clock `gclk_src` must fulfill the condition
 *
 *              16 * baud < f(gclk_src) ≤ 2²⁰ * baud
 *
 *          in Asynchronous Arithmetic mode and
 *
 *              16 * baud < f(gclk_src) ≤ 2¹⁷ * baud
 *
 *          in Asynchronous Fractional mode
 */
typedef struct {
    SercomUsart *dev;       /**< pointer to the used UART device */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t rts_pin;          /**< pin used for RTS */
    gpio_t cts_pin;          /**< pin used for CTS */
#endif
    gpio_mux_t mux;         /**< alternative function for pins */
    uart_rxpad_t rx_pad;    /**< pad selection for RX line */
    uart_txpad_t tx_pad;    /**< pad selection for TX line */
    uart_flag_t flags;      /**< set optional SERCOM flags */
    uint8_t gclk_src;       /**< GCLK source which supplys SERCOM */
} uart_conf_t;

enum {
    TIMER_TYPE_TC,          /**< Timer is a TC timer  */
    TIMER_TYPE_TCC,         /**< Timer is a TCC timer */
};

/**
 * @brief   Common configuration for timer devices
 */
typedef struct {
    union {
#ifdef REV_TC
        Tc *tc;                 /**< TC device to use */
#endif
#ifdef REV_TCC
        Tcc *tcc;               /**< TCC device to use */
#endif
    } dev;                      /**< The Timer device used for PWM */
#ifdef MCLK
    volatile uint32_t *mclk;    /**< Pointer to MCLK->APBxMASK.reg */
    uint32_t mclk_mask;         /**< MCLK_APBxMASK bits to enable Timer */
#else
    uint32_t pm_mask;           /**< PM_APBCMASK bits to enable Timer */
#endif
    uint16_t gclk_id;           /**< TCn_GCLK_ID */
    uint8_t type;               /**< Timer type (TC/TCC) */
} tc_tcc_cfg_t;

/**
 * @brief   Static initializer for TC timer configuration
 */
#ifdef MCLK
#define TC_CONFIG(tim)                    { \
        .dev       = {.tc = tim},           \
        .mclk      = MCLK_ ## tim,          \
        .mclk_mask = MCLK_ ## tim ## _MASK, \
        .gclk_id   = tim ## _GCLK_ID,       \
        .type      = TIMER_TYPE_TC,       }
#else
#define TC_CONFIG(tim)                    { \
        .dev       = {.tc = tim},           \
        .pm_mask   = PM_APBCMASK_ ## tim,   \
        .gclk_id   = tim ## _GCLK_ID,       \
        .type      = TIMER_TYPE_TC,       }
#endif

/**
 * @brief   Static initializer for TCC timer configuration
 */
#ifdef MCLK
#define TCC_CONFIG(tim)                   { \
        .dev       = {.tcc = tim},          \
        .mclk      = MCLK_ ## tim,          \
        .mclk_mask = MCLK_ ## tim ## _MASK, \
        .gclk_id   = tim ## _GCLK_ID,       \
        .type      = TIMER_TYPE_TCC,      }
#else
#define TCC_CONFIG(tim)                   { \
        .dev       = {.tcc = tim},          \
        .pm_mask   = PM_APBCMASK_ ## tim,   \
        .gclk_id   = tim ## _GCLK_ID,       \
        .type      = TIMER_TYPE_TCC,      }
#endif

/**
 * @brief   PWM channel configuration data structure
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin */
    gpio_mux_t mux;         /**< pin function multiplex value */
    uint8_t chan;           /**< TCC channel to use */
} pwm_conf_chan_t;

/**
 * @brief   PWM device configuration data structure
 */
typedef struct {
    tc_tcc_cfg_t tim;               /**< timer configuration */
    const pwm_conf_chan_t *chan;    /**< channel configuration */
    uint8_t chan_numof;             /**< number of channels */
    uint8_t gclk_src;               /**< GCLK source which clocks TIMER */
} pwm_conf_t;

/**
 * @brief   Special SERCOM ID to identify the QSPI
 *
 * When module `periph_spi_on_qspi` is used, we need to identify in the SPI
 * config that the SPI is not implemented using a SERCOM but using a QSPI
 * peripheral. This magic invalid SERCOM id is to be used for that.
 */
#define SERCOM_ID_QSPI      0xff

/**
 * @brief   Available values for SERCOM SPI MISO pad selection
 */
typedef enum {
    SPI_PAD_MISO_0 = 0x0,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_1 = 0x1,       /**< use pad 1 for MISO line */
    SPI_PAD_MISO_2 = 0x2,       /**< use pad 2 for MISO line */
    SPI_PAD_MISO_3 = 0x3,       /**< use pad 3 for MISO line */
} spi_misopad_t;

/**
 * @brief   Available values for SERCOM SPI MOSI and SCK pad selection
 */
typedef enum {
    SPI_PAD_MOSI_0_SCK_1 = 0x0, /**< use pad 0 for MOSI, pad 1 for SCK */
    SPI_PAD_MOSI_2_SCK_3 = 0x1, /**< use pad 2 for MOSI, pad 3 for SCK */
    SPI_PAD_MOSI_3_SCK_1 = 0x2, /**< use pad 3 for MOSI, pad 1 for SCK */
    SPI_PAD_MOSI_0_SCK_3 = 0x3, /**< use pad 0 for MOSI, pad 3 for SCK */
} spi_mosipad_t;

#ifndef DOXYGEN
/**
 * @brief   Override SPI modes
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0x0,       /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0x1,       /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = 0x2,       /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = 0x3        /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ =   100000U, /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ =   400000U, /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   =  1000000U, /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   =  5000000U, /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000U  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   SPI pin getters
 * @{
 */
#define spi_pin_mosi(dev) spi_config[dev].mosi_pin
#define spi_pin_miso(dev) spi_config[dev].miso_pin
#define spi_pin_clk(dev)  spi_config[dev].clk_pin
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @brief   SPI device configuration
 */
typedef struct {
    void *dev;              /**< pointer to the used SPI device */
    gpio_t miso_pin;        /**< used MISO pin */
    gpio_t mosi_pin;        /**< used MOSI pin */
    gpio_t clk_pin;         /**< used CLK pin */
    gpio_mux_t miso_mux;    /**< alternate function for MISO pin (mux) */
    gpio_mux_t mosi_mux;    /**< alternate function for MOSI pin (mux) */
    gpio_mux_t clk_mux;     /**< alternate function for CLK pin (mux) */
    spi_misopad_t miso_pad; /**< pad to use for MISO line */
    spi_mosipad_t mosi_pad; /**< pad to use for MOSI and CLK line */
    uint8_t gclk_src;       /**< GCLK source which supplies SERCOM */
#ifdef MODULE_PERIPH_DMA
    uint8_t tx_trigger;     /**< DMA trigger */
    uint8_t rx_trigger;     /**< DMA trigger */
#endif
} spi_conf_t;
/** @} */

/**
 * @brief   Available SERCOM I2C flag selections
 */
typedef enum {
    I2C_FLAG_NONE            = 0x0,    /**< No flags set */
    I2C_FLAG_RUN_STANDBY     = 0x1,    /**< run SERCOM in standby mode */
} i2c_flag_t;

#ifndef DOXYGEN
/**
 * @name    Override I2C clock speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 10000U,      /**< low speed mode:    ~10kbit/s */
    I2C_SPEED_NORMAL    = 100000U,     /**< normal mode:       ~100kbit/s */
    I2C_SPEED_FAST      = 400000U,     /**< fast mode:         ~400kbit/s */
    I2C_SPEED_FAST_PLUS = 1000000U,    /**< fast plus mode:    ~1Mbit/s */
    I2C_SPEED_HIGH      = 3400000U,    /**< high speed mode:   ~3.4Mbit/s */
} i2c_speed_t;
/** @} */

/**
 * @name    I2C pin getter functions
 * @{
 */
#define i2c_pin_sda(dev) i2c_config[dev].sda_pin
#define i2c_pin_scl(dev) i2c_config[dev].scl_pin
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @brief   I2C device configuration
 *          The frequency f() of the clock `gclk_src` must fulfill the condition
 *
 *              4 * speed ≤ f(gclk_src) ≤ 512 * speed
 *
 *          if speed ≤ 1 MHz and
 *
 *             12 * speed ≤ f(gclk_src) ≤ 520 * speed
 *
 *          if speed > 1 MHz
 */
typedef struct {
    SercomI2cm *dev;        /**< pointer to the used I2C device */
    i2c_speed_t speed;      /**< baudrate used for the bus */
    gpio_t scl_pin;         /**< used SCL pin */
    gpio_t sda_pin;         /**< used MOSI pin */
    gpio_mux_t mux;         /**< alternate function (mux) */
    uint8_t gclk_src;       /**< GCLK source which suppliess SERCOM */
    uint8_t flags;          /**< allow SERCOM to run in standby mode */
} i2c_conf_t;

/**
 * @brief   Timer device configuration
 */
typedef struct {
    Tc *dev;                /**< pointer to the used Timer device */
    IRQn_Type irq;          /**< IRQ# of Timer Interrupt */
#ifdef MCLK
    volatile uint32_t *mclk;/**< Pointer to MCLK->APBxMASK.reg */
    uint32_t mclk_mask;     /**< MCLK_APBxMASK bits to enable Timer */
    uint16_t gclk_id;       /**< TCn_GCLK_ID */
#else
    uint32_t pm_mask;       /**< PM_APBCMASK bits to enable Timer */
    uint16_t gclk_ctrl;     /**< GCLK_CLKCTRL_ID for the Timer */
#endif
    uint8_t gclk_src;       /**< GCLK source which supplys Timer */
    uint16_t flags;         /**< flags for CTRA, e.g. TC_CTRLA_MODE_COUNT32 */
} tc32_conf_t;

/**
 * @brief   Number of available timer channels
 */
#define TIMER_CHANNEL_NUMOF (2)

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

/**
 * @brief   Called before the power management enters a power mode
 *
 * @param[in] deep
 */
void gpio_pm_cb_enter(int deep);

/**
 * @brief   Called after the power management left a power mode
 *
 * @param[in] deep
 */
void gpio_pm_cb_leave(int deep);

/**
 * @brief   Called before the power management enters a power mode
 *
 * @param[in] deep
 */
void cpu_pm_cb_enter(int deep);

/**
 * @brief   Called after the power management left a power mode
 *
 * @param[in] deep
 */
void cpu_pm_cb_leave(int deep);

/**
 * @brief   Wrapper for cortexm_sleep calling power management callbacks
 *
 * @param[in] deep
 */
static inline void sam0_cortexm_sleep(int deep)
{
#ifdef MODULE_PERIPH_GPIO
    gpio_pm_cb_enter(deep);
#endif

    cpu_pm_cb_enter(deep);

    cortexm_sleep(deep);

    cpu_pm_cb_leave(deep);

#ifdef MODULE_PERIPH_GPIO
    gpio_pm_cb_leave(deep);
#endif
}

/**
 * @brief   Disable alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to reset the multiplexing for
 */
void gpio_disable_mux(gpio_t pin);

/**
 * @brief   Available voltage regulators on the supply controller.
 */
typedef enum {
    SAM0_VREG_LDO,  /*< LDO, always available but not very power efficient */
    SAM0_VREG_BUCK  /*< Buck converter, efficient but may clash with internal
                        fast clock generators (see errata sheets) */
} sam0_supc_t;

/**
 * @brief       Switch the internal voltage regulator used for generating the
 *              internal MCU voltages.
 *              Available options are:
 *
 *               - LDO: not very efficient, but will always work
 *               - BUCK converter: Most efficient, but incompatible with the
 *                 use of DFLL or DPLL.
 *                 Please refer to the errata sheet, further restrictions may
 *                 apply depending on the MCU.
 *
 * @param[in]   src
 */
static inline void sam0_set_voltage_regulator(sam0_supc_t src)
{
#ifdef REG_SUPC_VREG
    if (src == SAM0_VREG_BUCK) {
        SUPC->VREG.reg |= (1 << SUPC_VREG_SEL_Pos);
    }
    else {
        SUPC->VREG.reg &= ~(1 << SUPC_VREG_SEL_Pos);
    }
    while (!(SUPC->STATUS.reg & SUPC_STATUS_VREGRDY)) {}
#else
    (void) src;
    assert(0);
#endif
}

/**
 * @brief   Returns the frequency of a GCLK provider.
 *
 * @param[in] id    The ID of the GCLK
 *
 * @return          The frequency of the GCLK with the given ID.
 */
uint32_t sam0_gclk_freq(uint8_t id);

/**
 * @brief   Enables an on-demand GCLK that has been configured in cpu.c
 *
 * @param[in] id    The ID of the GCLK
 */
void sam0_gclk_enable(uint8_t id);

/**
 * @brief   Return the numeric id of a SERCOM device derived from its address
 *
 * @param[in] sercom    SERCOM device
 *
 * @return              numeric id of the given SERCOM device
 */
static inline uint8_t sercom_id(const void *sercom)
{
#ifdef SERCOM0
    if (sercom == SERCOM0) {
        return 0;
    }
#endif
#ifdef SERCOM1
    if (sercom == SERCOM1) {
        return 1;
    }
#endif
#ifdef SERCOM2
    if (sercom == SERCOM2) {
        return 2;
    }
#endif
#ifdef SERCOM3
    if (sercom == SERCOM3) {
        return 3;
    }
#endif
#ifdef SERCOM4
    if (sercom == SERCOM4) {
        return 4;
    }
#endif
#ifdef SERCOM5
    if (sercom == SERCOM5) {
        return 5;
    }
#endif
#ifdef SERCOM6
    if (sercom == SERCOM6) {
        return 6;
    }
#endif
#ifdef SERCOM7
    if (sercom == SERCOM7) {
        return 7;
    }
#endif

    /* should not be reached, so fail with assert */
    assert(false);

    return SERCOM_INST_NUM;
}

#ifdef CPU_COMMON_SAMD5X
static inline uint8_t _sercom_gclk_id_core(uint8_t sercom_id) {
    if (sercom_id < 2) {
        return sercom_id + 7;
    } else if (sercom_id < 4) {
        return sercom_id + 21;
    } else {
        return sercom_id + 30;
    }
}
#endif

/**
 * @brief   Configure generator clock for given SERCOM device
 *
 * @param[in] sercom    SERCOM device
 * @param[in] gclk      Generator clock
 */
static inline void sercom_set_gen(void *sercom, uint8_t gclk)
{
    const uint8_t id = sercom_id(sercom);
    sam0_gclk_enable(gclk);
#if defined(CPU_COMMON_SAMD21)
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gclk) |
                         (SERCOM0_GCLK_ID_CORE + id));
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
#elif defined(CPU_COMMON_SAMD5X)
    GCLK->PCHCTRL[_sercom_gclk_id_core(id)].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk));
#else
    if (id < 5) {
        GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + id].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk));
    }
#if defined(CPU_COMMON_SAML21)
    else {
        GCLK->PCHCTRL[SERCOM5_GCLK_ID_CORE].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk));
    }
#endif /* CPU_COMMON_SAML21 */
#endif
}

/**
 * @brief   Returns true if the CPU woke deep sleep (backup/standby)
 */
static inline bool cpu_woke_from_backup(void)
{
#ifdef RSTC_RCAUSE_BACKUP
    return RSTC->RCAUSE.reg & RSTC_RCAUSE_BACKUP;
#else
    return false;
#endif
}

/**
 * @brief ADC Channel Configuration
 */
typedef struct {
    uint32_t inputctrl;     /**< ADC channel pin multiplexer value  */
#ifdef ADC0
    Adc *dev;               /**< ADC device descriptor */
#endif
} adc_conf_chan_t;

/**
 * @brief Compatibility define for muxpos struct member
 *        Unused on all platforms that have DIFFMODE in CTRLB
 */
#ifndef ADC_INPUTCTRL_DIFFMODE
#define ADC_INPUTCTRL_DIFFMODE  (1 << 7)
#endif

/**
 * @brief Pin that can be used for external voltage reference A
 */
#define ADC_REFSEL_AREFA_PIN    GPIO_PIN(PA, 3)

/**
 * @brief Pin that can be used for external voltage reference B
 */
#define ADC_REFSEL_AREFB_PIN    GPIO_PIN(PA, 4)

#if defined(ADC_REFCTRL_REFSEL_AREFC) || DOXYGEN
/**
 * @brief Pin that can be used for external voltage reference C
 */
#define ADC_REFSEL_AREFC_PIN    GPIO_PIN(PA, 6)
#endif

#ifndef DOXYGEN
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xff,                       /**< not supported */
#if defined(ADC_CTRLB_RESSEL)
    ADC_RES_8BIT  = ADC_CTRLB_RESSEL_8BIT_Val,  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CTRLB_RESSEL_10BIT_Val, /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CTRLB_RESSEL_12BIT_Val, /**< ADC resolution: 12 bit */
#elif defined(ADC_CTRLC_RESSEL)
    ADC_RES_8BIT  = ADC_CTRLC_RESSEL_8BIT_Val,  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CTRLC_RESSEL_10BIT_Val, /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CTRLC_RESSEL_12BIT_Val, /**< ADC resolution: 12 bit */
#endif
    ADC_RES_16BIT_2SAMPL    = ( 0x1 << 2) | 0x1, /**< sum of 2 12 bit samples    */
    ADC_RES_16BIT_4SAMPL    = ( 0x2 << 2) | 0x1, /**< sum of 4 12 bit samples    */
    ADC_RES_16BIT_8SAMPL    = ( 0x3 << 2) | 0x1, /**< sum of 8 12 bit samples    */
    ADC_RES_16BIT_16SAMPL   = ( 0x4 << 2) | 0x1, /**< sum of 16 12 bit samples   */
    ADC_RES_16BIT_32SAMPL   = ( 0x5 << 2) | 0x1, /**< sum of 32 12 bit samples   */
    ADC_RES_16BIT_64SAMPL   = ( 0x6 << 2) | 0x1, /**< sum of 64 12 bit samples   */
    ADC_RES_16BIT_128SAMPL  = ( 0x7 << 2) | 0x1, /**< sum of 128 12 bit samples  */
    ADC_RES_16BIT_256SAMPL  = ( 0x8 << 2) | 0x1, /**< sum of 256 12 bit samples  */
    ADC_RES_16BIT_512SAMPL  = ( 0x9 << 2) | 0x1, /**< sum of 512 12 bit samples  */
    ADC_RES_16BIT_1024SAMPL = ( 0xA << 2) | 0x1, /**< sum of 1024 12 bit samples */
    ADC_RES_14BIT = 0xfe,                        /**< not supported */
} adc_res_t;

#define ADC_RES_16BIT   ADC_RES_16BIT_16SAMPL   /**< default to 16x oversampling */
#endif /* DOXYGEN */

/**
 * @name Ethernet peripheral parameters
 * @{
 */
#ifndef ETH_RX_BUFFER_COUNT
#define ETH_RX_BUFFER_COUNT (4)
#endif

#ifndef ETH_TX_BUFFER_COUNT
#define ETH_TX_BUFFER_COUNT (2)
#endif

#ifndef ETH_RX_BUFFER_SIZE
#define ETH_RX_BUFFER_SIZE (1536)
#endif

#ifndef ETH_TX_BUFFER_SIZE
#define ETH_TX_BUFFER_SIZE (1536)
#endif
/** @} */

/**
 * @brief Ethernet parameters struct
 */
#if defined(GMAC_INST_NUM) || defined(DOXYGEN)
typedef struct {
    Gmac *dev;                /**< ptr to the device registers */
    gpio_t refclk;            /**< REFCLK gpio */
    gpio_t txen;              /**< TXEN gpio */
    gpio_t txd0;              /**< TXD0 gpio */
    gpio_t txd1;              /**< TXD1 gpio */
    gpio_t crsdv;             /**< CRSDV gpio */
    gpio_t rxd0;              /**< RXD0 gpio */
    gpio_t rxd1;              /**< RXD1 gpio */
    gpio_t rxer;              /**< RXER gpio */
    gpio_t mdc;               /**< MII interface, clock gpio */
    gpio_t mdio;              /**< MII interface, data gpio */
    gpio_t rst_pin;           /**< PHY reset gpio */
    gpio_t int_pin;           /**< PHY interrupt gpio */
} sam0_common_gmac_config_t;
#endif

/**
 * @brief USBDEV buffers must be word aligned because of DMA restrictions
 */
#define USBDEV_CPU_DMA_ALIGNMENT       (4)

/**
 * @brief USBDEV buffer instantiation requirement
 */
#define USBDEV_CPU_DMA_REQUIREMENTS    __attribute__((aligned(USBDEV_CPU_DMA_ALIGNMENT)))

/**
 * @brief USB peripheral parameters
 */
#if defined(USB_INST_NUM) || defined(DOXYGEN)
typedef struct {
    gpio_t dm;              /**< D- line gpio                           */
    gpio_t dp;              /**< D+ line gpio                           */
    gpio_mux_t d_mux;       /**< alternate function (mux) for data pins */
    UsbDevice *device;      /**< ptr to the device registers            */
    uint8_t gclk_src;       /**< GCLK source which supplys 48 MHz       */
} sam0_common_usb_config_t;
#endif /* USB_INST_NUM */

/**
 * @brief SDIO/SDMMC buffer alignment for SDHC because of DMA/FIFO buffer restrictions
 */
#define SDMMC_CPU_DMA_ALIGNMENT     4

/**
 * @brief SDIO/SDMMC buffer instantiation requirement for SDHC
 */
#define SDMMC_CPU_DMA_REQUIREMENTS  __attribute__((aligned(SDMMC_CPU_DMA_ALIGNMENT)))

/**
 * @brief SDHC peripheral configuration
 */
typedef struct {
    void *sdhc; /**< SDHC peripheral */
    gpio_t cd;  /**< Card Detect pin (must be GPIO_UNDEF if not connected) */
    gpio_t wp;  /**< Write Protect pin (must be GPIO_UNDEF if not connected) */
} sdhc_conf_t;

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
/* Limits are in clock cycles according to data sheet.
   As the WDT is clocked by a 1024 Hz clock, 1 cycle ≈ 1 ms */
#define NWDT_TIME_LOWER_LIMIT          (8U)
#define NWDT_TIME_UPPER_LIMIT          (16384U)
/** @} */

/**
 * @brief Watchdog can be stopped.
 */
#define WDT_HAS_STOP                   (1)
/**
 * @brief Watchdog has to be initialized.
 */
#define WDT_HAS_INIT                   (1)

/**
 * @brief   Frequency meter configuration
 */
typedef struct {
    gpio_t pin;             /**< GPIO at which the frequency is to be measured */
    uint8_t gclk_src;       /**< GCLK source select for reference */
} freqm_config_t;

#if defined(REV_DMAC) || DOXYGEN
/**
 * @name sam0 DMA peripheral
 * @{
 *
 * The sam0 DMA peripheral has a number of channels. Each channel is a separate
 * data stream, triggered by a configurable trigger when enabled, or triggered
 * by software (not yet supported). In theory each DMA channel is equal and can
 * have a configurable priority and can be triggered by the full set of triggers
 * available.
 *
 * DMA descriptors, specifying a single transfer with size, source and
 * destination, are kept in RAM and are read when the channel is enabled and
 * triggered. On the SAML21 platform, these descriptors must reside in the LP
 * SRAM.
 *
 * The DMA addresses supplied must point to the **end** of the array to be
 * transferred. When address increment is enabled this means that the supplied
 * src or dst argument must point to array + length. When increment is disabled,
 * the source or destination address can be used directly. The calculation of
 * the end of the array must be done by the calling function, because the
 * beatsize and the increment can usually be hardcoded there and doesn't have to
 * be retrieved from the DMA register configuration.
 * See also section 20.6.2.7 of the SAM D21/DA1 Family Data Sheet.
 *
 * Example:
 * ```
 * void transfer_data(void *src, void *dst, size_t len)
 * {
 *      dma_t channel = dma_acquire_channel()
 *      if (channel == 0xff) {
 *          return -E_BUSY;
 *      }
 *
 *      dma_setup(channel, DMA_TRIGGER_MY_PERIH, 0, true);
 *      dma_prepare(channel, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
 *                  (uint8_t*)src + len, (uint8_t*)dst + len, len);
 *
 *      dma_start(channel);
 *      dma_wait(channel);
 *
 *      dma_release_channel(channel);
 * }
 * ```
 */

/**
 * @brief Indicates that the peripheral doesn't utilize the DMA controller.
 *        Matches with the register configuration for software based triggers.
 */
#define DMA_TRIGGER_DISABLED           0

/**
 * @brief Move the DMA descriptors to the LP SRAM. Required on the SAML21
 */
#if defined(CPU_COMMON_SAML21) || defined(DOXYGEN)
#define DMA_DESCRIPTOR_IN_LPSRAM
#endif

/**
 * @brief Extra attributes required for instantiating DMA descriptors.
 */
#ifdef DMA_DESCRIPTOR_IN_LPSRAM
#define DMA_DESCRIPTOR_ATTRS    __attribute__((section(".backup.bss")))
#else
#define DMA_DESCRIPTOR_ATTRS
#endif

/**
 * @brief DMA channel type
 */
typedef unsigned dma_t;

/**
 * @brief Available DMA address increment modes
 */
typedef enum {
    DMA_INCR_NONE   = 0,    /**< Don't increment any addresses after a beat */
    DMA_INCR_SRC    = 1,    /**< Increment the source address after a beat */
    DMA_INCR_DEST   = 2,    /**< Increment destination address after a beat */
    DMA_INCR_BOTH   = 3,    /**< Increment both addresses after a beat */
} dma_incr_t;

/**
 * @brief   Initialize DMA
 */
void dma_init(void);

/**
 * @brief Acquire a DMA channel.
 *
 * A free DMA channel is marked as allocated and a reference is returned.
 * DMA channels can be acquired for long periods of time, e.g. from the start to
 * end of a number of transfers or directly at boot and never released.
 *
 * @returns     A reference to the DMA channel
 * @returns     UINT8_MAX when no DMA channel is available
 */
dma_t dma_acquire_channel(void);

/**
 * @brief   Release a previously acquired DMA channel
 *
 * @param   dma     DMA channel to release
 */
void dma_release_channel(dma_t dma);

/**
 * @brief   Initialize a previously allocated DMA channel with one-time settings
 *
 * @param   dma     DMA channel reference
 * @param   trigger Trigger to use for this DMA channel
 * @param   prio    Channel priority
 * @param   irq     Whether to enable the interrupt handler for this channel
 */
void dma_setup(dma_t dma, unsigned trigger, uint8_t prio, bool irq);

/**
 * @brief   Prepare the DMA channel for an individual transfer.
 *
 * @note    When increment is enabled for source or destination, the @p src
 *          and/or @p dst must point to the **end** of the array.
 *
 * @param   dma     DMA channel reference
 * @param   width   Transfer beat size to use
 * @param   src     Source address for the transfer
 * @param   dst     Destination address for the transfer
 * @param   num     Number of beats to transfer
 * @param   incr    Which of the addresses to increment after a beat
 */
void dma_prepare(dma_t dma, uint8_t width, const void *src, void *dst,
                 size_t num, dma_incr_t incr);

/**
 * @brief   Prepare a transfer without modifying the destination address
 *          settings.
 *
 * Can be used when repeatedly using a dma channel to transfer to the same
 * peripheral address, leaving the destination address and related settings
 * untouched
 *
 * @note    This only touches the source address, number of transfers and source
 *          increment settings. Be sure to initialize the full descriptor
 *          beforehand with @ref dma_prepare
 *
 * @note    When increment is enabled for source, the @p src must point to the
 *          **end** of the array.
 *
 * @param   dma     DMA channel reference
 * @param   src     Source address for the transfer
 * @param   num     Number of beats to transfer
 * @param   incr    Whether to increment the source address after a beat
 */
void dma_prepare_src(dma_t dma, const void *src, size_t num, bool incr);

/**
 * @brief   Prepare a transfer without modifying the source address
 *          settings.
 *
 * Can be used when repeatedly using a dma channel to transfer from the same
 * peripheral address, leaving the source address and related settings
 * untouched
 *
 * @note    This only touches the destination address, the number of transfers
 *          and destination increment settings. Be sure to initialize the full
 *          descriptor beforehand with @ref dma_prepare
 *
 * @note    When increment is enabled for destination, @p dst must point to the
 *          **end** of the array.
 *
 * @param   dma     DMA channel reference
 * @param   dst     Destination address for the transfer
 * @param   num     Number of beats to transfer
 * @param   incr    Whether to increment the destination address after a beat
 */
void dma_prepare_dst(dma_t dma, void *dst, size_t num, bool incr);

/**
 * @brief   Append a second transfer descriptor after the default channel
 *          descriptor.
 *
 * @note    Only a single extra transfer descriptor is supported for now.
 *
 * @note    @p next must remain valid throughout the full transfer duration
 *
 * @note    When increment is enabled for source or destination, @p src
 *          and/or @p dst must point to the **end** of the array.
 *
 * @param   dma         DMA channel reference to add the descriptor to
 * @param   descriptor  Extra transfer descriptor to append
 * @param   width       Transfer beat size to use
 * @param   src         Source address for the transfer
 * @param   dst         Destination address for the transfer
 * @param   num         Number of beats to transfer
 * @param   incr        Which of the addresses to increment after a beat
 */
void dma_append(dma_t dma, DmacDescriptor *descriptor, uint8_t width,
                const void *src, void *dst, size_t num, dma_incr_t incr);

/**
 * @brief   Append a second transfer descriptor after the default channel
 *          descriptor, copying destination and block size from the initial
 *          descriptor.
 *
 * @note    Only a single extra transfer descriptor is supported for now.
 *
 * @note    @p next must remain valid throughout the full transfer duration
 *
 * @note    When increment is enabled for source, @p src must point to the
 *          **end** of the array.
 *
 * @param   dma     DMA channel reference to add the descriptor to
 * @param   next    Extra transfer descriptor to append
 * @param   src     Source address for the transfer
 * @param   num     Number of beats to transfer
 * @param   incr    Whether to increment the source address after a beat
 */
void dma_append_src(dma_t dma, DmacDescriptor *next, const void *src,
                    size_t num, bool incr);

/**
 * @brief   Append a second transfer descriptor after the default channel
 *          descriptor, copying source and block size from the initial
 *          descriptor.
 *
 * @note    Only a single extra transfer descriptor is supported for now.
 *
 * @note    @p next must remain valid throughout the full transfer duration
 *
 * @note    When increment is enabled for destination, @p dst must point to the
 *          **end** of the array.
 *
 * @param   dma     DMA channel reference to add the descriptor to
 * @param   next    Extra transfer descriptor to append
 * @param   dst     Destination address for the transfer
 * @param   num     Number of beats to transfer
 * @param   incr    Whether to increment the source address after a beat
 */
void dma_append_dst(dma_t dma, DmacDescriptor *next, void *dst, size_t num,
                    bool incr);

/**
 * @brief   Start a DMA transfer.
 *
 * @param   dma     DMA channel reference
 */
void dma_start(dma_t dma);

/**
 * @brief   Wait for a DMA channel to finish the transfer.
 *
 * This function uses a blocking mutex to wait for the transfer to finish
 *
 * @note Use only with DMA channels of which the interrupt is enabled
 *
 * @param   dma     DMA channel reference
 */
void dma_wait(dma_t dma);

/**
 * @brief   Cancel an active DMA transfer
 *
 * It is not harmful to call this on an inactive channel, but it will waste some
 * processing time
 *
 * @param   dma     DMA channel reference
 */
void dma_cancel(dma_t dma);
/** @} */
#endif /* REV_DMAC || DOXYGEN */

/**
 * @name sam0 RTC Tamper Detection
 * @{
 */

/**
 * @brief   Power on the RTC (if the RTC/RTT is not otherwise used)
 */
void rtc_tamper_init(void);

/**
 * @brief   Enable Tamper Detection IRQs
 *
 * @param   pin     The GPIO pin to be used for tamper detection
 * @param   flank   The Flank to trigger the even
 *
 * @return  0 on success, -1 if pin is not RTC pin
 */
int rtc_tamper_register(gpio_t pin, gpio_flank_t flank);

/**
 * @brief   Enable Tamper Detection IRQs
 */
void rtc_tamper_enable(void);

/**
 * @brief   Get and clear the RTC tamper event that has woken the CPU
 *          from Deep Sleep.
 *
 * @return  The set bits in the return value correspond to the tamper
 *          pin index inside the @ref rtc_tamper_pins array.
 */
uint8_t rtc_get_tamper_event(void);

/**
 * @brief   Get the tamper event mask for a certain pin.
 *          Can be used together with @ref rtc_get_tamper_event to
 *          check which RTC  pin caused the tamper event.
 *
 * @param pin   Pin to query
 *
 * @return  Bit mask with the bit corresponding to @p pin set
 *          0 if @p pin is no RTC tamper pin
 */
uint8_t rtc_tamper_pin_mask(gpio_t pin);
/** @} */

/**
 * @name sam0 User Configuration
 *
 *      The MCUs of this family contain a region of memory that is used to store
 *      CPU configuration & calibration data.
 *      It can be used to set persistent settings and has some additional space
 *      to store user configuration data.
 * @{
 */

/**
 * @brief MCU configuration applied on start. The contents of this struct differ
 *        between families.
 */
typedef struct sam0_aux_cfg_mapping nvm_user_page_t;

/**
 * @brief   Size of the free to use auxiliary area in the user page
 */
#ifdef FLASH_USER_PAGE_SIZE
#define FLASH_USER_PAGE_AUX_SIZE (FLASH_USER_PAGE_SIZE - sizeof(nvm_user_page_t))
#else
#define FLASH_USER_PAGE_AUX_SIZE (AUX_PAGE_SIZE * AUX_NB_OF_PAGES - sizeof(nvm_user_page_t))
#endif

/**
 * @brief   Reset the configuration area, apply a new configuration.
 *
 *
 * @param   cfg     New MCU configuration, may be NULL.
 *                  If cfg is NULL, this will clear the configuration area
 *                  and apply the current configuration again.
 */
void sam0_flashpage_aux_reset(const nvm_user_page_t *cfg);

/**
 * @brief   Write data to the user configuration area.
 *          This will write data to the remaining space after @see nvm_user_page_t
 *          The size of this area depends on the MCU family used.
 *
 *          Will only write bits 1 -> 0. To reset bits to 1, call @see sam0_flashpage_aux_reset
 *          This will reset the whole user area configuration.
 *
 *          Arbitrary data lengths and offsets are supported.
 *
 * @param   offset  Byte offset after @see nvm_user_page_t
 *                  must be less than `FLASH_USER_PAGE_AUX_SIZE`
 * @param   data    The data to write
 * @param   len     Size of the data
 */
void sam0_flashpage_aux_write(uint32_t offset, const void *data, size_t len);

/**
 * @brief   Get pointer to data in the user configuration area.
 *
 * @param   offset  Byte offset after @see nvm_user_page_t
 *                  must be less than `FLASH_USER_PAGE_AUX_SIZE`
 * @return  Pointer to the data in the User Page
 */
#define sam0_flashpage_aux_get(offset)  \
    (const void*)((uint8_t*)NVMCTRL_USER + sizeof(nvm_user_page_t) + (offset))

/**
 * @brief   Get pointer to data in the CPU configuration struct
 *
 * @return  Pointer to the @ref nvm_user_page_t structure
 */
#define sam0_flashpage_aux_cfg() \
    ((const nvm_user_page_t*)NVMCTRL_USER)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
