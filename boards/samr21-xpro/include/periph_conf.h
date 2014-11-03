/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_atsamr21-xpro
 * @{
 *
 * @file        periph_conf.h
 * @brief       Peripheral MCU configuration for the Atmel SAM R21 Xplained Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

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


#define F_REF				8000000UL
#define F_BAUD				115200
/* UART 1 device configuration */
#define UART_1_DEV
#define UART_1_IRQ
#define UART_1_ISR
/* UART 1 pin configuration */
#define UART_1_PORT
#define UART_1_PINS
/** @} */

/* Transceiver config */
#define AT86RF231_SPI 		SPI_0
#define AT86RF231_CS		GPIO_4
#define AT86RF231_INT		GPIO_5
#define AT86RF231_RESET		GPIO_6
#define AT86RF231_SLEEP		GPIO_7
#define PIN_RFCTRL1			PIN_PA09
#define PIN_RFCTRL2     	PIN_PA12
#define RFCTRL_CFG_ANT_DIV  4


/**
 * @ ADC Configuration
 * @{
 */
#define ADC_NUMOF			(1U)
#define ADC_0_EN			1
#define ADC_MAX_CHANNELS	8

/* ADC 0 device configuration */
#define ADC_0_DEV 			ADC
#define ADC_0_IRQ			ADC_IRQn 	
#define ADC_0_CHANNELS      8
/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE	0 //GCLK_GENERATOR_0
#define ADC_0_PRESCALER		ADC_CTRLB_PRESCALER_DIV4
#define ADC_0_WINDOW_MODE	ADC_WINCTRL_WINMODE_DISABLE

#define ADC_0_CORRECTION_EN 0 // disabled
#define ADC_0_GAIN_CORRECTION	ADC_GAINCORR_RESETVALUE
#define ADC_0_OFFSET_CORRECTION	ADC_OFFSETCORR_RESETVALUE
#define ADC_0_SAMPLE_LENGTH	0
#define ADC_0_PIN_SCAN_OFFSET_START	0 // disabled
#define	ADC_0_PIN_SCAN_INPUT_TO_SCAN	0 // disabled 	
#define ADC_0_LEFT_ADJUST	0 // disabled
#define ADC_0_DIFFERENTIAL_MODE	0 // disabled
#define ADC_0_FREE_RUNNING	0 // disabled
#define ADC_0_EVENT_ACTION	0 // disabled 	
#define ADC_0_RUN_IN_STANDBY	0 // disabled

/* ADC 0 Module Status flags */
#define ADC_0_STATUS_RESULT_READY	(1UL << 0)
#define ADC_0_STATUS_WINDOW			(1UL << 1)
#define ADC_0_STATUS_OVERRUN		(1UL << 2) 	


//#define ADC_0_CLKEN()       PM->APBCMASK//(RCC->APB2ENR |= RCC_APB2ENR_ADCEN)
//#define ADC_0_CLKDIS()      //(RCC->APB2ENR &= ~(RCC_APB2ENR_ADCEN))
//#define ADC_0_PORT          //GPIOC
//#define ADC_0_PORT_CLKEN()  //(RCC->AHBENR |= RCC_AHBENR_GPIOCEN)


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
#define ADC_0_GAIN_FACTOR_DEFAULT	ADC_0_GAIN_FACTOR_1X

/* ADC 0 Resolutions */
//#define ADC_0_RES_6BIT // NOT POSSIBLE! 	
#define ADC_0_RES_8BIT	ADC_CTRLB_RESSEL_8BIT
#define ADC_0_RES_10BIT ADC_CTRLB_RESSEL_10BIT
#define ADC_0_RES_12BIT	ADC_CTRLB_RESSEL_12BIT
//#define ADC_0_RES_14BIT // NOT POSSIBLE!
#define ADC_0_RES_16BIT	ADC_CTRLB_RESSEL_16BIT 

/* ADC 0 Voltage reference */
#define ADC_0_REF_INT_1V	ADC_REFCTRL_REFSEL_INT1V
#define ADC_0_REF_EXT_B		ADC_REFCTRL_REFSEL_AREFB
#define ADC_0_REF_COM_EN	0 // default
#define ADC_0_REF_DEFAULT	ADC_0_REF_INT_1V // Use this to define the value used

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
#define ADC_0_ACCUM_DEFAULT	ADC_0_ACCUM_1024 // ADC_0_ACCUM_DISABLE // Use this to define the value used

/* ADC 0 DEVIDE RESULT */
#define ADC_0_DIV_RES_DISABLE	0
#define ADC_0_DIV_RES_2			1
#define ADC_0_DIV_RES_4			2
#define ADC_0_DIV_RES_8			3
#define ADC_0_DIV_RES_16		4
#define ADC_0_DIV_RES_32		5
#define ADC_0_DIV_RES_64		6
#define ADC_0_DIV_RES_128		7
#define ADC_0_DIV_RES_DEFAULT	ADC_0_DIV_RES_DISABLE // Use this to define the value used
	
/* ADC 0 channel 0 pin config */
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



/**
 * @brief SPI configuration
 */
#define SPI_NUMOF       2
#define SPI_0_EN        1
#define SPI_1_EN		1

/* SPI0 */
#define SPI_0_DEV       SERCOM4->SPI
#define SPI_IRQ_0       SERCOM4_IRQn
#define SPI_0_DOPO			1 //1 MOSI = pad2, SCLK=pad3
#define SPI_0_DIPO			0
//#define SPI_0_DIPO			0 //used for loopback mode
#define SPI_0_F_REF         F_REF
#define SPI_0_PORT0			PORT->Group[2]
#define SPI_0_PORT1			PORT->Group[1]


#define SPI_0_SCLK_DEV		PORT->Group[2]
#define SPI_0_SCLK_PIN      PIN_PC18
#define SPI_0_SCLK_PAD		3

#define SPI_0_MISO_DEV		PORT->Group[2]
#define SPI_0_MISO_PIN      PIN_PC19
#define SPI_0_MISO_PAD		0

#define SPI_0_MOSI_DEV      PORT->Group[1]
#define SPI_0_MOSI_PIN      PIN_PB30
#define SPI_0_MOSI_PAD		2

#define SPI_0_CS_GPIO		GPIO_4
#define SPI_0_CS_DEV        PORT->Group[1]
#define SPI_0_CS_PIN        PIN_PB31

#define SPI_0_IRQ0_GPIO		GPIO_5
#define SPI_0_IRQ0_DEV      PORT->Group[1]
#define SPI_0_IRQ0_PIN      PIN_PB00

#define SPI_0_RESET_GPIO	GPIO_6
#define SPI_0_RESET_DEV     PORT->Group[1]
#define SPI_0_RESET_PIN     PIN_PB15

#define SPI_0_SLEEP_GPIO	GPIO_7
#define SPI_0_SLEEP_DEV     PORT->Group[0]
#define SPI_0_SLEEP_PIN     PIN_PA20


/* SPI1 */
#define SPI_1_DEV       SERCOM5->SPI
#define SPI_IRQ_1       SERCOM5_IRQn
#define SPI_1_DOPO			1
#define SPI_1_DIPO			2
#define SPI_1_F_REF         F_REF

#define SPI_1_SCLK_DEV		PORT->Group[1]
#define SPI_1_SCLK_PIN      PIN_PB23
#define SPI_1_SCLK_PAD		3

#define SPI_1_MISO_DEV		PORT->Group[1]
#define SPI_1_MISO_PIN      PIN_PB02
#define SPI_1_MISO_PAD		0

#define SPI_1_MOSI_DEV      PORT->Group[1]
#define SPI_1_MOSI_PIN      PIN_PB22
#define SPI_1_MOSI_PAD		2

#define SPI_1_CS_GPIO		GPIO_8
#define SPI_1_CS_DEV        PORT->Group[0]
#define SPI_1_CS_PIN        PIN_PA27


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
#define GPIO_NUMOF          (9U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
//SPI0
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
//SPI1
#define GPIO_8_EN           1
#define GPIO_9_EN           0
#define GPIO_10_EN          0
#define GPIO_11_EN          0

#define GPIO_12_EN          0
#define GPIO_13_EN          0
#define GPIO_14_EN          0
#define GPIO_15_EN          0


/* GPIO channel 0 config */
#define GPIO_0_DEV			PORT->Group[0]
#define GPIO_0_PIN          PIN_PA13 //EXT1 PIN5
#define GPIO_0_EXTINT		13//PIN_PA13A_EIC_EXTINT13
/* GPIO channel 1 config */
#define GPIO_1_DEV			PORT->Group[0]
#define GPIO_1_PIN          PIN_PA28 //EXT1 PIN6, and USER_BUTTON //ACTIVE LOW
#define GPIO_1_EXTINT		8//PIN_PA28A_EIC_EXTINT8
/* GPIO channel 2 config */
#define GPIO_2_DEV			PORT->Group[0]
#define GPIO_2_PIN          PIN_PA15 //EXT3 PIN5
#define GPIO_2_EXTINT		15//PIN_PA15A_EIC_EXTINT15
/* GPIO channel 3 config */
#define GPIO_3_DEV			PORT->Group[0]
#define GPIO_3_PIN 			PIN_PA19 //ACTIVE LOW ON LED
#define GPIO_3_EXTINT       3//PIN_PA19A_EIC_EXTINT3
/* GPIO channel 4 config */
#define GPIO_4_DEV			SPI_0_CS_DEV
#define GPIO_4_PIN          SPI_0_CS_PIN
#define GPIO_4_EXTINT       -1 //No external interrupt availible on this pin
/* GPIO channel 5 config */
#define GPIO_5_DEV			SPI_0_IRQ0_DEV
#define GPIO_5_PIN          SPI_0_IRQ0_PIN
#define GPIO_5_EXTINT       0 
/* GPIO channel 6 config */
#define GPIO_6_DEV			SPI_0_RESET_DEV
#define GPIO_6_PIN			SPI_0_RESET_PIN
#define GPIO_6_EXTINT       -1
/* GPIO channel 7 config */
#define GPIO_7_DEV			SPI_0_SLEEP_DEV
#define GPIO_7_PIN			SPI_0_SLEEP_PIN
#define GPIO_7_EXTINT       -1
/* GPIO channel 8 config */
#define GPIO_8_DEV			SPI_1_CS_DEV
#define GPIO_8_PIN			SPI_1_CS_PIN
#define GPIO_8_EXTINT       -1
/* GPIO channel 9 config */ 
#define GPIO_9_DEV 			
#define GPIO_9_PIN			
#define GPIO_9_EXTINT       
/* GPIO channel 10 config */ 
#define GPIO_10_DEV	
#define GPIO_10_PIN	
#define GPIO_10_EXTINT      
/* GPIO channel 11 config */
#define GPIO_11_DEV			
#define GPIO_11_PIN			
#define GPIO_11_EXTINT      
/* GPIO channel 12 config */
#define GPIO_12_PIN
#define GPIO_12_EXTINT         
/* GPIO channel 13 config */
#define GPIO_13_PIN
#define GPIO_13_EXTINT         
/* GPIO channel 14 config */
#define GPIO_14_PIN 
#define GPIO_14_EXTINT        
/* GPIO channel 15 config */
#define GPIO_15_PIN 
#define GPIO_15_EXTINT        
/** @} */

#endif /* __PERIPH_CONF_H */
/** @} */