/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32e230c-eval
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for GD32E230C-EVAL board
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "macros/units.h"
// #include "cpu.h"
#include "periph_cpu.h"
// #include "saul/periph.h"
// #include "board.h"
//#include "periph/gpio.h"
// #include "periph/gpio.h"
// #include "gd32e23x_periph.h"
// #include "gd32e23x_rcu.h"
// #include "gd32e23x_gpio.h"
// #include "cpu_common.h"
#include "cpu_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define CLOCK_CORECLOCK                     MHZ(72)
#define CLOCK_CORECLOCK                     MHZ(4)

#ifdef __cplusplus
}
#endif

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        //do we need all combinations of alternate functions for all pins?
        //how does it manage mutex combinations
        .dev        = USART0,
        .rcu_mask   = RCU_APB2EN_USART0EN_Msk,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        // .rx_af      = GPIO_AF_1,
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        // .tx_af      = GPIO_AF_2,
        // .de_pin     = ,
        // .de_af      = ,
        // .cts_pin    = ,
        // .cts_af     = ,
        // .rts_pin    = ,
        // .rts_af     = ,
        // .ck_pin     = ,
        // .ck_af      = ,
        .bus        = APB2,
        .irqn       = USART0_IRQn

    },
    {
        .dev        = USART1,
        .rcu_mask   = RCU_APB1EN_USART1EN_Msk,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        // .rx_af      = GPIO_AF_1,
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        // .tx_af      = GPIO_AF_1,
        // .de_pin     = ,
        // .de_af      = ,
        // .cts_pin    = ,
        // .cts_af     = ,
        // .rts_pin    = ,
        // .rts_af     = ,
        // .ck_pin     = ,
        // .ck_af      = ,
        .bus        = APB1,
        .irqn       = USART1_IRQn
    }
};

#define UART_0_ISR          (isr_usart0)
#define UART_1_ISR          (isr_usart1)

#define UART_0_IRQN         USART0_IRQn
#define UART_1_IRQN         USART1_IRQn

#define UART_NUMOF          ARRAY_SIZE(uart_config)


/**
 * @brief   All STM timers have 4 capture-compare channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

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
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIMER0,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER0EN_Msk,
        .bus      = APB2,
        // .irqn     = TIMER0_Channel_IRQn
    },
    {
        .dev      = TIMER2,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER5,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER5EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER13,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER13EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER14,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER14EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER15,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER15EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER16,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER16EN_Msk,
        .bus      = APB2,
    },
};

#define TIMER_0_ISR         isr_tim1_cc

#define TIMER_0_IRQN        TIMER0_Channel_IRQn
#define TIMER_2_IRQN        TIMER2_IRQn
#define TIMER_5_IRQN        TIMER5_IRQn
#define TIMER_13_IRQN       TIMER13_IRQn
#define TIMER_14_IRQN       TIMER14_IRQn
#define TIMER_15_IRQN       TIMER15_IRQn
#define TIMER_16_IRQN       TIMER16_IRQn

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */

/**
 * @brief   PWM channel
 */
typedef struct {
    uint32_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct
{
    TIMER_Type *dev;               /**< Timer used */
    uint32_t rcu_mask;              /**< bit in clock enable register */
    pwm_chan_t chan[TIMER_CHANNEL_NUMOF]; /**< channel mapping
                                           *   set to {GPIO_UNDEF, 0}
                                           *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
} pwm_conf_t;

static const pwm_conf_t pwm_config[] = {
    {
        .dev      = (TIMER_Type *) TIMER0,  // TODO for later pointers, the type is different b/c timer0 is "advanced"
        .rcu_mask = RCU_APB2EN_TIMER0EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_A,  8), .cc_chan = 1 },
                        { .pin = GPIO_PIN(PORT_A,  9), .cc_chan = 2 },
                        { .pin = GPIO_PIN(PORT_A, 10), .cc_chan = 3 },
                        { .pin = GPIO_PIN(PORT_A, 11), .cc_chan = 4 },
                    },
        // .af       = GPIO_AF_2,
        .bus      = APB2
    },
    {
        .dev      = (TIMER_Type *) TIMER2,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_A,  6), .cc_chan = 1 }, //B4
                        { .pin = GPIO_PIN(PORT_A,  7), .cc_chan = 2 }, //B5
                        { .pin = GPIO_PIN(PORT_B,  0), .cc_chan = 3 },
                        { .pin = GPIO_PIN(PORT_B,  1), .cc_chan = 4 },
                    },
        // .af       = GPIO_AF_1,
        .bus      = APB1
    },
    {
        .dev      = (TIMER_Type *) TIMER13,
        .rcu_mask = RCU_APB1EN_TIMER13EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B,  1), .cc_chan = 1 }, //AF4 = A4, A7
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_0,
        .bus      = APB1
    },
    {
        .dev      = (TIMER_Type *) TIMER14,
        .rcu_mask = RCU_APB2EN_TIMER14EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_A,  2), .cc_chan = 1 }, //AF1 B14
                        { .pin = GPIO_PIN(PORT_A,  3), .cc_chan = 2 }, //AF1 B15
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_0,
        .bus      = APB2
    },
    {
        .dev      = (TIMER_Type *) TIMER15,
        .rcu_mask = RCU_APB2EN_TIMER15EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B,  8), .cc_chan = 1 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_2,
        .bus      = APB2
    },
    {
        .dev      = (TIMER_Type *) TIMER16,
        .rcu_mask = RCU_APB2EN_TIMER16EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B,  9), .cc_chan = 1 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_2,
        .bus      = APB2
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
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
//#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
//    I2C_SPEED_LOW,          /**< low speed mode: ~10kit/s */
    I2C_SPEED_NORMAL,       /**< normal mode:  ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:    ~400kbit/s */
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode: ~1Mbit/s */
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
    uint8_t irqn;           /**< I2C event interrupt number */
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
 * @brief   Timing register settings
 *
 * These parameters are valid for HSI16 input clock.
 * See reference manual of supported CPU for example of timing settings:
 * - STM32F030/F070: see RM0360, section 22.4.10, p.560, table 76
 *
 * @ref i2c_timing_param_t
 */
static const i2c_timing_param_t timing_params[] = {
    [ I2C_SPEED_NORMAL ]    = {
        .presc  = 3,
        .scll   = 0x13,     /* t_SCLL   = 5.0us  */
        .sclh   = 0xF,      /* t_SCLH   = 4.0us  */
        .sdadel = 0x2,      /* t_SDADEL = 500ns  */
        .scldel = 0x4,      /* t_SCLDEL = 1250ns */
    },
    [ I2C_SPEED_FAST ]      = {
        .presc  = 1,
        .scll   = 0x9,      /* t_SCLL   = 1250ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns  */
        .sdadel = 0x2,      /* t_SDADEL = 250ns  */
        .scldel = 0x3,      /* t_SCLDEL = 500ns  */
    },
    [ I2C_SPEED_FAST_PLUS ] = {
        .presc =  0,
        .scll =   0x4,      /* t_SCLL   = 312.5ns */
        .sclh =   0x2,      /* t_SCLH   = 187.5ns */
        .sdadel = 0x0,      /* t_SDADEL = 0ns     */
        .scldel = 0x2,      /* t_SCLDEL = 187.5ns */
    }
};


/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C0,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        // .scl_af         = GPIO_AF_1,
        // .sda_af         = GPIO_AF_1,
        .bus            = APB1,
        .rcu_mask       = RCU_APB1EN_I2C0EN_Msk,// RCU_APB1ENR_I2C1EN,
        // .rcu_sw_mask    = RCU_CFGR3_I2C1SW,
        .irqn           = I2C0_EV_IRQn, //or I2C0_ER_IRQn
    },
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        // .scl_af         = GPIO_AF_1,
        // .sda_af         = GPIO_AF_1,
        .bus            = APB1,
        .rcu_mask       = RCU_APB1EN_I2C1EN_Msk,
        // .rcu_sw_mask    = ,
        .irqn           = I2C1_EV_IRQn,
    }
};

#define I2C_0_ISR         isr_i2c0_cc
#define I2C_1_ISR         isr_i2c1_cc

#define I2C_NUMOF         ARRAY_SIZE(i2c_config)



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
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_B, 6),
        // .mosi_af  = GPIO_AF_1,
        // .miso_af  = GPIO_AF_1,
        // .sclk_af  = GPIO_AF_1,
        // .cs_af    = GPIO_AF_1,
        .rcumask  = RCU_APB2EN_SPI0EN_Msk, //SPI0_CTL0_SPIEN_Msk
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = 0,
        .rx_dma   = 0,
        .rx_dma_chan = 0,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

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
    GD32_PM_DEEPSLEEP = 1,     /**< DEEPSLEEP mode, croresponds to STOP mode of STM32 */
    GD32_PM_IDLE = 2           /**< IDLE mode */
};
/** @} */

#ifndef PM_EWUP_CONFIG
/**
 * @brief   Wake-up pins configuration (CSR register)
 */
#define PM_EWUP_CONFIG          (0U)
#endif
/** @} */

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
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .chan = 0 },
    { .pin = GPIO_PIN(PORT_A, 1), .chan = 1 },
    { .pin = GPIO_PIN(PORT_A, 2), .chan = 2 },
    { .pin = GPIO_PIN(PORT_A, 3), .chan = 3 },
    { .pin = GPIO_PIN(PORT_A, 4), .chan = 4 },
    { .pin = GPIO_PIN(PORT_A, 5), .chan = 5 },
    { .pin = GPIO_PIN(PORT_A, 6), .chan = 6 },
    { .pin = GPIO_PIN(PORT_A, 7), .chan = 7 },
    { .pin = GPIO_PIN(PORT_B, 0), .chan = 8 },
    { .pin = GPIO_PIN(PORT_B, 1), .chan = 9 },
    // { .pin = GPIO_UNDEF, .chan = 16 },
    // { .pin = GPIO_UNDEF, .chan = 17 },
    // { .pin = GPIO_UNDEF, .chan = 18 }, // VBAT VREF ?
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */



#endif /* PERIPH_CONF_H */
/** @} */
