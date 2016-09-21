#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_HSE           (8000000U)               /* external oscillator */
#define CLOCK_CORECLOCK     (120000000U)             /* targeted core clock frequency */

/* configuration of PLL prescaler and multiply values */
#define CLOCK_PLL_M         (CLOCK_HSE / 1000000)
#define CLOCK_PLL_N         ((CLOCK_CORECLOCK / 1000000) * 2)
#define CLOCK_PLL_P         (2U)
#define CLOCK_PLL_Q         (CLOCK_PLL_N / 48)

/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 120MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2     /* APB2 clock -> 60MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4     /* APB1 clock -> 30MHz */

/* resulting bus clocks */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)

/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_0WS
/** @} */

/**
 * @name PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = { };

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF          (0)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (4U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1
#define TIMER_3_EN          1
#define TIMER_IRQ_PRIO      1

static const timer_conf_t timer_config[TIMER_NUMOF] = {
    {
        .dev      = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn,
    },
    {
        .dev      = TIM5,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn,
    },
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn,
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .bus      = APB1,
        .irqn     = TIM4_IRQn,
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim5
#define TIMER_2_ISR         isr_tim3
#define TIMER_3_ISR         isr_tim4
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev          = USART1,
        .rcc_mask     = RCC_APB2ENR_USART1EN,
        .rx_pin       = GPIO_PIN(PORT_A, 10),
        .tx_pin       = GPIO_PIN(PORT_A, 9),
        .rx_af        = GPIO_AF7,
        .tx_af        = GPIO_AF7,
        .bus          = APB2,
        .irqn         = USART1_IRQn,
#ifdef UART_USE_DMA
        .dma_stream   = 8 + 7,
        .dma_chan     = 4
#endif
    }
};

/* assign ISR vector names */
#define UART_0_ISR          isr_usart1
#define UART_0_DMA_ISR      isr_dma2_stream7

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name   SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 30000000Hz */
        7,  /* -> 117187Hz */
        5,  /* -> 468750Hz */
        4,  /* -> 937500Hz */
        2,  /* -> 3750000Hz */
        1   /* -> 7500000Hz */
    },
    {       /* for APB2 @ 60000000Hz */
        7,  /* -> 234375Hz */
        6,  /* -> 468750Hz */
        5,  /* -> 937500Hz */
        3,  /* -> 3750000Hz */
        2   /* -> 7500000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_C, 2),
        .af       = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev      = SPI3,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_B, 3),
        .cs_pin   = GPIO_PIN(PORT_A, 15),
        .af       = GPIO_AF6,
        .rccmask  = RCC_APB1ENR_SPI3EN,
        .apbbus   = APB1
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF (1U)
#define I2C_0_EN 1
#define I2C_IRQ_PRIO 1
#define I2C_APBCLK (CLOCK_APB1)

#define I2C_0_DEV I2C1
#define I2C_0_CLKEN() (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define I2C_0_CLKDIS() (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ I2C1_EV_IRQn
#define I2C_0_EVT_ISR isr_i2c1_ev
#define I2C_0_ERR_IRQ I2C1_ER_IRQn
#define I2C_0_ERR_ISR isr_i2c1_er
/* I2C 0 pin configuration */
#define I2C_0_SCL_PORT GPIOB
#define I2C_0_SCL_PIN 6
#define I2C_0_SCL_AF 4
#define I2C_0_SCL_PULLUP 0
#define I2C_0_SCL_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define I2C_0_SDA_PORT GPIOB
#define I2C_0_SDA_PIN 7
#define I2C_0_SDA_AF 4
#define I2C_0_SDA_PULLUP 0
#define I2C_0_SDA_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
/** @} */

/**
 * @brief DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

/**
 * @brief   RTC configuration
 * @{
 */
#define RTC_NUMOF           (0)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
