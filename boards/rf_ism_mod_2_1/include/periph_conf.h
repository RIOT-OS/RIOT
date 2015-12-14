/*
 * periph_conf.h
 *
 *  Created on: 4 sie 2015
 *      Author: kkk
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define EFM32_HFXO_FREQ	(16000000UL)	/* External high freq crystal. */
#define EFM32_LFXO_FREQ	(32768UL)		/* External RTC crystal */
#define CLOCK_CORECLOCK (28000000UL)	/* Core frequency */

/**
 * @brief xtimer configuration values
 * @{
 */
#define XTIMER                      TIMER_0
#define XTIMER_CHAN                 (0)
#define XTIMER_SHIFT                (5)
#define XTIMER_MASK                 (0xffff0000)
#define XTIMER_BACKOFF              (40)
#define LETIMER_EN

/**
 * @name Timer configuration
 * @{
 */
#if defined LETIMER_EN
#define TIMER_NUMOF			(1U)
#define TIMER_0_EN			1
#define TIMER_IRQ_PRIO		20

#else

#define TIMER_NUMOF			(2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
//#define TIMER_2_EN			1
//#define TIMER_3_EN			1
#define TIMER_IRQ_PRIO      20

/* Timer 0 configuration */
#define TIMER_0_DEV         ((TIMER_TypeDef *)TIMER0)
#define TIMER_0_CHANNELS	3
#define TIMER_0_CLKEN()		{CMU_ClockEnable(cmuClock_TIMER0, true);}
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_ISR         TIMER0_IRQHandler
#define TIMER_0_IRQ_CHAN    TIMER0_IRQn
/* Timer 1 configuration */
#define TIMER_1_DEV         ((TIMER_TypeDef *)TIMER1)
#define TIMER_1_CHANNELS	3
#define TIMER_1_CLKEN()		{CMU_ClockEnable(cmuClock_TIMER1, true);}
#define TIMER_1_MAX_VALUE   (0xffff)
#define TIMER_1_ISR         TIMER1_IRQHandler
#define TIMER_1_IRQ_CHAN    TIMER1_IRQn
/* Timer 2 configuration */
#define TIMER_2_DEV         ((TIMER_TypeDef *)TIMER2)
#define TIMER_2_CHANNELS	3
#define TIMER_2_CLKEN()		{CMU_ClockEnable(cmuClock_TIMER2, true);}
#define TIMER_2_MAX_VALUE   (0xffff)
#define TIMER_2_ISR         TIMER2_IRQHandler
#define TIMER_2_IRQ_CHAN    TIMER2_IRQn
/* Timer 3 configuration */
#define TIMER_3_DEV         ((TIMER_TypeDef *)TIMER3)
#define TIMER_3_CHANNELS	3
#define TIMER_3_CLKEN()		{CMU_ClockEnable(cmuClock_TIMER3, true);}
#define TIMER_3_MAX_VALUE   (0xffff)
#define TIMER_3_ISR         TIMER3_IRQHandler
#define TIMER_3_IRQ_CHAN    TIMER3_IRQn
/** @} */
#endif

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           0
#define UART_1_EN           0
#define UART_2_EN			1
#define UART_0_IRQ_TX_PRIO  1
#define UART_0_IRQ_RX_PRIO	2
#define UART_1_IRQ_TX_PRIO  3
#define UART_1_IRQ_RX_PRIO	4
#define UART_2_IRQ_PRIO		5

/* UART 0 device configuration */
#define UART_0_DEV          USART0
#define UART_0_IRQ_TX_CHAN  USART0_TX_IRQn
#define UART_0_IRQ_RX_CHAN	USART0_RX_IRQn
#define UART_0_TX_ISR		USART0_TX_IRQHandler
#define UART_0_RX_ISR		USART0_RX_IRQHandler
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() {CMU_ClockEnable(cmuClock_GPIO, true);}
#define UART_0_PORT         GPIO_PORT_E
#define UART_0_LOC			UART_ROUTE_LOCATION_LOC0
#define UART_0_TX_PIN       10
#define UART_0_RX_PIN       11
/* buffering setting */
#define UART_0_ENABLE_BUF   1
#define UART_0_BUFSIZE     512

/* UART 1 device configuration */
#define UART_1_DEV          USART1
#define UART_1_IRQ_TX_CHAN  USART1_TX_IRQn
#define UART_1_IRQ_RX_CHAN	USART1_RX_IRQn
#define UART_1_TX_ISR		USART1_TX_IRQHandler
#define UART_1_RX_ISR		USART1_RX_IRQHandler
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() {CMU_ClockEnable(cmuClock_GPIO, true);}
#define UART_1_CLKDIS()		{CMU_ClockEnable(cmuClock_USART1, false);}
#define UART_1_PORT         GPIO_PORT_D
#define UART_1_LOC			UART_ROUTE_LOCATION_LOC2
#define UART_1_TX_PIN       7
#define UART_1_RX_PIN       6
/* buffering setting */
#define UART_1_ENABLE_BUF   1
#define UART_1_BUFSIZE     512

/* UART 2 device configuration, it is LEUART device */
#define UART_2_DEV          LEUART0
#define UART_2_IRQ 			LEUART0_IRQn
#define UART_2_ISR			LEUART0_IRQHandler
/* UART 2 pin configuration */
#define UART_2_PORT_CLKEN() {CMU_ClockEnable(cmuClock_GPIO, true);}
#define UART_2_CLKDIS()		{CMU_ClockEnable(cmuClock_LEUART0, false);}
#define UART_2_PORT         GPIO_PORT_D
#define UART_2_LOC			UART_ROUTE_LOCATION_LOC0
#define UART_2_TX_PIN       4
#define UART_2_RX_PIN       5
/* buffering setting */
#define UART_2_ENABLE_BUF   1
#define UART_2_BUFSIZE     2048
/** @} */

///**
// * @name ADC configuration
// * @{
// */
//#define ADC_NUMOF           (2U)
//#define ADC_0_EN            1
//#define ADC_1_EN            1
//#define ADC_MAX_CHANNELS    2
//
///* ADC 0 configuration */
//#define ADC_0_DEV           ADC1
//#define ADC_0_CHANNELS      2
//#define ADC_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_ADC1EN)
//#define ADC_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN))
//#define ADC_0_PORT          GPIOA
//#define ADC_0_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
///* ADC 0 channel 0 pin config */
//#define ADC_0_CH0           1
//#define ADC_0_CH0_PIN       1
///* ADC 0 channel 1 pin config */
//#define ADC_0_CH1           4
//#define ADC_0_CH1_PIN       4
//
///* ADC 1 configuration */
//#define ADC_1_DEV           ADC2
//#define ADC_1_CHANNELS      2
//#define ADC_1_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_ADC2EN)
//#define ADC_1_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC2EN))
//
//#define ADC_1_PORT          GPIOC
//#define ADC_1_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
///* ADC 1 channel 0 pin config */
//#define ADC_1_CH0           11
//#define ADC_1_CH0_PIN       1
///* ADC 1 channel 1 pin config */
//#define ADC_1_CH1           12
//#define ADC_1_CH1_PIN       2
///** @} */
//
///**
// * @name DAC configuration
// * @{
// */
//#define DAC_NUMOF          (1U)
//#define DAC_0_EN           1
//#define DAC_MAX_CHANNELS   2
//
///* DAC 0 configuration */
//#define DAC_0_DEV            DAC
//#define DAC_0_CHANNELS       2
//#define DAC_0_CLKEN()        (RCC->APB1ENR |=  (RCC_APB1ENR_DACEN))
//#define DAC_0_CLKDIS()       (RCC->APB1ENR &= ~(RCC_APB1ENR_DACEN))
//#define DAC_0_PORT           GPIOA
//#define DAC_0_PORT_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
///* DAC 0 channel config */
//#define DAC_0_CH0_PIN        4
//#define DAC_0_CH1_PIN        5
//
///**
// * @name PWM configuration
// * @{
// */
//#define PWM_NUMOF           (2U)
//#define PWM_0_EN            1
//#define PWM_1_EN            1
//#define PWM_MAX_CHANNELS    4
//
///* PWM 0 device configuration */
//#define PWM_0_DEV           TIM1
//#define PWM_0_CHANNELS      4
//#define PWM_0_CLK           (168000000U)
//#define PWM_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_TIM1EN)
//#define PWM_0_CLKDIS()      (RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN)
///* PWM 0 pin configuration */
//#define PWM_0_PORT          GPIOE
//#define PWM_0_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN)
//#define PWM_0_PIN_CH0       9
//#define PWM_0_PIN_CH1       11
//#define PWM_0_PIN_CH2       13
//#define PWM_0_PIN_CH3       14
//#define PWM_0_PIN_AF        1
//
///* PWM 1 device configuration */
//#define PWM_1_DEV           TIM3
//#define PWM_1_CHANNELS      3
//#define PWM_1_CLK           (84000000U)
//#define PWM_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
//#define PWM_1_CLKDIS()      (RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN)
///* PWM 1 pin configuration */
//#define PWM_1_PORT          GPIOB
//#define PWM_1_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
//#define PWM_1_PIN_CH0       4
//#define PWM_1_PIN_CH1       5
//#define PWM_1_PIN_CH2       0
//#define PWM_1_PIN_CH3       1
//#define PWM_1_PIN_AF        2
///** @} */
//
///**
// * @name Random Number Generator configuration
// * @{
// */
//#define RANDOM_NUMOF        (1U)
///** @} */
//
/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN			0
#define SPI_2_EN			1
#define SPI_IRQ_PRIO        0
#define SPI_PERCLK_FREQ		(14000000UL)

#if defined SPI_0_EN
#define SPI_0_MASTER			1
/* SPI 0 device config */
#define SPI_0_DEV               USART0
#define SPI_0_CLKEN()           {CMU_ClockEnable(cmuClock_USART0, true);}
#define SPI_0_CLKDIS()          {CMU_ClockEnable(cmuClock_USART0, true);}
#define SPI_0_FREQ           	1000000U
#define SPI_0_IRQ_TX			USART0_TX_IRQn
#define SPI_0_IRQ_RX			USART0_RX_IRQn
#define SPI_0_IRQ_TX_HANDLER	USART0_TX_IRQHandler
#define SPI_0_IRQ_RX_HANDLER	USART0_RX_IRQHandler
/* Peripheral frequency of SPI should be given to achieve desired SPI speed. */
/* SPI 0 pin configuration */
#define SPI_0_LOC				0
#endif

#if defined SPI_1_EN
/* SPI 1 device config */
#define SPI_1_DEV               USART1
#define SPI_1_CLKEN()           {CMU_ClockEnable(cmuClock_USART1, true);}
#define SPI_1_CLKDIS()          {CMU_ClockEnable(cmuClock_USART1, true);}
#define SPI_1_FREQ           	1000000U
#define SPI_1_IRQ_TX			USART1_TX_IRQn
#define SPI_1_IRQ_RX			USART1_RX_IRQn
#define SPI_1_IRQ_TX_HANDLER	USART1_TX_IRQHandler
#define SPI_1_IRQ_RX_HANDLER	USART1_RX_IRQHandler
/* Peripheral frequency of SPI should be given to achieve desired SPI speed. */
/* SPI 0 pin configuration */
#define SPI_1_LOC				1
#endif

#if defined SPI_2_EN
/* SPI 0 device config */
#define SPI_2_DEV               USART2
#define SPI_2_CLKEN()           {CMU_ClockEnable(cmuClock_USART2, true);}
#define SPI_2_CLKDIS()          {CMU_ClockEnable(cmuClock_USART2, true);}
#define SPI_2_FREQ           	1000000U
#define SPI_2_IRQ_TX			USART2_TX_IRQn
#define SPI_2_IRQ_RX			USART2_RX_IRQn
#define SPI_2_IRQ_TX_HANDLER	USART2_TX_IRQHandler
#define SPI_2_IRQ_RX_HANDLER	USART2_RX_IRQHandler
/* Peripheral frequency of SPI should be given to achieve desired SPI speed. */
/* SPI 0 pin configuration */
#define SPI_2_LOC				0
#endif
/** @} */

///**
// * @name I2C configuration
// * @{
// */
//#define I2C_NUMOF           (1U)
//#define I2C_0_EN            1
//#define I2C_IRQ_PRIO        1
//#define I2C_APBCLK          (42000000U)
//
///* I2C 0 device configuration */
//#define I2C_0_DEV           I2C1
//#define I2C_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
//#define I2C_0_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
//#define I2C_0_EVT_IRQ       I2C1_EV_IRQn
//#define I2C_0_EVT_ISR       isr_i2c1_ev
//#define I2C_0_ERR_IRQ       I2C1_ER_IRQn
//#define I2C_0_ERR_ISR       isr_i2c1_er
///* I2C 0 pin configuration */
//#define I2C_0_SCL_PORT      GPIOB
//#define I2C_0_SCL_PIN       6
//#define I2C_0_SCL_AF        4
//#define I2C_0_SCL_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
//#define I2C_0_SDA_PORT      GPIOB
//#define I2C_0_SDA_PIN       7
//#define I2C_0_SDA_AF        4
//#define I2C_0_SDA_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
///** @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
