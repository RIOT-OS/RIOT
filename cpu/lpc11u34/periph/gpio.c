/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"
#include "periph/gpio.h"

typedef struct {
    gpio_cb_t cb;
    void *arg;
} gpio_state_t;

/* Static IOCON registers definition */
volatile uint32_t * const lpc_pin_registers[] = {
    /* PORT 0 (PIO0_0 -> PIO0_23) */
    &LPC_IOCON->RESET_PIO0_0,  &LPC_IOCON->PIO0_1,
    &LPC_IOCON->PIO0_2,        &LPC_IOCON->PIO0_3,
    &LPC_IOCON->PIO0_4,        &LPC_IOCON->PIO0_5,
    &LPC_IOCON->PIO0_6,        &LPC_IOCON->PIO0_7,
    &LPC_IOCON->PIO0_8,        &LPC_IOCON->PIO0_9,
    &LPC_IOCON->SWCLK_PIO0_10, &LPC_IOCON->TDI_PIO0_11,
    &LPC_IOCON->TMS_PIO0_12,   &LPC_IOCON->TDO_PIO0_13,
    &LPC_IOCON->TRST_PIO0_14,  &LPC_IOCON->SWDIO_PIO0_15,
    &LPC_IOCON->PIO0_16,       &LPC_IOCON->PIO0_17,
    &LPC_IOCON->PIO0_18,       &LPC_IOCON->PIO0_19,
    &LPC_IOCON->PIO0_20,       &LPC_IOCON->PIO0_21,
    &LPC_IOCON->PIO0_22,       &LPC_IOCON->PIO0_23,
    /* PORT 1 (PIO1_0 -> PIO0_31) */
    &LPC_IOCON->PIO1_0,        &LPC_IOCON->PIO1_1,
    &LPC_IOCON->PIO1_2,        &LPC_IOCON->PIO1_3,
    &LPC_IOCON->PIO1_4,        &LPC_IOCON->PIO1_5,
    &LPC_IOCON->PIO1_6,        &LPC_IOCON->PIO1_7,
    &LPC_IOCON->PIO1_8,        &LPC_IOCON->PIO1_9,
    &LPC_IOCON->PIO1_10,       &LPC_IOCON->PIO1_11,
    &LPC_IOCON->PIO1_12,       &LPC_IOCON->PIO1_13,
    &LPC_IOCON->PIO1_14,       &LPC_IOCON->PIO1_15,
    &LPC_IOCON->PIO1_16,       &LPC_IOCON->PIO1_17,
    &LPC_IOCON->PIO1_18,       &LPC_IOCON->PIO1_19,
    &LPC_IOCON->PIO1_20,       &LPC_IOCON->PIO1_21,
    &LPC_IOCON->PIO1_22,       &LPC_IOCON->PIO1_23,
    &LPC_IOCON->PIO1_24,       &LPC_IOCON->PIO1_25,
    &LPC_IOCON->PIO1_26,       &LPC_IOCON->PIO1_27,
    &LPC_IOCON->PIO1_28,       &LPC_IOCON->PIO1_29,
    &LPC_IOCON->PIO1_30,       &LPC_IOCON->PIO1_31,
};

static int8_t flex_int_mapping[GPIO_NUMOF];

static gpio_state_t gpio_config[GPIO_NUMOF];
static uint8_t gpio_int_id = 0;

/* static port mappings */
static const uint8_t gpio_port_map[GPIO_NUMOF] = {
#if GPIO_0_EN
    [GPIO_0] = GPIO_0_PORT,
#endif
#if GPIO_1_EN
    [GPIO_1] = GPIO_1_PORT,
#endif
#if GPIO_2_EN
    [GPIO_2] = GPIO_2_PORT,
#endif
#if GPIO_3_EN
    [GPIO_3] = GPIO_3_PORT,
#endif
#if GPIO_4_EN
    [GPIO_4] = GPIO_4_PORT,
#endif
#if GPIO_5_EN
    [GPIO_5] = GPIO_5_PORT,
#endif
#if GPIO_6_EN
    [GPIO_6] = GPIO_6_PORT,
#endif
#if GPIO_7_EN
    [GPIO_7] = GPIO_7_PORT,
#endif
#if GPIO_8_EN
    [GPIO_8] = GPIO_8_PORT,
#endif
#if GPIO_9_EN
    [GPIO_9] = GPIO_9_PORT,
#endif
#if GPIO_10_EN
    [GPIO_10] = GPIO_10_PORT,
#endif
#if GPIO_11_EN
    [GPIO_11] = GPIO_11_PORT,
#endif
#if GPIO_12_EN
    [GPIO_12] = GPIO_12_PORT,
#endif
#if GPIO_13_EN
    [GPIO_13] = GPIO_13_PORT,
#endif
#if GPIO_14_EN
    [GPIO_14] = GPIO_14_PORT,
#endif
#if GPIO_15_EN
    [GPIO_15] = GPIO_15_PORT,
#endif
#if GPIO_16_EN
    [GPIO_16] = GPIO_16_PORT,
#endif
#if GPIO_17_EN
    [GPIO_17] = GPIO_17_PORT,
#endif
#if GPIO_18_EN
    [GPIO_18] = GPIO_18_PORT,
#endif
#if GPIO_19_EN
    [GPIO_19] = GPIO_19_PORT,
#endif
#if GPIO_20_EN
    [GPIO_20] = GPIO_20_PORT,
#endif
#if GPIO_21_EN
    [GPIO_21] = GPIO_21_PORT,
#endif
#if GPIO_22_EN
    [GPIO_22] = GPIO_22_PORT,
#endif
#if GPIO_23_EN
    [GPIO_23] = GPIO_23_PORT,
#endif
#if GPIO_24_EN
    [GPIO_24] = GPIO_24_PORT,
#endif
#if GPIO_25_EN
    [GPIO_25] = GPIO_25_PORT,
#endif
#if GPIO_26_EN
    [GPIO_26] = GPIO_26_PORT,
#endif
#if GPIO_27_EN
    [GPIO_27] = GPIO_27_PORT,
#endif
#if GPIO_28_EN
    [GPIO_28] = GPIO_28_PORT,
#endif
#if GPIO_29_EN
    [GPIO_29] = GPIO_29_PORT,
#endif
#if GPIO_30_EN
    [GPIO_30] = GPIO_30_PORT,
#endif
#if GPIO_31_EN
    [GPIO_31] = GPIO_31_PORT,
#endif
};

/* static pin mappings */
static const uint8_t gpio_pin_map[GPIO_NUMOF] = {
#if GPIO_0_EN
    [GPIO_0] = GPIO_0_PIN,
#endif
#if GPIO_1_EN
    [GPIO_1] = GPIO_1_PIN,
#endif
#if GPIO_2_EN
    [GPIO_2] = GPIO_2_PIN,
#endif
#if GPIO_3_EN
    [GPIO_3] = GPIO_3_PIN,
#endif
#if GPIO_4_EN
    [GPIO_4] = GPIO_4_PIN,
#endif
#if GPIO_5_EN
    [GPIO_5] = GPIO_5_PIN,
#endif
#if GPIO_6_EN
    [GPIO_6] = GPIO_6_PIN,
#endif
#if GPIO_7_EN
    [GPIO_7] = GPIO_7_PIN,
#endif
#if GPIO_8_EN
    [GPIO_8] = GPIO_8_PIN,
#endif
#if GPIO_9_EN
    [GPIO_9] = GPIO_9_PIN,
#endif
#if GPIO_10_EN
    [GPIO_10] = GPIO_10_PIN,
#endif
#if GPIO_11_EN
    [GPIO_11] = GPIO_11_PIN,
#endif
#if GPIO_12_EN
    [GPIO_12] = GPIO_12_PIN,
#endif
#if GPIO_13_EN
    [GPIO_13] = GPIO_13_PIN,
#endif
#if GPIO_14_EN
    [GPIO_14] = GPIO_14_PIN,
#endif
#if GPIO_15_EN
    [GPIO_15] = GPIO_15_PIN,
#endif
#if GPIO_16_EN
    [GPIO_16] = GPIO_16_PIN,
#endif
#if GPIO_17_EN
    [GPIO_17] = GPIO_17_PIN,
#endif
#if GPIO_18_EN
    [GPIO_18] = GPIO_18_PIN,
#endif
#if GPIO_19_EN
    [GPIO_19] = GPIO_19_PIN,
#endif
#if GPIO_20_EN
    [GPIO_20] = GPIO_20_PIN,
#endif
#if GPIO_21_EN
    [GPIO_21] = GPIO_21_PIN,
#endif
#if GPIO_22_EN
    [GPIO_22] = GPIO_22_PIN,
#endif
#if GPIO_23_EN
    [GPIO_23] = GPIO_23_PIN,
#endif
#if GPIO_24_EN
    [GPIO_24] = GPIO_24_PIN,
#endif
#if GPIO_25_EN
    [GPIO_25] = GPIO_25_PIN,
#endif
#if GPIO_26_EN
    [GPIO_26] = GPIO_26_PIN,
#endif
#if GPIO_27_EN
    [GPIO_27] = GPIO_27_PIN,
#endif
#if GPIO_28_EN
    [GPIO_28] = GPIO_28_PIN,
#endif
#if GPIO_29_EN
    [GPIO_29] = GPIO_29_PIN,
#endif
#if GPIO_30_EN
    [GPIO_30] = GPIO_30_PIN,
#endif
#if GPIO_31_EN
    [GPIO_31] = GPIO_31_PIN,
#endif
};

int gpio_init(gpio_t dev, gpio_dir_t dir, gpio_pp_t pullup)
{
    uint8_t port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    /* Get the port and pin values */
    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    /* Put pin in the primary function */
    *lpc_pin_registers[pin + (port * 24)] = *lpc_pin_registers[pin + (port * 24)] & ~(0x07);

    /* Disable resistors */
    *lpc_pin_registers[pin + (port * 24)] &= ~(3 << 3);
    /* Set resistors */
    if (pullup == GPIO_PULLUP) {
        *lpc_pin_registers[pin + (port * 24)] |= (2 << 3);
    }
    else if (pullup == GPIO_PULLDOWN) {
        *lpc_pin_registers[pin + (port * 24)] |= (1 << 3);
    }

    /* Set direction */
    if (dir == GPIO_DIR_OUT) {
        LPC_GPIO->DIR[port] |= (1 << pin);           /* set pin to output mode */
    }
    else {
        LPC_GPIO->DIR[port] &= ~(1 << pin);           /* set pin to output mode */
    }

    return 0; /* all OK */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    uint8_t pin;
    uint8_t port;

    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 19);

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    if (gpio_int_id > 7) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    /* configure pin as input */
    res = gpio_init(dev, GPIO_DIR_IN, pullup);
    if (res < 0) {
        return res;
    }

    /* Disable interrupts */
    NVIC_DisableIRQ(gpio_int_id);
    /* Register pin as flex interrupt */
    LPC_SYSCON->PINTSEL[gpio_int_id] = (port * 24) + pin;

    /* set callback */
    gpio_config[gpio_int_id].cb = cb;
    gpio_config[gpio_int_id].arg = arg;

    /* configure the event that triggers an interrupt */
    switch (flank) {
        case GPIO_RISING:
            LPC_GPIO_PIN_INT->ISEL &= ~(1 << gpio_int_id); /* Set PMODE=edge sensitive */
            LPC_GPIO_PIN_INT->IENR |= (1 << gpio_int_id);  /* Enable rising edge. */
            LPC_GPIO_PIN_INT->IENF &= ~(1 << gpio_int_id); /* Disable falling edge. */
            break;
        case GPIO_FALLING:
            LPC_GPIO_PIN_INT->ISEL &= ~(1 << gpio_int_id); /* Set PMODE=edge sensitive */
            LPC_GPIO_PIN_INT->IENR &= ~(1 << gpio_int_id); /* Disable rising edge. */
            LPC_GPIO_PIN_INT->IENF |= (1 << gpio_int_id);  /* Enable falling edge. */
            break;
        case GPIO_BOTH:
            LPC_GPIO_PIN_INT->ISEL &= ~(1 << gpio_int_id); /* Set PMODE=edge sensitive */
            LPC_GPIO_PIN_INT->IENR |= (1 << gpio_int_id);  /* Enable rising edge. */
            LPC_GPIO_PIN_INT->IENF |= (1 << gpio_int_id);  /* Enable falling edge. */
            break;
    }

    /* clear any pending requests */
    LPC_GPIO_PIN_INT->RISE = (1 << gpio_int_id); /* Clear rising edge (sort of) flag */
    LPC_GPIO_PIN_INT->FALL = (1 << gpio_int_id); /* Clear falling edge (sort of) flag */

    NVIC_SetPriority(gpio_int_id, 3);

    flex_int_mapping[dev] = gpio_int_id+1;

    gpio_int_id++;

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    if (dev >= GPIO_NUMOF) {
        return;
    }

    NVIC_EnableIRQ(flex_int_mapping[dev]-1);
}

void gpio_irq_disable(gpio_t dev)
{
    uint8_t int_id;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    int_id = flex_int_mapping[dev] - 1;

    NVIC_DisableIRQ(int_id);
}

int gpio_read(gpio_t dev)
{
    uint8_t port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    if(LPC_GPIO->PIN[port] & (1 << pin)) /* read output data register */
        return 1;
    else
        return 0;
}

void gpio_set(gpio_t dev)
{
    uint8_t port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    LPC_GPIO->SET[port] = (1 << pin);
}

void gpio_clear(gpio_t dev)
{
    uint8_t port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    LPC_GPIO->CLR[port] = (1 << pin);
}

void gpio_toggle(gpio_t dev)
{
    if (dev >= GPIO_NUMOF) {
        return;
    }

    LPC_GPIO->NOT[gpio_port_map[dev]] = (1 << gpio_pin_map[dev]);
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    } else {
        gpio_clear(dev);
    }
}

static inline void isr_common(uint8_t int_id) {

    LPC_GPIO_PIN_INT->IST |= (1 << int_id); /* Clear pending interrupt */
    gpio_config[int_id].cb(gpio_config[int_id].arg);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_pinint0(void)
{
    isr_common(0);
}
void isr_pinint1(void)
{
    isr_common(1);
}
void isr_pinint2(void)
{
    isr_common(2);
}
void isr_pinint3(void)
{
    isr_common(3);
}
void isr_pinint4(void)
{
    isr_common(4);
}
void isr_pinint5(void)
{
    isr_common(5);
}
void isr_pinint6(void)
{
    isr_common(6);
}
void isr_pinint7(void)
{
    isr_common(7);
}
