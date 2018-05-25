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
#elif defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1)
#define CLOCK_LSI           (37000U)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32L4)
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
 * @brief   Number of usable low power modes
 */
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L0) || defined(DOXYGEN)
#define PM_NUM_MODES    (2U)

/**
 * @name    Power modes
 * @{
 */
#define STM32_PM_STOP         (1U)
#define STM32_PM_STANDBY      (0U)
/** @} */
#endif

/**
 * @brief   Available peripheral buses
 */
typedef enum {
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
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
    int stream;            /**< DMA stream */
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
#ifdef MODULE_PERIPH_DMA
    dma_t dma;              /**< Logical DMA stream used for TX */
    uint8_t dma_chan;       /**< DMA channel used for TX */
#endif
#ifdef MODULE_STM32_PERIPH_UART_HW_FC
    gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    gpio_t rts_pin;         /**< RTS pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t cts_af;       /**< alternate function for CTS pin */
    gpio_af_t rts_af;       /**< alternate function for RTS pin */
#endif
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
    gpio_af_t af;           /**< pin alternate function */
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

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4)

/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_NORMAL,       /**< normal mode:       ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:         ~400kbit/s */
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode:    ~1Mbit/s */
} i2c_speed_t;
/** @} */

/**
 * @brief   Structure for I2C configuration data
 */
typedef struct {
    I2C_TypeDef *dev;       /**< i2c device */
    i2c_speed_t speed;      /**< i2c bus speed */
    gpio_t scl_pin;         /**< scl pin number */
    gpio_t sda_pin;         /**< sda pin number */
    gpio_af_t scl_af;       /**< scl pin alternate function value */
    gpio_af_t sda_af;       /**< sda pin alternate function value */
    uint8_t bus;            /**< APB bus */
    uint32_t rcc_mask;      /**< bit in clock enable register */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    uint32_t rcc_sw_mask;   /**< bit to switch I2C clock */
#endif
    uint8_t irqn;           /**< I2C event interrupt number */
} i2c_conf_t;

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
 * @param[in]  chan    DMA channel
 * @param[in]  src     source buffer
 * @param[out] dst     destination buffer
 * @param[in]  len     length to transfer
 * @param[in]  mode    DMA mode
 * @param[in]  flags   DMA configuration
 *
 * @return < 0 on error, the number of transfered bytes otherwise
 */
int dma_transfer(dma_t dma, int chan, const void *src, void *dst, size_t len,
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
 * @param[in]  chan    DMA channel
 * @param[in]  src     source buffer
 * @param[out] dst     destination buffer
 * @param[in]  len     length to transfer
 * @param[in]  mode    DMA mode
 * @param[in]  flags   DMA configuration
 *
 * @return < 0 on error, 0 on success
 */
int dma_configure(dma_t dma, int chan, const void *src, void *dst, size_t len,
                  dma_mode_t mode, uint8_t flags);

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA stream handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 stream0 equals 0, DMA2 stream0 equals 8,
 * DMA2 stream 7 equals 15 and so on.
 *
 * @param[in] stream    physical DMA stream
 */
static inline DMA_TypeDef *dma_base(int stream)
{
    return (stream < 8) ? DMA1 : DMA2;
}

/**
 * @brief   Power on the DMA device the given stream belongs to
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_poweron(int stream)
{
    if (stream < 8) {
        periph_clk_en(AHB1, RCC_AHB1ENR_DMA1EN);
    }
    else {
        periph_clk_en(AHB1, RCC_AHB1ENR_DMA2EN);
    }
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] stream    physical DMA stream
 *
 * @return  base address for the selected DMA stream
 */
static inline DMA_Stream_TypeDef *dma_stream(int stream)
{
    uint32_t base = (uint32_t)dma_base(stream);

    return (DMA_Stream_TypeDef *)(base + (0x10 + (0x18 * (stream & 0x7))));
}

/**
 * @brief   Select high or low DMA interrupt register based on stream number
 *
 * @param[in] stream    physical DMA stream
 *
 * @return  0 for streams 0-3, 1 for streams 3-7
 */
static inline int dma_hl(int stream)
{
    return ((stream & 0x4) >> 2);
}

/**
 * @brief   Get the interrupt flag clear bit position in the DMA LIFCR register
 *
 * @param[in] stream    physical DMA stream
 */
static inline uint32_t dma_ifc(int stream)
{
    switch (stream & 0x3) {
        case 0:
            return (1 << 5);
        case 1:
            return (1 << 11);
        case 2:
            return (1 << 21);
        case 3:
            return (1 << 27);
        default:
            return 0;
    }
}

/**
 * @brief   Enable the interrupt of a given stream
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_isr_enable(int stream)
{
    if (stream < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 7) {
        NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    }
    else if (stream < 13) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream - 8)));
    }
    else if (stream < 16) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream - 13)));
    }
}

/**
 * @brief   Disable the interrupt of a given stream
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_isr_disable(int stream)
{
    if (stream < 7) {
        NVIC_DisableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 7) {
        NVIC_DisableIRQ(DMA1_Stream7_IRQn);
    }
    else if (stream < 13) {
        NVIC_DisableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream - 8)));
    }
    else if (stream < 16) {
        NVIC_DisableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream - 13)));
    }
}

/**
 * @brief   Clear the interrupt of a given stream
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_isr_clear(int stream)
{
    if (stream < 7) {
        NVIC_ClearPendingIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 7) {
        NVIC_ClearPendingIRQ((IRQn_Type)DMA1_Stream7_IRQn);
    }
    else if (stream < 13) {
        NVIC_ClearPendingIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream - 8)));
    }
    else if (stream < 16) {
        NVIC_ClearPendingIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream - 13)));
    }
}
#endif /* MODULE_PERIPH_DMA */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
