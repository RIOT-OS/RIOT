/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */


/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the Eistec Mulle
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef MULLE_PERIPH_CONF_H
#define MULLE_PERIPH_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define KINETIS_CPU_USE_MCG               1

#define KINETIS_MCG_USE_ERC               1
#define KINETIS_MCG_USE_PLL               0
#define KINETIS_MCG_DCO_RANGE             (96000000U)
#define KINETIS_MCG_ERC_OSCILLATOR        0
#define KINETIS_MCG_ERC_FRDIV             0
#define KINETIS_MCG_ERC_RANGE             0
#define KINETIS_MCG_ERC_FREQ              (32768U)

/* Base clocks, used by SystemCoreClockUpdate */
/** Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL_CLK_HZ                 8000000u
/** Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              32768u
/** Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_SLOW_CLK_HZ             32768u
/** Value of the fast internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             4000000u
/** Default System clock value */
#define DEFAULT_SYSTEM_CLOCK            (CPU_XTAL32k_CLK_HZ * 2929u)

/** @todo Investigate the side effects of making F_CPU run-time variable */
#define F_CPU DEFAULT_SYSTEM_CLOCK

/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF             (1U)
#define TIMER_0_EN              1
#define TIMER_1_EN              0
#define TIMER_IRQ_PRIO          1
#define TIMER_DEV               PIT
#define TIMER_MAX_VALUE         (0xffffffff)
#define TIMER_CLOCK             SystemBusClock
#define TIMER_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_PIT_SHIFT) = 1)

/* Timer 0 configuration */
#define TIMER_0_PRESCALER_CH    0
#define TIMER_0_COUNTER_CH      1
#define TIMER_0_ISR             isr_pit1
#define TIMER_0_IRQ_CHAN        PIT1_IRQn

/* Timer 1 configuration */
#define TIMER_1_PRESCALER_CH    2
#define TIMER_1_COUNTER_CH      3
#define TIMER_1_ISR             isr_pit3
#define TIMER_1_IRQ_CHAN        PIT3_IRQn

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
#define UART_4_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART1
#define UART_0_CLKEN()      (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_UART1_SHIFT) = 1)
#define UART_0_CLKDIS()     (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_UART1_SHIFT) = 0)
#define UART_0_CLK          (SystemSysClock)
#define UART_0_IRQ_CHAN     UART1_RX_TX_IRQn
#define UART_0_ISR          isr_uart1_status
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define UART_0_PORT         PORTC
#define UART_0_TX_PIN       4
#define UART_0_RX_PIN       3
/* Function number in pin multiplex, see K60 Sub-Family Reference Manual,
 * section 10.3.1 K60 Signal Multiplexing and Pin Assignments */
#define UART_0_AF           3
#define UART_0_TX_PCR_MUX   3
#define UART_0_RX_PCR_MUX   3

/* UART 1 device configuration */
#define UART_1_DEV          UART0
#define UART_1_CLKEN()      (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_UART0_SHIFT) = 1)
#define UART_1_CLKDIS()     (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_UART0_SHIFT) = 0)
#define UART_1_CLK          (SystemSysClock)
#define UART_1_IRQ_CHAN     UART0_RX_TX_IRQn
#define UART_1_ISR          isr_uart0_status
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTA_SHIFT) = 1)
#define UART_1_PORT         PORTA
#define UART_1_TX_PIN       14
#define UART_1_RX_PIN       15
/* Function number in pin multiplex, see K60 Sub-Family Reference Manual,
 * section 10.3.1 K60 Signal Multiplexing and Pin Assignments */
#define UART_1_AF           3
#define UART_1_TX_PCR_MUX   3
#define UART_1_RX_PCR_MUX   3

/** @} */


/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (2U)
#define ADC_0_EN            1
#define ADC_1_EN            1
#define ADC_MAX_CHANNELS    6

/* ADC 0 configuration */
#define ADC_0_DEV           ADC0
#define ADC_0_CHANNELS      4
#define ADC_0_CLKEN()       (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_ADC0_SHIFT) = 1)
#define ADC_0_CLKDIS()      (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_ADC0_SHIFT) = 0)
#define ADC_0_PORT_CLKEN()  /* no PORT pins configured */
#define ADC_0_MODULE_CLOCK  SystemBusClock
/* ADC 0 channel 0 pin config */
#define ADC_0_CH0           26 /* Temp sensor channel */
#define ADC_0_CH0_PORT      0 /* this channel is not part of the pin mux on this CPU */
#define ADC_0_CH0_PIN       0
#define ADC_0_CH0_PIN_AF    0
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1           27 /* Band gap channel */
#define ADC_0_CH1_PORT      0 /* this channel is not part of the pin mux on this CPU */
#define ADC_0_CH1_PIN       0
#define ADC_0_CH1_PIN_AF    0
/* ADC 0 channel 2 pin config */
#define ADC_0_CH2           29 /* V_REFSH */
#define ADC_0_CH2_PORT      0 /* this channel is not part of the pin mux on this CPU */
#define ADC_0_CH2_PIN       0
#define ADC_0_CH2_PIN_AF    0
/* ADC 0 channel 3 pin config */
#define ADC_0_CH3           30 /* V_REFSL */
#define ADC_0_CH3_PORT      0 /* this channel is not part of the pin mux on this CPU */
#define ADC_0_CH3_PIN       0
#define ADC_0_CH3_PIN_AF    0
/* ADC 0 channel 4 pin config */
#define ADC_0_CH4           4
#define ADC_0_CH4_PORT      0
#define ADC_0_CH4_PIN       0
#define ADC_0_CH4_PIN_AF    0
/* ADC 0 channel 5 pin config */
#define ADC_0_CH5           5
#define ADC_0_CH5_PORT      0
#define ADC_0_CH5_PIN       0
#define ADC_0_CH5_PIN_AF    0

/* ADC 1 configuration */
#define ADC_1_DEV           ADC1
#define ADC_1_CHANNELS      2
#define ADC_1_CLKEN()       (BITBAND_REG(SIM->SCGC3, SIM_SCGC3_ADC1_SHIFT) = 1)
#define ADC_1_CLKDIS()      (BITBAND_REG(SIM->SCGC3, SIM_SCGC3_ADC1_SHIFT) = 0)
#define ADC_1_PORT_CLKEN()  /* no PORT pins configured */
#define ADC_1_MODULE_CLOCK  SystemBusClock
/* ADC 1 channel 0 pin config */
#define ADC_1_CH0           0 /* DADP0, connected externally to Mulle Vbat/2 on PGA1_DP */
#define ADC_1_CH0_PORT      0 /* this channel is not part of the pin mux on this CPU */
#define ADC_1_CH0_PIN       0
#define ADC_1_CH0_PIN_AF    0
/* ADC 1 channel 1 pin config */
#define ADC_1_CH1           19 /* AD19, connected externally to Mulle Vchr/2 on PGA1_DM */
#define ADC_1_CH1_PORT      0  /* this channel is not part of the pin mux on this CPU */
#define ADC_1_CH1_PIN       0
#define ADC_1_CH1_PIN_AF    0
/* ADC 1 channel 2 pin config */
#define ADC_1_CH2           12
#define ADC_1_CH2_PIN       0
#define ADC_1_CH2_PIN_AF    0
#define ADC_1_CH2_PORT      0
/* ADC 1 channel 3 pin config */
#define ADC_1_CH3           13
#define ADC_1_CH3_PIN       0
#define ADC_1_CH3_PIN_AF    0
#define ADC_1_CH3_PORT      0
/* ADC 1 channel 4 pin config */
#define ADC_1_CH4           14
#define ADC_1_CH4_PIN       0
#define ADC_1_CH4_PIN_AF    0
#define ADC_1_CH4_PORT      0
/* ADC 1 channel 5 pin config */
#define ADC_1_CH5           15
#define ADC_1_CH5_PIN       0
#define ADC_1_CH5_PIN_AF    0
#define ADC_1_CH5_PORT      0
/** @} */


/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (0U)
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    4

/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           3
#define SPI_0_EN            1
#define SPI_1_EN            1
#define SPI_2_EN            1
#define SPI_3_EN            0
#define SPI_4_EN            0
#define SPI_5_EN            0
#define SPI_6_EN            0
#define SPI_7_EN            0

#define MULLE_PASTE_PARTS(left, index, right) MULLE_PASTE_PARTS2(left, index, right)
#define MULLE_PASTE_PARTS2(left, index, right) left##index##right

/* SPI 0 device config */
/* SPI_0 (in RIOT) is mapped to SPI0, CTAS=0 in hardware */
#define SPI_0_INDEX             0
#define SPI_0_CTAS              0
#define SPI_0_DEV               MULLE_PASTE_PARTS(SPI, SPI_0_INDEX, )
#define SPI_0_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 1)
#define SPI_0_CLKDIS()          (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 0)
#define SPI_0_IRQ               MULLE_PASTE_PARTS(SPI, SPI_0_INDEX, _IRQn)
#define SPI_0_IRQ_HANDLER       MULLE_PASTE_PARTS(isr_spi, SPI_0_INDEX, )
#define SPI_0_IRQ_PRIO          1
#define SPI_0_FREQ              SystemBusClock
/* SPI 0 pin configuration */
#define SPI_0_SCK_PORT          PORTD
#define SPI_0_SCK_PIN           1
#define SPI_0_SCK_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_SCK_AF            2
#define SPI_0_SIN_PORT          PORTD
#define SPI_0_SIN_PIN           3
#define SPI_0_SIN_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_SIN_AF            2
#define SPI_0_SOUT_PORT         PORTD
#define SPI_0_SOUT_PIN          2
#define SPI_0_SOUT_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_SOUT_AF  2
#define SPI_0_PCS0_PORT         PORTD
#define SPI_0_PCS0_PIN          0
#define SPI_0_PCS0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_0_PCS0_ACTIVE_LOW   1
#define SPI_0_PCS1_ACTIVE_LOW   1
#define SPI_0_PCS2_ACTIVE_LOW   1
#define SPI_0_PCS3_ACTIVE_LOW   1

/* SPI 1 device config */
/* SPI_1 (in RIOT) is mapped to SPI1, CTAS=0 in hardware */
#define SPI_1_INDEX             1
#define SPI_1_CTAS              0
#define SPI_1_DEV               MULLE_PASTE_PARTS(SPI, SPI_1_INDEX, )
#define SPI_1_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI1_SHIFT) = 1)
#define SPI_1_CLKDIS()          (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI1_SHIFT) = 0)
#define SPI_1_IRQ               MULLE_PASTE_PARTS(SPI, SPI_1_INDEX, _IRQn)
#define SPI_1_IRQ_HANDLER       MULLE_PASTE_PARTS(isr_spi, SPI_1_INDEX, )
#define SPI_1_IRQ_PRIO          1
#define SPI_1_FREQ              SystemBusClock
/* SPI 0 pin configuration */
#define SPI_1_SCK_PORT          PORTE
#define SPI_1_SCK_PIN           2
#define SPI_1_SCK_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_SCK_AF            2
#define SPI_1_SIN_PORT          PORTE
#define SPI_1_SIN_PIN           3
#define SPI_1_SIN_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_SIN_AF            2
#define SPI_1_SOUT_PORT         PORTE
#define SPI_1_SOUT_PIN          1
#define SPI_1_SOUT_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_SOUT_AF  2
#define SPI_1_PCS0_PORT         PORTE
#define SPI_1_PCS0_PIN          4
#define SPI_1_PCS0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_1_PCS0_ACTIVE_LOW   1
#define SPI_1_PCS1_ACTIVE_LOW   1
#define SPI_1_PCS2_ACTIVE_LOW   1
#define SPI_1_PCS3_ACTIVE_LOW   1

/* SPI 2 device config */
/* SPI_2 (in RIOT) is mapped to SPI0, CTAS=1 in hardware */
#define SPI_2_INDEX             0
#define SPI_2_CTAS              1
#define SPI_2_DEV               MULLE_PASTE_PARTS(SPI, SPI_2_INDEX, )
#define SPI_2_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 1)
#define SPI_2_CLKDIS()          (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 0)
#define SPI_2_IRQ               MULLE_PASTE_PARTS(SPI, SPI_2_INDEX, _IRQn)
/* #define SPI_2_IRQ_HANDLER       MULLE_PASTE_PARTS(isr_spi, SPI_2_INDEX, ) */
#define SPI_2_IRQ_PRIO          1
#define SPI_2_FREQ              SystemBusClock
/* SPI 2 pin configuration, must be the same as the other RIOT device using this
 * hardware module */
#define SPI_2_SCK_PORT          PORTD
#define SPI_2_SCK_PIN           1
#define SPI_2_SCK_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_SCK_AF            2
#define SPI_2_SIN_PORT          PORTD
#define SPI_2_SIN_PIN           3
#define SPI_2_SIN_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_SIN_AF            2
#define SPI_2_SOUT_PORT         PORTD
#define SPI_2_SOUT_PIN          2
#define SPI_2_SOUT_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_SOUT_AF  2
#define SPI_2_PCS0_PORT         PORTD
#define SPI_2_PCS0_PIN          0
#define SPI_2_PCS0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_2_PCS0_ACTIVE_LOW   1
#define SPI_2_PCS1_ACTIVE_LOW   1
#define SPI_2_PCS2_ACTIVE_LOW   1
#define SPI_2_PCS3_ACTIVE_LOW   1

/**
 * @name SPI delay timing configuration
 * @{ */
/* These values are necessary for communicating with the AT86RF212B when running
 * the MCU core at high clock frequencies. */
/* NB: The given values are the reciprocals of the time, in order to compute the
 * scalers using only integer math. */
#define SPI_0_TCSC_FREQ (5555555) /* It looks silly, but this is correct. 1/180e-9 */
#define SPI_0_TASC_FREQ (5454545) /* It looks silly, but this is correct. 1/183e-9 */
#define SPI_0_TDT_FREQ  (4000000) /* 1/250e-9 */

/* SPI_1 timings */
#define SPI_1_TCSC_FREQ (0)
#define SPI_1_TASC_FREQ (0)
#define SPI_1_TDT_FREQ  (0)

/* SPI_2 timings */
#define SPI_2_TCSC_FREQ (0)
#define SPI_2_TASC_FREQ (0)
#define SPI_2_TDT_FREQ  (0)

/** @} */

/** @} */


/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF               (1U)
#define I2C_CLK                 SystemBusClock
#define I2C_0_EN                1
#define I2C_1_EN                0
#define I2C_IRQ_PRIO            1
/**
 * @name I2C baud rate configuration
 * @{
 */
/* Low (10 kHz): MUL = 4, SCL divider = 2560, total: 10240 */
#define KINETIS_I2C_F_ICR_LOW        (0x3D)
#define KINETIS_I2C_F_MULT_LOW       (2)
/* Normal (100 kHz): MUL = 2, SCL divider = 240, total: 480 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1F)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 128, total: 128 */
#define KINETIS_I2C_F_ICR_FAST       (0x17)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 48, total: 48 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x10)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)
/** @} */

/* I2C 0 device configuration */
#define I2C_0_DEV               I2C0
#define I2C_0_CLKEN()           (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT) = 1)
#define I2C_0_CLKDIS()          (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT) = 0)
#define I2C_0_IRQ               I2C0_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT              PORTB
#define I2C_0_PORT_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define I2C_0_PIN_AF            2
#define I2C_0_SDA_PIN           1
#define I2C_0_SCL_PIN           2
#define I2C_0_PORT_CFG          (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)
/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          26
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
#define GPIO_16_EN          1
#define GPIO_17_EN          1
#define GPIO_18_EN          1
#define GPIO_19_EN          1
#define GPIO_20_EN          1
#define GPIO_21_EN          1
#define GPIO_22_EN          1
#define GPIO_23_EN          1
#define GPIO_24_EN          1
#define GPIO_25_EN          1
#define GPIO_26_EN          1
#define GPIO_IRQ_PRIO       1

/* GPIO channel 0 config */
/* Red LED */
#define GPIO_0_PORT         PORTC
#define GPIO_0_PORT_BASE    PORTC_BASE
#define GPIO_0_DEV          PTC
#define GPIO_0_PIN          15
#define GPIO_0_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_0_IRQ          PORTC_IRQn
#define GPIO_0_ISR          isr_portc_pin_detect

/* GPIO channel 1 config */
/* Yellow LED */
#define GPIO_1_PORT         PORTC
#define GPIO_1_PORT_BASE    PORTC_BASE
#define GPIO_1_DEV          PTC
#define GPIO_1_PIN          14
#define GPIO_1_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_1_IRQ          PORTC_IRQn
#define GPIO_1_ISR          isr_portc_pin_detect

/* GPIO channel 2 config */
/* Green LED */
#define GPIO_2_PORT         PORTC
#define GPIO_2_PORT_BASE    PORTC_BASE
#define GPIO_2_DEV          PTC
#define GPIO_2_PIN          13
#define GPIO_2_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_2_IRQ          PORTC_IRQn
#define GPIO_2_ISR          isr_portc_pin_detect

/* GPIO channel 3 config */
/* LIS3DH INT1 */
#define GPIO_3_PORT         PORTC
#define GPIO_3_PORT_BASE    PORTC_BASE
#define GPIO_3_DEV          PTC
#define GPIO_3_PIN          18
#define GPIO_3_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_3_IRQ          PORTC_IRQn
#define GPIO_3_ISR          isr_portc_pin_detect

/* GPIO channel 4 config */
/* LIS3DH INT2 */
#define GPIO_4_PORT         PORTC
#define GPIO_4_PORT_BASE    PORTC_BASE
#define GPIO_4_DEV          PTC
#define GPIO_4_PIN          17
#define GPIO_4_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_4_IRQ          PORTC_IRQn
#define GPIO_4_ISR          isr_portc_pin_detect

/* GPIO channel 5 config */
/* VSEC enable */
#define GPIO_5_PORT         PORTB
#define GPIO_5_PORT_BASE    PORTB_BASE
#define GPIO_5_DEV          PTB
#define GPIO_5_PIN          16
#define GPIO_5_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_5_IRQ          PORTB_IRQn
#define GPIO_5_ISR          isr_portb_pin_detect

/* GPIO channel 6 config */
/* AVDD enable */
#define GPIO_6_PORT         PORTB
#define GPIO_6_PORT_BASE    PORTB_BASE
#define GPIO_6_DEV          PTB
#define GPIO_6_PIN          17
#define GPIO_6_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_6_IRQ          PORTB_IRQn
#define GPIO_6_ISR          isr_portb_pin_detect

/* GPIO channel 7 config */
/* VPERIPH enable */
#define GPIO_7_PORT         PORTD
#define GPIO_7_PORT_BASE    PORTD_BASE
#define GPIO_7_DEV          PTD
#define GPIO_7_PIN          7
#define GPIO_7_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_7_IRQ          PORTD_IRQn
#define GPIO_7_ISR          isr_portd_pin_detect

/* GPIO channel 8 config */
/* MC34673 enable */
#define GPIO_8_PORT         PORTB
#define GPIO_8_PORT_BASE    PORTB_BASE
#define GPIO_8_DEV          PTB
#define GPIO_8_PIN          23
#define GPIO_8_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_8_IRQ          PORTB_IRQn
#define GPIO_8_ISR          isr_portb_pin_detect

/* GPIO channel 9 config */
/* MC34673 CHG */
#define GPIO_9_PORT         PORTB
#define GPIO_9_PORT_BASE    PORTB_BASE
#define GPIO_9_DEV          PTB
#define GPIO_9_PIN          22
#define GPIO_9_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_9_IRQ          PORTB_IRQn
#define GPIO_9_ISR          isr_portb_pin_detect

/* GPIO channel 10 config */
/* MC34673 PPR */
#define GPIO_10_PORT        PORTB
#define GPIO_10_PORT_BASE   PORTB_BASE
#define GPIO_10_DEV         PTB
#define GPIO_10_PIN         21
#define GPIO_10_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_10_IRQ         PORTB_IRQn
#define GPIO_10_ISR         isr_portb_pin_detect

/* GPIO channel 11 config */
/* MC34673 FAST */
#define GPIO_11_PORT        PORTB
#define GPIO_11_PORT_BASE   PORTB_BASE
#define GPIO_11_DEV         PTB
#define GPIO_11_PIN         20
#define GPIO_11_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_11_IRQ         PORTB_IRQn
#define GPIO_11_ISR         isr_portb_pin_detect

/* GPIO channel 12 config */
/* AT86RF212 IRQ */
#define GPIO_12_PORT        PORTB
#define GPIO_12_PORT_BASE   PORTB_BASE
#define GPIO_12_DEV         PTB
#define GPIO_12_PIN         9
#define GPIO_12_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_12_IRQ         PORTB_IRQn
#define GPIO_12_ISR         isr_portb_pin_detect

/* GPIO channel 13 config */
/* AT86RF212 SLP_TR */
#define GPIO_13_PORT        PORTE
#define GPIO_13_PORT_BASE   PORTE_BASE
#define GPIO_13_DEV         PTE
#define GPIO_13_PIN         6
#define GPIO_13_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define GPIO_13_IRQ         PORTE_IRQn
#define GPIO_13_ISR         isr_porte_pin_detect

/* GPIO channel 14 config */
/* AT86RF212 SS */
#define GPIO_14_PORT        PORTD
#define GPIO_14_PORT_BASE   PORTD_BASE
#define GPIO_14_DEV         PTD
#define GPIO_14_PIN         4
#define GPIO_14_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_14_IRQ         PORTD_IRQn
#define GPIO_14_ISR         isr_portd_pin_detect

/* GPIO channel 15 config */
/* LIS3DH CS */
#define GPIO_15_PORT        PORTD
#define GPIO_15_PORT_BASE   PORTD_BASE
#define GPIO_15_DEV         PTD
#define GPIO_15_PIN         0
#define GPIO_15_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_15_IRQ         PORTD_IRQn
#define GPIO_15_ISR         isr_portd_pin_detect

/* GPIO channel 16 config */
/* FM25L04B CS */
#define GPIO_16_PORT        PORTD
#define GPIO_16_PORT_BASE   PORTD_BASE
#define GPIO_16_DEV         PTD
#define GPIO_16_PIN         6
#define GPIO_16_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_16_IRQ         PORTD_IRQn
#define GPIO_16_ISR         isr_portd_pin_detect

/* GPIO channel 17 config */
/* M25P16 CS */
#define GPIO_17_PORT        PORTD
#define GPIO_17_PORT_BASE   PORTD_BASE
#define GPIO_17_DEV         PTD
#define GPIO_17_PIN         5
#define GPIO_17_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_17_IRQ         PORTD_IRQn
#define GPIO_17_ISR         isr_portd_pin_detect

/* GPIO channel 18 config */
/* General purpose expansion PTB18 */
#define GPIO_18_PORT        PORTB
#define GPIO_18_PORT_BASE   PORTB_BASE
#define GPIO_18_DEV         PTB
#define GPIO_18_PIN         18
#define GPIO_18_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_18_IRQ         PORTB_IRQn
#define GPIO_18_ISR         isr_portb_pin_detect

/* GPIO channel 19 config */
/* General purpose expansion PTB19 */
#define GPIO_19_PORT        PORTB
#define GPIO_19_PORT_BASE   PORTB_BASE
#define GPIO_19_DEV         PTB
#define GPIO_19_PIN         19
#define GPIO_19_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_19_IRQ         PORTB_IRQn
#define GPIO_19_ISR         isr_portb_pin_detect

/* GPIO channel 20 config */
/* General purpose expansion PTC0 */
#define GPIO_20_PORT        PORTC
#define GPIO_20_PORT_BASE   PORTC_BASE
#define GPIO_20_DEV         PTC
#define GPIO_20_PIN         0
#define GPIO_20_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_20_IRQ         PORTC_IRQn
#define GPIO_20_ISR         isr_portc_pin_detect

/* GPIO channel 21 config */
/* General purpose expansion PTC1 */
#define GPIO_21_PORT        PORTC
#define GPIO_21_PORT_BASE   PORTC_BASE
#define GPIO_21_DEV         PTC
#define GPIO_21_PIN         1
#define GPIO_21_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_21_IRQ         PORTC_IRQn
#define GPIO_21_ISR         isr_portc_pin_detect

/* GPIO channel 22 config */
/* General purpose expansion PTC2 */
#define GPIO_22_PORT        PORTC
#define GPIO_22_PORT_BASE   PORTC_BASE
#define GPIO_22_DEV         PTC
#define GPIO_22_PIN         2
#define GPIO_22_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_22_IRQ         PORTC_IRQn
#define GPIO_22_ISR         isr_portc_pin_detect

/* GPIO channel 23 config */
/* General purpose expansion PTC5 */
#define GPIO_23_PORT        PORTC
#define GPIO_23_PORT_BASE   PORTC_BASE
#define GPIO_23_DEV         PTC
#define GPIO_23_PIN         5
#define GPIO_23_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_23_IRQ         PORTC_IRQn
#define GPIO_23_ISR         isr_portc_pin_detect

/* GPIO channel 24 config */
/* General purpose expansion PTC6 */
#define GPIO_24_PORT        PORTC
#define GPIO_24_PORT_BASE   PORTC_BASE
#define GPIO_24_DEV         PTC
#define GPIO_24_PIN         6
#define GPIO_24_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_24_IRQ         PORTC_IRQn
#define GPIO_24_ISR         isr_portc_pin_detect

/* GPIO channel 25 config */
/* General purpose expansion PTC7 */
#define GPIO_25_PORT        PORTC
#define GPIO_25_PORT_BASE   PORTC_BASE
#define GPIO_25_DEV         PTC
#define GPIO_25_PIN         7
#define GPIO_25_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_25_IRQ         PORTC_IRQn
#define GPIO_25_ISR         isr_portc_pin_detect

/* GPIO channel 26 config */
/* General purpose expansion PTE4 */
#define GPIO_26_PORT        PORTE
#define GPIO_26_PORT_BASE   PORTE_BASE
#define GPIO_26_DEV         PTE
#define GPIO_26_PIN         4
#define GPIO_26_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define GPIO_26_IRQ         PORTE_IRQn
#define GPIO_26_ISR         isr_porte_pin_detect
/** @} */


/**
 * @name RTC configuration
 * @{
 */
/* RIOT RTC implementation uses RTT for underlying timekeeper */
#define RTC_NUMOF           (1U)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_IRQ_PRIO        1
#define RTT_IRQ             RTC_IRQn
#define RTT_ISR             isr_rtc_alarm
#define RTT_DEV             RTC
#define RTT_UNLOCK()        (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_RTC_SHIFT) = 1)
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (1)             /* in Hz */

/**
 * RTC module crystal load capacitance configuration bits.
 */
/* enable 12pF load capacitance, might need adjusting.. */
#define RTT_LOAD_CAP_BITS   (RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK)
/** @} */


/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF            (1U)
#define RANDOM_CLKEN()          (BITBAND_REG(SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT) = 1)
#define RANDOM_CLKDIS()         (BITBAND_REG(SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT) = 0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MULLE_PERIPH_CONF_H */
/** @} */
