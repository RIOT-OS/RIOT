/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup        cpu_cc3200
 * @{
 *
 * @file
 * @brief           GPIO driver implementation
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "vendor/hw_gpio.h"
#include "vendor/hw_memmap.h"
#include "vendor/hw_ocp_shared.h"
#include "vendor/rom.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define GPIO_PINS_PER_PORT 8        /**< Number of pins per port */
#define GPIO_DIR_MASK 0x00000001    /**< GPIO direction configuration mask */

/**
 * GPIO Pin type value used to configure pin to GPIO
 */
#define PIN_MODE_GPIO 0x00000000

#define PAD_MODE_MASK 0x0000000F /**< GPIO mask for setting pin mode */
/* GPIO mask for setting pin strength */
#define PAD_STRENGTH_MASK 0x000000E0
/* GPIO mask for setting pin type e.g. PULL */
#define PAD_TYPE_MASK 0x00000310
#define PAD_CONFIG_BASE ((OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_0))

/* Define interrupt signal for GPIOA4 since it is not exported by TIs hw_ints.h
 */
#define INT_GPIOA4 20

/**
 * @brief get hardware configuration register for a pin
 *
 */
#define PAD_CONFIG_REG(pin) \
    (*((volatile uint32_t *)((gpio_pin << 2) + PAD_CONFIG_BASE)))

/**
 * @brief gpio base addresses
 *
 */
static const uint32_t ports[] = {
    GPIOA0_BASE, GPIOA1_BASE, GPIOA2_BASE, GPIOA3_BASE, GPIOA4_BASE,
};

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   static callback memory
 */
static gpio_isr_ctx_t isr_ctx[ARRAY_SIZE(ports)][GPIO_PINS_PER_PORT];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief pin to GPIO pin numbers mappings
 *
 */
static const uint8_t pin_to_gpio_num[64] = {
    10,  11,  12,  13,  14,  15,  16,  17,  255, 255, 18,  19,  20,
    21,  22,  23,  24,  40,  28,  29,  25,  255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 31,  255, 255, 255, 255, 0,   255, 32,
    30,  255, 1,   255, 2,   3,   4,   5,   6,   7,   8,   9
};

/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t gpio_pin_num(gpio_t pin)
{
    return (pin & 0x3f);
}

/**
 * @brief     Extract the port number of a given pin
 */
static inline uint8_t gpio_port_num(gpio_t pin)
{
    return (pin >> 6) & 0x3;
}

/**
 * @brief get pin mask for a given pin
 *
 * @param dev
 * @return gpio pin offset for port
 */
uint8_t gpio_pin_mask(uint8_t dev)
{
    return 1 << (pin_to_gpio_num[gpio_pin_num(dev)] % GPIO_PINS_PER_PORT);
}
/**
 * @brief gpio_port_by_num returns the port base address for a pin
 *
 * @param dev external pin number
 * @return port base address
 */
static inline cc3200_gpio_t *gpio_port_by_num(uint8_t port_num)
{
    return (cc3200_gpio_t *)(ports[port_num]);
}

/**
 * @brief returns the masked value for a external pin
 *
 * @param pin external pin
 * @param val value to be masked
 * @return masked value for a given pin with value val
 */
static inline uint8_t _gpio_pin_value_mask(uint8_t pin, uint8_t val)
{
    return val << (pin & 0x7);
}

/**
 * @brief Access GPIO low-level device
 *
 * @param[in] pin   gpio pin
 *
 * @return          pointer to gpio low level device address
 */
static inline cc3200_gpio_t *gpio(gpio_t pin)
{
    return (cc3200_gpio_t *)(gpio_port_by_num(gpio_port_num(pin)));
}

void gpio_init_af(gpio_t dev, uint32_t strength, uint32_t type)
{
    /* does not support analog pin types, but not a problem for GPIO */
    uint8_t pin = gpio_pin_num(dev);
    uint8_t gpio_pin = pin_to_gpio_num[pin];

    /* copied from TIs PinConfigSet. The register is not documented so for */
    /* now only replecate behaviour. */

    /* enable input */
    HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CMN_CONFIG) &=
        ~((0x80 << gpio_pin) & (0x1E << 8));

    /* write config to hardware register referred by TI as PAD */
    PAD_CONFIG_REG(gpio_pin) =
        ((PAD_CONFIG_REG(gpio_pin) & ~(PAD_STRENGTH_MASK | PAD_TYPE_MASK)) |
         (strength | type));
}

void gpio_pin_mode_set(gpio_t dev, uint32_t mode)
{
    /* does not support analog pin types, but not a problem for GPIO */
    uint8_t pin = gpio_pin_num(dev);
    uint8_t gpio_pin = pin_to_gpio_num[pin];

    /* set mode */
    PAD_CONFIG_REG(gpio_pin) =
        (((PAD_CONFIG_REG(gpio_pin) & ~PAD_MODE_MASK) | mode) & ~(3 << 10));
}

int gpio_init(gpio_t dev, gpio_mode_t mode)
{
    /* get pin mask */
    uint8_t ipin = gpio_pin_mask(dev);

    /* make sure pin is in the default state (this is quicker then reading */
    /* first) */
    gpio_init_af(dev, PIN_STRENGTH_2MA, PIN_TYPE_STD);
    gpio_pin_mode_set(dev, PIN_MODE_GPIO);

    /* set gpio direction IN/OUT */
    if (mode & GPIO_DIR_MASK) {
        /* out */
        gpio(dev)->dir |= ipin;
    }
    else {
        /* in */
        gpio(dev)->dir &= ipin;
    }

    /* set strength and drain mode */
    gpio_init_af(dev, PIN_STRENGTH_2MA, mode & PAD_TYPE_MASK);

    return 0;
}

#ifdef MODULE_PERIPH_GPIO_IRQ

/**
 * @brief isr interrupt handler
 *
 * @param portAddr base address of the GPIO PORT
 */
void handle_isr(uint8_t port_num)
{
    uint32_t state = gpio_port_by_num(port_num)->mis;

    /* clear interrupt */
    gpio_port_by_num(port_num)->icr = state;

    /* call ISR handlers */
    state &= (GPIO_PINS_PER_PORT - 1);
    while (state) {
        int pin = 8 * sizeof(state) - __builtin_clz(state) - 1;
        state &= ~(1 << pin);
        isr_ctx[port_num][pin].cb(isr_ctx[port_num][pin].arg);
    }

    cortexm_isr_end();
}

void isr_gpio_a0(void)
{
    handle_isr(0);
}

void isr_gpio_a1(void)
{
    handle_isr(1);
}

void isr_gpio_a2(void)
{
    handle_isr(2);
}

void isr_gpio_a3(void)
{
    handle_isr(3);
}

void isr_gpio_a4(void)
{
    handle_isr(4);
}

/**
 * @brief enable GPIO interrupt
 * @param dev pin
 */
void gpio_irq_enable(gpio_t dev)
{
    gpio(dev)->im |= gpio_pin_mask(dev);
}

/**
 * @brief disable GPIO interrupt
 * @param dev pin
 */
void gpio_irq_disable(gpio_t dev)
{
    gpio(dev)->im &= ~(gpio_pin_mask(dev));
}

int gpio_init_int(gpio_t dev, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* Note: gpio_init() also checks if the gpio is enabled. */
    int res = gpio_init(dev, mode);

    if (res != 0) {
        return res;
    }

    assert(flank != GPIO_NONE);

    uint8_t portNum = gpio_port_num(dev);
    uint8_t pinNum = gpio_pin_num(dev);
    uint8_t bit = gpio_pin_mask(dev);
    /* convert to number since we use the address for later compare */
    uint32_t portBase = (uint32_t)gpio_port_by_num(portNum);

    /* store callback information; */
    isr_ctx[portNum][pinNum].cb = cb;
    isr_ctx[portNum][pinNum].arg = arg;

    ROM_IntMasterDisable();

    /* clear interrupt specified pin */
    gpio(dev)->icr = bit;

    /* configure active flanks */
    gpio(dev)->ibe =
        (flank & GPIO_BOTH) ? gpio(dev)->ibe | bit : gpio(dev)->ibe & ~bit;
    gpio(dev)->is = (flank & GPIO_VALUE_LOW) ? gpio(dev)->is | bit :
                    gpio(dev)->is & ~bit;
    gpio(dev)->iev = (flank & GPIO_RISING) ? gpio(dev)->iev | bit :
                     gpio(dev)->iev & ~bit;

    /* enable gpio interripts */
    gpio(dev)->im |= bit;

    /* register interrupt handlers */
    /* TODO: replace with cortex common */
    switch (portBase) {
        case GPIOA0_BASE:
            ROM_GPIOIntRegister(portBase, isr_gpio_a0);
            ROM_IntEnable(INT_GPIOA0);
            break;
        case GPIOA1_BASE:
            ROM_GPIOIntRegister(portBase, isr_gpio_a1);
            ROM_IntEnable(INT_GPIOA1);
            break;
        case GPIOA2_BASE:
            ROM_GPIOIntRegister(portBase, isr_gpio_a2);
            ROM_IntEnable(INT_GPIOA2);
            break;
        case GPIOA3_BASE:
            ROM_GPIOIntRegister(portBase, isr_gpio_a3);
            ROM_IntEnable(INT_GPIOA3);
            break;
        case GPIOA4_BASE:
            ROM_GPIOIntRegister(portBase, isr_gpio_a4);
            ROM_IntEnable(INT_GPIOA4);
            break;
    }

    ROM_IntMasterEnable();

    return 0;
}
#endif

/**
 * @brief gpio_write writes to a GPIO pin dev (external) the value value
 *
 * @param dev external pin
 * @param value value to be written (will be masked)
 */
void gpio_write(gpio_t dev, int value)
{
    uint8_t port = gpio_port_num(dev);
    uint8_t ipin = gpio_pin_mask(dev);
    uint32_t portAddr = (uint32_t)gpio_port_by_num(port);

    /* write to pin at portBase + pinOffset */
    HWREG(portAddr + (ipin << 2)) =
        _gpio_pin_value_mask(pin_to_gpio_num[gpio_pin_num(dev)], value);
}

/**
 * @brief read a pins value
 *
 * @param dev external pin
 * @return int current value of a pin
 */
int gpio_read(gpio_t dev)
{
    uint8_t port = gpio_port_num(dev);
    uint8_t ipin = gpio_pin_mask(dev);
    uint32_t portAddr = (uint32_t)gpio_port_by_num(port);

    /* read from pin at portBase + pinOffset */
    /* cast value to int {0, 1} */
    return (HWREG(portAddr + (ipin << 2)) ? GPIO_VALUE_HIGH : GPIO_VALUE_LOW);
}

/**
 * @brief set a pins value to GPIO_VALUE_HIGH (1)
 *
 * @param dev external pin
 */
void gpio_set(gpio_t dev)
{
    gpio_write(dev, GPIO_VALUE_HIGH);
}

/**
 * @brief set a pins value to GPIO_VALUE_LOW (0)
 *
 * @param dev external pin
 */
void gpio_clear(gpio_t dev)
{
    gpio_write(dev, GPIO_VALUE_LOW);
}

/**
 * @brief toggle a gpio pins value
 *
 * @param dev external pin
 */
void gpio_toggle(gpio_t dev)
{
    gpio_read(dev) ? gpio_clear(dev) : gpio_set(dev);
}
