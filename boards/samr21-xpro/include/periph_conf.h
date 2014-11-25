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

#define TIMER_NUMOF        (2U)
#define TIMER_0_EN         1
#define TIMER_1_EN         1

/* Timer 0 configuration */
#define TIMER_0_DEV        TC3->COUNT16
#define TIMER_0_CHANNELS   2
#define TIMER_0_MAX_VALUE  (0xffff)
#define TIMER_0_ISR        isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV        TC4->COUNT32
#define TIMER_1_CHANNELS   2
#define TIMER_1_MAX_VALUE  (0xffffffff)
#define TIMER_1_ISR        isr_tc4

/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          SERCOM0->USART
#define UART_0_IRQ          SERCOM0_IRQn
#define UART_0_ISR          isr_sercom0
/* UART 0 pin configuration */
#define UART_0_PORT         (PORT->Group[0])
#define UART_0_TX_PIN       (4)
#define UART_0_RX_PIN       (5)
#define UART_0_PINS         (PORT_PA04 | PORT_PA05)
#define UART_0_REF_F        (8000000UL)
// #define UART_1_DEV          SERCOM0->USART
// #define UART_1_IRQ          SERCOM0_IRQn
// #define UART_1_ISR          isr_sercom0
// /* UART 0 pin configuration */
// #define UART_1_PORT         (PORT->Group[0])
// #define UART_1_TX_PIN       (4)
// #define UART_1_RX_PIN       (5)
// #define UART_1_PINS         (PORT_PA04 | PORT_PA05)
// #define UART_1_REF_F        (8000000UL)

/* UART 1 device configuration */
#define UART_1_DEV            SERCOM2->USART
#define UART_1_IRQ            SERCOM2_IRQn
#define UART_1_ISR            isr_sercom2
/* UART 1 pin configuration */
#define UART_1_PORT         (PORT->Group[0])
#define UART_1_TX_PIN       (14)
#define UART_1_RX_PIN       (15)
#define UART_1_PINS         (PORT_PA14 | PORT_PA15)
#define UART_1_REF_F        (8000000UL)
// #define UART_0_DEV            SERCOM2->USART
// #define UART_0_IRQ            SERCOM2_IRQn
// #define UART_0_ISR            isr_sercom2
// /* UART 1 pin configuration */
// #define UART_0_PORT         (PORT->Group[0])
// #define UART_0_TX_PIN       (14)
// #define UART_0_RX_PIN       (15)
// #define UART_0_PINS         (PORT_PA14 | PORT_PA15)
// #define UART_0_REF_F        (8000000UL)
/** @} */




/**
 * @ I2C Configuration
 * @{
 */
//#define I2C_NUMOF    (1U)
//#define I2C_0_EN    (1)

/* I2C device configuration */


/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (1U)
#define I2C_0_EN           1
#define I2C_1_EN           0
#define I2C_2_EN           0
#define I2C_3_EN           0
#define I2C_IRQ_PRIO       1

#define I2C_0_DEV       SERCOM3->I2CM
#define I2C_0_IRQ       SERCOM3_IRQn
#define I2C_0_ISR       isr_sercom2
/* I2C 0 pin configuration */
#define I2C_0_PORT    (PORT->Group[0])
#define I2C_SDA          PIN_PA16
#define I2C_SCL             PIN_PA17
#define I2C_0_PINS    (PORT_PA16 | PORT_PA17)
#define I2C_0_REF_F      (8000000UL) // Default Clock Source on reset OSC8M - 8MHz 


//#define I2C_0_DEV    SERCOM3->I2CM//SERCOM1->I2CM

/* I2C Default values*/
#define I2C_0_F_REF                                 F_REF
#define I2C_0_CLK_SOURCE                            0
#define I2C_0_RUN_IN_STANDBY                        0
#define I2C_0_START_HOLD_TIME                        SERCOM_I2CM_CTRLA_SDAHOLD(2) //I2C_MASTER_START_HOLD_TIME_300NS_600NS
#define I2C_BUFFER_TIMEOUT                            65535
#define I2C_UNKNOWN_BUS_STATE_TIMEOUT                65535
#define I2C_SCL_LOW_TIMEOUT                            0
#define I2C_INACTIVE_TIMEOUT                        SERCOM_I2CM_CTRLA_INACTOUT(0) // I2C_MASTER_INACTIVE_TIMEOUT_DISABLED


/*I2C pinmux*/
#define I2C_PINMUX_DEV                                (PORT->Group[0])
#define I2C_PINMUX_PAD0_PIN                         PIN_PA16
#define I2C_PINMUX_PAD0_MUXPOSITION                    (I2C_PINMUX_PAD0_PIN & 0xFFFF)
#define I2C_PINMUX_PAD1_PIN                            PIN_PA17
#define I2C_PINMUX_PAD1_MUXPOSITION                    (I2C_PINMUX_PAD1_PIN & 0xFFFF)

//#define I2C_0_PINS         (PORT_PA16 | PORT_PA17)

#define I2C_PINMUX_MUXPOSITION                        (1 << 7) //SYSTEM_PINMUX_GPIO    
#define I2C_PINMUX_DIRECTION                        2 //SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK//0 //SYSTEM_PINMUX_PIN_DIR_INPUT
#define I2C_PINMUX_INPUT_PULL                        1 //SYSTEM_PINMUX_PIN_PULL_UP
#define I2C_PINMUX_POWERSAVE                        0

#define FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED    0 //1
#define FEATURE_I2C_SCL_STRETCH_MODE                 0 //1
#define FEATURE_I2C_SCL_EXTEND_TIMEOUT                 0 //1

#ifdef FEATURE_I2C_SCL_STRETCH_MODE
#define I2C_scl_stretch_only_after_ack_bit             0
#endif
#ifdef FEATURE_I2C_SCL_EXTEND_TIMEOUT
#define I2C_slave_scl_low_extend_timeout             0
#define I2C_master_scl_low_extend_timeout             0
#endif
#define I2C_0_BAUDRATE                                100 // 100KHz
#define I2C_0_TRANSFER_SPEED                        SERCOM_I2CM_CTRLA_SPEED(0) //I2C_MASTER_SPEED_STANDARD_AND_FAST
#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
#define I2C_0_HS_BAUDRATE                            3400
#endif     


/** @} */

/**
 * @ ADC Configuration
 * @{
 */
#define ADC_NUMOF                          (1U)
#define ADC_0_EN                           1
#define ADC_MAX_CHANNELS                   8

/* ADC 0 device configuration */
#define ADC_0_DEV                          ADC
#define    ADC_0_PORT                      (PORT->Group[0])
#define ADC_0_IRQ                          ADC_IRQn     
#define ADC_0_CHANNELS                     8
/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE                   0 /* GCLK_GENERATOR_0 */
#define ADC_0_PRESCALER                    ADC_CTRLB_PRESCALER_DIV4
#define ADC_0_WINDOW_MODE                  ADC_WINCTRL_WINMODE_DISABLE
#define ADC_0_WINDOW_LOWER                 0
#define ADC_0_WINDOW_HIGHER                0     

#define ADC_0_CORRECTION_EN                0 /* disabled */
#define ADC_0_GAIN_CORRECTION              ADC_GAINCORR_RESETVALUE//(0b100100000101)//ADC_GAINCORR_RESETVALUE
#define ADC_0_OFFSET_CORRECTION            ADC_OFFSETCORR_RESETVALUE//(0b000000010000)//ADC_OFFSETCORR_RESETVALUE
#define ADC_0_SAMPLE_LENGTH                0
#define ADC_0_PIN_SCAN_OFFSET_START        0 /* disabled */
#define    ADC_0_PIN_SCAN_INPUT_TO_SCAN    0 /* disabled */    
#define ADC_0_LEFT_ADJUST                  0 /* disabled */
#define ADC_0_DIFFERENTIAL_MODE            0 /* disabled */
#define ADC_0_FREE_RUNNING                 0 /* disabled */
#define ADC_0_EVENT_ACTION                 0 /* disabled */    
#define ADC_0_RUN_IN_STANDBY               0 /* disabled */

/* ADC 0 Module Status flags */
#define ADC_0_STATUS_RESULT_READY          (1UL << 0)
#define ADC_0_STATUS_WINDOW                (1UL << 1)
#define ADC_0_STATUS_OVERRUN               (1UL << 2)     


/* ADC 0 Positive Input Pins */
#define ADC_0_POS_INPUT                    ADC_INPUTCTRL_MUXPOS_PIN6

/* ADC 0 Negative Input Pins */     
#define ADC_0_NEG_INPUT                    ADC_INPUTCTRL_MUXNEG_GND

/* ADC 0 Gain Factor */
#define ADC_0_GAIN_FACTOR_1X               ADC_INPUTCTRL_GAIN_1X
#define ADC_0_GAIN_FACTOR_2X               ADC_INPUTCTRL_GAIN_2X
#define ADC_0_GAIN_FACTOR_4X               ADC_INPUTCTRL_GAIN_4X
#define ADC_0_GAIN_FACTOR_8X               ADC_INPUTCTRL_GAIN_8X
#define ADC_0_GAIN_FACTOR_16X              ADC_INPUTCTRL_GAIN_16X
/* Use this to define the value used */
#define ADC_0_GAIN_FACTOR_DEFAULT          ADC_0_GAIN_FACTOR_1X

/* ADC 0 Resolutions */    
#define ADC_0_RES_8BIT                     ADC_CTRLB_RESSEL_8BIT
#define ADC_0_RES_10BIT                    ADC_CTRLB_RESSEL_10BIT
#define ADC_0_RES_12BIT                    ADC_CTRLB_RESSEL_12BIT
#define ADC_0_RES_16BIT                    ADC_CTRLB_RESSEL_16BIT 

/* ADC 0 Voltage reference */
#define ADC_0_REF_INT_1V                   ADC_REFCTRL_REFSEL_INT1V
#define ADC_0_REF_EXT_B                    ADC_REFCTRL_REFSEL_AREFB
#define ADC_0_REF_COM_EN        0
/* Use this to define the value used */ 
#define ADC_0_REF_DEFAULT                  ADC_0_REF_EXT_B

/* ADC 0 ACCUMULATE */
#define ADC_0_ACCUM_DISABLE                ADC_AVGCTRL_SAMPLENUM_1
#define ADC_0_ACCUM_2                      ADC_AVGCTRL_SAMPLENUM_2
#define ADC_0_ACCUM_4                      ADC_AVGCTRL_SAMPLENUM_4
#define ADC_0_ACCUM_8                      ADC_AVGCTRL_SAMPLENUM_8
#define ADC_0_ACCUM_16                     ADC_AVGCTRL_SAMPLENUM_16
#define ADC_0_ACCUM_32                     ADC_AVGCTRL_SAMPLENUM_32
#define ADC_0_ACCUM_64                     ADC_AVGCTRL_SAMPLENUM_64
#define ADC_0_ACCUM_128                    ADC_AVGCTRL_SAMPLENUM_128
#define ADC_0_ACCUM_256                    ADC_AVGCTRL_SAMPLENUM_256
#define ADC_0_ACCUM_512                    ADC_AVGCTRL_SAMPLENUM_512
#define ADC_0_ACCUM_1024                   ADC_AVGCTRL_SAMPLENUM_1024
/* Use this to define the value used */
#define ADC_0_ACCUM_DEFAULT                ADC_0_ACCUM_1024 

/* ADC 0 DEVIDE RESULT */
#define ADC_0_DIV_RES_DISABLE              0
#define ADC_0_DIV_RES_2                    1
#define ADC_0_DIV_RES_4                    2
#define ADC_0_DIV_RES_8                    3
#define ADC_0_DIV_RES_16                   4
#define ADC_0_DIV_RES_32                   5
#define ADC_0_DIV_RES_64                   6
#define ADC_0_DIV_RES_128                  7
/* Use this to define the value used */
#define ADC_0_DIV_RES_DEFAULT             ADC_0_DIV_RES_DISABLE 

/* ADC 0 channel 0 pin config - NOT implemented yet */
#define ADC_0_CH0               PORT_PB08B_ADC_AIN2
#define ADC_0_CH0_PIN           PIN_PB08B_ADC_AIN2
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1               PORT_PB09B_ADC_AIN3
#define ADC_0_CH1_PIN           PIN_PB09B_ADC_AIN3
/* ADC 0 channel 2 pin config */
#define ADC_0_CH2               PORT_PA04B_ADC_AIN4
#define ADC_0_CH2_PIN              PIN_PA04B_ADC_AIN4
/* ADC 0 channel 3 pin config */
#define ADC_0_CH3               PORT_PA05B_ADC_AIN5
#define ADC_0_CH3_PIN           PIN_PA05B_ADC_AIN5
/* ADC 0 channel 4 pin config */
#define ADC_0_CH4               PORT_PA06B_ADC_AIN6
#define ADC_0_CH4_PIN           PIN_PA06B_ADC_AIN6
/* ADC 0 channel 5 pin config */
#define ADC_0_CH5               PORT_PA07B_ADC_AIN7
#define ADC_0_CH5_PIN           PIN_PA07B_ADC_AIN7
/* ADC 0 channel 6 pin config */
#define ADC_0_CH6               PORT_PB00B_ADC_AIN8
#define ADC_0_CH6_PIN           PIN_PB00B_ADC_AIN8
/* ADC 0 channel 7 pin config */
#define ADC_0_CH7               PORT_PB02B_ADC_AIN10
#define ADC_0_CH7_PIN           PIN_PB02B_ADC_AIN10
/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF          (2)
#define SPI_0_EN           1
#define SPI_1_EN           1

/*      SPI0             */
#define SPI_0_DEV          SERCOM4->SPI
#define SPI_IRQ_0          SERCOM4_IRQn
#define SPI_0_DOPO         (1)
#define SPI_0_DIPO         (0)
#define SPI_0_F_REF        (8000000UL)

#define SPI_0_SCLK_DEV     PORT->Group[2]
#define SPI_0_SCLK_PIN     (18)

#define SPI_0_MISO_DEV     PORT->Group[2]
#define SPI_0_MISO_PIN     (19)

#define SPI_0_MOSI_DEV     PORT->Group[1]
#define SPI_0_MOSI_PIN     (30)

/*      SPI1             */
#define SPI_1_DEV          SERCOM5->SPI
#define SPI_IRQ_1          SERCOM5_IRQn
#define SPI_1_DOPO         (1)
#define SPI_1_DIPO         (2)
#define SPI_1_F_REF        (8000000UL)

#define SPI_1_SCLK_DEV     PORT->Group[1]
#define SPI_1_SCLK_PIN     (23)

#define SPI_1_MISO_DEV     PORT->Group[1]
#define SPI_1_MISO_PIN     (02)

#define SPI_1_MOSI_DEV     PORT->Group[1]
#define SPI_1_MOSI_PIN     (22)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF       (0U)
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF         (9U)
#define GPIO_0_EN          1
#define GPIO_1_EN          1
#define GPIO_2_EN          0
#define GPIO_3_EN          1
/*4-7 -> internal */
#define GPIO_4_EN          1
#define GPIO_5_EN          1
#define GPIO_6_EN          1
#define GPIO_7_EN          1
#define GPIO_8_EN          1
#define GPIO_9_EN          0
#define GPIO_10_EN         0
#define GPIO_11_EN         0

#define GPIO_12_EN         0
#define GPIO_13_EN         0
#define GPIO_14_EN         0
#define GPIO_15_EN         0

#define GPIO_NO_EXTINT     (18)

/* GPIO channel 0 config */
#define GPIO_0_DEV         PORT->Group[0]
#define GPIO_0_PIN         (13)
#define GPIO_0_EXTINT      (13)
/* GPIO channel 1 config */
#define GPIO_1_DEV         PORT->Group[0]
#define GPIO_1_PIN         (28)
#define GPIO_1_EXTINT      (8)
/* GPIO channel 2 config */
#define GPIO_2_DEV         //PORT->Group[0]
#define GPIO_2_PIN         //(15)
#define GPIO_2_EXTINT      //(15)
/* GPIO channel 3 config */
#define GPIO_3_DEV         PORT->Group[0]
#define GPIO_3_PIN         (19)
#define GPIO_3_EXTINT      (3)
/* GPIO 4-7 Internal radio pins*/
/* GPIO channel 4 config  radio CS*/
#define GPIO_4_DEV         PORT->Group[1]
#define GPIO_4_PIN         (31)
#define GPIO_4_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 5 config  radio IRQ0*/
#define GPIO_5_DEV         PORT->Group[1]
#define GPIO_5_PIN         (0)
#define GPIO_5_EXTINT      (0)
/* GPIO channel 6 config  radio reset*/
#define GPIO_6_DEV         PORT->Group[1]
#define GPIO_6_PIN         (15)
#define GPIO_6_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 7 config radio sleep*/
#define GPIO_7_DEV         PORT->Group[0]
#define GPIO_7_PIN         (20)
#define GPIO_7_EXTINT      GPIO_NO_EXTINT
/* GPIO channel 8 config */
#define GPIO_8_DEV         PORT->Group[0]
#define GPIO_8_PIN         (27)
#define GPIO_8_EXTINT      GPIO_NO_EXTINT
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

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
