/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <limits.h>

#include "cpu.h"
#include "macros/units.h"

#if defined(CPU_FAM_STM32F0)
#include "periph/f0/periph_cpu.h"
#elif defined(CPU_FAM_STM32F1)
#include "periph/f1/periph_cpu.h"
#elif defined(CPU_FAM_STM32F2)
#include "periph/f2/periph_cpu.h"
#elif defined(CPU_FAM_STM32F3)
#include "periph/f3/periph_cpu.h"
#elif defined(CPU_FAM_STM32F4)
#include "periph/f4/periph_cpu.h"
#elif defined(CPU_FAM_STM32F7)
#include "periph/f7/periph_cpu.h"
#elif defined(CPU_FAM_STM32G0)
#include "periph/g0/periph_cpu.h"
#elif defined(CPU_FAM_STM32G4)
#include "periph/g4/periph_cpu.h"
#elif defined(CPU_FAM_STM32L0)
#include "periph/l0/periph_cpu.h"
#elif defined(CPU_FAM_STM32L1)
#include "periph/l1/periph_cpu.h"
#elif defined(CPU_FAM_STM32L4)
#include "periph/l4/periph_cpu.h"
#elif defined(CPU_FAM_STM32L5)
#include "periph/l5/periph_cpu.h"
#elif defined(CPU_FAM_STM32WB)
#include "periph/wb/periph_cpu.h"
#endif

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
      defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L4) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
      defined(CPU_FAM_STM32MP1) || defined(CPU_FAM_STM32WL)
#define CLOCK_LSI           (32000U)
#else
#error "error: LSI clock speed not defined for your target CPU"
#endif

#ifdef Doxygen
/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR
#endif

/**
 * @brief   Length of the CPU_ID in octets
 *
 * This is the same for all members of the stm32 family
 */
#define CPUID_LEN           (12U)

/**
 * @brief   Starting address of the CPU ID
 */
#ifndef CPUID_ADDR
#define CPUID_ADDR          (UID_BASE)
#endif

/**
 * @brief   All STM timers have 4 capture-compare channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

/**
 * @brief   Define a macro for accessing a timer channel
 */
#define TIM_CHAN(tim, chan) *(&dev(tim)->CCR1 + chan)

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
#if defined(CPU_FAM_STM32WL)
    APB3,
#endif
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32WL)
    APB12,          /**< AHB1 bus, second register */
#endif
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
    AHB,            /**< AHB bus */
    IOP,            /**< IOP bus */
#elif defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1) || \
      defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    AHB,            /**< AHB bus */
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32WL)
    AHB1,           /**< AHB1 bus */
    AHB2,           /**< AHB2 bus */
    AHB3,           /**< AHB3 bus */
#elif defined(CPU_FAM_STM32MP1)
    AHB1,           /**< AHB1 bus */
    AHB2,           /**< AHB2 bus */
    AHB3,           /**< AHB3 bus */
#else
#warning "unsupported stm32XX family"
#endif
#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32MP1)
    AHB4,           /**< AHB4 bus */
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
#if defined(CPU_FAM_STM32MP1)
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 12)) | y)
#else
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)
#endif

/**
 * @brief   Available GPIO ports
 */
enum {
#ifdef GPIOA
    PORT_A = 0,             /**< port A */
#endif
#ifdef GPIOB
    PORT_B = 1,             /**< port B */
#endif
#ifdef GPIOC
    PORT_C = 2,             /**< port C */
#endif
#ifdef GPIOD
    PORT_D = 3,             /**< port D */
#endif
#ifdef GPIOE
    PORT_E = 4,             /**< port E */
#endif
#ifdef GPIOF
    PORT_F = 5,             /**< port F */
#endif
#ifdef GPIOG
    PORT_G = 6,             /**< port G */
#endif
#ifdef GPIOH
    PORT_H = 7,             /**< port H */
#endif
#ifdef GPIOI
    PORT_I = 8,             /**< port I */
#endif
#ifdef GPIOJ
    PORT_J = 9,             /**< port J */
#endif
#ifdef GPIOK
    PORT_K = 10,            /**< port K */
#endif
};

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
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32MP1)
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

#ifdef CPU_FAM_STM32F1
#ifndef DOXYGEN
/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 4 bit to determine the pin functions:
 * - bit 4: ODR value
 * - bit 2+3: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(mode, cnf, odr)       (mode | (cnf << 2) | (odr << 4))

/**
 * @brief   Override GPIO mode options
 *
 * We use 4 bit to encode CNF and MODE.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 2, 1),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(3, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(3, 1, 0),    /**< open-drain w/o pull R */
    GPIO_OD_PU = (0xff)                 /**< not supported by HW */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 4,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 9,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 8       /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */
#else /* CPU_FAM_STM32F1 */
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
#endif /* ndef DOXYGEN */
#endif /* ndef CPU_FAM_STM32F1 */

#ifndef DOXYGEN
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
    DMA_PERIPH_TO_MEM = 0,     /**< Peripheral to memory */
    DMA_MEM_TO_PERIPH = 1,     /**< Memory to peripheral */
    DMA_MEM_TO_MEM = 2,        /**< Memory to memory */
} dma_mode_t;

/**
 * @brief   DMA channel/trigger configuration for DMA peripherals without
 *          channel/trigger filtering such as the stm32f1 and stm32f3.
 */
#define DMA_CHAN_CONFIG_UNSUPPORTED  (UINT8_MAX)

/**
 * @name    DMA Increment modes
 * @{
 */
#define DMA_INC_SRC_ADDR  (0x04)
#define DMA_INC_DST_ADDR  (0x08)
#define DMA_INC_BOTH_ADDR (DMA_INC_SRC_ADDR | DMA_INC_DST_ADDR)
/** @} */

/**
 * @name    DMA data width
 * @{
 */
#define DMA_DATA_WIDTH_BYTE      (0x00)
#define DMA_DATA_WIDTH_HALF_WORD (0x01)
#define DMA_DATA_WIDTH_WORD      (0x02)
#define DMA_DATA_WIDTH_MASK      (0x03)
#define DMA_DATA_WIDTH_SHIFT     (0)
/** @} */

/**
 * @brief   Available number of ADC devices
 */
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2)
#define ADC_DEVS            (2U)
#elif defined(CPU_LINE_STM32F401xE) || defined(CPU_LINE_STM32F410Rx) \
    || defined(CPU_LINE_STM32F411xE) || defined(CPU_LINE_STM32F412Zx) \
    || defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx) \
    || defined(CPU_MODEL_STM32L452RE) || defined(CPU_MODEL_STM32L432KC)
#define ADC_DEVS            (1U)
#elif defined(CPU_LINE_STM32F405xx)  || defined(CPU_LINE_STM32F407xx) \
    || defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F429xx) \
    || defined(CPU_LINE_STM32F437xx) || defined(CPU_LINE_STM32F446xx) \
    || defined(CPU_MODEL_STM32L476RG) || defined(CPU_MODEL_STM32L475VG)
#define ADC_DEVS            (3U)
#endif

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
#if !defined(CPU_FAM_STM32F0) && !defined(CPU_FAM_STM32L0) && \
    !defined(CPU_FAM_STM32L1)
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
#endif
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
    pwm_chan_t chan[TIMER_CHANNEL_NUMOF]; /**< channel mapping
                                           *   set to {GPIO_UNDEF, 0}
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
#ifndef CPU_FAM_STM32F1
    gpio_af_t af;                   /**< alternate function used */
#endif
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
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t cts_pin;         /**< CTS pin - set to GPIO_UNDEF when not using HW flow control */
    gpio_t rts_pin;         /**< RTS pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t cts_af;       /**< alternate function for CTS pin */
    gpio_af_t rts_af;       /**< alternate function for RTS pin */
#endif
#endif
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32MP1) || \
    defined(CPU_FAM_STM32WL)
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
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32MP1)
    I2C_SPEED_LOW,          /**< low speed mode: ~10kit/s */
#endif
    I2C_SPEED_NORMAL,       /**< normal mode:  ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:    ~400kbit/s */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
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
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32MP1)
    uint32_t clk;           /**< bus frequency as defined in board config */
#endif
    uint8_t irqn;           /**< I2C event interrupt number */
} i2c_conf_t;

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
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
 * @brief USB OTG peripheral type.
 *
 * High speed peripheral is assumed to have DMA support available.
 *
 * @warning Only one of each type is supported at the moment, it is not
 * supported to have two FS type or two HS type peripherals enabled on a
 * single MCU.
 */
typedef enum {
    STM32_USB_OTG_FS = 0,   /**< Full speed peripheral */
    STM32_USB_OTG_HS = 1,   /**< High speed peripheral */
} stm32_usb_otg_fshs_type_t;

/**
 * @brief Type of USB OTG peripheral phy.
 *
 * The FS type only supports the built-in type, the HS phy can have either the
 * FS built-in phy enabled or the HS ULPI interface enabled.
 */
typedef enum {
    STM32_USB_OTG_PHY_BUILTIN,
    STM32_USB_OTG_PHY_ULPI,
} stm32_usb_otg_fshs_phy_t;

/**
 * @brief stm32 USB OTG configuration
 */
typedef struct {
    uint8_t *periph;                /**< USB peripheral base address */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    stm32_usb_otg_fshs_phy_t phy;   /**< Built-in or ULPI phy */
    stm32_usb_otg_fshs_type_t type; /**< FS or HS type */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t ahb;                    /**< AHB bus */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
} stm32_usb_otg_fshs_config_t;

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

/**
 * @brief   Low level initial DMA stream configuration
 *
 * This function is supposed to be used together with @ref dma_prepare. This
 * function sets up the one-time configuration of a stream and @ref dma_prepare
 * configures the per-transfer registers.
 *
 * @param[in]   dma         Logical DMA stream
 * @param[in]   chan        DMA channel (on stm32f2/4/7, CxS or unused on others)
 * @param[in]   periph_addr Peripheral register address
 * @param[in]   mode        DMA direction mode
 * @param[in]   width       DMA transfer width
 * @param[in]   inc_periph  Increment peripheral address after read/write
 */
void dma_setup(dma_t dma, int chan, void *periph_addr, dma_mode_t mode,
               uint8_t width, bool inc_periph);

/**
 * @brief   Low level DMA transfer configuration
 *
 * @param[in]   dma         Logical DMA stream
 * @param[in]   mem         Memory address
 * @param[in]   len         Transfer length
 * @param[in]   inc_mem     Increment the memory address after read/write
 */
void dma_prepare(dma_t dma, void *mem, size_t len, bool incr_mem);

#endif /* MODULE_PERIPH_DMA */

#ifdef MODULE_PERIPH_CAN
#include "candev_stm32.h"
#endif

/**
 * @brief STM32 Ethernet configuration mode
 */
typedef enum {
    MII  = 18,                  /**< Configuration for MII */
    RMII = 9,                   /**< Configuration for RMII */
    SMI  = 2,                   /**< Configuration for SMI */
} eth_mode_t;

/**
 * @brief   Ethernet Peripheral configuration
 */
typedef struct {
    eth_mode_t mode;            /**< Select configuration mode */
    uint16_t speed;             /**< Speed selection */
    uint8_t dma;                /**< Locical CMA Descriptor used for TX */
    uint8_t dma_chan;           /**< DMA channel used for TX */
    uint8_t phy_addr;           /**< PHY address */
    gpio_t pins[];              /**< Pins to use. MII requires 18 pins,
                                     RMII 9 and SMI 9. Not all speeds are
                                     supported by all modes. */
} eth_conf_t;

/**
 * @brief   Layout of enhanced RX/TX DMA descriptor
 *
 * @note    Don't confuse this with the normal RX/TX descriptor format.
 * @warning The content of the status and control bits is different for RX and
 *          TX DMA descriptors
 */
typedef struct eth_dma_desc {
    volatile uint32_t status;                   /**< Mostly status bits, some control bits */
    volatile uint32_t control;                  /**< Control bits */
    char * volatile buffer_addr;                /**< RX/TX buffer */
    struct eth_dma_desc * volatile desc_next;   /**< Address of next DMA descriptor */
    volatile uint32_t reserved1_ext;            /**< RX: Extended status, TX: reserved */
    volatile uint32_t reserved2;                /**< Reserved for future use */
    /**
     * @brief   Sub-second part of PTP timestamp of transmitted / sent frame
     *
     * For TX: If PTP timestamping is enabled and the TTSE bit in the
     * transmit descriptor word 0 (struct eth_dma_desc::status) is set, the
     * MAC will store the PTP timestamp of when the Start of Frame Delimiter
     * was sent. The TTSS bit is send by the hardware if the timestamp was
     * correctly set.
     *
     * For RX: If PTP timestamping is enabled, the timestamp of all received
     * frames is captured.
     */
    volatile uint32_t ts_low;
    volatile uint32_t ts_high;                  /**< Second part of PTP timestamp */
} edma_desc_t;

/**
 * @name    Flags in the status word of the Ethernet enhanced RX DMA descriptor
 * @{
 */
#define RX_DESC_STAT_LS         (BIT8)  /**< If set, descriptor is the last of a frame */
#define RX_DESC_STAT_FS         (BIT9)  /**< If set, descriptor is the first of a frame */
/**
 * @brief   Frame length
 *
 * The length of the frame in host memory order including CRC. Only valid if
 * @ref RX_DESC_STAT_LS is set and @ref RX_DESC_STAT_DE is not set.
 */
#define RX_DESC_STAT_FL         (0x3FFF0000) /* bits 16-29 */
#define RX_DESC_STAT_DE         (BIT14) /**< If set, a frame too large to fit buffers given by descriptors was received */
#define RX_DESC_STAT_ES         (BIT14) /**< If set, an error occurred during RX */
#define RX_DESC_STAT_OWN        (BIT31) /**< If set, descriptor is owned by DMA, otherwise by CPU */
/** @} */
/**
 * @name    Flags in the control word of the Ethernet enhanced RX DMA descriptor
 * @{
 */
/**
 * @brief   Indicates if RDES3 points to the next DMA descriptor (1), or to a second buffer (0)
 *
 * If the bit is set, RDES3 (@ref edma_desc_t::desc_next) will point to the
 * next DMA descriptor rather than to a second frame-segment buffer. This is
 * always set by the driver
 */
#define RX_DESC_CTRL_RCH        (BIT14)
/** @} */
/**
 * @name    Flags in the status word of the Ethernet enhanced TX DMA descriptor
 * @{
 */
#define TX_DESC_STAT_UF         (BIT1)  /**< If set, an underflow occurred while sending */
#define TX_DESC_STAT_EC         (BIT8)  /**< If set, TX was aborted due to excessive collisions (half-duplex only) */
#define TX_DESC_STAT_NC         (BIT10) /**< If set, no carrier was detected (TX aborted) */
#define TX_DESC_STAT_ES         (BIT15) /**< If set, one or more error occurred */
#define TX_DESC_STAT_TTSS       (BIT17) /**< If set, the descriptor contains a valid PTP timestamp */
/**
 * @brief   Indicates if TDES3 points to the next DMA descriptor (1), or to a second buffer (0)
 *
 * If the bit is set, TDES3 (@ref edma_desc_t::desc_next) will point to the
 * next DMA descriptor rather than to a second frame-segment buffer. This is
 * always set by the driver
 */
#define TX_DESC_STAT_TCH        (BIT20)
#define TX_DESC_STAT_TER        (BIT21) /**< If set, DMA will return to first descriptor in ring afterwards */
/**
 * @brief   Checksum insertion control
 *
 * | Value  | Meaning                                                                       |
 * |:------ |:----------------------------------------------------------------------------- |
 * | `0b00` | Checksum insertion disabled                                                   |
 * | `0b01` | Calculate and insert checksum in IPv4 header                                  |
 * | `0b10` | Calculate and insert IPv4 checksum, insert pre-calculated payload checksum    |
 * | `0b11  | Calculated and insert both IPv4 and payload checksum                          |
 */
#define TX_DESC_STAT_CIC        (BIT22 | BIT23)
#define TX_DESC_STAT_TTSE       (BIT25) /**< If set, an PTP timestamp is added to the descriptor after TX completed */
#define TX_DESC_STAT_FS         (BIT28) /**< If set, buffer contains first segment of frame to transmit */
#define TX_DESC_STAT_LS         (BIT29) /**< If set, buffer contains last segment of frame to transmit */
#define TX_DESC_STAT_IC         (BIT30) /**< If set, trigger IRQ on completion */
#define TX_DESC_STAT_OWN        (BIT31) /**< If set, descriptor is owned by DMA, otherwise by CPU */
/** @} */

#ifdef MODULE_PERIPH_ETH_COMMON
/**
 * @brief   Perform ETH initialization common to periph_stm32_eth and
 *          periph_ptp_clock
 */
void stm32_eth_common_init(void);
#endif /* MODULE_PERIPH_ETH_COMMON */

/**
 * @name    PTP clock configuration
 * @{
 */
#define HAVE_PTP_CLOCK_READ         1   /**< Native implementation available */
#define HAVE_PTP_CLOCK_SET          1   /**< Native implementation available */
#define HAVE_PTP_TIMER_SET_ABSOLUTE 1   /**< Native implementation available */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
