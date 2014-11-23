/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_samr21-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Atmel SAM R21 Xplained Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (3U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC3->COUNT16
#define TIMER_0_CHANNELS    2
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_ISR         isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV         TC4->COUNT16
#define TIMER_1_CHANNELS    2
#define TIMER_1_MAX_VALUE   (0xffff)
#define TIMER_1_ISR         isr_tc4

/* Timer 2 configuration */
#define TIMER_2_DEV         TC5->COUNT16
#define TIMER_2_CHANNELS    2
#define TIMER_2_MAX_VALUE   (0xffff)
#define TIMER_2_ISR         isr_tc5
/** @} */


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          SERCOM0->USART
#define UART_0_IRQ          SERCOM0_IRQn
#define UART_0_ISR          isr_sercom0
/* UART 0 pin configuration */
#define UART_0_PORT         (PORT->Group[0])
#define UART_0_TX_PIN       PIN_PA04
#define UART_0_RX_PIN       PIN_PA05
#define UART_0_PINS         (PORT_PA04 | PORT_PA05)
#define UART_0_REF_F        (8000000UL)


/* UART 1 device configuration */
#define UART_1_DEV
#define UART_1_IRQ
#define UART_1_ISR
/* UART 1 pin configuration */
#define UART_1_PORT
#define UART_1_PINS
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (0U)
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          (4U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1

/* GPIO channel 0 config */
#define GPIO_0_DEV          PORT->Group[0]
#define GPIO_0_PIN          PIN_PA13
#define GPIO_0_EXTINT		13
/* GPIO channel 1 config */
/* SW0 Button, configure w/ GPIO_PULLUP and GPIO_FALLING */
#define GPIO_1_DEV          PORT->Group[0]
#define GPIO_1_PIN          PIN_PA28
#define GPIO_1_EXTINT		8
/* GPIO channel 2 config */
#define GPIO_2_DEV          PORT->Group[0]
#define GPIO_2_PIN          PIN_PA15
#define GPIO_2_EXTINT		15
/* GPIO channel 3 config */
#define GPIO_3_DEV          PORT->Group[0]
#define GPIO_3_PIN          PIN_PA19
#define GPIO_3_EXTINT       3
/** @} */

/** @} */

/**
 * @ ADC Configuration
 * @{
 */
#define ADC_NUMOF		(1U)
#define ADC_0_EN		1
#define ADC_MAX_CHANNELS	8

/* ADC 0 device configuration */
#define ADC_0_DEV 			ADC
#define	ADC_0_PORT			(PORT->Group[0])
#define ADC_0_IRQ			ADC_IRQn 	
#define ADC_0_CHANNELS      	8
/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE	0 /* GCLK_GENERATOR_0 */
#define ADC_0_PRESCALER		ADC_CTRLB_PRESCALER_DIV4
#define ADC_0_WINDOW_MODE	ADC_WINCTRL_WINMODE_DISABLE

#define ADC_0_CORRECTION_EN 0 /* disabled */
#define ADC_0_GAIN_CORRECTION	ADC_GAINCORR_RESETVALUE
#define ADC_0_OFFSET_CORRECTION	ADC_OFFSETCORR_RESETVALUE
#define ADC_0_SAMPLE_LENGTH	0
#define ADC_0_PIN_SCAN_OFFSET_START	0 /* disabled */
#define	ADC_0_PIN_SCAN_INPUT_TO_SCAN	0 /* disabled */	
#define ADC_0_LEFT_ADJUST	0 /* disabled */
#define ADC_0_DIFFERENTIAL_MODE	0 /* disabled */
#define ADC_0_FREE_RUNNING	0 /* disabled */
#define ADC_0_EVENT_ACTION	0 /* disabled */	
#define ADC_0_RUN_IN_STANDBY	0 /* disabled */

/* ADC 0 Module Status flags */
#define ADC_0_STATUS_RESULT_READY	(1UL << 0)
#define ADC_0_STATUS_WINDOW		(1UL << 1)
#define ADC_0_STATUS_OVERRUN		(1UL << 2) 	

/* ADC 0 Positive Input Pins */
#define ADC_0_POS_INPUT		ADC_INPUTCTRL_MUXPOS_PIN6

/* ADC 0 Negative Input Pins */ 	
#define ADC_0_NEG_INPUT		ADC_INPUTCTRL_MUXNEG_GND

/* ADC 0 Gain Factor */
#define ADC_0_GAIN_FACTOR_1X	ADC_INPUTCTRL_GAIN_1X
#define ADC_0_GAIN_FACTOR_2X	ADC_INPUTCTRL_GAIN_2X
#define ADC_0_GAIN_FACTOR_4X	ADC_INPUTCTRL_GAIN_4X
#define ADC_0_GAIN_FACTOR_8X	ADC_INPUTCTRL_GAIN_8X
#define ADC_0_GAIN_FACTOR_16X	ADC_INPUTCTRL_GAIN_16X
 	/* Use this to define the value used */
#define ADC_0_GAIN_FACTOR_DEFAULT	ADC_0_GAIN_FACTOR_1X

/* ADC 0 Resolutions */	
#define ADC_0_RES_8BIT	ADC_CTRLB_RESSEL_8BIT
#define ADC_0_RES_10BIT ADC_CTRLB_RESSEL_10BIT
#define ADC_0_RES_12BIT	ADC_CTRLB_RESSEL_12BIT
#define ADC_0_RES_16BIT	ADC_CTRLB_RESSEL_16BIT 

/* ADC 0 Voltage reference */
#define ADC_0_REF_INT_1V	ADC_REFCTRL_REFSEL_INT1V
#define ADC_0_REF_EXT_B		ADC_REFCTRL_REFSEL_AREFB
#define ADC_0_REF_COM_EN	0
/* Use this to define the value used */ 
#define ADC_0_REF_DEFAULT	ADC_0_REF_EXT_B

/* ADC 0 ACCUMULATE */
#define ADC_0_ACCUM_DISABLE	ADC_AVGCTRL_SAMPLENUM_1
#define ADC_0_ACCUM_2		ADC_AVGCTRL_SAMPLENUM_2
#define ADC_0_ACCUM_4		ADC_AVGCTRL_SAMPLENUM_4
#define ADC_0_ACCUM_8		ADC_AVGCTRL_SAMPLENUM_8
#define ADC_0_ACCUM_16		ADC_AVGCTRL_SAMPLENUM_16
#define ADC_0_ACCUM_32		ADC_AVGCTRL_SAMPLENUM_32
#define ADC_0_ACCUM_64		ADC_AVGCTRL_SAMPLENUM_64
#define ADC_0_ACCUM_128		ADC_AVGCTRL_SAMPLENUM_128
#define ADC_0_ACCUM_256		ADC_AVGCTRL_SAMPLENUM_256
#define ADC_0_ACCUM_512		ADC_AVGCTRL_SAMPLENUM_512
#define ADC_0_ACCUM_1024	ADC_AVGCTRL_SAMPLENUM_1024
/* Use this to define the value used */
#define ADC_0_ACCUM_DEFAULT	ADC_AVGCTRL_SAMPLENUM_32 

/* ADC 0 DEVIDE RESULT */
#define ADC_0_DIV_RES_DISABLE	0
#define ADC_0_DIV_RES_2			1
#define ADC_0_DIV_RES_4			2
#define ADC_0_DIV_RES_8			3
#define ADC_0_DIV_RES_16		4
#define ADC_0_DIV_RES_32		5
#define ADC_0_DIV_RES_64		6
#define ADC_0_DIV_RES_128		7
/* Use this to define the value used */
#define ADC_0_DIV_RES_DEFAULT	ADC_0_DIV_RES_DISABLE 

/* ADC 0 channel 0 pin config - NOT implemented yet */
#define ADC_0_CH0           PORT_PB08B_ADC_AIN2//10
#define ADC_0_CH0_PIN       PIN_PB08B_ADC_AIN2//0
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1           PORT_PB09B_ADC_AIN3//11
#define ADC_0_CH1_PIN       PIN_PB09B_ADC_AIN3//1
/* ADC 0 channel 2 pin config */
#define ADC_0_CH2       	PORT_PA04B_ADC_AIN4//12
#define ADC_0_CH2_PIN      	PIN_PA04B_ADC_AIN4//2
/* ADC 0 channel 3 pin config */
#define ADC_0_CH3           PORT_PA05B_ADC_AIN5//13
#define ADC_0_CH3_PIN       PIN_PA05B_ADC_AIN5//3
/* ADC 0 channel 4 pin config */
#define ADC_0_CH4           PORT_PA06B_ADC_AIN6//14
#define ADC_0_CH4_PIN       PIN_PA06B_ADC_AIN6//4
/* ADC 0 channel 5 pin config */
#define ADC_0_CH5           PORT_PA07B_ADC_AIN7//15
#define ADC_0_CH5_PIN       PIN_PA07B_ADC_AIN7//5
/* ADC 0 channel 6 pin config */
#define ADC_0_CH6           PORT_PB00B_ADC_AIN8//14
#define ADC_0_CH6_PIN       PIN_PB00B_ADC_AIN8//4
/* ADC 0 channel 7 pin config */
#define ADC_0_CH7           PORT_PB02B_ADC_AIN10//15
#define ADC_0_CH7_PIN       PIN_PB02B_ADC_AIN10//5
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
