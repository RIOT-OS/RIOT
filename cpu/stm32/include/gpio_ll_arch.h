/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @ingroup         drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief           CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include "architecture.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

#define GPIO_PORT_NUMBERING_ALPHABETIC  1

#ifdef GPIOA_BASE
#  define GPIO_PORT_0       GPIOA_BASE
#endif

#ifdef GPIOB_BASE
#  define GPIO_PORT_1       GPIOB_BASE
#endif

#ifdef GPIOC_BASE
#  define GPIO_PORT_2       GPIOC_BASE
#endif

#ifdef GPIOD_BASE
#  define GPIO_PORT_3       GPIOD_BASE
#endif

#ifdef GPIOE_BASE
#  define GPIO_PORT_4       GPIOE_BASE
#endif

#ifdef GPIOF_BASE
#  define GPIO_PORT_5       GPIOF_BASE
#endif

#ifdef GPIOG_BASE
#  define GPIO_PORT_6       GPIOG_BASE
#endif

#ifdef GPIOH_BASE
#  define GPIO_PORT_7       GPIOH_BASE
#endif

#ifdef GPIOI_BASE
#  define GPIO_PORT_8       GPIOI_BASE
#endif

#ifdef GPIOJ_BASE
#  define GPIO_PORT_9       GPIOJ_BASE
#endif

#ifdef GPIOK_BASE
#  define GPIO_PORT_10      GPIOK_BASE
#endif

static inline gpio_port_t gpio_port(uword_t num)
{
#if defined(CPU_FAM_STM32MP1)
    return GPIOA_BASE + (num << 12);
#else
    return GPIOA_BASE + (num << 10);
#endif
}

static inline uword_t gpio_port_num(gpio_port_t port)
{
#if defined(CPU_FAM_STM32MP1)
    return (port - GPIOA_BASE) >> 12;
#else
    return (port - GPIOA_BASE) >> 10;
#endif
}

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    GPIO_TypeDef *p = (GPIO_TypeDef *)port;
    return p->IDR;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    GPIO_TypeDef *p = (GPIO_TypeDef *)port;
    return p->ODR;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    GPIO_TypeDef *p = (GPIO_TypeDef *)port;
    p->BSRR = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    GPIO_TypeDef *p = (GPIO_TypeDef *)port;
    /* The STM32F4 vendor header files do include defines for accessing the
     * BRR register, but do not have a BRR register.
     * See https://github.com/STMicroelectronics/cmsis_device_f4/pull/7 */
#if defined(GPIO_BRR_BR0) && !defined(CPU_FAM_STM32F4)
    p->BRR = mask;
#else
    /* The first half-word sets GPIOs, the second half-world clears GPIOs */
    volatile uint16_t *brr = (volatile uint16_t *)&(p->BSRR);
    brr[1] = (uint16_t)mask;
#endif
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    GPIO_TypeDef *p = (GPIO_TypeDef *)port;
    unsigned irq_state = irq_disable();
    p->ODR ^= mask;
    irq_restore(irq_state);
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    GPIO_TypeDef *p = (GPIO_TypeDef *)port;
    p->ODR = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return pin & 0xfffffff0LU;
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return pin & 0xfLU;
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if (port < GPIOA_BASE) {
        return (void *)port;
    }

    return NULL;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    switch (num) {
    default:
        return false;
#ifdef GPIOA_BASE
    case 0:
#endif
#ifdef GPIOB_BASE
    case 1:
#endif
#ifdef GPIOC_BASE
    case 2:
#endif
#ifdef GPIOD_BASE
    case 3:
#endif
#ifdef GPIOE_BASE
    case 4:
#endif
#ifdef GPIOF_BASE
    case 5:
#endif
#ifdef GPIOG_BASE
    case 6:
#endif
#ifdef GPIOH_BASE
    case 7:
#endif
#ifdef GPIOI_BASE
    case 8:
#endif
#ifdef GPIOJ_BASE
    case 9:
#endif
#ifdef GPIOK_BASE
    case 10:
#endif
#ifdef GPIOL_BASE
    case 11:
#endif
#ifdef GPIOM_BASE
    case 12:
#endif
#ifdef GPION_BASE
    case 13:
#endif
#ifdef GPIOO_BASE
    case 14:
#endif
#ifdef GPIOP_BASE
    case 15:
#endif
#ifdef GPIOQ_BASE
    case 16:
#endif
#ifdef GPIOR_BASE
    case 17:
#endif
#ifdef GPIOS_BASE
    case 18:
#endif
#ifdef GPIOT_BASE
    case 19:
#endif
#ifdef GPIOU_BASE
    case 20:
#endif
#ifdef GPIOV_BASE
    case 21:
#endif
#ifdef GPIOW_BASE
    case 22:
#endif
#ifdef GPIOX_BASE
    case 23:
#endif
#ifdef GPIOY_BASE
    case 24:
#endif
#ifdef GPIOZ_BASE
    case 25:
#endif
    return true;
    }
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

#endif /* GPIO_LL_ARCH_H */
/** @} */
