/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
#include "stmclk.h"
#endif

#include "periph/spi.h"
#include "periph_conf.h"
#include "cpu.h" // For numbers of ports
#include "periph/init.h"

extern void clk_init(void);

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#define PM_STOP_CONFIG (PWR_CR_LPDS | PWR_CR_FPDS)
#endif

#if defined(CPU_FAM_STM32L1)
/* Variables definitions to LPM */
static uint32_t ahb_gpio_clocks;
static uint32_t tmpreg;
static uint32_t lpm_gpio_moder[CPU_NUMBER_OF_PORTS];
static uint32_t lpm_gpio_pupdr[CPU_NUMBER_OF_PORTS];
static uint16_t lpm_gpio_otyper[CPU_NUMBER_OF_PORTS];
static uint16_t lpm_gpio_odr[CPU_NUMBER_OF_PORTS];
static uint16_t lpm_portmask_system[CPU_NUMBER_OF_PORTS] = { 0 };
static uint16_t lpm_portmask_user[CPU_NUMBER_OF_PORTS] = { 0 };
// static uint8_t  lpm_usart[UART_NUMOF];

static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return ((pin >> 10) & 0x0f);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

/* We are not using gpio_init as it sets GPIO clock speed to maximum */
/* We add GPIOs we touched to exclusion mask lpm_portmask_system */
static void pin_set(gpio_t pin, uint8_t value) {
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);
    tmpreg = port->MODER;
    tmpreg &= ~(3 << (2*pin_num));
    tmpreg |= (1 << (2*pin_num));
    port->MODER = tmpreg;

    port->PUPDR &= ~(3 << (2*pin_num));
    port->OTYPER &= ~(1 << pin_num);
    if (value) {
        port->ODR |= (1 << pin_num);
    } else {
        port->ODR &= ~(1 << pin_num);
    }

    // lpm_portmask_system[((uint32_t)&port>> 10) & 0x0f] |= 1 << pin;
    lpm_portmask_system[_port_num(pin)] |= 1 << pin_num;
}

/* Do not change GPIO state in sleep mode */
void lpm_arch_add_gpio_exclusion(gpio_t gpio) {
	uint8_t port = ((uint32_t)gpio >> 10) & 0x0f;
	uint8_t pin = ((uint32_t)gpio & 0x0f);

	lpm_portmask_user[port] |= (uint16_t)(1<<pin);
}

/* Change GPIO state to AIN in sleep mode */
void lpm_arch_del_gpio_exclusion(gpio_t gpio) {
	uint8_t port = ((uint32_t)gpio >> 10) & 0x0f;
	uint8_t pin = ((uint32_t)gpio & 0x0f);

	lpm_portmask_user[port] &= ~(uint16_t)(1<<pin);
}

void lpm_before_i_go_to_sleep(void){
    uint8_t i;
    uint8_t p;
    uint32_t mask;
    GPIO_TypeDef *port;


    for (i = 0; i < CPU_NUMBER_OF_PORTS; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));

        /* save GPIO registers values */
        lpm_gpio_moder[i] = port->MODER;
        lpm_gpio_pupdr[i] = port->PUPDR;
        lpm_gpio_otyper[i] = (uint16_t)(port->OTYPER & 0xFFFF);
        lpm_gpio_odr[i] = (uint16_t)(port->ODR & 0xFFFF);
	}

    /* specifically set GPIOs used for external SPI devices */
    if (SPI_0_ISON()) {
        pin_set(spi_config[0].sclk_pin, 0);
        pin_set(spi_config[0].mosi_pin, 0);
        pin_set(spi_config[0].cs_pin, 1);
    } else {
		p = ((uint32_t)PORT_A >> 10) & 0x0f;
		lpm_portmask_system[p] &= ~(1 << 5);
		lpm_portmask_system[p] &= ~(1 << 7);
        p = ((uint32_t)PORT_B >> 10) & 0x0f;
		lpm_portmask_system[p] &= ~(1 << 6);
	}

    /* save GPIO clock configuration */
    ahb_gpio_clocks = RCC->AHBENR & 0xFF;
    /* enable all GPIO clocks */
    periph_clk_en(AHB, 0xFF);

    for (i = 0; i < CPU_NUMBER_OF_PORTS; i++) {
           port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
           mask = 0xFFFFFFFF;
           if (EXTI->IMR) {
               for (p = 0; p < 16; p ++) {
                   /* exclude GPIOs we previously set with pin_set */
                   if ((lpm_portmask_system[i] | lpm_portmask_user[i]) & (1 << p)) {
                       mask &= ~(0x03 << (p*2));
                   } else {
                       if ((EXTI->IMR & (1 << p)) && ((((SYSCFG->EXTICR[p >> 2]) >> ((p & 0x03) * 4)) & 0xF) == i)) {
                           mask &= ~(0x03 << (p*2));
                       }
                   }
               }
           }

           /* disable pull-ups on GPIOs */
           tmpreg = port->PUPDR;
           tmpreg &= ~mask;
           port->PUPDR = tmpreg;

           /* set GPIOs to AIN mode */
           tmpreg = port->MODER;
           tmpreg |= mask;
           port->MODER = tmpreg;
       }

    /* restore GPIO clocks */
    tmpreg = RCC->AHBENR;
    tmpreg &= ~((uint32_t)0xFF);
    tmpreg |= ahb_gpio_clocks;
    periph_clk_en(AHB, tmpreg);
}

/* restore GPIO settings */
static void lpm_when_i_wake_up (void) {
    /* enable all GPIO clocks */
    periph_clk_en(AHB, 0xFF);

    uint8_t i;
    GPIO_TypeDef *port;

    /* restore GPIO settings */
    for (i = 0; i < CPU_NUMBER_OF_PORTS; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));

        port->PUPDR = lpm_gpio_pupdr[i];
        port->OTYPER = lpm_gpio_otyper[i];
        port->ODR = lpm_gpio_odr[i];
        port->MODER = lpm_gpio_moder[i];
    }

    /* restore GPIO clocks */
    tmpreg = RCC->AHBENR;
    tmpreg &= ~((uint32_t)0xFF);
    tmpreg |= ahb_gpio_clocks;
    periph_clk_en(AHB, tmpreg);

}
#endif

void pm_set(unsigned mode)
{
    int deep = 0;

/* I just copied it from stm32f1/2/4, but I suppose it would work for the
 * others... /KS */
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
    switch (mode) {
        case 0:
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
            /* Enable WKUP pin to use for wakeup from standby mode */
            PWR->CSR |= PWR_CSR_EWUP;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
        case 1:                 /* STM Stop mode */
            /* Clear PDDS and LPDS bits to enter stop mode on */
            /* deepsleep with voltage regulator on */
            PWR->CR &= ~(PWR_CR_PDDS | PWR_CR_LPDS);
            PWR->CR |= PM_STOP_CONFIG;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
    }
#else
    (void) mode;
#endif

#if defined(CPU_FAM_STM32L1)
    switch (mode) {
        case 0:             /* Stand by mode */

            /* Select STANDBY mode */
            PWR->CR |= PWR_CR_PDDS;
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Enable Ultra Low Power mode Ver */
            PWR->CR |= PWR_CR_ULP;
            /* This option is used to ensure that store operations are completed */
            #if defined (__CC_ARM)
            __force_stores();
            #endif

            deep = 1;

        case 1:             /* Stop mode */
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Regulator in LP mode */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;

            lpm_before_i_go_to_sleep();

            deep = 1;
            break;

        case 2:              /* Low power sleep mode TODO*/

             /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Enable low-power mode of the voltage regulator */
            PWR->CR |= PWR_CR_LPSDSR;

            deep = 0;
            break;
    }
#endif

    cortexm_sleep(deep);

#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
    if (deep) {
        /* Re-init clock after STOP */
        stmclk_init_sysclk();
    }
#endif

#if defined(CPU_FAM_STM32L1)
    if (deep) {
        /* Clear SLEEPDEEP bit */
        SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);

        /* Re-init clock after STOP */
        clk_init();

        lpm_when_i_wake_up();
    }
#endif
}

#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1)
void pm_off(void)
{
    irq_disable();
    pm_set(0);
}
#endif
