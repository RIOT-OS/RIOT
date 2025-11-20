/*
 * SPDX-FileCopyrightText: 2014 Hauke Petersen <devel@haukepetersen.de>
 * SPDX-FileCopyrightText: 2015 Hamburg University of Applied Sciences
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include "bitarithm.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief Bit positions in the GPIO mode value
 * @{
 */
#define MODE_BIT_IO         (0x1)
#define MODE_BIT_PUE        (0x2)
#define MODE_BIT_ODE        (0x4)
/** @} */

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief We store 4 bit for each external interrupt line (each pin) that can
 *        mapped to an entry in the exti_ctx table
 */
#define EXTI_MAP_LENGTH     (16U)

/**
 * @brief We allow for 7 (4-bit - 1) concurrent external interrupts to be set
 */
#define CTX_NUMOF           (7U)

/**
 * @brief Allocation of memory for 7 independent interrupt slots
 */
static gpio_isr_ctx_t exti_ctx[CTX_NUMOF];

/**
 * @brief Allocation of 4 bit per pin to map a pin to an interrupt context
 */
static uint32_t exti_map[EXTI_MAP_LENGTH] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};

/**
 * @brief Get context for a specific pin
 */
static inline unsigned _ctx(int port, int pin)
{
    return (exti_map[(port * 4) + (pin >> 3)] >> ((pin & 0x7) * 4)) & 0xf;
}

/**
 * @brief Write an entry to the context map array
 */
static void _write_map(int port, int pin, int ctx)
{
    exti_map[(port * 4) + (pin >> 3)] &= ~(0xf << ((pin & 0x7) * 4));
    exti_map[(port * 4) + (pin >> 3)] |=  (ctx << ((pin & 0x7) * 4));
}

/**
 * @brief Find a free spot in the array containing the interrupt contexts
 */
static int _get_free_ctx(void)
{
    for (unsigned i = 0; i < CTX_NUMOF; i++) {
        if (exti_ctx[i].cb == NULL) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Clear the context of the given pin
 */
static void _ctx_clear(int port, int pin)
{
    unsigned ctx = _ctx(port, pin);
    if (ctx < CTX_NUMOF) {
        exti_ctx[ctx].cb = NULL;
        _write_map(port, pin, CTX_NUMOF);
    }
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief Extract the pin's port base address from the given pin identifier
 */
static inline Pio *_port(gpio_t pin)
{
    return (Pio *)(pin & ~(0x1f));
}

/**
 * @brief Extract the port number from the given pin identifier
 *
 * Isolating bits 9 to 12 of the port base addresses leads to unique port
 * numbers.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 9) & 0x0f) - 7);
}

/**
 * @brief Test if the given port is valid
 */
static bool _port_valid(Pio *port)
{
    if (port == PIOA || port == PIOB || port == PIOC) {
        return true;
    }
#ifdef PIOD
    if (port == PIOD) {
        return true;
    }
#endif
    return false;
}

/**
 * @brief Get the pin number from the pin identifier, encoded in the LSB 5 bit
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x1f);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    Pio *port = _port(pin);
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* make sure port is valid and no pull-down is selected*/
    if (!_port_valid(port) || (mode == GPIO_IN_PD)) {
        return -1;
    }

    /* power on the corresponding port */
    PMC->PMC_PCER0 = (1 << (port_num + 11));

    /* SAM4s MCUs requires WPKEY for enabling peripheral */
#ifdef PIO_WPMR_WPKEY_PASSWD
    port->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
#endif

    /* disable interrupt and clear context (to be safe) */
    port->PIO_IDR = (1 << pin_num);
#ifdef MODULE_PERIPH_GPIO_IRQ
    _ctx_clear(port_num, pin_num);
#endif /* MODULE_PERIPH_GPIO_IRQ */

     /* give the PIO module the power over the corresponding pin */
    port->PIO_PER = (1 << pin_num);

    /* configure pin direction (in/out) */
    if (mode & MODE_BIT_IO) {
        port->PIO_OER = (1 << pin_num);
    }
    else {
        port->PIO_ODR = (1 << pin_num);
    }
    /* set pull-up */
    if (mode & MODE_BIT_PUE) {
        port->PIO_PUER = (1 << pin_num);
    }
    else {
        port->PIO_PUDR = (1 << pin_num);
    }
    /* set multi-driver (open-drain) mode */
    if (mode & MODE_BIT_ODE) {
        port->PIO_MDER = (1 << pin_num);
    }
    else {
        port->PIO_MDDR = (1 << pin_num);
    }

    return 0;
}

void gpio_init_mux(gpio_t pin, gpio_mux_t mux)
{
    /* power on the corresponding port */
    PMC->PMC_PCER0 = (1 << (_port_num(pin) + 11));
    /* give peripheral control over the pin */
    _port(pin)->PIO_PDR = (1 << _pin_num(pin));
    /* and configure the MUX */
#ifdef PIO_ABCDSR_P0
    _port(pin)->PIO_ABCDSR[0] &= ~(1 << _pin_num(pin));
    _port(pin)->PIO_ABCDSR[1] &= ~(1 << _pin_num(pin));
    if (mux & 0x2) {
        _port(pin)->PIO_ABCDSR[1] |= 1 << _pin_num(pin);
    }
    if (mux & 0x1) {
        _port(pin)->PIO_ABCDSR[0] |= 1 << _pin_num(pin);
    }
#else
    _port(pin)->PIO_ABSR &= ~(1 << _pin_num(pin));
    _port(pin)->PIO_ABSR |=  (mux << _pin_num(pin));
#endif
}

void gpio_set(gpio_t pin)
{
    _port(pin)->PIO_SODR = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->PIO_CODR = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        _port(pin)->PIO_CODR = (1 << _pin_num(pin));
    } else {
        _port(pin)->PIO_SODR = (1 << _pin_num(pin));
    }
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        _port(pin)->PIO_SODR = (1 << _pin_num(pin));
    } else {
        _port(pin)->PIO_CODR = (1 << _pin_num(pin));
    }
}

void gpio_irq_enable(gpio_t pin)
{
    NVIC_EnableIRQ((1 << (_port_num(pin) + PIOA_IRQn)));
}

void gpio_irq_disable(gpio_t pin)
{
    NVIC_DisableIRQ((1 << (_port_num(pin) + PIOA_IRQn)));
}

bool gpio_read(gpio_t pin)
{
    Pio *port = _port(pin);
    int pin_num = _pin_num(pin);

     if (port->PIO_OSR & (1 << pin_num)) {
        return (port->PIO_ODSR & (1 << pin_num)) ? 1 : 0;
    }
    else {
        return (port->PIO_PDSR & (1 << pin_num)) ? 1 : 0;
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    Pio *port = _port(pin);
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* make sure pin is valid */
    if (!_port_valid(port)) {
        return -1;
    }

    /* configure pin as input */
    gpio_init(pin, mode);

    /* try go grab a free spot in the context array */
    int ctx_num = _get_free_ctx();
    if (ctx_num < 0) {
        return -1;
    }
    /* save context */
    exti_ctx[ctx_num].cb = cb;
    exti_ctx[ctx_num].arg = arg;
    _write_map(port_num, pin_num, ctx_num);

    /* set the active flank */
    switch (flank) {
        case GPIO_FALLING:
            port->PIO_AIMER = (1 << pin_num);
            port->PIO_ESR = (1 << pin_num);
            port->PIO_FELLSR =(1 << pin_num);
            break;
        case GPIO_RISING:
            port->PIO_AIMER = (1 << pin_num);
            port->PIO_ESR = (1 << pin_num);
            port->PIO_REHLSR = (1 << pin_num);
            break;
        case GPIO_BOTH:
            port->PIO_AIMDR = (1 << pin_num);
            break;
    }
    /* clean interrupt status register */
    port->PIO_ISR;
    /* enable the interrupt for the given channel */
    NVIC_EnableIRQ(port_num + PIOA_IRQn);
    port->PIO_IER = (1 << pin_num);

    return 0;
}

static inline void isr_handler(Pio *port, int port_num)
{
    /* take interrupt flags only from pins which interrupt is enabled */
    uint32_t status = (port->PIO_ISR & port->PIO_IMR);

    while (status) {
        uint8_t pin_number;
        status = bitarithm_test_and_clear(status, &pin_number);
        int ctx = _ctx(port_num, pin_number);
        exti_ctx[ctx].cb(exti_ctx[ctx].arg);
    }
    cortexm_isr_end();
}

void isr_pioa(void)
{
    isr_handler(PIOA, PA);
}

void isr_piob(void)
{
    isr_handler(PIOB, PB);
}

void isr_pioc(void)
{
    isr_handler(PIOC, PC);
}

#ifdef PIOD
void isr_piod(void)
{
    isr_handler(PIOD, PD);
}
#endif
#endif /* MODULE_PERIPH_GPIO_IRQ */
