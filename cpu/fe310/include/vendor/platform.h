// See LICENSE for license details.

#ifndef _SIFIVE_PLATFORM_H
#define _SIFIVE_PLATFORM_H



#include "vendor/aon.h"
#include "vendor/clint.h"
#include "vendor/gpio.h"
#include "vendor/i2c.h"
#include "vendor/otp.h"
#include "vendor/plic.h"
#include "vendor/prci.h"
#include "vendor/pwm.h"
#include "vendor/spi.h"
#include "vendor/uart.h"


/****************************************************************************
 * Platform definitions
 *****************************************************************************/

// Memory map
#define MASKROM_MEM_ADDR 		(0x00001000)
#define TRAPVEC_TABLE_CTRL_ADDR	(0x00001010)
#define OTP_MEM_ADDR 			(0x00020000)
#define CLINT_CTRL_ADDR 		(0x02000000)
#define ITIM_MEM_ADDR 		    (0x08000000)
#define PLIC_CTRL_ADDR 			(0x0C000000)
#define AON_CTRL_ADDR 			(0x10000000)
#define PRCI_CTRL_ADDR 			(0x10008000)
#define OTP_CTRL_ADDR 			(0x10010000)
#define GPIO_CTRL_ADDR 			(0x10012000)
#define UART0_CTRL_ADDR 		(0x10013000)
#define SPI0_CTRL_ADDR 			(0x10014000)
#define PWM0_CTRL_ADDR 			(0x10015000)
#define I2C0_CTRL_ADDR 			(0x10016000)
#define UART1_CTRL_ADDR 		(0x10023000)
#define SPI1_CTRL_ADDR 			(0x10024000)
#define PWM1_CTRL_ADDR 			(0x10025000)
#define SPI2_CTRL_ADDR 			(0x10034000)
#define PWM2_CTRL_ADDR 			(0x10035000)
#define SPI0_MEM_ADDR 			(0x20000000)
#define MEM_CTRL_ADDR 			(0x80000000)

// IOF masks
#define IOF0_SPI1_MASK          (0x000007FC)
#define SPI11_NUM_SS     		(4)
#define IOF_SPI1_SS0          	(2u)
#define IOF_SPI1_SS1          	(8u)
#define IOF_SPI1_SS2          	(9u)
#define IOF_SPI1_SS3          	(10u)
#define IOF_SPI1_MOSI         	(3u)
#define IOF_SPI1_MISO         	(4u)
#define IOF_SPI1_SCK          	(5u)
#define IOF_SPI1_DQ0          	(3u)
#define IOF_SPI1_DQ1          	(4u)
#define IOF_SPI1_DQ2          	(6u)
#define IOF_SPI1_DQ3          	(7u)

#define IOF0_SPI2_MASK          (0xFC000000)
#define SPI2_NUM_SS       		(1)
#define IOF_SPI2_SS0          	(26u)
#define IOF_SPI2_MOSI         	(27u)
#define IOF_SPI2_MISO         	(28u)
#define IOF_SPI2_SCK          	(29u)
#define IOF_SPI2_DQ0          	(27u)
#define IOF_SPI2_DQ1          	(28u)
#define IOF_SPI2_DQ2          	(30u)
#define IOF_SPI2_DQ3          	(31u)

#define IOF0_I2C_MASK           (0x00003000)
#define IOF_I2C0_SDA 			(12u)
#define IOF_I2C0_SCL 			(13u)

#define IOF0_UART0_MASK         (0x00030000)
#define IOF_UART0_RX   			(16u)
#define IOF_UART0_TX   			(17u)

#define IOF0_UART1_MASK         (0x00840000)
#define IOF_UART1_RX 			(18u)
#define IOF_UART1_TX 			(23u)

#define IOF1_PWM0_MASK          (0x0000000F)
#define IOF1_PWM1_MASK          (0x00780000)
#define IOF1_PWM2_MASK          (0x00003C00)

// Interrupt numbers
#define INT_RESERVED 			0
#define INT_WDOGCMP 			1
#define INT_RTCCMP 				2
#define INT_UART0_BASE 			3
#define INT_UART1_BASE 			4
#define INT_SPI0_BASE 			5
#define INT_SPI1_BASE 			6
#define INT_SPI2_BASE 			7
#define INT_GPIO_BASE 			8
#define INT_PWM0_BASE 			40
#define INT_PWM1_BASE 			44
#define INT_PWM2_BASE 			48
#define INT_I2C_BASE 			52

// Helper functions
#define _REG32(p, i) 			(*(volatile uint32_t *) ((p) + (i)))
#define _REG32P(p, i) 			((volatile uint32_t *) ((p) + (i)))
#define AON_REG(offset) 		_REG32(AON_CTRL_ADDR, offset)
#define CLINT_REG(offset) 		_REG32(CLINT_CTRL_ADDR, offset)
#define GPIO_REG(offset) 		_REG32(GPIO_CTRL_ADDR, offset)
#define I2C0_REG(offset) 		_REG32(I2C0_CTRL_ADDR, offset)
#define OTP_REG(offset)  		_REG32(OTP_CTRL_ADDR, offset)
#define PLIC_REG(offset) 		_REG32(PLIC_CTRL_ADDR, offset)
#define PRCI_REG(offset) 		_REG32(PRCI_CTRL_ADDR, offset)
#define PWM0_REG(offset) 		_REG32(PWM0_CTRL_ADDR, offset)
#define PWM1_REG(offset) 		_REG32(PWM1_CTRL_ADDR, offset)
#define PWM2_REG(offset) 		_REG32(PWM2_CTRL_ADDR, offset)
#define SPI0_REG(offset) 		_REG32(SPI0_CTRL_ADDR, offset)
#define SPI1_REG(offset) 		_REG32(SPI1_CTRL_ADDR, offset)
#define SPI2_REG(offset) 		_REG32(SPI2_CTRL_ADDR, offset)
#define UART0_REG(offset) 		_REG32(UART0_CTRL_ADDR, offset)
#define UART1_REG(offset) 		_REG32(UART1_CTRL_ADDR, offset)

// Misc

#define NUM_GPIO 			32

#define PLIC_NUM_INTERRUPTS 52
#define PLIC_NUM_PRIORITIES 7

#define RTC_FREQ 			32768

#define ITIM_MEM_LEN		8192

#endif /* _SIFIVE_PLATFORM_H */
