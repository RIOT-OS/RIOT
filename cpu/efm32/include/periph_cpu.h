/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "kernel_defines.h"
#include "mutex.h"

#include "cpu.h"
#include "cpu_conf.h"

#if defined(_SILICON_LABS_32B_SERIES_2)
#include "em_iadc.h"
#else
#include "em_adc.h"
#endif
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_usart.h"
#include "em_wdog.h"
#include "em_rtc.h"
#if defined(_SILICON_LABS_32B_SERIES_0)
#include "em_dac.h"
#elif defined (_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
#include "em_vdac.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock mux configuration
 */
typedef struct {
    CMU_Clock_TypeDef clk;   /**< Clock domain */
    CMU_Select_TypeDef src;  /**< Source clock */
} clk_mux_t;

/**
 * @brief   Clock divider configuration
 */
typedef struct {
    CMU_Clock_TypeDef clk;   /**< Clock domain */
    CMU_ClkDiv_TypeDef div;  /**< Divisor */
} clk_div_t;


/**
 * @brief   Length of CPU ID in octets.
 */
#define CPUID_LEN           (8U)

/**
 * @brief   CPU Frequency Define
 */
#define CLOCK_CORECLOCK     SystemCoreClock

#if defined(DAC_COUNT) && DAC_COUNT > 0
/**
 * @brief   DAC device configuration
 */
typedef struct {
    DAC_TypeDef *dev;       /**< DAC device used */
    DAC_Ref_TypeDef ref;    /**< DAC voltage reference */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
} dac_conf_t;

/**
 * @brief   DAC channel configuration
 */
typedef struct {
    uint8_t dev;            /**< device index */
    uint8_t index;          /**< channel index */
} dac_chan_conf_t;

#elif defined(VDAC_COUNT) && VDAC_COUNT > 0
/**
 * @brief   DAC device configuration (VDAC configuration of EFM32 Series 1)
 */
typedef struct {
    VDAC_TypeDef *dev;      /**< DAC device used */
    VDAC_Ref_TypeDef ref;   /**< DAC voltage reference */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
} dac_conf_t;

/**
 * @brief   DAC channel configuration (VDAC configuration of EFM32 Series 1)
 */
typedef struct {
    uint8_t dev;            /**< device index */
    uint8_t index;          /**< channel index */
} dac_chan_conf_t;
#endif

/**
 * @name    Real time counter configuration
 * @{
 */
/* RTT_MAX_VALUE some are 24bit, some are 32bit */
#if defined(_RTC_CNT_MASK)
#define RTT_MAX_VALUE       _RTC_CNT_MASK        /* mask has all bits set ==> MAX*/
#elif defined(_RTCC_CNT_MASK)
#define RTT_MAX_VALUE       _RTCC_CNT_MASK       /* mask has all bits set ==> MAX*/
#endif
#define RTT_MAX_FREQUENCY   (32768U)             /* in Hz */
#define RTT_MIN_FREQUENCY   (1U)                 /* in Hz */
#define RTT_CLOCK_FREQUENCY (32768U)             /* in Hz, LFCLK*/

/** @} */

/**
 * @brief   Define a custom type for GPIO pins.
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value.
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Mandatory function for defining a GPIO pins.
 */
#define GPIO_PIN(x, y)      ((gpio_t) ((x << 4) | y))

/**
 * @brief   Internal macro for combining pin mode (x) and pull-up/down (y).
 */
#define GPIO_MODE(x, y)     ((x << 1) | y)

/**
 * @brief   Available ports on the EFM32.
 */
enum {
#if (_GPIO_PORT_A_PIN_COUNT > 0)
    PA = gpioPortA,         /**< port A */
#endif
#if (_GPIO_PORT_B_PIN_COUNT > 0)
    PB = gpioPortB,         /**< port B */
#endif
#if (_GPIO_PORT_C_PIN_COUNT > 0)
    PC = gpioPortC,         /**< port C */
#endif
#if (_GPIO_PORT_D_PIN_COUNT > 0)
    PD = gpioPortD,         /**< port D */
#endif
#if (_GPIO_PORT_E_PIN_COUNT > 0)
    PE = gpioPortE,         /**< port E */
#endif
#if (_GPIO_PORT_F_PIN_COUNT > 0)
    PF = gpioPortF,         /**< port F */
#endif
#if (_GPIO_PORT_G_PIN_COUNT > 0)
    PG = gpioPortG,         /**< port G */
#endif
#if (_GPIO_PORT_H_PIN_COUNT > 0)
    PH = gpioPortH,         /**< port H */
#endif
#if (_GPIO_PORT_I_PIN_COUNT > 0)
    PI = gpioPortI,         /**< port I */
#endif
#if (_GPIO_PORT_J_PIN_COUNT > 0)
    PJ = gpioPortJ,         /**< port J */
#endif
#if (_GPIO_PORT_K_PIN_COUNT > 0)
    PK = gpioPortK          /**< port K */
#endif
};

#ifndef DOXYGEN
/**
 * @brief   Override direction values.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(gpioModeInput, 0),             /**< pin as input */
    GPIO_IN_PD = GPIO_MODE(gpioModeInputPull, 0),         /**< pin as input with pull-down */
    GPIO_IN_PU = GPIO_MODE(gpioModeInputPull, 1),         /**< pin as input with pull-up */
    GPIO_OUT   = GPIO_MODE(gpioModePushPull, 0),          /**< pin as output */
    GPIO_OD    = GPIO_MODE(gpioModeWiredAnd, 1),          /**< pin as open-drain */
    GPIO_OD_PU = GPIO_MODE(gpioModeWiredAndPullUp, 1),    /**< pin as open-drain with pull-up */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override active flank configuration values.
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

#if defined(_SILICON_LABS_32B_SERIES_2)
/**
 * @brief   Internal macro for combining over-sampling rate (osr), digital
 *          averaging count (avg) and output resolution (res).
 *
 * @note    The efr32xg23 reference manual provides this folumar:
 *          res = 11 bit + log_2(osr * avg) bit
 */
#if defined(_IADC_CFG_DIGAVG_MASK)
#define ADC_MODE(osr, avg, res)  ((osr << 16) | (avg << 8) | res)
#else
#define ADC_MODE(osr, res)  ((osr << 16) | res)
#endif

/**
 * @brief   Internal macro to extract averaging count
 */
#define ADC_MODE_OSR(mode)       ((mode & 0xff0000) >> 16)

#if defined(_IADC_CFG_DIGAVG_MASK)
/**
 * @brief   Internal macro to extract over-sampling rate
 */
#define ADC_MODE_AVG(mode)       ((mode & 0x00ff00) >> 8)
#endif

/**
 * @brief   Internal macro to extract output resolution
 */
#define ADC_MODE_RES(mode)       ((mode & 0x0000ff) >> 0)

/**
 * @brief   Possible ADC resolution settings
 * @{
 */
#define HAVE_ADC_RES_T
#if defined(_IADC_CFG_DIGAVG_MASK)
typedef enum {
    ADC_RES_6BIT  = ADC_MODE(iadcCfgOsrHighSpeed2x, iadcDigitalAverage1, 6),
    ADC_RES_8BIT  = ADC_MODE(iadcCfgOsrHighSpeed2x, iadcDigitalAverage1, 8),
    ADC_RES_10BIT = ADC_MODE(iadcCfgOsrHighSpeed2x, iadcDigitalAverage1, 10),
    ADC_RES_12BIT = ADC_MODE(iadcCfgOsrHighSpeed2x, iadcDigitalAverage1, 12),
    ADC_RES_14BIT = ADC_MODE(iadcCfgOsrHighSpeed8x, iadcDigitalAverage1, 14),
    ADC_RES_16BIT = ADC_MODE(iadcCfgOsrHighSpeed16x, iadcDigitalAverage2, 16),
} adc_res_t;
#else
typedef enum {
    ADC_RES_6BIT  = ADC_MODE(iadcCfgOsrHighSpeed2x, 6),
    ADC_RES_8BIT  = ADC_MODE(iadcCfgOsrHighSpeed2x, 8),
    ADC_RES_10BIT = ADC_MODE(iadcCfgOsrHighSpeed2x, 10),
    ADC_RES_12BIT = ADC_MODE(iadcCfgOsrHighSpeed2x, 12),
    ADC_RES_14BIT = ADC_MODE(iadcCfgOsrHighSpeed8x, 14),
    ADC_RES_16BIT = ADC_MODE(iadcCfgOsrHighSpeed32x, 16),
} adc_res_t;
#endif

/**
 * @brief   ADC device configuration
 */
typedef struct {
    /**
     * IADC device configuration
     */
    IADC_TypeDef *dev;

    /**
     * CMU gate for the IADC device
     */
    CMU_Clock_TypeDef cmu;

    /**
     * Voltage reference to use
     */
    IADC_CfgReference_t reference;

    /**
     * Voltage of the reference in mV
     *
     * @note  Required internally for offset correction.
     */
    uint32_t reference_mV;

    /**
     * Ampilfication of the analog input signal
     *
     * @note  The maximum input voltage is
     *        \ref adc_conf_t.gain * \ref adc_conf_t.reference_mV
     */
    IADC_CfgAnalogGain_t gain;

    /**
     * Available resoltions
     *
     * @note  Resolutions made available to the applications have to be
     *        specified during \ref adc_init. This will configure the IADC
     *        accordingly and allows for quick \ref adc_sample calls.
     */
    adc_res_t available_res[IADC0_CONFIGNUM];
} adc_conf_t;

/**
 * @brief   ADC channel configuration
 */
typedef struct {
    /**
     * \ref adc_conf_t device index
     */
    uint8_t dev;

    /**
     * Positive analog input
     */
    gpio_t input_pos;

    /**
     * Negative analog input.
     * Can be set to \ref GPIO_UNDEF for single-ended ADC lines.
     *
     * @note  For differential inputs make sure that
     *        \ref adc_chan_conf_t.input_pos is an even pin number and
     *        \ref adc_chan_conf_t.input_neg is an odd pin number or the other
     *        way around.
     */
    gpio_t input_neg;
} adc_chan_conf_t;
#else  /* defined(_SILICON_LABS_32B_SERIES_2) */
/**
 * @brief   Internal macro for combining ADC resolution (x) with number of
 *          shifts (y).
 */
#define ADC_MODE(x, y)      ((y << 4) | x)

/**
 * @brief   Internal define to note that resolution is not supported.
 */
#define ADC_MODE_UNDEF(x)   (ADC_MODE(x, 15))

#ifndef DOXYGEN
/**
 * @brief   Possible ADC resolution settings
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = ADC_MODE(adcRes6Bit, 0),    /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = ADC_MODE(adcRes8Bit, 0),    /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_MODE(adcRes12Bit, 2),   /**< ADC resolution: 10 bit (shifted from 12 bit) */
    ADC_RES_12BIT = ADC_MODE(adcRes12Bit, 0),   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = ADC_MODE_UNDEF(0),          /**< ADC resolution: 14 bit (unsupported) */
    ADC_RES_16BIT = ADC_MODE_UNDEF(1),          /**< ADC resolution: 16 bit (unsupported) */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   ADC device configuration
 */
typedef struct {
    ADC_TypeDef *dev;                 /**< ADC device used */
    CMU_Clock_TypeDef cmu;            /**< the device CMU channel */
} adc_conf_t;

/**
 * @brief   ADC channel configuration
 */
typedef struct {
    uint8_t dev;                      /**< device index */
#if defined(_SILICON_LABS_32B_SERIES_0)
    ADC_SingleInput_TypeDef input;    /**< input channel */
#elif defined(_SILICON_LABS_32B_SERIES_1)
    ADC_PosSel_TypeDef input;         /**< input channel */
#endif
    ADC_Ref_TypeDef reference;        /**< channel voltage reference */
    ADC_AcqTime_TypeDef acq_time;     /**< channel acquisition time */
} adc_chan_conf_t;
#endif  /* !defined(_SILICON_LABS_32B_SERIES_2) */

/**
 * @brief   Override hardware crypto supported methods.
 * @{
 */
#define HAVE_HWCRYPTO_AES128
#ifdef AES_CTRL_AES256
#define HAVE_HWCRYPTO_AES256
#endif
#if defined(_SILICON_LABS_32B_SERIES_1)
#define HAVE_HWCRYPTO_SHA1
#define HAVE_HWCRYPTO_SHA256
#endif
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override I2C speed values.
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW = 10000,            /**< low speed mode: ~10kbit/s */
    I2C_SPEED_NORMAL = 100000,        /**< normal mode: ~100kbit/s */
    I2C_SPEED_FAST = 400000,          /**< fast mode: ~400kbit/sj */
    I2C_SPEED_FAST_PLUS = 1000000,    /**< fast plus mode: ~1Mbit/s */
    I2C_SPEED_HIGH = 3400000,         /**< high speed mode: ~3.4Mbit/s */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C device configuration.
 */
typedef struct {
    I2C_TypeDef *dev;       /**< USART device used */
    gpio_t sda_pin;         /**< pin used for SDA */
    gpio_t scl_pin;         /**< pin used for SCL */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
    uint32_t loc;           /**< location of I2C pins */
#endif
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
    uint32_t speed;         /**< the bus speed */
} i2c_conf_t;

/**
 * @brief   Declare needed generic I2C functions.
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_WRITE_REG
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override PWM mode values.
 * @{
 */
#define HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT = timerModeUp,           /*< use left aligned PWM */
    PWM_RIGHT = timerModeDown,        /*< use right aligned PWM */
    PWM_CENTER = timerModeUp          /*< not supported, use left aligned */
} pwm_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   PWM channel configuration.
 */
typedef struct {
    uint8_t index;          /**< TIMER channel to use */
    gpio_t pin;             /**< pin used for pwm */
    uint32_t loc;           /**< location of the pin */
} pwm_chan_conf_t;

/**
 * @brief   PWM device configuration.
 */
typedef struct {
    TIMER_TypeDef *dev;               /**< TIMER device used */
    CMU_Clock_TypeDef cmu;            /**< the device CMU channel */
    IRQn_Type irq;                    /**< the devices base IRQ channel */
    uint8_t channels;                 /**< the number of available channels */
    const pwm_chan_conf_t* channel;   /**< pointer to first channel config */
} pwm_conf_t;

#ifndef DOXYGEN
/**
 * @brief   Override SPI clocks.
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = usartClockMode0,
    SPI_MODE_1 = usartClockMode1,
    SPI_MODE_2 = usartClockMode2,
    SPI_MODE_3 = usartClockMode3
} spi_mode_t;
/** @} */

/**
 * @brief   Define a set of pre-defined SPI clock speeds.
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100000,          /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400000,          /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ = 1000000,           /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ = 5000000,           /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ = 10000000          /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   SPI device configuration.
 */
typedef struct {
    USART_TypeDef *dev;     /**< USART device used */
    gpio_t mosi_pin;        /**< pin used for MOSI */
    gpio_t miso_pin;        /**< pin used for MISO */
    gpio_t clk_pin;         /**< pin used for CLK */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
    uint32_t loc;           /**< location of SPI pins */
#endif
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
} spi_dev_t;

/**
 * @brief   Declare needed generic SPI functions.
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Define timer configuration values
 *
 * @note    For the configuration of series 0 and 1, prescale and actual timer
 *          must be adjacent to each other (e.g. TIMER0 and TIMER1, or TIMER2
 *          and TIMER3, etc.).
 * @{
 */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1) || defined(DOXYGEN)
typedef struct {
    void *dev;              /**< TIMER_TypeDef or LETIMER_TypeDef device used */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
} timer_dev_t;
#endif

typedef struct {
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1) || defined(DOXYGEN)
    timer_dev_t prescaler;  /**< the lower neighboring timer (not initialized for LETIMER) */
    timer_dev_t timer;      /**< the higher numbered timer */
    IRQn_Type irq;          /**< number of the higher timer IRQ channel */
    uint8_t channel_numof;  /**< number of channels per timer */
#else
    void *dev;              /**< TIMER_TypeDef or LETIMER_TypeDef device used */
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< number of the higher timer IRQ channel */
#endif
} timer_conf_t;

#define LETIMER_MAX_VALUE _LETIMER_TOP_MASK  /**< max timer value of LETIMER peripheral */
#define TIMER_MAX_VALUE _TIMER_TOP_MASK      /**< max timer value of TIMER peripheral */
/** @} */

/**
 * @brief   Use LETIMER as the base timer for XTIMER
 */
#ifndef CONFIG_EFM32_XTIMER_USE_LETIMER
#define CONFIG_EFM32_XTIMER_USE_LETIMER   0
#endif

/**
 * @brief   UART device configuration.
 */
#ifndef DOXYGEN
/**
 * @brief   Marker for unsupported UART modes
 */
#define UART_MODE_UNSUPPORTED 0xf0

/**
 * @brief   Override parity values
 * @{
 */
#define HAVE_UART_PARITY_T
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD = 1,
    UART_PARITY_EVEN = 2,
    UART_PARITY_MARK = 3,
    UART_PARITY_SPACE = 4,
} uart_parity_t;
#else
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 2,
    UART_PARITY_ODD = 3,
    UART_PARITY_MARK = UART_MODE_UNSUPPORTED | 0,
    UART_PARITY_SPACE = UART_MODE_UNSUPPORTED | 1,
} uart_parity_t;
#endif
/** @} */

/**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
typedef enum {
    UART_DATA_BITS_5 = 5,
    UART_DATA_BITS_6 = 6,
    UART_DATA_BITS_7 = 7,
    UART_DATA_BITS_8 = 8,
} uart_data_bits_t;
#else
typedef enum {
    UART_DATA_BITS_5 = UART_MODE_UNSUPPORTED | 0,
    UART_DATA_BITS_6 = UART_MODE_UNSUPPORTED | 1,
    UART_DATA_BITS_7 = 1,
    UART_DATA_BITS_8 = 2,
} uart_data_bits_t;
#endif
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1)
typedef enum {
   UART_STOP_BITS_1 = 2,
   UART_STOP_BITS_2 = 4,
} uart_stop_bits_t;
#else
typedef enum {
   UART_STOP_BITS_1 = 1,
   UART_STOP_BITS_2 = 3,
} uart_stop_bits_t;
#endif
/** @} */
#endif /* ndef DOXYGEN */

typedef struct {
    void *dev;              /**< UART, USART or LEUART device used */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
#if defined(_SILICON_LABS_32B_SERIES_0) || defined(_SILICON_LABS_32B_SERIES_1) || defined(DOXYGEN)
    uint32_t loc;           /**< location of UART pins */
#endif
    CMU_Clock_TypeDef cmu;  /**< the device CMU channel */
    IRQn_Type irq;          /**< the devices base IRQ channel */
} uart_conf_t;

/**
 * @brief   CPU provides own pm_off() function
 */
#define PROVIDES_PM_OFF

/**
 * @brief   CPU provides own pm_off() function
 */
#define PROVIDES_PM_LAYERED_OFF

/**
 * @brief   Number of usable power modes.
 */
#define PM_NUM_MODES    (3U)

/**
 * @name   Available power modes
 * @{
 */
#define EFM32_PM_MODE_EM3  (0U)  /**< CPU sleeps, peripherals in EM3 domain are active */
#define EFM32_PM_MODE_EM2  (1U)  /**< CPU sleeps, peripherals in EM2 + EM3 domain are active */
#define EFM32_PM_MODE_EM1  (2U)  /**< CPU sleeps, all peripherals are active */
/** @} */

/**
 * @name    Watchdog timer (WDT) configuration
 * @{
 */
#define WDT_CLOCK_HZ            (1000U)

#define NWDT_TIME_LOWER_LIMIT   ((1U << (3U + wdogPeriod_9)) + 1U)
#define NWDT_TIME_UPPER_LIMIT   ((1U << (3U + wdogPeriod_256k)) + 1U)

#if defined(_SILICON_LABS_32B_SERIES_1) || defined(_SILICON_LABS_32B_SERIES_2)
#define WDT_TIME_LOWER_LIMIT    NWDT_TIME_LOWER_LIMIT
#define WDT_TIME_UPPER_LIMIT    NWDT_TIME_UPPER_LIMIT
#endif

#define WDT_HAS_STOP            (1U)
/** @} */

/**
 * @name    USB device definitions
 * @{
 */
#define USBDEV_NUM_ENDPOINTS    7   /**< Number of USB OTG FS endpoints including EP0 */
/** @} */

/* GPIO_LL's overrides */
#ifndef DOXYGEN

/* Not supported by hardware */
#define HAVE_GPIO_SLEW_T
typedef enum {
    GPIO_SLEW_SLOWEST = 0,
    GPIO_SLEW_SLOW = 0,
    GPIO_SLEW_FAST = 0,
    GPIO_SLEW_FASTEST = 0,
} gpio_slew_t;

/* Not supported by hardware */
#define HAVE_GPIO_PULL_STRENGTH_T
typedef enum {
    GPIO_PULL_WEAKEST = 0,
    GPIO_PULL_WEAK = 0,
    GPIO_PULL_STRONG = 0,
    GPIO_PULL_STRONGEST = 0
} gpio_pull_strength_t;

/* Not implemented, see gpio_ll_arch.h comments */
#define HAVE_GPIO_DRIVE_STRENGTH_T
typedef enum {
    GPIO_DRIVE_WEAKEST = 0,
    GPIO_DRIVE_WEAK = 0,
    GPIO_DRIVE_STRONG = 0,
    GPIO_DRIVE_STRONGEST = 0
} gpio_drive_strength_t;

#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
