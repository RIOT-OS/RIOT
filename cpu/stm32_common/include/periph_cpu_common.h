/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32_common
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CPU specific LSI clock speed
 */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
#define CLOCK_LSI           (40000U)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define CLOCK_LSI           (37000U)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L4)
#define CLOCK_LSI           (32000U)
#else
#error "error: LSI clock speed not defined for your target CPU"
#endif

/**
 * @brief   Length of the CPU_ID in octets
 *
 * This is the same for all members of the stm32 family
 */
#define CPUID_LEN           (12U)

/**
 * @brief   We provide our own pm_off() function for all STM32-based CPUs
 */
#define PROVIDES_PM_LAYERED_OFF

/**
 * @brief   All STM timers have 4 capture-compare channels
 */
#define TIMER_CHAN          (4U)

/**
 * @brief   All STM QDEC timers have 2 capture channels
 */
#define QDEC_CHAN           (2U)

/**
 * @brief   Use the shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @name    PM definitions
 * @{
 */
/**
 * @brief   Number of usable low power modes
 */
#define PM_NUM_MODES    (2U)

/**
 * @name    Power modes
 * @{
 */
#define STM32_PM_STOP         (1U)
#define STM32_PM_STANDBY      (0U)
/** @} */

#ifndef PM_EWUP_CONFIG
/**
 * @brief   Wake-up pins configuration (CSR register)
 */
#define PM_EWUP_CONFIG          (0U)
#endif
/** @} */

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
/* Actual Lower Limit is ~100us so round up */
#define NWDT_TIME_LOWER_LIMIT          (1U)
#define NWDT_TIME_UPPER_LIMIT          (4U * US_PER_MS * 4096U * (1 << 6U) \
                                        / CLOCK_LSI)
/* Once enabled wdt can't be stopped */
#define WDT_HAS_STOP                   (0U)
#if defined(CPU_FAM_STM32L4)
#define WDT_HAS_INIT                   (1U)
#else
#define WDT_HAS_INIT                   (0U)
#endif
/** @} */

/**
 * @brief   Available peripheral buses
 */
typedef enum {
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
#if defined(CPU_FAM_STM32L4)
    APB12,          /**< AHB1 bus, second register */
#endif
#if defined(CPU_FAM_STM32L0)
    AHB,            /**< AHB bus */
    IOP,            /**< IOP bus */
#elif defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1) \
    || defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    AHB,            /**< AHB bus */
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) \
    || defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7)
    AHB1,           /**< AHB1 bus */
    AHB2,           /**< AHB2 bus */
    AHB3            /**< AHB3 bus */
#else
#warning "unsupported stm32XX family"
#endif
} bus_t;

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

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
 * @name    Use the shared I2C functions
 * @{
 */
/** Use read reg function from periph common */
#define PERIPH_I2C_NEED_READ_REG
/** Use write reg function from periph common */
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_READ_REGS
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F4)
#define PERIPH_I2C_NEED_WRITE_REGS
#endif
/** @} */

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
#ifdef CPU_FAM_STM32F1
    GPIO_AF_OUT_PP = 0xb,   /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD = 0xf,   /**< alternate function output - open-drain */
#else
    GPIO_AF0 = 0,           /**< use alternate function 0 */
    GPIO_AF1,               /**< use alternate function 1 */
    GPIO_AF2,               /**< use alternate function 2 */
    GPIO_AF3,               /**< use alternate function 3 */
    GPIO_AF4,               /**< use alternate function 4 */
    GPIO_AF5,               /**< use alternate function 5 */
    GPIO_AF6,               /**< use alternate function 6 */
    GPIO_AF7,               /**< use alternate function 7 */
#ifndef CPU_FAM_STM32F0
    GPIO_AF8,               /**< use alternate function 8 */
    GPIO_AF9,               /**< use alternate function 9 */
    GPIO_AF10,              /**< use alternate function 10 */
    GPIO_AF11,              /**< use alternate function 11 */
    GPIO_AF12,              /**< use alternate function 12 */
    GPIO_AF13,              /**< use alternate function 13 */
    GPIO_AF14,              /**< use alternate function 14 */
    GPIO_AF15               /**< use alternate function 15 */
#endif
#endif
} gpio_af_t;

#ifndef CPU_FAM_STM32F1
/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO mode options
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< open-drain w/o pull R */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1)     /**< open-drain with pull-up */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */
#endif /* ndef CPU_FAM_STM32F1 */

#ifdef MODULE_PERIPH_DMA
/**
 * @brief   DMA configuration
 */
typedef struct {
    /** DMA stream on stm32f2/4/7, channel on others
     * STM32F2/4/7:
     *  - 0: DMA1 / Stream0
     *  - 1: DMA1 / Stream1
     *  - ...
     *  - 7: DMA1 / Stream7
     *  - 8: DAM2 / Stream0
     *  - ...
     *  - 15: DMA2 / Stream7
     * STM32F0/1/L0/1/4:
     *  - 0: DMA1 / Channel1
     *  - ...
     *  - 4: DMA1 / Channel5
     *  - ...
     *  - 6: DMA1 / Channel7
     *  - 7: Reserved
     *  - 8: DMA2 / Channel1
     *  - ...
     *  - 12: DMA2 / Channel5
     *  - ...
     *  - 14: DMA2 / Channel7
     */
    int stream;
} dma_conf_t;

/**
 * @brief   DMA type
 */
typedef unsigned dma_t;

/**
 * @brief   DMA modes
 */
typedef enum {
    DMA_PERIPH_TO_MEM,     /**< Peripheral to memory */
    DMA_MEM_TO_PERIPH,     /**< Memory to peripheral */
    DMA_MEM_TO_MEM,        /**< Memory to memory */
} dma_mode_t;

/**
 * @name    DMA Increment modes
 * @{
 */
#define DMA_INC_SRC_ADDR  (0x01)
#define DMA_INC_DST_ADDR  (0x02)
#define DMA_INC_BOTH_ADDR (DMA_INC_SRC_ADDR | DMA_INC_DST_ADDR)
/** @} */

/**
 * @name    DMA data width
 * @{
 */
#define DMA_DATA_WIDTH_BYTE      (0x00)
#define DMA_DATA_WIDTH_HALF_WORD (0x04)
#define DMA_DATA_WIDTH_WORD      (0x08)
#define DMA_DATA_WIDTH_MASK      (0x0C)
#define DMA_DATA_WIDTH_SHIFT     (2)
/** @} */
#endif /* MODULE_PERIPH_DMA */

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIM_TypeDef *dev;       /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint32_t rcc_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
} timer_conf_t;

/**
 * @brief   PWM channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    pwm_chan_t chan[TIMER_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0}
                                     *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
} pwm_conf_t;

/**
 * @brief   QDEC channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} qdec_chan_t;

/**
 * @brief   QDEC configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t max;                   /**< Maximum counter value */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    qdec_chan_t chan[QDEC_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0}
                                     *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
    uint8_t irqn;                   /**< global IRQ channel */
} qdec_conf_t;

/**
 * @brief UART hardware module types
 */
typedef enum {
    STM32_USART,            /**< STM32 USART module type */
    STM32_LPUART,           /**< STM32 Low-power UART (LPUART) module type */
} uart_type_t;

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
   UART_PARITY_NONE = 0,                               /**< no parity */
   UART_PARITY_EVEN = USART_CR1_PCE,                   /**< even parity */
   UART_PARITY_ODD = (USART_CR1_PCE | USART_CR1_PS),   /**< odd parity */
   UART_PARITY_MARK = UART_INVALID_MODE | 4,           /**< not supported */
   UART_PARITY_SPACE = UART_INVALID_MODE  | 5          /**< not supported */
} uart_parity_t;
/** @} */

/**
 * @brief   Override data bits length values
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = UART_INVALID_MODE | 1,   /**< not supported */
    UART_DATA_BITS_6 = UART_INVALID_MODE | 2,   /**< not supported unless parity is set */
#if defined(USART_CR1_M1)
    UART_DATA_BITS_7 = USART_CR1_M1,            /**< 7 data bits */
#else
    UART_DATA_BITS_7 = UART_INVALID_MODE | 3,   /**< not supported unless parity is set */
#endif
    UART_DATA_BITS_8 = 0,                       /**< 8 data bits */
} uart_data_bits_t;
/** @} */

/**
 * @brief   Override stop bits length values
 * @{
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1 = 0,                  /**< 1 stop bit */
   UART_STOP_BITS_2 = USART_CR2_STOP_1,   /**< 2 stop bits */
} uart_stop_bits_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_TypeDef *dev;     /**< UART device base register address */
    uint32_t rcc_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t rx_af;        /**< alternate function for RX pin */
    gpio_af_t tx_af;        /**< alternate function for TX pin */
#endif
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
#ifdef MODULE_STM32_PERIPH_UART_HW_FC
    gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    gpio_t rts_pin;         /**< RTS pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t cts_af;       /**< alternate function for CTS pin */
    gpio_af_t rts_af;       /**< alternate function for RTS pin */
#endif
#endif
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L4)
    uart_type_t type;       /**< hardware module type (USART or LPUART) */
    uint32_t clk_src;       /**< clock source used for UART */
#endif
#ifdef MODULE_PERIPH_DMA
    dma_t dma;              /**< Logical DMA stream used for TX */
    uint8_t dma_chan;       /**< DMA channel used for TX */
#endif
} uart_conf_t;

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    SPI_TypeDef *dev;       /**< SPI device base register address */
    gpio_t mosi_pin;        /**< MOSI pin */
    gpio_t miso_pin;        /**< MISO pin */
    gpio_t sclk_pin;        /**< SCLK pin */
    gpio_t cs_pin;          /**< HWCS pin, set to GPIO_UNDEF if not mapped */
#ifndef CPU_FAM_STM32F1
    gpio_af_t mosi_af;      /**< MOSI pin alternate function */
    gpio_af_t miso_af;      /**< MISO pin alternate function */
    gpio_af_t sclk_af;      /**< SCLK pin alternate function */
    gpio_af_t cs_af;        /**< HWCS pin alternate function */
#endif
    uint32_t rccmask;       /**< bit in the RCC peripheral enable register */
    uint8_t apbbus;         /**< APBx bus the device is connected to */
#ifdef MODULE_PERIPH_DMA
    dma_t tx_dma;           /**< Logical DMA stream used for TX */
    uint8_t tx_dma_chan;    /**< DMA channel used for TX */
    dma_t rx_dma;           /**< Logical DMA stream used for RX */
    uint8_t rx_dma_chan;    /**< DMA channel used for RX */
#endif
} spi_conf_t;

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1)
    I2C_SPEED_LOW,          /**< low speed mode: ~10kit/s */
#endif
    I2C_SPEED_NORMAL,       /**< normal mode:  ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:    ~400kbit/s */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4)
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode: ~1Mbit/s */
#endif
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for I2C configuration data
 */
typedef struct {
    I2C_TypeDef *dev;       /**< i2c device */
    i2c_speed_t speed;      /**< i2c bus speed */
    gpio_t scl_pin;         /**< scl pin number */
    gpio_t sda_pin;         /**< sda pin number */
#ifndef CPU_FAM_STM32F1
    gpio_af_t scl_af;       /**< scl pin alternate function value */
    gpio_af_t sda_af;       /**< sda pin alternate function value */
#endif
    uint8_t bus;            /**< APB bus */
    uint32_t rcc_mask;      /**< bit in clock enable register */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    uint32_t rcc_sw_mask;   /**< bit to switch I2C clock */
#endif
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1)
    uint32_t clk;           /**< bus frequency as defined in board config */
#endif
    uint8_t irqn;           /**< I2C event interrupt number */
} i2c_conf_t;

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4)
/**
 * @brief   Structure for I2C timing register settings
 *
 * These parameters are valid for 48MHz (16MHz for L0) input clock.
 * See reference manual of supported CPU for example of timing settings:
 * - STM32F030/F070: see RM0360, section 22.4.10, p.560, table 76
 * - STM32F303: see RM0316, section 28.4.9, p.849, table 148
 * - STM32F72X: see RM0431, section 26.4.9, p.851, table 149
 * - STM32L0x2: see RM0376, section 27.4.10, p.686, table 117
 * - STM32L4X5/6: see RM0351, section 39.4.9, p.1297, table 234
 */
typedef struct {
    uint8_t presc;          /**< Timing prescaler value */
    uint8_t scll;           /**< SCL Low period */
    uint8_t sclh;           /**< SCL High period */
    uint8_t sdadel;         /**< Data hold time */
    uint8_t scldel;         /**< Data setup time */
} i2c_timing_param_t;
#endif

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

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

#ifdef MODULE_PERIPH_DMA
/**
 * @brief   DMA stream not defined
 */
#define DMA_STREAM_UNDEF (UINT_MAX)

/**
 * @brief   Initialize DMA
 */
void dma_init(void);

/**
 * @brief   Execute a DMA transfer
 *
 * This function blocks until the transfer is completed. This is a convenience
 * function which configure, start, wait and stop a DMA transfer.
 *
 * @param[in]  dma     logical DMA stream
 * @param[in]  chan    DMA channel (on stm32f2/4/7, CxS or unused on others)
 * @param[in]  src     source buffer
 * @param[out] dst     destination buffer
 * @param[in]  len     length to transfer
 * @param[in]  mode    DMA mode
 * @param[in]  flags   DMA configuration
 *
 * @return < 0 on error, the number of transferred bytes otherwise
 */
int dma_transfer(dma_t dma, int chan, const volatile void *src, volatile void *dst, size_t len,
                 dma_mode_t mode, uint8_t flags);

/**
 * @brief   Acquire a DMA stream
 *
 * @param[in] dma     logical DMA stream
 */
void dma_acquire(dma_t dma);

/**
 * @brief   Release a DMA stream
 *
 * @param[in] dma     logical DMA stream
 */
void dma_release(dma_t dma);

/**
 * @brief   Start a DMA transfer on a stream
 *
 * Start a DMA transfer on a given stream. The stream must be configured first
 * by a @p dma_configure call.
 *
 * @param[in] dma     logical DMA stream
 */
void dma_start(dma_t dma);

/**
 * @brief   Suspend a DMA transfer on a stream
 *
 * @param[in] dma     logical DMA stream
 *
 * @return the remaining number of bytes to transfer
 */
uint16_t dma_suspend(dma_t dma);

/**
 * @brief   Resume a suspended DMA transfer on a stream
 *
 * @param[in] dma         logical DMA stream
 * @param[in] reamaining  the remaining number of bytes to transfer
 */
void dma_resume(dma_t dma, uint16_t remaining);

/**
 * @brief   Stop a DMA transfer on a stream
 *
 * @param[in] dma     logical DMA stream
 */
void dma_stop(dma_t dma);

/**
 * @brief   Wait for the end of a transfer
 *
 * @param[in] dma     logical DMA stream
 */
void dma_wait(dma_t dma);

/**
 * @brief   Configure a DMA stream for a new transfer
 *
 * @param[in]  dma     logical DMA stream
 * @param[in]  chan    DMA channel (on stm32f2/4/7, CxS or unused on others)
 * @param[in]  src     source buffer
 * @param[out] dst     destination buffer
 * @param[in]  len     length to transfer
 * @param[in]  mode    DMA mode
 * @param[in]  flags   DMA configuration
 *
 * @return < 0 on error, 0 on success
 */
int dma_configure(dma_t dma, int chan, const volatile void *src, volatile void *dst, size_t len,
                  dma_mode_t mode, uint8_t flags);

#endif /* MODULE_PERIPH_DMA */

#ifdef MODULE_PERIPH_CAN
#include "candev_stm32.h"
#endif

/**
 * @brief STM32 Ethernet configuration mode
 */
typedef enum {
    MII  = 18,                       /**< Configuration for MII */
    RMII = 9,                       /**< Configuration for RMII */
    SMI  = 2,                        /**< Configuration for SMI */
} eth_mode_t;

/**
 * @brief STM32 Ethernet speed options
 */
typedef enum {
    ETH_SPEED_10T_HD   = 0x0000,
    ETH_SPEED_10T_FD   = 0x0100,
    ETH_SPEED_100TX_HD = 0x2000,
    ETH_SPEED_100TX_FD = 0x2100,
} eth_speed_t;

/**
 * @brief   Ethernet Peripheral configuration
 */
typedef struct {
    eth_mode_t mode;      /**< Select configuration mode */
    char mac[6];                /**< Ethernet MAC address */
    eth_speed_t speed;    /**< Speed selection */
    uint8_t dma;                /**< Locical CMA Descriptor used for TX */
    uint8_t dma_chan;           /**< DMA channel used for TX */
    char phy_addr;              /**< PHY address */
    gpio_t pins[];              /**< Pins to use. MII requires 18 pins,
                                        RMII 9 and SMI 9. Not all speeds are
                                        supported by all modes. */
} eth_conf_t;

/**
* @name Ethernet PHY Common Registers
* @{
*/
#define PHY_BMCR                           (0x00)
#define PHY_BSMR                           (0x01)
#define PHY_PHYIDR1                        (0x02)
#define PHY_PHYIDR2                        (0x03)
#define PHY_ANAR                           (0x04)
#define PHY_ANLPAR                         (0x05)
#define PHY_ANER                           (0x06)
#define PHY_ANNPTR                         (0x07)
/** @} */

/**
* @name Ethernet PHY BMCR Fields
* @{
*/
#define BMCR_RESET                         (0x8000)
#define BMCR_LOOPBACK                      (0x4000)
#define BMCR_SPEED_SELECT                  (0x2000)
#define BMCR_AN                            (0x1000)
#define BMCR_POWER_DOWN                    (0x0800)
#define BMCR_ISOLATE                       (0x0400)
#define BMCR_RESTART_AN                    (0x0200)
#define BMCR_DUPLEX_MODE                   (0x0100)
#define BMCR_COLLISION_TEST                (0x0080)
/** @} */

/**
* @name Ethernet PHY BSMR Fields
* @{
*/
#define BSMR_100BASE_T4                    (0x8000)
#define BSMR_100BASE_TX_FDUPLEX            (0x4000)
#define BSMR_100BASE_TX_HDUPLEX            (0x2000)
#define BSMR_10BASE_T_FDUPLEX              (0x1000)
#define BSMR_10BASE_T_HDUPLEX              (0x0800)
#define BSMR_NO_PREAMBLE                   (0x0040)
#define BSMR_AN_COMPLETE                   (0x0020)
#define BSMR_REMOTE_FAULT                  (0x0010)
#define BSMR_AN_ABILITY                    (0x0008)
#define BSMR_LINK_STATUS                   (0x0004)
#define BSMR_JABBER_DETECT                 (0x0002)
#define BSMR_EXTENDED_CAP                  (0x0001)
/** @} */

/**
* @name Ethernet PHY PHYIDR1 Fields
*/
#define PHYIDR1_OUI                        (0xffff)

/**
* @name Ethernet PHY PHYIDR2 Fields
* @{
*/
#define PHYIDR2_OUI                        (0xfe00)
#define PHYIDR2_MODEL                      (0x01f0)
#define PHYIDR2_REV                        (0x0007)
/** @} */

/**
* @name Ethernet PHY ANAR Fields
* @{
*/
#define ANAR_NEXT_PAGE                     (0x8000)
#define ANAR_REMOTE_FAULT                  (0x2000)
#define ANAR_PAUSE                         (0x0600)
#define ANAR_100BASE_T4                    (0x0200)
#define ANAR_100BASE_TX_FDUPLEX            (0x0100)
#define ANAR_100BASE_TX_HDUPLEX            (0x0080)
#define ANAR_10BASE_T_FDUPLEX              (0x0040)
#define ANAR_10BASE_T_HDUPLEX              (0x0020)
#define ANAR_SELECTOR                      (0x000f)
/** @} */

/**
* @name Ethernet PHY ANLPAR Fields
* @{
*/
#define ANLPAR_NEXT_PAGE                   (0x8000)
#define ANLPAR_ACK                         (0x4000)
#define ANLPAR_REMOTE_FAULT                (0x2000)
#define ANLPAR_PAUSE                       (0x0600)
#define ANLPAR_100BASE_T4                  (0x0200)
#define ANLPAR_100BASE_TX_FDUPLEX          (0x0100)
#define ANLPAR_100BASE_TX_HDUPLEX          (0x0080)
#define ANLPAR_10BASE_T_FDUPLEX            (0x0040)
#define ANLPAR_10BASE_T_HDUPLEX            (0x0020)
#define ANLPAR_SELECTOR                    (0x000f)
/** @} */

/**
* @name Ethernet PHY ANNPTR Fields
* @{
*/
#define ANNPTR_NEXT_PAGE                   (0x8000)
#define ANNPTR_MSG_PAGE                    (0x2000)
#define ANNPTR_ACK2                        (0x1000)
#define ANNPTR_TOGGLE_TX                   (0x0800)
#define ANNPTR_CODE                        (0x03ff)
/** @} */

/**
* @name Ethernet PHY ANER Fields
* @{
*/
#define ANER_PDF                           (0x0010)
#define ANER_LP_NEXT_PAGE_ABLE             (0x0008)
#define ANER_NEXT_PAGE_ABLE                (0x0004)
#define ANER_PAGE_RX                       (0x0002)
#define ANER_LP_AN_ABLE                    (0x0001)
/** @} */

#ifdef MODULE_STM32_ETH
/**
 * @brief Read a PHY register
 *
 * @param[in] addr      address of the PHY to read
 * @param[in] reg       register to be read
 *
 * @return value in the register, or <=0 on error
 */
int32_t stm32_eth_phy_read(uint16_t addr, uint8_t reg);

/**
 * @brief Write a PHY register
 *
 * @param[in] addr      address of the PHY to write
 * @param[in] reg       register to be written
 * @param[in] value     value to write into the register
 *
 * @return 0 in case of success or <=0 on error
 */
int32_t stm32_eth_phy_write(uint16_t addr, uint8_t reg, uint16_t value);
#endif /* MODULE_STM32_ETH */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
