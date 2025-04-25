/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       Shared CPU specific definitions for the GD32E23x family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

// #include <limits.h>
#include <inttypes.h>

#include "cpu.h"
#include "macros/units.h"
// #include "gd32e23x.h"
// #include "gd32e23x_gpio.h"

// #if defined(CPU_FAM_GD32E23X)
// #include "vendor/GD32E23x_Firmware_Library/GD32E23x_standard_peripheral/Include/gd32e23x_gpio.h"
// #include "vendor/GD32E23x_Firmware_Library/GD32E23x_standard_peripheral/Include/gd32e23x_usart.h"
// #endif

// #include "periph/cpu_gpio.h"
// #include "periph/cpu_timer.h"

#include "macros/units.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      (GPIOA_BASE | (x << 10) | y)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))

/**
 * @brief   Override GPIO mode options
 *
 * We use 4 bit to encode CNF and MODE.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
                        //CTL, PUD, OM
    GPIO_IN     = GPIO_MODE(0, 0, 0),   /**< input w/o pull R */
    GPIO_IN_PD  = GPIO_MODE(0, 2, 0),   /**< input with pull-down */
    GPIO_IN_PU  = GPIO_MODE(0, 1, 0),   /**< input with pull-up */
    GPIO_OUT    = GPIO_MODE(1, 0, 0),   /**< push-pull output */
    GPIO_OD     = GPIO_MODE(1, 0, 1),   /**< open-drain w/o pull R */
    GPIO_OD_PU  = GPIO_MODE(1, 1, 1)    /**< not supported by HW */
} gpio_mode_t;
/** @} */

/**
 * @brief   Available peripheral buses
 */
typedef enum {
  //check order here, VF103 has AHB, APB1, APB2
  //no definition of AHB 1 vs 2 (AHB2 is only used for GPIO and isn't adjustable in any way?)
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
    AHB,            /**< AHB bus */
} bus_t;

/**
 * @brief   Available GPIO ports
 */
enum {
#ifdef GPIOA
    PORT_A  = 0,            /**< port A */
#endif
#ifdef GPIOB
    PORT_B  = 1,            /**< port B */
#endif
#ifdef GPIOC
    PORT_C  = 2,            /**< port C */
#endif
#ifdef GPIOD
    PORT_D  = 3,            /**< port D */
#endif
#ifdef GPIOE
    PORT_E  = 4,            /**< port E */
#endif
#ifdef GPIOF
    PORT_F  = 5,            /**< port F */
#endif
};

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Override alternative GPIO mode options
 */
//TODO need to remap these.  the values from GD32VF103 AF PP/OD are not the same as for E23 AF OD/PP and they are in different registers
typedef enum {
    GPIO_AF_OUT_PP  = 0xb,  /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD  = 0xf,  /**< alternate function output - open-drain */
} gpio_af_t;


/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @name    PTP clock configuration
 * @{
 */
#define HAVE_PTP_CLOCK_READ         1   /**< Native implementation available */
#define HAVE_PTP_CLOCK_SET          1   /**< Native implementation available */
#define HAVE_PTP_TIMER_SET_ABSOLUTE 1   /**< Native implementation available */
/** @} */

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT           (1)
/* Ensure the internal "count" variable stays within the uint32 bounds.
   This variable corresponds to max_time * RTC_FREQ / MS_PER_SEC. On fe310,
   RTC_FREQ is 32768Hz. The 15 right shift is equivalent to a division by RTC_FREQ.
 */
#define NWDT_TIME_UPPER_LIMIT           ((UINT32_MAX >> 15)  * MS_PER_SEC + 1)
/** @} */

/**
 * @brief   UART interrupt priority
 */
#define UART_ISR_PRIO       (2)

void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

/**
 * @brief   All STM timers have 4 capture-compare channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

/**
 * @brief   Macro for accessing the capture/compare register of a timer channel
 */
#define TIMER_CHANNEL(tim, chan) *(&dev(tim)->CH0CV + (chan * 2))

/**
 * @brief   PWM channel
 *
 * When using a general-purpose timer for a PWM device, the outputs OC<n> of
 * each of the four capture/compare channels can be used as PWM channel.
 * The respective capture/compare channel is then specified with 0...3 in
 * `cc_chan` for the outputs OC1...OC4.
 *
 * Advanced timers like TIM1 and TIM8 have additionally three complementary
 * outputs OC<n>N of the capture/compare channels, which can also be used
 * as PWM channels. These complementary outputs are defined with an offset
 * of 4, i.e. they are specified in `cc_chan` with 4...6 for OC1N...OC3N.
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< Capture/compare channel used: 0..3 for OC1..OC4
                                 or 4..6 for OC1N..OC3N for advanced timers */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct {
    TIMER_Type *dev;                /**< Timer used */
    uint32_t rcu_mask;              /**< bit in clock enable register */
    pwm_chan_t chan[TIMER_CHANNEL_NUMOF]; /**< channel mapping
                                           *   set to {GPIO_UNDEF, 0}
                                           *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
} pwm_conf_t;



/**
 * @name    PM definitions
 * @{
 */
/**
 * @brief   Number of usable low power modes
 */
#define PM_NUM_MODES            (3U)

/**
 * @name    Power modes
 * @{
 */
enum {
    //SLEEP == IDLE?
    GD32_PM_STANDBY = 0,       /**< STANDBY mode,  */
    GD32_PM_DEEPSLEEP = 1,     /**< DEEPSLEEP mode, corresponds to STOP mode of STM32 */
    GD32_PM_IDLE = 2           /**< IDLE mode */
};
/** @} */

#ifndef PM_EWUP_CONFIG
/**
 * @brief   Wake-up pins configuration (CSR register)
 */
#define CONFIG_PM_EWUP_USED    (0U)
#endif
/** @} */



/**
 * @name    Use the shared I2C functions
 * @{
 */
/** Use read reg function from periph common */
#define PERIPH_I2C_NEED_READ_REG
/** Use write reg function from periph common */
#define PERIPH_I2C_NEED_WRITE_REG
/** Use read regs function from periph common */
#define PERIPH_I2C_NEED_READ_REGS
/** Use write regs function from periph common */
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    // I2C_SPEED_LOW       = KHZ(10),      /**< low speed mode: ~10kit/s */
    I2C_SPEED_NORMAL    = KHZ(100),     /**< normal mode:    ~100kbit/s */
    I2C_SPEED_FAST      = KHZ(400),     /**< fast mode:      ~400kbit/s */
    I2C_SPEED_FAST_PLUS = MHZ(1),       /**< fast plus mode: ~1Mbit/s */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for I2C configuration data
 */
typedef struct {
    I2C_Type *dev;       /**< i2c device */
    i2c_speed_t speed;      /**< i2c bus speed */
    gpio_t scl_pin;         /**< scl pin number */
    gpio_t sda_pin;         /**< sda pin number */
    gpio_af_t scl_af;       /**< scl pin alternate function value */
    gpio_af_t sda_af;       /**< sda pin alternate function value */
    uint8_t bus;            /**< APB bus */
    uint32_t rcu_mask;      /**< bit in clock enable register */
    // uint32_t rcu_sw_mask;   /**< bit to switch I2C clock */
    IRQn_Type irqn;           /**< I2C event interrupt number */
    uint32_t timing;        /**< I2C Timing Params for TIMINGR register */
} i2c_conf_t;

/**
 * @brief   Structure for I2C timing register settings
 */
typedef struct {
    uint8_t presc;          /**< Timing prescaler value */
    uint8_t scll;           /**< SCL Low period */
    uint8_t sclh;           /**< SCL High period */
    uint8_t sdadel;         /**< Data hold time */
    uint8_t scldel;         /**< Data setup time */
} i2c_timing_param_t;



/**
 * @brief   Define a magic number that tells us to use hardware chip select
 *
 * We use a random value here, that does clearly differentiate from any possible
 * GPIO_PIN(x) value.
 */
#define SPI_HWCS_MASK       (0xffffff00)

/**
 * @brief   Override the default SPI hardware chip select access macro
 *
 * Since the CPU does only support one single hardware chip select line, we can
 * detect the usage of non-valid lines by comparing to SPI_HWCS_VALID.
 */
#define SPI_HWCS(x)         (SPI_HWCS_MASK | x)

/**
 * @brief   Define value for unused CS line
 */
#define SPI_CS_UNDEF        (GPIO_UNDEF)

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default spi_cs_t type definition
 * @{
 */
#define HAVE_SPI_CS_T
typedef uint32_t spi_cs_t;
/** @} */
#endif

/**
 * @brief   Use the shared SPI functions
 * @{
 */
/** Use transfer byte function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
/** Use transfer reg function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
/** Use transfer regs function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */


/**
 * @brief   SPI clock type
 */
typedef uint32_t spi_clk_t;
/** @} */

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    SPI_Type *dev;          /**< SPI device base register address */
    gpio_t mosi_pin;        /**< MOSI pin */
    gpio_t miso_pin;        /**< MISO pin */
    gpio_t sclk_pin;        /**< SCLK pin */
    spi_cs_t cs_pin;        /**< HWCS pin, set to SPI_CS_UNDEF if not mapped */
    gpio_af_t mosi_af;      /**< MOSI pin alternate function */
    gpio_af_t miso_af;      /**< MISO pin alternate function */
    gpio_af_t sclk_af;      /**< SCLK pin alternate function */
    gpio_af_t cs_af;        /**< HWCS pin alternate function */
    uint32_t rcumask;       /**< bit in the RCC peripheral enable register */
    uint8_t apbbus;         /**< APBx bus the device is connected to */
#ifdef MODULE_PERIPH_DMA
    dma_t tx_dma;           /**< Logical DMA stream used for TX */
    uint8_t tx_dma_chan;    /**< DMA channel used for TX */
    dma_t rx_dma;           /**< Logical DMA stream used for RX */
    uint8_t rx_dma_chan;    /**< DMA channel used for RX */
#endif
} spi_conf_t;


/**
 * @brief   Define a magic number that tells us to use hardware chip select
 *
 * We use a random value here, that does clearly differentiate from any possible
 * GPIO_PIN(x) value.
 */
#define SPI_HWCS_MASK       (0xffffff00)

/**
 * @brief   Override the default SPI hardware chip select access macro
 *
 * Since the CPU does only support one single hardware chip select line, we can
 * detect the usage of non-valid lines by comparing to SPI_HWCS_VALID.
 */
#define SPI_HWCS(x)         (SPI_HWCS_MASK | x)

/**
 * @brief   Define value for unused CS line
 */
#define SPI_CS_UNDEF        (GPIO_UNDEF)

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default spi_cs_t type definition
 * @{
 */
#define HAVE_SPI_CS_T
typedef uint32_t spi_cs_t;
/** @} */
#endif

/**
 * @brief   Use the shared SPI functions
 * @{
 */
/** Use transfer byte function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
/** Use transfer reg function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
/** Use transfer regs function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
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
 * @brief   Define a macro for accessing a timer channel
 */
// #define TIM_CHAN(tim, chan) *(&dev(tim)->CCR1 + chan)
#define TIM_CHAN(tim, chan) *(&dev(tim)->CH0CV + chan)

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIMER_Type *dev;       /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint32_t rcu_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
} timer_conf_t;


/**
 * @brief UART hardware module types
 */
typedef enum {
    GD32_USART,
} uart_type_t;

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE    (64)
#endif

#ifndef DOXYGEN
/**
 * @brief   Invalid UART mode mask
 *
 * This mask is also used to force data_bits_t to be uint32_t type
 * since it may be assigned a uint32_t variable in uart_mode
 */
#define UART_INVALID_MODE   (0x8000000)

/**
 * @brief   Override parity values
 * @{
 */
#define HAVE_UART_PARITY_T
typedef enum {
   UART_PARITY_NONE, //= USART_PM_NONE,                 /**< no parity */
   UART_PARITY_EVEN, //= USART_PM_EVEN,                 /**< even parity */
   UART_PARITY_ODD, //= USART_PM_ODD,                   /**< odd parity */
   UART_PARITY_MARK,// = UART_INVALID_MODE,             /**< not supported */
   UART_PARITY_SPACE,// = UART_INVALID_MODE             /**< not supported */
} uart_parity_t;
/** @} */

/**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5, //= UART_INVALID_MODE | 1,   /**< not supported */
    UART_DATA_BITS_6 ,//= UART_INVALID_MODE | 2,   /**< not supported unless parity is set */
    UART_DATA_BITS_7,// = UART_INVALID_MODE | 3,   /**< not supported unless parity is set */
    UART_DATA_BITS_8,// = 0,                       /**< 8 data bits */
} uart_data_bits_t;
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,                  /**< 1 stop bit */
   UART_STOP_BITS_2// = USART_CR2_STOP_1,   /**< 2 stop bits */
} uart_stop_bits_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_Type *dev;     /**< UART device base register address */
    uint32_t rcu_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_af_t rx_af;        /**< alternate function for RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_af_t tx_af;        /**< alternate function for TX pin */
    // gpio_t de_pin;          /**< DE pin */
    // gpio_af_t de_af;        /**< alternate function for DE pin */
    // gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    // gpio_af_t cts_af;       /**< alternate function for CTS pin */
    // gpio_t rts_pin;         /**< RTS pin */
    // gpio_af_t rts_af;       /**< alternate function for RTS pin */
    // gpio_t ck_pin;          /**< CK pin */
    // gpio_af_t ck_af;        /**< alternate function for CK pin */
//     uart_type_t type;       /**< hardware module type (USART or LPUART) */
//     uint32_t clk_src;       /**< clock source used for UART */
//     dma_t dma;              /**< Logical DMA stream used for TX */
//     uint8_t dma_chan;       /**< DMA channel used for TX */
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
} uart_conf_t;


#ifdef Doxygen
/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define GD32_BOOTLOADER_ADDR
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (12U)

/**
 * @brief   Starting address of the CPU ID
 */
#ifndef CPUID_ADDR
#define CPUID_ADDR          (UID_BASE)
#endif

/**
 * @brief   CPU specific LSI clock speed
 */
// #define CLOCK_LSI           (40000U)

/**
 * @brief   Get the actual bus clock frequency for the APB buses
 *
 * @param[in] bus       target APBx bus
 *
 * @return              bus clock frequency in Hz
 */
uint32_t periph_apb_clk(uint8_t bus);

/**
 * @brief   Get the actual timer clock frequency
 *
 * @param[in] bus       corresponding APBx bus
 *
 * @return              timer clock frequency in Hz
 */
uint32_t periph_timer_clk(uint8_t bus);

/**
 * @brief   Enable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_lpclk_dis(bus_t bus, uint32_t mask);

/**
 * @brief   Enable the given peripheral clock in low power mode
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_lpclk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock in low power mode
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_dis(bus_t bus, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
