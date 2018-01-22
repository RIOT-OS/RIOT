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

#include "cpu.h" // For numbers of ports
#include "periph/pm.h"

#include "periph/spi.h"
#include "periph_conf.h"
#include "periph/init.h"
#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @name Available power modes
 */
enum pm_mode {
    PM_OFF,            /**< MCU is off */
    PM_POWERDOWN,      /**< MCU is powered down */
    PM_SLEEP,          /**< MCU in sleep mode */
    PM_IDLE,           /**< MCU is idle */
    PM_ON,             /**< MCU is active */
    PM_UNKNOWN = -1    /**< status unknown/unavailable */
};

// make clk_init available, could be added to cpu.h as an alternative
extern void clk_init(void);

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#define PM_STOP_CONFIG (PWR_CR_LPDS | PWR_CR_FPDS)
#endif

/* Variables definitions to LPM */
static uint32_t ahb_gpio_clocks;
static uint32_t tmpreg;
static uint32_t pm_gpio_moder[CPU_NUMBER_OF_PORTS];
static uint32_t pm_gpio_pupdr[CPU_NUMBER_OF_PORTS];
static uint16_t pm_gpio_otyper[CPU_NUMBER_OF_PORTS];
static uint16_t pm_gpio_odr[CPU_NUMBER_OF_PORTS];
static uint16_t pm_portmask_system[CPU_NUMBER_OF_PORTS] = { 0 };
static uint16_t pm_portmask_user[CPU_NUMBER_OF_PORTS] = { 0 };

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
/* We add GPIOs we touched to exclusion mask pm_portmask_system */
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

    // pm_portmask_system[((uint32_t)&port>> 10) & 0x0f] |= 1 << pin;
    pm_portmask_system[_port_num(pin)] |= 1 << pin_num;
}

/* Do not change GPIO state in sleep mode */
void pm_arch_add_gpio_exclusion(gpio_t gpio) {
    uint8_t port = ((uint32_t)gpio >> 10) & 0x0f;
    uint8_t pin = ((uint32_t)gpio & 0x0f);

    pm_portmask_user[port] |= (uint16_t)(1<<pin);
}

/* Change GPIO state to AIN in sleep mode */
void pm_arch_del_gpio_exclusion(gpio_t gpio) {
    uint8_t port = ((uint32_t)gpio >> 10) & 0x0f;
    uint8_t pin = ((uint32_t)gpio & 0x0f);

    pm_portmask_user[port] &= ~(uint16_t)(1<<pin);
}

void pm_before_i_go_to_sleep(void){
    uint8_t i;
    uint8_t p;
    uint32_t mask;
    GPIO_TypeDef *port;


    for (i = 0; i < CPU_NUMBER_OF_PORTS; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));

        /* save GPIO registers values */
        pm_gpio_moder[i] = port->MODER;
        pm_gpio_pupdr[i] = port->PUPDR;
        pm_gpio_otyper[i] = (uint16_t)(port->OTYPER & 0xFFFF);
        pm_gpio_odr[i] = (uint16_t)(port->ODR & 0xFFFF);
    }

    /* specifically set GPIOs used for external SPI devices */
    /* NSS = 1, MOSI = 0, SCK = 0, MISO doesn't matter */
    for (i = 0; i < SPI_NUMOF; i++) {
        /* port and pin numbers */
        GPIO_TypeDef *port = _port(spi_config[i].sclk_pin);
        int pin_num = _pin_num(spi_config[i].sclk_pin);

        /* check if alternate functions are enabled for that device*/
        if (((port->AFR[(pin_num > 7) ? 1 : 0] >> (pin_num * 4)) & 0x0f)  == spi_config[i].af) {
            if (spi_config[i].cs_pin != GPIO_UNDEF) {
                pin_set(spi_config[i].cs_pin, 1);
            }
            pin_set(spi_config[i].mosi_pin, 0);
            pin_set(spi_config[i].sclk_pin, 0);
        }
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
                   if ((pm_portmask_system[i] | pm_portmask_user[i]) & (1 << p)) {
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
static void pm_when_i_wake_up (void) {
    /* enable all GPIO clocks */
    periph_clk_en(AHB, 0xFF);

    uint8_t i;
    GPIO_TypeDef *port;

    /* restore GPIO settings */
    for (i = 0; i < CPU_NUMBER_OF_PORTS; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));

        port->PUPDR = pm_gpio_pupdr[i];
        port->OTYPER = pm_gpio_otyper[i];
        port->ODR = pm_gpio_odr[i];
        port->MODER = pm_gpio_moder[i];
    }

    /* restore GPIO clocks */
    tmpreg = RCC->AHBENR;
    tmpreg &= ~((uint32_t)0xFF);
    tmpreg |= ahb_gpio_clocks;
    periph_clk_en(AHB, tmpreg);

}

/* Select CPU clocking between default (PM_ON) and medium-speed (PM_IDLE) */
static void pm_select_run_mode(uint8_t pm_mode) {
    switch(pm_mode) {
    case PM_ON:
        DEBUG("Switching to PM_ON");
        clk_init();
    break;
    case PM_IDLE:
        DEBUG("Switching to PM_IDLE");
        /* 115200 bps stdio UART with default 16x oversamplig needs 2 MHz or 4 MHz MSI clock */
        /* at 1 MHz, it will be switched to 8x oversampling with 3.55 % baudrate error */
        /* if you need stdio UART at lower frequencies, change its settings to lower baudrate */
        // switch_to_msi(RCC_ICSCR_MSIRANGE_5, RCC_CFGR_HPRE_DIV1);
    break;
    default:
        DEBUG("Switching to PM_IDLE");
        clk_init();
    break;
    }

#ifdef MODULE_XTIMER
    /* Recalculate xtimer frequency */
    /* NB: default XTIMER_HZ clock is 1 MHz, so CPU clock must be at least 1 MHz for xtimer to work properly */
    xtimer_init();
#endif
}

void pm_set(unsigned mode)
{
    unsigned state;

    switch (mode) {
        case PM_OFF:             /* Stand by mode */
            DEBUG("Switching to PM_OFF");
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;

            /* Select STANDBY mode */
            PWR->CR |= PWR_CR_PDDS;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP;

            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;

            /* This option is used to ensure that store operations are completed */
            #if defined (__CC_ARM)
            __force_stores();
            #endif

            irq_disable();

            /* Request Wait For Interrupt */
            __DSB();
            __WFI();
            break;

        case PM_POWERDOWN:             /* Stop mode */
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Regulator in LP mode */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP;

            state = irq_disable();

            pm_before_i_go_to_sleep();

            /* Request Wait For Interrupt */
            __DSB();
            __WFI();

            /* Clear SLEEPDEEP bit */
            SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);

            /* Restore clocks and PLL */
            pm_select_run_mode(PM_ON);

            pm_when_i_wake_up();

            irq_restore(state);
            break;

        case PM_SLEEP:              /* Low-power sleep mode */
            DEBUG("Switching to PM_SLEEP");
            /* Clear Wakeup flag */
            PWR->CR |= PWR_CR_CWUF;
            /* Enable low-power mode of the voltage regulator */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Clear SLEEPDEEP bit */
            SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP);

            state = irq_disable();

            pm_before_i_go_to_sleep();

            /* Switch to 65kHz clock */
            // switch_to_msi(RCC_ICSCR_MSIRANGE_0, RCC_CFGR_HPRE_DIV1);

            /* Request Wait For Interrupt */
            __DSB();
            __WFI();

            /* Switch back to default speed */
			pm_select_run_mode(PM_ON);

            pm_when_i_wake_up();

            irq_restore(state);
            break;
        case PM_IDLE:
            DEBUG("Switching to PM_IDLE");
            state = irq_disable();
            pm_select_run_mode(PM_IDLE);
            irq_restore(state);
        break;

        case PM_ON:
             DEBUG("Switching to PM_ON");
             state = irq_disable();
             pm_select_run_mode(PM_ON);
             irq_restore(state);
             break;

        case PM_UNKNOWN:
             break;

        default:
            break;
    }

}

void pm_off(void)
{
    irq_disable();
    pm_set(0);
}
