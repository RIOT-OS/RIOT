/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_stm32f3discovery
 * @{
 *
 * @file        periph_conf.h
 * @brief       Peripheral MCU configuration for the STM32F3discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H


/**
 * @brief Timer configuration
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
// #define TIMER_2_EN          0

// Timer 0 configuration
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (71U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE)
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQCHAN     TIM2_IRQn
#define TIMER_0_IRQ_PRIO    1

// Timer 1 configuration
#define TIMER_1_DEV         TIM6
#define TIMER_1_CHANNELS    1
#define TIMER_1_PRESCALER   (71U)
#define TIMER_1_MAX_VALUE   (0xffff)
#define TIMER_1_CLKEN()     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
#define TIMER_1_ISR         isr_tim6_dac
#define TIMER_1_IRQCHAN     TIM3_IRQn
#define TIMER_1_IRQ_PRIO    1

// Timer 2 configuration
#define TIMER_2_DEV         TIM7
#define TIMER_2_CHANNELS    1
#define TIMER_2_PRESCALER   (71U)
#define TIMER_2_MAX_VALUE   (0xffff)
#define TIMER_2_CLKEN()     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE)
#define TIMER_2_ISR         isr_tim7
#define TIMER_2_IRQCHAN     TIM3_IRQn
#define TIMER_2_IRQ_PRIO    1



/**
 * @brief ADC configuration
 */
#define ADC_NUMOF           (1U)
#define ADC_0_EN            1
#define ADC_1_EN            0

// ADC 0 configuration
#define ADC_0_DEV           ADC1                                                        // TODO !!!!!!!
#define ADC_0_SAMPLE_TIMER  
// ADC 0 channel 0 pin config
#define ADC_0_C0_PORT       
#define ADC_0_C0_PIN        
#define ADC_0_C0_CLKEN()    
#define ADC_0_C0_AFCFG()    
// ADC 0 channel 1 pin config
#define ADC_0_C1_PORT       
#define ADC_0_C1_PIN        
#define ADC_0_C1_CLKEN()    
#define ADC_0_C1_AFCFG()    
// ADC 0 channel 2 pin config
#define ADC_0_C2_PORT       
#define ADC_0_C2_PIN        
#define ADC_0_C2_CLKEN()    
#define ADC_0_C2_AFCFG()    
// ADC 0 channel 3 pin config
#define ADC_0_C3_PORT       
#define ADC_0_C3_PIN        
#define ADC_0_C3_CLKEN()    
#define ADC_0_C3_AFCFG()    

// ADC 0 configuration
#define ADC_1_DEV           ADC2                                                        // TODO !!!!!!!
#define ADC_1_SAMPLE_TIMER  
// ADC 0 channel 0 pin config
#define ADC_1_C0_PORT       
#define ADC_1_C0_PIN        
#define ADC_1_C0_CLKEN()    
#define ADC_1_C0_AFCFG()    
// ADC 0 channel 1 pin config
#define ADC_1_C1_PORT       
#define ADC_1_C1_PIN        
#define ADC_1_C1_CLKEN()    
#define ADC_1_C1_AFCFG()    
// ADC 0 channel 2 pin config
#define ADC_1_C2_PORT       
#define ADC_1_C2_PIN        
#define ADC_1_C2_CLKEN()    
#define ADC_1_C2_AFCFG()    
// ADC 0 channel 3 pin config
#define ADC_1_C3_PORT       
#define ADC_1_C3_PIN        
#define ADC_1_C3_CLKEN()    
#define ADC_1_C3_AFCFG()    


/**
 * @brief PWM configuration
 */
#define PWM_NUMOF           (0U)                                                        // TODO !!!!!!!
#define PWM_0_EN            0
#define PWM_1_EN            0

/* PWM 0 device configuration */
#define PWM_0_DEV           TIM3
#define PWM_0_CHANNELS      4
/* PWM 0 pin configuration */
#define PWM_0_PORT          
#define PWM_0_PINS          
#define PWM_0_PORT_CLKEN()  
#define PWM_0_CH1_AFCFG()   
#define PWM_0_CH2_AFCFG()   
#define PWM_0_CH3_AFCFG()   
#define PWM_0_CH4_AFCFG()   

/* PWM 1 device configuration */
#define PWM_1_DEV           TIM4
#define PWM_1_CHANNELS      4
/* PWM 1 pin configuration */
#define PWM_1_PORT          
#define PWM_1_PINS          
#define PWM_1_PORT_CLKEN()  
#define PWM_1_CH1_AFCFG()   
#define PWM_1_CH2_AFCFG()   
#define PWM_1_CH3_AFCFG()   
#define PWM_1_CH4_AFCFG()   


/**
 * @brief UART configuration
 */
#define UART_NUMOF          (3U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           1
#define UART_IRQ_PRIO       1

// UART 0 device configuration
#define UART_0_DEV          USART1
#define UART_0_CLKEN()      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define UART_0_IRQ          USART1_IRQn
#define UART_0_ISR          isr_usart1
// UART 0 pin configuration
#define UART_0_PORT         GPIOA
#define UART_0_PINS         (GPIO_Pin_9 | GPIO_Pin_10)
#define UART_0_PORT_CLKEN() RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE)
#define UART_0_RX_AFCFG()   (GPIO_PinAFConfig(UART_0_PORT, GPIO_PinSource9, GPIO_AF_7))
#define UART_0_TX_AFCFG()   (GPIO_PinAFConfig(UART_0_PORT, GPIO_PinSource10, GPIO_AF_7))

// UART 1 device configuration
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
#define UART_1_IRQ          USART2_IRQn
#define UART_1_ISR          isr_usart2
// UART 1 pin configuration
#define UART_1_PORT         GPIOD
#define UART_1_PINS         (GPIO_Pin_5 | GPIO_Pin_6)
#define UART_1_PORT_CLKEN() RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE)
#define UART_1_RX_AFCFG()   GPIO_PinAFConfig(UART_1_PORT, GPIO_PinSource5, GPIO_AF_7)
#define UART_1_TX_AFCFG()   GPIO_PinAFConfig(UART_1_PORT, GPIO_PinSource6, GPIO_AF_7)

// UART 2 device configuration
#define UART_2_DEV          USART3
#define UART_2_CLKEN()      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE)
#define UART_2_IRQ          USART3_IRQn
#define UART_2_ISR          isr_usart3
// UART 2 pin configuration
#define UART_2_PORT         GPIOD
#define UART_2_PINS         (GPIO_Pin_8 | GPIO_Pin_9)
#define UART_2_PORT_CLKEN() RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE)
#define UART_2_RX_AFCFG()   GPIO_PinAFConfig(UART_2_PORT, GPIO_PinSource8, GPIO_AF_7)
#define UART_2_TX_AFCFG()   GPIO_PinAFConfig(UART_2_PORT, GPIO_PinSource9, GPIO_AF_7)



/**
 * @brief SPI configuration
 */
#define SPI_NUMOF           (0U)                                                        // TODO !!!!!!!
#define SPI_0_EN            0
#define SPI_1_EN            0

// SPI 0 device config
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()       
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_IRQ_HANDLER   
#define SPI_0_IRQ_PRIO      1
// SPI 1 pin configuration
#define SPI_0_PORT          
#define SPI_0_PINS          ()
#define SPI_1_PORT_CLKEN()  
#define SPI_1_SCK_AFCFG()   
#define SPI_1_MISO_AFCFG()  
#define SPI_1_MOSI_AFCFG()  

// SPI 1 device config
#define SPI_1_DEV           SPI3
#define SPI_1_CLKEN()       
#define SPI_1_IRQ           SPI3_IRQn
#define SPI_1_IRQ_HANDLER   
#define SPI_1_IRQ_PRIO      1
// SPI 1 pin configuration
#define SPI_1_PORT          
#define SPI_1_PINS          ()
#define SPI_1_PORT_CLKEN()  
#define SPI_1_SCK_AFCFG()   
#define SPI_1_MISO_AFCFG()  
#define SPI_1_MOSI_AFCFG()  


/**
 * @brief I2C configuration
 */
#define I2C_NUMOF           (0U)
#define I2C_0_EN            0
#define I2C_0_EN            0

/* SPI 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       
#define I2C_0_ISR           isr_i2c1
#define I2C_0_IRQ           I2C1_IRQn
#define I2C_0_IRQ_PRIO      1
/* SPI 0 pin configuration */
#define I2C_0_PORT          GPIOB
#define I2C_0_PINS          (GPIO_Pin_6 | GPIO_Pin_7)
#define I2C_0_PORT_CLKEN()  
#define I2C_0_SCL_AFCFG()   
#define I2C_0_SDA_AFCFG()   

/* SPI 1 device configuration */
#define I2C_1_DEV           I2C2
#define I2C_1_CLKEN()       
#define I2C_1_ISR           isr_i2c2
#define I2C_1_IRQ           I2C2_IRQn
#define I2C_1_IRQ_PRIO      1
/* SPI 1 pin configuration */
#define I2C_1_PORT          GPIOF
#define I2C_1_PINS          (GPIO_Pin_0 | GPIO_Pin_1)
#define I2C_1_PORT_CLKEN()  
#define I2C_1_SCL_AFCFG()   
#define I2C_1_SDA_AFCFG()   


/**
 * @brief GPIO configuration
 */
#define GPIO_NUMOF          16
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_IRQ_PRIO       1

// IRQ config
#define GPIO_IRQ_0          GPIO_7 // alternative GPIO_4
#define GPIO_IRQ_1          GPIO_5
#define GPIO_IRQ_2          GPIO_0
#define GPIO_IRQ_3          GPIO_3
#define GPIO_IRQ_4          GPIO_1
#define GPIO_IRQ_5          GPIO_2
#define GPIO_IRQ_6          GPIO_6
#define GPIO_IRQ_7          GPIO_1        // Channel unconfigured
#define GPIO_IRQ_8          GPIO_9
#define GPIO_IRQ_9          GPIO_8
#define GPIO_IRQ_10         GPIO_10
#define GPIO_IRQ_11         GPIO_12
#define GPIO_IRQ_12         GPIO_14
#define GPIO_IRQ_13         GPIO_15
#define GPIO_IRQ_14         GPIO_13
#define GPIO_IRQ_15         GPIO_11

// GPIO channel 0 config
#define GPIO_0_PORT         GPIOE                   // Used for LSM303DLHC DRDY
#define GPIO_0_PIN          GPIO_Pin_2
#define GPIO_0_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_0_LINE         EXTI_Line2
#define GPIO_0_IRQ          EXTI2_TS_IRQn
#define GPIO_0_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2)
// GPIO channel 1 config
#define GPIO_1_PORT         GPIOE                   // Used for LSM303DLHC INT 1
#define GPIO_1_PIN          GPIO_Pin_4
#define GPIO_1_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_1_LINE         EXTI_Line4
#define GPIO_1_IRQ          EXTI4_IRQn
#define GPIO_1_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4)
// GPIO channel 2 config
#define GPIO_2_PORT         GPIOE                   // Used for LSM303DLHC INT 2
#define GPIO_2_PIN          GPIO_Pin_5
#define GPIO_2_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_2_LINE         EXTI_Line5
#define GPIO_2_IRQ          EXTI9_5_IRQn
#define GPIO_2_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5)
// GPIO channel 3 config
#define GPIO_3_PORT         GPIOE                   // Used for L3GD20 CS
#define GPIO_3_PIN          GPIO_Pin_3
#define GPIO_3_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_3_LINE         EXTI_Line3
#define GPIO_3_IRQ          EXTI0_IRQn
#define GPIO_3_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3)
// GPIO channel 4 config
#define GPIO_4_PORT         GPIOE                   // Used for L3GD20 INT1
#define GPIO_4_PIN          GPIO_Pin_0
#define GPIO_4_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_4_LINE         EXTI_Line0
#define GPIO_4_IRQ          EXTI0_IRQn
#define GPIO_4_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0)
// GPIO channel 5 config
#define GPIO_5_PORT         GPIOE                   // Used for L3GD20 INT2
#define GPIO_5_PIN          GPIO_Pin_1
#define GPIO_5_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_5_LINE         EXTI_Line1
#define GPIO_5_IRQ          EXTI1_IRQn
#define GPIO_5_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1)
// GPIO channel 6 config
#define GPIO_6_PORT         GPIOC                   // defined to PC.6
#define GPIO_6_PIN          GPIO_Pin_6
#define GPIO_6_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE)
#define GPIO_6_LINE         EXTI_Line6
#define GPIO_6_IRQ          EXTI9_5_IRQn
#define GPIO_6_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6)
// GPIO channel 7 config
#define GPIO_7_PORT         GPIOA                   // user button 1
#define GPIO_7_PIN          GPIO_Pin_0
#define GPIO_7_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE)
#define GPIO_7_LINE         EXTI_Line0
#define GPIO_7_IRQ          EXTI0_IRQn
#define GPIO_7_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0)
// GPIO channel 8 config
#define GPIO_8_PORT         GPIOE                   // LED 3
#define GPIO_8_PIN          GPIO_Pin_9
#define GPIO_8_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_8_LINE         EXTI_Line9
#define GPIO_8_IRQ          EXTI9_5_IRQn
#define GPIO_8_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource9)
// GPIO channel 9 config
#define GPIO_9_PORT         GPIOE                   // LED 4
#define GPIO_9_PIN          GPIO_Pin_8
#define GPIO_9_CLKEN()      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_9_LINE         EXTI_Line8
#define GPIO_9_IRQ          EXTI9_5_IRQn
#define GPIO_9_EXTI_MAP()   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8)
// GPIO channel 10 config
#define GPIO_10_PORT        GPIOE                   // LED 5
#define GPIO_10_PIN         GPIO_Pin_10
#define GPIO_10_CLKEN()     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_10_LINE        EXTI_Line10
#define GPIO_10_IRQ         EXTI15_10_IRQn
#define GPIO_10_EXTI_MAP()  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10)
// GPIO channel 11 config
#define GPIO_11_PORT        GPIOE                   // LED 6
#define GPIO_11_PIN         GPIO_Pin_15
#define GPIO_11_CLKEN()     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_11_LINE        EXTI_Line15
#define GPIO_11_IRQ         EXTI15_10_IRQn
#define GPIO_11_EXTI_MAP()  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource15)
// GPIO channel 12 config
#define GPIO_12_PORT        GPIOE                   // LED 7
#define GPIO_12_PIN         GPIO_Pin_11
#define GPIO_12_CLKEN()     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_12_LINE        EXTI_Line11
#define GPIO_12_IRQ         EXTI15_10_IRQn
#define GPIO_12_EXTI_MAP()  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource11)
// GPIO channel 13 config
#define GPIO_13_PORT        GPIOE                   // LED 8
#define GPIO_13_PIN         GPIO_Pin_14
#define GPIO_13_CLKEN()     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_13_LINE        EXTI_Line14
#define GPIO_13_IRQ         EXTI15_10_IRQn
#define GPIO_13_EXTI_MAP()  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource14)
// GPIO channel 14 config
#define GPIO_14_PORT        GPIOE                   // LED 9
#define GPIO_14_PIN         GPIO_Pin_12
#define GPIO_14_CLKEN()     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_14_LINE        EXTI_Line12
#define GPIO_14_IRQ         EXTI15_10_IRQn
#define GPIO_14_EXTI_MAP()  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource12)
// GPIO channel 15 config
#define GPIO_15_PORT        GPIOE                   // LED 10
#define GPIO_15_PIN         GPIO_Pin_13
#define GPIO_15_CLKEN()     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE)
#define GPIO_15_LINE        EXTI_Line13
#define GPIO_15_IRQ         EXTI15_10_IRQn
#define GPIO_15_EXTI_MAP()  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource13)


#endif /* __PERIPH_CONF_H */
