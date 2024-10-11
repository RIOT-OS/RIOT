/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_rpx0xx
 * @{
 *
 * @file
 * @brief           RP2040 specific definitions for handling peripherals
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "vendor/RP2040.h"
#include "io_reg.h"
#include "macros/units.h"
#include "periph/pio.h" /* pio_t */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(PLL_SYS_REF_DIV) || defined(DOXYGEN)
/**
 * @brief   For generating the system clock via PLL, the XOSC reference clock can be divided. The
 *          datasheet says for the supported frequency range of the crystal, only a divider of 1 is
 *          reasonable.
 */
#define PLL_SYS_REF_DIV         1
#endif

#if !defined(PLL_USB_REF_DIV) || defined(DOXYGEN)
/**
 * @brief   Same as @ref PLL_SYS_REF_DIV but for the PLL generating the USB clock
 */
#define PLL_USB_REF_DIV         1
#endif

#if !defined(PLL_SYS_VCO_FEEDBACK_SCALE) || defined(DOXYGEN)
/**
 * @brief   VCO feedback scale used for system clock
 *
 * @note    The result of `PLL_SYS_VCO_FEEDBACK_SCALE * CLOCK_XOSC` must be in the range of
 *          400 MHz to 1600 MHz
 */
#define PLL_SYS_VCO_FEEDBACK_SCALE  125
#endif

#if !defined(PLL_SYS_POSTDIV1) || defined(DOXYGEN)
/**
 * @brief   First post-PLL clock divider for system clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_SYS_POSTDIV1            6
#endif

#if !defined(PLL_SYS_POSTDIV2) || defined(DOXYGEN)
/**
 * @brief   Second post-PLL clock divider for system clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_SYS_POSTDIV2            2
#endif

#if !defined(PLL_USB_VCO_FEEDBACK_SCALE) || defined(DOXYGEN)
/**
 * @brief   VCO feedback scale used for USB clock
 *
 * @note    The result of `PLL_USB_VCO_FEEDBACK_SCALE * CLOCK_XOSC` must be in the range of
 *          400 MHz to 1600 MHz
 */
#define PLL_USB_VCO_FEEDBACK_SCALE  40
#endif

#if !defined(PLL_USB_POSTDIV1) || defined(DOXYGEN)
/**
 * @brief   First post-PLL clock divider for USB clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_USB_POSTDIV1            5
#endif

#if !defined(PLL_USB_POSTDIV2) || defined(DOXYGEN)
/**
 * @brief   Second post-PLL clock divider for USB clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_USB_POSTDIV2            2
#endif

#if !defined(CLOCK_XOSC) || defined(DOXYGEN)
/**
 * @brief   The RP2040 reference design attaches a 12 MHz crystal to the MCU, so we take this
 *          value as default
 */
#define CLOCK_XOSC          MHZ(12)
#endif

/**
 * @brief   Calculate the resulting PLL clock frequency for the given parameters
 */
#define PLL_CLOCK(vco_feedback, postdiv1, postdiv2) \
    (CLOCK_XOSC * (vco_feedback) / (postdiv1) / (postdiv2))

/**
 * @brief   System core clock speed, 125 MHz unless board changes parameters
 */
#define CLOCK_CORECLOCK \
    PLL_CLOCK(PLL_SYS_VCO_FEEDBACK_SCALE, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2)

/**
 * @brief   USB clock speed
 */
#define CLOCK_USB \
    PLL_CLOCK(PLL_USB_VCO_FEEDBACK_SCALE, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2)

/**
 * @name    Ranges for clock frequencies and clock settings
 * @{
 */
#define CLOCK_XOSC_MAX              MHZ(15) /**< Maximum crystal frequency */
#define CLOCK_XOSC_MIN              MHZ(5)  /**< Minimum crystal frequency */
#define PLL_POSTDIV_MIN             (1U)    /**< Minimum value of the post PLL clock divers */
#define PLL_POSTDIV_MAX             (7U)    /**< Maximum value of the post PLL clock divers */
#define PLL_VCO_FEEDBACK_SCALE_MIN  (16U)   /**< Minimum value of the PLL VCO feedback scaler */
#define PLL_VCO_FEEDBACK_SCALE_MAX  (320U)  /**< Maximum value of the PLL VCO feedback scaler */
#define PLL_REF_DIV_MIN             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
#define PLL_REF_DIV_MAX             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
/** @} */

#if CLOCK_USB != MHZ(48)
#error "USB clock != 48 MHz, check PLL_USB_VCO_FEEDBACK_SCALE, PLL_USB_POSTDIV1, PLL_SYS_POSTDIV2"
#endif

#if (CLOCK_XOSC > CLOCK_XOSC_MAX) || (CLOCK_XOSC < CLOCK_XOSC_MIN)
#error "Value for CLOCK_XOSC out of range, check config"
#endif

#if (PLL_SYS_REF_DIV < PLL_REF_DIV_MIN) || (PLL_SYS_REF_DIV > PLL_REF_DIV_MAX)
#error "Value for PLL_SYS_REF_DIV out of range, check config"
#endif

#if (PLL_USB_REF_DIV < PLL_REF_DIV_MIN) || (PLL_USB_REF_DIV > PLL_REF_DIV_MAX)
#error "Value for PLL_USB_REF_DIV out of range, check config"
#endif

#if (PLL_SYS_VCO_FEEDBACK_SCALE < PLL_VCO_FEEDBACK_SCALE_MIN) \
    || (PLL_SYS_VCO_FEEDBACK_SCALE > PLL_VCO_FEEDBACK_SCALE_MAX)
#error "Value for PLL_SYS_VCO_FEEDBACK_SCALE out of range, check config"
#endif

#if (PLL_USB_VCO_FEEDBACK_SCALE < PLL_VCO_FEEDBACK_SCALE_MIN) \
    || (PLL_USB_VCO_FEEDBACK_SCALE > PLL_VCO_FEEDBACK_SCALE_MAX)
#error "Value for PLL_USB_VCO_FEEDBACK_SCALE out of range, check config"
#endif

#if (PLL_SYS_POSTDIV1 < PLL_POSTDIV_MIN) || (PLL_SYS_POSTDIV1 > PLL_POSTDIV_MAX)
#error "Value for PLL_SYS_POSTDIV1 out of range, check config"
#endif

#if (PLL_SYS_POSTDIV2 < PLL_POSTDIV_MIN) || (PLL_SYS_POSTDIV2 > PLL_POSTDIV_MAX)
#error "Value for PLL_SYS_POSTDIV2 out of range, check config"
#endif

#if (PLL_USB_POSTDIV1 < PLL_POSTDIV_MIN) || (PLL_USB_POSTDIV1 > PLL_POSTDIV_MAX)
#error "Value for PLL_USB_POSTDIV1 out of range, check config"
#endif

#if (PLL_USB_POSTDIV2 < PLL_POSTDIV_MIN) || (PLL_USB_POSTDIV2 > PLL_POSTDIV_MAX)
#error "Value for PLL_USB_POSTDIV2 out of range, check config"
#endif

#if !defined(CLOCK_PERIPH_SOURCE) || defined(DOXYGEN)
/**
 * @brief   How to source the peripheral clock (default: 125 MHz system clock)
 */
#define CLOCK_PERIPH_SOURCE CLOCKS_CLK_PERI_CTRL_AUXSRC_clk_sys
#endif

#if !defined(CLOCK_PERIPH) || defined(DOXYGEN)
/**
 * @brief   Peripheral clock (by default source from system clock)
 */
#define CLOCK_PERIPH        CLOCK_CORECLOCK
#endif

/**
 * @brief   Periphery blocks that can be reset
 */
#define RESETS_RESET_MASK               \
    (RESETS_RESET_usbctrl_Msk       |   \
     RESETS_RESET_uart1_Msk         |   \
     RESETS_RESET_uart0_Msk         |   \
     RESETS_RESET_timer_Msk         |   \
     RESETS_RESET_tbman_Msk         |   \
     RESETS_RESET_sysinfo_Msk       |   \
     RESETS_RESET_syscfg_Msk        |   \
     RESETS_RESET_spi1_Msk          |   \
     RESETS_RESET_spi0_Msk          |   \
     RESETS_RESET_rtc_Msk           |   \
     RESETS_RESET_pwm_Msk           |   \
     RESETS_RESET_pll_usb_Msk       |   \
     RESETS_RESET_pll_sys_Msk       |   \
     RESETS_RESET_pio1_Msk          |   \
     RESETS_RESET_pio0_Msk          |   \
     RESETS_RESET_pads_qspi_Msk     |   \
     RESETS_RESET_pads_bank0_Msk    |   \
     RESETS_RESET_jtag_Msk          |   \
     RESETS_RESET_io_qspi_Msk       |   \
     RESETS_RESET_io_bank0_Msk      |   \
     RESETS_RESET_i2c1_Msk          |   \
     RESETS_RESET_i2c0_Msk          |   \
     RESETS_RESET_dma_Msk           |   \
     RESETS_RESET_busctrl_Msk       |   \
     RESETS_RESET_adc_Msk)

/**
 * @brief   Convert (port, pin) tuple to a @ref gpio_t value
 *
 * @note    The RPX0XX MCU family only has a single GPIO port. Still the API requires a
 *          port parameter, which is currently ignored.
 */
#define GPIO_PIN(port, pin)     ((((port) & 0)) | (pin))

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          UINT32_MAX

/**
 * @brief   Override GPIO active flank values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LEVEL_LOW  = 0x1,          /**< emit interrupt level-triggered on low input */
    GPIO_LEVEL_HIGH = 0x2,          /**< emit interrupt level-triggered on low input */
    GPIO_FALLING    = 0x4,          /**< emit interrupt on falling flank */
    GPIO_RISING     = 0x8,          /**< emit interrupt on rising flank */
    GPIO_BOTH       = 0xc           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @name    GPIO pad configuration register defines and types
 * @{
 */
/**
 * @brief   Reset value of the GPIO pad configuration register
 */
#define GPIO_PAD_REGISTER_RESET_VALUE    (0x00000056)
/**
 * @brief   Possible drive strength values for @ref gpio_pad_ctrl_t::drive_strength
 */
enum {
    DRIVE_STRENGTH_2MA,         /**< set driver strength to 2 mA */
    DRIVE_STRENGTH_4MA,         /**< set driver strength to 4 mA */
    DRIVE_STRENGTH_8MA,         /**< set driver strength to 8 mA */
    DRIVE_STRENGTH_12MA,        /**< set driver strength to 12 mA */
    DRIVE_STRENGTH_NUMOF        /**< number of different drive strength options */
};

/**
 * @brief   Memory layout of GPIO control register in pads bank 0
 */
typedef struct {
    uint32_t slew_rate_fast         : 1;    /**< set slew rate control to fast */
    uint32_t schmitt_trig_enable    : 1;    /**< enable Schmitt trigger */
    uint32_t pull_down_enable       : 1;    /**< enable pull down resistor */
    uint32_t pull_up_enable         : 1;    /**< enable pull up resistor */
    uint32_t drive_strength         : 2;    /**< GPIO driver strength */
    uint32_t input_enable           : 1;    /**< enable as input */
    uint32_t output_disable         : 1;    /**< disable output, overwrite output enable from
                                             *   peripherals */
    uint32_t                        : 24;   /**< 24 bits reserved for future use */
} gpio_pad_ctrl_t;
/** @} */

/**
 * @name    GPIO I/O configuration register defines and types
 * @{
 */
/**
 * @brief   Reset value of the GPIO I/O configuration register
 */
#define GPIO_IO_REGISTER_RESET_VALUE    (0x0000001f)
/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::function_select
 */
typedef enum {
    FUNCTION_SELECT_SPI     = 1,    /**< connect pin to the SPI peripheral
                                     *   (MISO/MOSI/SCK depends on pin) */
    FUNCTION_SELECT_UART    = 2,    /**< connect pin to the UART peripheral
                                     *   (TXD/RXD depends on pin) */
    FUNCTION_SELECT_I2C     = 3,    /**< connect pin to the I2C peripheral
                                     *   (SCL/SDA depends on pin) */
    FUNCTION_SELECT_PWM     = 4,    /**< connect pin to the timer for PWM
                                     *   (channel depends on pin) */
    FUNCTION_SELECT_SIO     = 5,    /**< use pin as vanilla GPIO */
    FUNCTION_SELECT_PIO0    = 6,    /**< connect pin to the first PIO peripheral */
    FUNCTION_SELECT_PIO1    = 7,    /**< connect pin to the second PIO peripheral */
    FUNCTION_SELECT_CLOCK   = 8,    /**< connect pin to the timer (depending on pin: external clock,
                                     *   clock output, or not supported) */
    FUNCTION_SELECT_USB     = 9,    /**< connect pin to the USB peripheral
                                     *   (function depends on pin) */
    FUNCTION_SELECT_NONE    = 31,   /**< Reset value, pin unconnected */
} gpio_function_select_t;

/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::output_override
 */
enum {
    OUTPUT_OVERRIDE_NORMAL,         /**< drive pin from connected peripheral */
    OUTPUT_OVERRIDE_INVERT,         /**< drive pin from connected peripheral, but invert output */
    OUTPUT_OVERRIDE_LOW,            /**< drive pin low, overriding peripheral signal */
    OUTPUT_OVERRIDE_HIGH,           /**< drive pin high, overriding peripheral signal */
    OUTPUT_OVERRIDE_NUMOF           /**< number of possible output override settings */
};

/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::output_enable_override
 */
enum {
    OUTPUT_ENABLE_OVERRIDE_NOMARL,  /**< enable output as specified by connected peripheral */
    OUTPUT_ENABLE_OVERRIDE_INVERT,  /**< invert output enable setting of peripheral */
    OUTPUT_ENABLE_OVERRIDE_DISABLE, /**< disable output, overriding peripheral signal */
    OUTPUT_ENABLE_OVERRIDE_ENABLE,  /**< enable output, overriding peripheral signal */
    OUTPUT_ENABLE_OVERRIDE_NUMOF    /**< number of possible output enable override settings */
};

/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::input_override
 */
enum {
    INPUT_OVERRIDE_NOMARL,          /**< don't mess with peripheral input signal */
    INPUT_OVERRIDE_INVERT,          /**< invert signal to connected peripheral */
    INPUT_OVERRIDE_LOW,             /**< signal low to connected peripheral */
    INPUT_OVERRIDE_HIGH,            /**< signal high to connected peripheral */
    INPUT_OVERRIDE_NUMOF            /**< number of possible input override settings */
};

/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::irq_override
 */
enum {
    IRQ_OVERRIDE_NORMAL,            /**< don't mess with IRQ signal */
    IRQ_OVERRIDE_INVERT,            /**< invert IRQ signal */
    IRQ_OVERRIDE_LOW,               /**< set IRQ signal to low */
    IRQ_OVERRIDE_HIGH,              /**< set IRQ signal to high */
    IRQ_OVERRIDE_NUMOF              /**< number of possible IRQ override settings */
};

/**
 * @brief   Memory layout of GPIO control register in IO bank 0
 */
typedef struct {
    uint32_t function_select        : 5;    /**< select GPIO function */
    uint32_t                        : 3;    /**< 3 bits reserved for future use */
    uint32_t output_override        : 2;    /**< output value override */
    uint32_t                        : 2;    /**< 2 bits reserved for future use */
    uint32_t output_enable_override : 2;    /**< output enable override */
    uint32_t                        : 2;    /**< 2 bits reserved for future use */
    uint32_t input_override         : 2;    /**< input value override */
    uint32_t                        : 10;   /**< 10 bits reserved for future use */
    uint32_t irq_override           : 2;    /**< interrupt inversion override */
    uint32_t                        : 2;    /**< 2 bits reserved for future use */
} gpio_io_ctrl_t;
/** @} */

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< Pin connected to the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   Number of slices available per PWM device
 */
#define PWM_SLICE_NUMOF   (8)

/**
 * @brief   Number of channels available per slice
 */
#define PWM_CHANNEL_NUMOF (2)

/**
 * @brief   PWM channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;

/**
 * @brief   PWM device configuration data structure
 */
typedef struct {
    uint8_t pwm_slice;                      /**< PWM slice instance,
                                                must be < to PWM_SLICE_NUMOF */
    pwm_chan_t chan[PWM_CHANNEL_NUMOF];     /**< channel mapping set to
                                                 {GPIO_UNDEF, 0} if not used */
} pwm_conf_t;

/**
 * @brief   Configuration details for an UART interface needed by the RPX0XX peripheral
 */
typedef struct {
    UART0_Type *dev;    /**< Base address of the I/O registers of the device */
    gpio_t rx_pin;      /**< GPIO pin to use for RX */
    gpio_t tx_pin;      /**< GPIO pin to use for TX */
    IRQn_Type irqn;     /**< IRQ number of the UART interface */
} uart_conf_t;

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Configuration type of a timer channel
 */
typedef struct {
    IRQn_Type irqn;                 /**< timer channel interrupt number */
} timer_channel_conf_t;

/**
 * @brief   Configuration type of a timer device @ref timer_conf_t::dev,
 *          having @ref timer_conf_t::ch_numof number of channels,
 *          each one modeled as @ref timer_channel_conf_t
 */
typedef struct {
    TIMER_Type *dev;                /**< pointer to timer base address */
    const timer_channel_conf_t *ch; /**< pointer to timer channel configuration */
    uint8_t ch_numof;               /**< number of timer channels */
} timer_conf_t;

/**
 * @brief   PIO configuration type
 */
typedef struct {
    PIO0_Type *dev;                 /**< PIO device */
    IRQn_Type irqn0;                /**< PIO IRQ0 interrupt number */
    IRQn_Type irqn1;                /**< PIO IRQ1 interrupt number */
} pio_conf_t;

/**
 * @brief   PIO I2C configuration type
 */
typedef struct {
    pio_t pio;                      /**< PIO number of the PIO to run this configuration */
    gpio_t sda;                     /**< Pin to use as SDA pin */
    gpio_t scl;                     /**< Pin to use as SCL pin */
    unsigned irq;                   /**< PIO IRQ line to use */
} pio_i2c_conf_t;

/**
 * @brief   Get the PAD control register for the given GPIO pin as word
 *
 * Note: Only perform 32-bit writes to I/O registers.
 */
static inline volatile uint32_t * gpio_pad_register(uint8_t pin)
{
    return (uint32_t *)(PADS_BANK0_BASE + 4 + (pin << 2));
}

/**
 * @brief   Convenience function to set the pad configuration of the given pin
 *          using the bit-field convenience type
 */
static inline void gpio_set_pad_config(uint8_t pin, gpio_pad_ctrl_t config)
{
    uint32_t *c = (uint32_t *)&config;
    *gpio_pad_register(pin) = *c;
}

/**
 * @brief   Get the IO control register for the given GPIO pin as word
 */
static inline volatile uint32_t * gpio_io_register(uint8_t pin)
{
    return (uint32_t *)(IO_BANK0_BASE + 4 + (pin << 3));
}

/**
 * @brief   Convenience function to set the I/O configuration of the given pin
 *          using the bit-field convenience type
 */
static inline void gpio_set_io_config(uint8_t pin, gpio_io_ctrl_t config)
{
    uint32_t *c = (uint32_t *)&config;
    *gpio_io_register(pin) = *c;
}

/**
 * @brief   Set the function select subregister for the given pin to the given value
 */
static inline void gpio_set_function_select(uint8_t pin, gpio_function_select_t funcsel)
{
    io_reg_write_dont_corrupt(gpio_io_register(pin), funcsel << IO_BANK0_GPIO0_CTRL_FUNCSEL_Pos,
                              IO_BANK0_GPIO0_CTRL_FUNCSEL_Msk);
}

/**
 * @brief   Restore the default I/O and PAD configuration of the given GPIO pin
 */
static inline void gpio_reset_all_config(uint8_t pin)
{
    *gpio_io_register(pin) = GPIO_IO_REGISTER_RESET_VALUE;
    *gpio_pad_register(pin) = GPIO_PAD_REGISTER_RESET_VALUE;
}

/**
 * @brief   Reset hardware components
 *
 * @param   components bitmask of components to be reset,
 *          @see RESETS_RESET_MASK
 */
static inline void periph_reset(uint32_t components)
{
    io_reg_atomic_set(&RESETS->RESET, components);
}

/**
 * @brief   Waits until hardware components have been reset
 *
 * @param   components bitmask of components that must have reset,
 *          @see RESETS_RESET_MASK
 */
static inline void periph_reset_done(uint32_t components)
{
    io_reg_atomic_clear(&RESETS->RESET, components);
    while ((~RESETS->RESET_DONE) & components) { }
}

/**
 * @name    RP2040 clock configuration
 * @{
 */

/**
 * @brief   Configure the system clock to run from the reference clock,
 *          which is the default on boot
 *
 * @param   f_in        Input frequency of the reference clock
 * @param   f_out       Output frequency of the system clock
 * @param   source      Clock source
 */
void clock_sys_configure_source(uint32_t f_in, uint32_t f_out,
                                CLOCKS_CLK_SYS_CTRL_SRC_Enum source);

/**
 * @brief   Configure the system clock to run from an auxiliary clock source,
 *          like PLL
 *
 * @note    The auxiliary must have been configured beforehand
 *
 * @param   f_in        Input frequency of the auxiliary clock source
 * @param   f_out       Output frequency of the system clock
 * @param   aux         Which auxiliary clock source to use
 */
void clock_sys_configure_aux_source(uint32_t f_in, uint32_t f_out,
                                    CLOCKS_CLK_SYS_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure the reference clock to run from a clock source,
 *          which is either the ROSC or the XOSC
 *
 * @note    Make sure that ROSC or XOSC are properly set up
 *
 * @param   f_in        Input frequency of the reference clock
 * @param   f_out       Output frequency of the system clock
 * @param   source      Clock source
 */
void clock_ref_configure_source(uint32_t f_in, uint32_t f_out,
                                CLOCKS_CLK_REF_CTRL_SRC_Enum source);

/**
 * @brief   Configure the reference clock to run from an auxiliary clock source,
 *          like PLL
 *
 * @note    The auxiliary must have been configured beforehand
 *
 * @param   f_in        Input frequency of the auxiliary clock source
 * @param   f_out       Output frequency of the reference clock
 * @param   aux         Which auxiliary clock source to use
 */
void clock_ref_configure_aux_source(uint32_t f_in, uint32_t f_out,
                                    CLOCKS_CLK_REF_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure the peripheral clock to run from a dedicated auxiliary
 *          clock source
 *
 * @param   aux     Auxiliary clock source
 */
void clock_periph_configure(CLOCKS_CLK_PERI_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure gpio21 as clock output pin
 *
 * @details Can be used as an external clock source for another circuit or
 *          to check the expected signal with a logic analyzer
 *
 * @param   f_in        Input frequency
 * @param   f_out       Output frequency
 * @param   aux         Auxiliary clock source
 */
void clock_gpout0_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure gpio23 as clock output pin
 *
 * @details Can be used as an external clock source for another circuit or
 *          to check the expected signal with a logic analyzer
 *
 * @param   f_in        Input frequency
 * @param   f_out       Output frequency
 * @param   aux         Auxiliary clock source
 */
void clock_gpout1_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT1_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure gpio24 as clock output pin
 *
 * @details Can be used as an external clock source for another circuit or
 *          to check the expected signal with a logic analyzer
 *
 * @param   f_in        Input frequency
 * @param   f_out       Output frequency
 * @param   aux         Auxiliary clock source
 */
void clock_gpout2_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT2_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure gpio25 as clock output pin
 *
 * @details Can be used as an external clock source for another circuit or
 *          to check the expected signal with a logic analyzer
 *
 * @param   f_in        Input frequency
 * @param   f_out       Output frequency
 * @param   aux         Auxiliary clock source
 */
void clock_gpout3_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT3_CTRL_AUXSRC_Enum aux);

/**
 * @brief   Configure the ADC clock to run from a dedicated auxiliary
 *          clock source
 *
 * @param   aux     Auxiliary clock source
 */
void clock_adc_configure(CLOCKS_CLK_ADC_CTRL_AUXSRC_Enum aux);
/** @} */

/**
 * @name    RP2040 PLL configuration
 * @{
 */

/**
 * @brief   Start the PLL for the system clock
 *          output[MHz] = f_ref / @p ref_div * @p vco_feedback_scale / @p post_div_1 / @p post_div_2
 *
 * @note    Usual setting should be (12 MHz, 1, 125, 6, 2) to get a 125 MHz system clock signal
 *
 * @param   ref_div     Input clock divisor
 * @param   vco_feedback_scale  VCO feedback scales
 * @param   post_div_1  Output post divider factor 1
 * @param   post_div_2  Output post divider factor 2
 */
void pll_start_sys(uint8_t ref_div,
                   uint16_t vco_feedback_scale,
                   uint8_t post_div_1, uint8_t post_div_2);

/**
 * @brief   Start the PLL for the USB clock
 *          output[MHz] = f_ref / @p ref_div * @p vco_feedback_scale / @p post_div_1 / @p post_div_2
 *
 * @note    Usual setting should be (12 MHz, 1, 40, 5, 2) to get a 48 MHz USB clock signal
 *
 * @param   ref_div     Input clock divisor
 * @param   vco_feedback_scale  VCO feedback scales
 * @param   post_div_1  Output post divider factor 1
 * @param   post_div_2  Output post divider factor 2
 */
void pll_start_usb(uint8_t ref_div,
                   uint16_t vco_feedback_scale,
                   uint8_t post_div_1, uint8_t post_div_2);

/**
 * @brief   Stop the PLL of the system clock
 */
void pll_stop_sys(void);

/**
 * @brief   Stop the PLL of the USB clock
 */
void pll_stop_usb(void);

/**
 * @brief   Reset the PLL of the system clock
 */
void pll_reset_sys(void);

/**
 * @brief   Reset the PLL of the USB clock
 */
void pll_reset_usb(void);

/** @} */

/**
 * @name    RP2040 XOSC configuration
 * @{
 */
/**
 * @brief   Configures the Crystal to run.
 *
 * @param   f_ref       Desired frequency in Hz
 *
 * @pre     1 MHz <= @p f_ref <= 15 MHz.
 *
 * The reference hardware manual suggests to use a 12 MHz crystal.
 */
void xosc_start(uint32_t f_ref);

/**
 * @brief   Stop the crystal.
 */
void xosc_stop(void);

/** @} */

/**
 * @name    RP2040 ROSC configuration
 * @{
 */

/**
 * @brief   Start the ring oscillator in default mode.
 *
 * The ROSC is running at boot time but may be turned off to save power when switching to the
 * accurate XOSC. The default ROSC provides an instable frequency of 1.8 MHz to 12 MHz.
 */
void rosc_start(void);

/**
 * @brief   Turn off the ROSC to save power.
 *          The system clock must be switched to to another lock source
 *          before the ROSC is stopped, other wise the chip will be lock up.
 */
void rosc_stop(void);

/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
enum {
    SPI_CLK_100KHZ = KHZ(100), /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = KHZ(400), /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = MHZ(1),   /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = MHZ(5),   /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = MHZ(10),  /**< drive the SPI bus with 10MHz */
};

/**
 * @brief   SPI clock type
 */
typedef uint32_t spi_clk_t;
/** @} */

/**
 * @brief   Configuration details for an SPI interface needed by the RPX0XX peripheral
 */
typedef struct {
    SPI0_Type *dev;     /**< Base address of the I/O registers of the device */
    gpio_t miso_pin;    /**< GPIO pin to use for MISO */
    gpio_t mosi_pin;    /**< GPIO pin to use for MOSI */
    gpio_t clk_pin;     /**< GPIO pin to use for CLK */
} spi_conf_t;

#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
