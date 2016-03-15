/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f2
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

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
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

/**
 * @brief   Override HWSC macro
 */
#define SPI_HWCS(x)         (x)

/**
 * @brief   Peripheral buses
 */
enum {
    BUS_APB1    = 0,
    BUS_APB2    = 1
};

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
 * @brief   Override I2C speed options
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW = 0,      /**< low speed mode:    ~10kbit/s */
    I2C_SPEED_NORMAL,       /**< normal mode:       ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:         ~400kbit/s */
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode:    ~1Mbit/s */
    I2C_SPEED_HIGH,         /**< high speed mode:   ~3.4Mbit/s */
} i2c_speed_t;
/** @} */

/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x03000000,  /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x02000000,  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01000000,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x00000000,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 1,           /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = 2            /**< ADC resolution: 16 bit (not supported)*/
} adc_res_t;
/** @} */

/**
 * @brief   Available ports on the STM32F2 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    PORT_H = 7,             /**< port H */
    PORT_I = 8              /**< port I */
};

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
    GPIO_AF0 = 0,           /**< use alternate function 0 */
    GPIO_AF1,               /**< use alternate function 1 */
    GPIO_AF2,               /**< use alternate function 2 */
    GPIO_AF3,               /**< use alternate function 3 */
    GPIO_AF4,               /**< use alternate function 4 */
    GPIO_AF5,               /**< use alternate function 5 */
    GPIO_AF6,               /**< use alternate function 6 */
    GPIO_AF7,               /**< use alternate function 7 */
    GPIO_AF8,               /**< use alternate function 8 */
    GPIO_AF9,               /**< use alternate function 9 */
    GPIO_AF10,              /**< use alternate function 10 */
    GPIO_AF11,              /**< use alternate function 11 */
    GPIO_AF12,              /**< use alternate function 12 */
    GPIO_AF13,              /**< use alternate function 13 */
    GPIO_AF14               /**< use alternate function 14 */
} gpio_af_t;

/**
 * @brief   Structure for UART configuration data
 * @{
 */
typedef struct {
    USART_TypeDef *dev;     /**< UART device base register address */
    uint32_t rcc_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_af_t af;           /**< alternate pin function to use */
    uint8_t irqn;           /**< IRQ channel */
    uint8_t dma_stream;     /**< DMA stream used for TX */
    uint8_t dma_chan;       /**< DMA channel used for TX */
} uart_conf_t;
/** @} */

/**
 * @brief   I2C configuration options
 * @{
 */
typedef struct {
    I2C_TypeDef *dev;       /**< I2C device */
    gpio_t scl_pin;         /**< clock pin */
    gpio_t sda_pin;         /**< data pin */
    uint8_t scl_af;         /**< SCL pin alternate function */
    uint8_t sda_af;         /**< SDA pin alternate function */
    i2c_speed_t speed;      /**< I2C bus speed */
    uint8_t rcc_bit;        /**< bit in the rcc register */
    uint8_t irqn;           /**< IRQ channel */
} i2c_conf_t;
/** @} */

#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0      = 0,    /**< CPOL=0, CPHA=0 */
    SPI_MODE_1,             /**< CPOL=0, CPHA=1 */
    SPI_MODE_2,             /**< CPOL=1, CPHA=0 */
    SPI_MODE_3,             /**< CPOL=1, CPHA=1 */
} spi_mode_t;

/**
 * @brief   Define the SPI CLK devider values for APB1
 *          (assume APB1 clock speed is 30MHz and APB2clk = APB1clk * 2)
 *
 * @note    calculate with: value = (int)((sqrt(CLOCK_APB1 / (spi_clk)) / 2) - 1)
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ  = 0x7,  /**< 117 KHz */
    SPI_CLK_400KHZ  = 0x5,  /**< 468 KHz */
    SPI_CLK_1MHZ    = 0x4,  /**< 937 KHz */
    SPI_CLK_5MHZ    = 0x2,  /**< 3.75 MHz */
    SPI_CLK_10MHZ   = 0x1,  /**< 7.5 MHz */
} spi_clk_t;

/**
 * @brief   Structure for SPI configuration data
 * @{
 */
typedef struct {
    SPI_TypeDef *dev;       /**< SPI device base register address */
    gpio_t mosi_pin;        /**< MOSI pin */
    gpio_t miso_pin;        /**< MISO pin */
    gpio_t sclk_pin;        /**< SCLK pin */
    gpio_af_t af;           /**< pin alternate function */
    uint8_t abpbus;         /**< APB bus, 0 := APB1, 1:= APB2 */
    uint32_t rccmask;       /**< bit in the RCC peripheral enable register */
} spi_conf_t;
/** @} */

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
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
 * @brief   Configure the alternate function for the given pin
 *
 * @note    This is meant for internal use in STM32F4 peripheral drivers only
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

/**
 * @brief   Power on the DMA device the given stream belongs to
 *
 * @param[in] stream    logical DMA stream
 */
static inline void dma_poweron(int stream)
{
    if (stream < 8) {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    } else {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    }
}

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA stream handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 stream0 equals 0, DMA2 stream0 equals 8,
 * DMA2 stream 7 equals 15 and so on.
 *
 * @param[in] stream    logical DMA stream
 */
static inline DMA_TypeDef *dma_base(int stream)
{
    return (stream < 8) ? DMA1 : DMA2;
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] stream    logical DMA stream
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
 * @param[in] stream    logical DMA stream
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
 * @param[in] stream    logical DMA stream
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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
