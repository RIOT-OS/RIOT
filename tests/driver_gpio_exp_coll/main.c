/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       GPIO expander compile time gpio_t collision test
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "assert.h"
#include "gpio_exp.h"
#include "periph/gpio.h"

#if defined(CPU_MSP430FXYZ)
#define COLL_PINMASK    (0x07)
#else
#define COLL_PINMASK    (0x0f)
#endif

#define COLL_TEST(port) \
    (GPIO_PIN(port, COLL_PINMASK) > GPIO_EXP_THRESH)

#if defined(CPU_ATMEGA1281)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G)

#elif defined(CPU_ATMEGA1284P)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D)

#elif defined(CPU_ATMEGA2560)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H) \
    || COLL_TEST(PORT_J) \
    || COLL_TEST(PORT_K) \
    || COLL_TEST(PORT_L)

#elif defined(CPU_ATMEGA256RFR2)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G)

#elif defined(CPU_ATMEGA328P)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D)

#elif defined(CPU_CC2538)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D)

#elif defined(CPU_CC26X0)
/* CPU does have port definitions */
#define COLL_TEST_PORTS \
    COLL_TEST(0)

#elif defined(CPU_EFM32)
/* PORT_A */
#if (_GPIO_PORT_A_PIN_COUNT > 0)
#define COLL_TEST_PORTA (COLL_TEST(PA))
#else
#define COLL_TEST_PORTA (0)
#endif
/* PORT_B */
#if (_GPIO_PORT_B_PIN_COUNT > 0)
#define COLL_TEST_PORTB (COLL_TEST_PORTA || COLL_TEST(PB))
#else
#define COLL_TEST_PORTB (COLL_TEST_PORTA)
#endif
/* PORT_C */
#if (_GPIO_PORT_C_PIN_COUNT > 0)
#define COLL_TEST_PORTC (COLL_TEST_PORTB || COLL_TEST(PC))
#else
#define COLL_TEST_PORTC (COLL_TEST_PORTB)
#endif
/* PORT_D */
#if (_GPIO_PORT_D_PIN_COUNT > 0)
#define COLL_TEST_PORTD (COLL_TEST_PORTC || COLL_TEST(PD))
#else
#define COLL_TEST_PORTD (COLL_TEST_PORTC)
#endif
/* PORT_E */
#if (_GPIO_PORT_E_PIN_COUNT > 0)
#define COLL_TEST_PORTE (COLL_TEST_PORTD || COLL_TEST(PE))
#else
#define COLL_TEST_PORTE (COLL_TEST_PORTD)
#endif
/* PORT_F */
#if (_GPIO_PORT_F_PIN_COUNT > 0)
#define COLL_TEST_PORTF (COLL_TEST_PORTE || COLL_TEST(PF))
#else
#define COLL_TEST_PORTF (COLL_TEST_PORTE)
#endif
/* PORT_G */
#if (_GPIO_PORT_G_PIN_COUNT > 0)
#define COLL_TEST_PORTG (COLL_TEST_PORTF || COLL_TEST(PG))
#else
#define COLL_TEST_PORTG (COLL_TEST_PORTF)
#endif
/* PORT_H */
#if (_GPIO_PORT_H_PIN_COUNT > 0)
#define COLL_TEST_PORTH (COLL_TEST_PORTG || COLL_TEST(PH))
#else
#define COLL_TEST_PORTH (COLL_TEST_PORTG)
#endif
/* PORT_I */
#if (_GPIO_PORT_I_PIN_COUNT > 0)
#define COLL_TEST_PORTI (COLL_TEST_PORTH || COLL_TEST(PI))
#else
#define COLL_TEST_PORTI (COLL_TEST_PORTH)
#endif
/* PORT_J */
#if (_GPIO_PORT_J_PIN_COUNT > 0)
#define COLL_TEST_PORTJ (COLL_TEST_PORTI || COLL_TEST(PJ))
#else
#define COLL_TEST_PORTJ (COLL_TEST_PORTI)
#endif
/* PORT_K */
#if (_GPIO_PORT_K_PIN_COUNT > 0)
#define COLL_TEST_PORTK (COLL_TEST_PORTJ || COLL_TEST(PK))
#else
#define COLL_TEST_PORTK (COLL_TEST_PORTJ)
#endif
/* Combination of all existent ports  */
#define COLL_TEST_PORTS (COLL_TEST_PORTK)

#elif defined(CPU_EZR32WG)
#define COLL_TEST_PORTS \
    COLL_TEST(PA) \
    || COLL_TEST(PB) \
    || COLL_TEST(PC) \
    || COLL_TEST(PD) \
    || COLL_TEST(PE) \
    || COLL_TEST(PF)

#elif defined(CPU_FE310)
/* CPU does not have port definitions */
#define COLL_TEST_PORTS \
    COLL_TEST(0)

#elif defined(CPU_KINETIS)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G)

#elif defined(CPU_LM4F120)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F)

#elif defined(CPU_LPC1768)
/* CPU does not have port definitions */
#define COLL_TEST_PORTS \
    COLL_TEST(0)

#elif defined(CPU_LPC2387)
/* CPU does not have port definitions */
#define COLL_TEST_PORTS \
    COLL_TEST(0)

#elif defined(CPU_MIPS_PIC32MX) || defined(CPU_MIPS_PIC32MZ)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G)

#elif defined(CPU_MSP430FXYZ)
#define COLL_TEST_PORTS \
    COLL_TEST(P1) \
    || COLL_TEST(P2) \
    || COLL_TEST(P3) \
    || COLL_TEST(P4) \
    || COLL_TEST(P5) \
    || COLL_TEST(P6)

#elif defined(CPU_NRF51) || defined(CPU_NRF52)
/* CPU does not have port definitions */
#define COLL_TEST_PORTS \
    COLL_TEST(0)

#elif defined(CPU_SAM3)
#define COLL_TEST_PORTS \
    COLL_TEST(PA) \
    || COLL_TEST(PB) \
    || COLL_TEST(PC) \
    || COLL_TEST(PD)

#elif defined(CPU_SAMD21) || defined(CPU_SAML21)
#define COLL_TEST_PORTS \
    COLL_TEST(PA) \
    || COLL_TEST(PB) \
    || COLL_TEST(PC)

#elif defined(CPU_STM32F0)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F)

#elif defined(CPU_STM32F1)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G)

#elif defined(CPU_STM32F2)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H) \
    || COLL_TEST(PORT_I)

#elif defined(CPU_STM32F3)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H)

#elif defined(CPU_STM32F4)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H) \
    || COLL_TEST(PORT_I)

#elif defined(CPU_STM32F7)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H) \
    || COLL_TEST(PORT_I) \
    || COLL_TEST(PORT_J) \
    || COLL_TEST(PORT_K)

#elif defined(CPU_STM32L0)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_H)

#elif defined(CPU_STM32L1)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H)

#elif defined(CPU_STM32L4)
#define COLL_TEST_PORTS \
    COLL_TEST(PORT_A) \
    || COLL_TEST(PORT_B) \
    || COLL_TEST(PORT_C) \
    || COLL_TEST(PORT_D) \
    || COLL_TEST(PORT_E) \
    || COLL_TEST(PORT_F) \
    || COLL_TEST(PORT_G) \
    || COLL_TEST(PORT_H)

#else

/* unknown CPU (default to simpler, less thorough test) */
#define COLL_TEST_PORTS \
    COLL_TEST(0)

#endif /* defined(CPU) */

#define COLL_RES (!(COLL_TEST_PORTS))

static_assert(COLL_RES, "[driver_gpio_exp_coll] Collision in GPIO_EXP_PIN");

int main(void)
{
    return 0;
}
