/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_iot-lab_M3
 * @{
 *
 * @file        system_stm32f1.c
 * @brief       Board specific clock setup
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */


#include "stm32f10x.h"
#include "board.h"

uint32_t SystemCoreClock = F_CPU;

#define VECT_TAB_OFFSET  0x0

static void set_system_clock(void)
{
    volatile uint32_t startup_counter = 0, HSE_status = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSE_status = RCC->CR & RCC_CR_HSERDY;
        startup_counter++;
    }
    while ((HSE_status == 0) && (startup_counter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSE_status = (uint32_t)0x01;
    }
    else {
        HSE_status = (uint32_t)0x00;
    }

    if (HSE_status == (uint32_t)0x01) {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;


        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

        /* NOTE : agilefox : modified to take into account the 16MHz
                             crystal instead of 8MHz */
        /*  PLL configuration: PLLCLK = HSE / 2  * 9 = 72 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC
                                            | RCC_CFGR_PLLXTPRE
                                            | RCC_CFGR_PLLMULL));

        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE
                                | RCC_CFGR_PLLXTPRE_HSE_Div2
                                | RCC_CFGR_PLLMULL9);

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08) {
        }
    }
    else {
        /* If HSE fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this error */
    }
}

void SystemInit(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR &= (uint32_t)0xF0FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x009F0000;

    /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
    /* Configure the Flash Latency cycles and enable prefetch buffer */
    set_system_clock();

    /* Vector Table Relocation in Internal FLASH. */
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
}
