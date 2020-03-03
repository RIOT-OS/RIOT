/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23x17
 * @brief       Device driver implementation for Microchip MCP23x17 I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <stdint.h>

#include "assert.h"
#include "periph/i2c.h"
#include "xtimer.h"

#include "mcp23x17.h"
#include "mcp23x17_regs.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#if ENABLE_DEBUG

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[mcp23x17] %s dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define DEV_BUS     (dev->params.dev)
#define DEV_ADDR    (dev->params.addr)
#define DEV_CS      (dev->params.cs)
#define DEV_CLK     (dev->params.clk)

#if MODULE_PERIPH_GPIO_IRQ

/* interrutp service routine for IRQs */
static void _irq_isr(void *arg);

/* IRQ event queue */
static event_queue_t _irq_queue;

/* stack for the IRQ handler thread */
static char _irq_thread_stack[MCP23X17_IRQ_THREAD_STACKSIZE];

/* PID of IRQ handler thread, KERNEL_PID_UNDEF if not created yet */
static kernel_pid_t _irq_thread_pid = KERNEL_PID_UNDEF;

/* IRQ handler thread */
static void *_irq_thread(void *arg);

/* declaration of IRQ handler function */
static void _irq_handler(event_t *event);

#endif /* MODULE_PERIPH_GPIO_IRQ */

/* forward declarations of internal functions */
static int _update_pin(mcp23x17_t *dev, uint8_t reg, gpio_t pin, int value);
static int _read(mcp23x17_t *dev, uint8_t reg, uint8_t *data, size_t len);
static int _write(mcp23x17_t *dev, uint8_t reg, const uint8_t *data, size_t len);

/* static power on reset configuration */
static const uint8_t _reset_conf[] =
{
    0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int mcp23x17_init(mcp23x17_t *dev, const mcp23x17_params_t* params)
{
    assert(dev);
    assert(params);

    dev->params = *params;
    dev->od_pins = 0;

    DEBUG_DEV("params %p", dev, params);

#if MODULE_MCP23S17
    /* initialize the chip select line if SPI is used */
    if (DEV_CS != GPIO_UNDEF && spi_init_cs(DEV_BUS, DEV_CS) != SPI_OK) {
        DEBUG_DEV("CS pin defined but could not be initialized\n", dev);
        return -MCP23X17_ERROR_NO_DEV;
    }
#endif
    if (params->reset_pin != GPIO_UNDEF) {
        /* if the low active RESET pin is defined, initialize it */
        if (gpio_init(params->reset_pin, GPIO_OUT)) {
            DEBUG_DEV("RESET pin defined but could not be initialized", dev);
            return -MCP23X17_ERROR_RESET_PIN;
        }
        /* hardware reset impuls for 10 us */
        gpio_clear(params->reset_pin);
        xtimer_usleep(10);
        gpio_set(params->reset_pin);
    }

    /*
     * After power on reset or hardware reset, the default BANK mode is 0
     * i.e., the A/B registers are paired and in the same bank with
     * subsequent ascending addresses.
     * Since the BANK mode is never changed, we can rely on the addressing
     * scheme as define in mcp23x17_regs.h.
     */

    uint8_t iocon; /* configuration register is the same for port A and B */

    /* read the configuration registers to see whether device is reachable */
    if (_read(dev, MCP23X17_REG_IOCONA, &iocon, 1) ||
        _read(dev, MCP23X17_REG_IOCONB, &iocon, 1)) {
        DEBUG_DEV("error reading IOCON registers", dev);
        return -MCP23X17_ERROR_NO_DEV;
    }

    /*
     * After power on reset or hardware reset, the configuration of GPIOs is:
     *
     * - GPIO pins are defined as inputs
     * - GPIO registers reflects the same logic state of the input pin
     * - GPIO pin value is compared against previous pin value for interrupt-on-change
     * - GPIO input pins are disabled for interrupts on change
     * - GPIO pull-ups are disabled
     * - GPIO default output values are 0
     *
     * If hardware reset is not used, we have restore this configuration after
     * system reboots
     */
    int res = _write(dev, MCP23X17_REG_IODIR, _reset_conf,
                     sizeof(_reset_conf) / sizeof(_reset_conf[0]));
    if (res) {
        DEBUG_DEV("error writing power on reset configuration", dev);
        return res;
    }

#if MODULE_PERIPH_GPIO_IRQ
    /*
     * If GPIO pin for interrupt signals INTA and INTB is defined in parameters,
     * we use interrupts.
     */
    if (params->int_pin != GPIO_UNDEF) {

        /* GPIO for interrupt signal has to be initialized */
        if (gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING,
                                               _irq_isr, (void*)dev)) {
            DEBUG_DEV("INT pin defined but could not be initialized", dev);
            return -MCP23X17_ERROR_INT_PIN;
        }

        /* create the interrupt handler thread */
        if (_irq_thread_pid == KERNEL_PID_UNDEF) {
            DEBUG("[%s] create irq_handler thread\n", __func__);
            _irq_thread_pid = thread_create(_irq_thread_stack,
                                            sizeof(_irq_thread_stack),
                                            MCP23X17_IRQ_THREAD_PRIO,
                                            THREAD_CREATE_STACKTEST,
                                            _irq_thread, NULL, "irq_handler");
            assert(_irq_thread_pid != KERNEL_PID_UNDEF);
        }

        /* initialize the IRQ event object used for delaying interrupts */
        dev->irq_event.event.handler = _irq_handler;
        dev->irq_event.dev = dev;

        for (unsigned i = 0; i < MCP23X17_GPIO_PIN_NUM; i++) {
            dev->isr[i].cb = NULL;
            dev->isr[i].arg = NULL;
        }


        /* We use push/pull outputs and active low interrupt signals. */
        iocon &= ~MCP23X17_IOCON_ODR;
        iocon &= ~MCP23X17_IOCON_INTPOL;

        /*
         * Since we use only one pin for INTA and INTB signal, we have to use
         * the MIRROR mode, i.e., an interrupt on either port will cause both
         * interrupt pins to activate.
         */
        iocon |= MCP23X17_IOCON_MIRROR;

        /*
         * Reset all interrupt-on-change control bits to 0, that is, pin values
         * are compared against the previous value for interrupt-on-change.
         * Disable all interrupts. Since this corresponds to the power on reset
         * configuration written before, we have not execute it here.
         */
        #if 0
        uint8_t zero = 0;
        if ((res = _write(dev, MCP23X17_REG_INTCONA, &zero, 1) ||
            (res = _write(dev, MCP23X17_REG_INTCONB, &zero, 1) ||
            (res = _write(dev, MCP23X17_REG_GPINTENA, &zero, 1) ||
            (res = _write(dev, MCP23X17_REG_GPINTENB, &zero, 1)) {

            DEBUG_DEV("error writing INTCON and GPINTEN registers", dev);
            return res;
        }
        #endif
    }
#endif /* MODULE_PERIPH_GPIO_IRQ */

    /* sequential operation and slew rate control for SDA used */
    iocon &= ~MCP23X17_IOCON_SEQOP;
    iocon &= ~MCP23X17_IOCON_DISSLW;

    /* write back configuration registers */
    if ((res = _write(dev, MCP23X17_REG_IOCONA, &iocon, 1)) ||
        (res = _write(dev, MCP23X17_REG_IOCONB, &iocon, 1))) {
        DEBUG_DEV("error writing IOCON registers", dev);
        return res;
    }

    return MCP23X17_OK;
}

int mcp23x17_gpio_init(mcp23x17_t *dev, gpio_t pin, gpio_mode_t mode)
{
    assert(dev);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %u, mode %d", dev, pin, mode);

    uint8_t port = pin >> 3;
    uint8_t pin_bit = 1 << (pin & 0x07);

    uint8_t iodir;
    uint8_t gppu;
    uint8_t gpinten;
    int res;

    /* read the I/O direction configuration and GPIO pull-up register */
    if ((res = _read(dev, MCP23X17_REG_IODIR + port, &iodir, 1)) ||
        (res = _read(dev, MCP23X17_REG_GPPU + port, &gppu, 1)) ||
        (res = _read(dev, MCP23X17_REG_GPINTEN + port, &gpinten, 1))) {
        DEBUG_DEV("error reading IODIR, GPPU or GPINTEN register", dev);
        return res;
    }

    /* set default configuration first */
    iodir |= pin_bit;               /* input pin */
    gppu &= ~pin_bit;               /* no pull-up */
    gpinten &= ~pin_bit;            /* interrupt disabled */
    dev->od_pins &= ~(1 << pin);    /* open-drain flag not set */

    /* override only non default settings */
    switch (mode) {
        case GPIO_OUT:   iodir &= ~pin_bit;          /* change direction to output */
                         break;
        case GPIO_OD_PU: gppu |= pin_bit;            /* enable pull-up */
        case GPIO_OD:    dev->od_pins |= (1 << pin); /* set open-drain flag */
                         _update_pin(dev, MCP23X17_REG_GPIO, pin, 0); /* clear pin */
                         break;
        case GPIO_IN_PU: gppu |= pin_bit;            /* enable pull-up */
        case GPIO_IN:    break;

        default: DEBUG_DEV("invalid pin mode", dev);
                 return -MCP23X17_ERROR_INV_MODE;
    }

    /* write back the I/O direction configuration and GPIO pull-up register */
    if ((res = _write(dev, MCP23X17_REG_GPINTEN + port, &gpinten, 1)) ||
        (res = _write(dev, MCP23X17_REG_IODIR + port, &iodir, 1)) ||
        (res = _write(dev, MCP23X17_REG_GPPU + port, &gppu, 1))) {
        DEBUG_DEV("error writing IODIR, GPPU or GPINTEN register", dev);
        return res;
    }

    return MCP23X17_OK;
}

int mcp23x17_gpio_read(mcp23x17_t *dev, gpio_t pin)
{
    assert(dev);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %u", dev, pin);

    uint8_t port = pin >> 3;
    pin -= (port << 3);

    uint8_t gpio;

    /* read the GPIO port register */
    int res = _read(dev, MCP23X17_REG_GPIO + port, &gpio, 1);
    if (res) {
        DEBUG_DEV("error reading GPIO register", dev);
        return res;
    }

    return (gpio & (1 << pin)) ? 1 : 0;
}

void mcp23x17_gpio_write(mcp23x17_t *dev, gpio_t pin, int value)
{
    assert(dev);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %u, value %d", dev, pin,value);

    /* check whether it is an emulated OD pin, OD pins */
    if (dev->od_pins & (1 << pin)) {
        if (value) {
            /* simply set direction to input */
            _update_pin(dev, MCP23X17_REG_IODIR, pin, 1);
        }
        else {
            /* set direction to output, was cleared during initialization */
            _update_pin(dev, MCP23X17_REG_IODIR, pin, 0);
        }
    }
    else {
        _update_pin(dev, MCP23X17_REG_GPIO, pin, value);
    }
}

void mcp23x17_gpio_set(mcp23x17_t *dev, gpio_t pin)
{
    mcp23x17_gpio_write(dev, pin, 1);
}

void mcp23x17_gpio_clear(mcp23x17_t *dev, gpio_t pin)
{
    mcp23x17_gpio_write(dev, pin, 0);
}

void mcp23x17_gpio_toggle(mcp23x17_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin %u", dev, pin);
    mcp23x17_gpio_write(dev, pin, mcp23x17_gpio_read(dev, pin) ? 0 : 1);
}

#if MODULE_PERIPH_GPIO_IRQ

int mcp23x17_gpio_init_int(mcp23x17_t *dev, gpio_t pin,
                                            gpio_mode_t mode,
                                            gpio_flank_t flank,
                                            gpio_cb_t isr,
                                            void *arg)
{
    assert(dev);
    assert(pin < MCP23X17_GPIO_PIN_NUM);
    assert(isr != NULL);

    DEBUG_DEV("pin %u, mode %d, flank %d, isr %p, arg %p",
              dev, pin, mode, flank, isr, arg);

    /* initialize the pin */
    int res = mcp23x17_gpio_init(dev, pin, mode);
    if (res != MCP23X17_OK) {
        return res;
    }

    switch (flank) {
        case GPIO_FALLING:
        case GPIO_RISING:
        case GPIO_BOTH: dev->isr[pin].cb = isr;
                        dev->isr[pin].arg = arg;
                        dev->flank[pin] = flank;
                        break;
        default: DEBUG_DEV("invalid flank %d for pin %d", dev, flank, pin);
                 return -MCP23X17_ERROR_INV_FLANK;
    }

    /* enable the interrupt */
    mcp23x17_gpio_irq_enable(dev, pin);

    return MCP23X17_OK;
}

void mcp23x17_gpio_irq_enable(mcp23x17_t *dev, gpio_t pin)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %u", dev, pin);

    _update_pin(dev, MCP23X17_REG_GPINTEN, pin, 1);
}

void mcp23x17_gpio_irq_disable(mcp23x17_t *dev, gpio_t pin)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %u", dev, pin);

    _update_pin(dev, MCP23X17_REG_GPINTEN, pin, 0);
}

/* interrupt service routine for IRQs */
static void _irq_isr(void *arg)
{
    /* some parameter sanity checks */
    assert(arg != NULL);

    /* just indicate that an interrupt occured and return */
    event_post(&_irq_queue, (event_t*)&((mcp23x17_t*)arg)->irq_event);
}

/* IRQ handler thread */
static void *_irq_thread(void *arg)
{
    /* initialize the IRQ event queue */
    event_queue_init(&_irq_queue);
    /* wait for pending IRQ events and call the IRQ handler */
    event_loop(&_irq_queue);

    return NULL;
}

/* handle one IRQ event of device referenced by the event */
static void _irq_handler(event_t* event)
{
    mcp23x17_irq_event_t* irq_event = (mcp23x17_irq_event_t*)event;

    assert(irq_event != NULL);
    assert(irq_event->dev);

    mcp23x17_t *dev = irq_event->dev;

    DEBUG_DEV("event %p", dev, event);

    uint32_t state = irq_disable();
    uint8_t regs[4];

    /* read the GPIO port register */
    if (_read(dev, MCP23X17_REG_INTF, regs, sizeof(regs) / sizeof(regs[0]))) {
        DEBUG_DEV("error reading INTF and INTCAP registers", dev);
        return;
    }

    /* iterate over all pins to check whether ISR has to be called */
    for (unsigned i = 0; i < MCP23X17_GPIO_PIN_NUM; i++) {
        uint8_t port = i >> 3;
        uint8_t pin_bit = 1 << (i & 0x07);

        /* test whether interrupt flag is set and cb is defined for the pin */
        if ((regs[port] & pin_bit) && dev->isr[i].cb != NULL) {

            /* check the flank and the activated flank mode */
            if (dev->flank[i] == GPIO_BOTH  ||        /* no matter what flank */
                ((regs[2 + port] & pin_bit) == 0 &&   /* new value is 0 -> falling flank */
                 (dev->flank[i] == GPIO_FALLING)) ||
                ((regs[2 + port] & pin_bit) &&        /* new value is 1 -> rising flank */
                 (dev->flank[i] == GPIO_RISING))) {
                /* call the ISR */
                dev->isr[i].cb(dev->isr[i].arg);
            }
        }
    }

    irq_restore(state);
}

#endif /* MODULE_PERIPH_GPIO_IRQ */

/* internal functions */

static int _read(mcp23x17_t *dev, uint8_t reg, uint8_t *data, size_t len)
{
#if MODULE_MCP23S17
    if (DEV_CS != GPIO_UNDEF) {
        if (spi_acquire(DEV_BUS, DEV_CS, SPI_MODE_0, DEV_CLK) != SPI_OK) {
            DEBUG_DEV("SPI acquire error", dev);
            return MCP23X17_ERROR_SPI;
        }
        spi_transfer_byte(DEV_BUS, DEV_CS, true, (DEV_ADDR << 1) | 1);
        spi_transfer_byte(DEV_BUS, DEV_CS, true, reg);
        spi_transfer_bytes(DEV_BUS, DEV_CS, false, NULL, data, len);

        spi_release(DEV_BUS);
        return MCP23X17_OK;
    }
    else {
#endif
#ifdef MODULE_MCP23017
        i2c_acquire(DEV_BUS);

        /* read the I/O direction configuration and GPIO pull-up register */
        if (i2c_read_regs(DEV_BUS, DEV_ADDR, reg, data, len, 0)) {
            DEBUG_DEV("I2C read error starting from register %02x", dev);
            i2c_release(DEV_BUS);
            return -MCP23X17_ERROR_I2C;
        }

        i2c_release(DEV_BUS);
        return MCP23X17_OK;
#endif
#if MODULE_MCP23S17
    }
#endif
    return -MCP23X17_ERROR_I2C;
}

static int _write(mcp23x17_t *dev, uint8_t reg, const uint8_t *data, size_t len)
{
#if MODULE_MCP23S17
    if (DEV_CS != GPIO_UNDEF) {
        if (spi_acquire(DEV_BUS, DEV_CS, SPI_MODE_0, DEV_CLK) != SPI_OK) {
            DEBUG_DEV("SPI acquire error", dev);
            return MCP23X17_ERROR_SPI;
        }
        spi_transfer_byte(DEV_BUS, DEV_CS, true, (DEV_ADDR << 1));
        spi_transfer_byte(DEV_BUS, DEV_CS, true, reg);
        spi_transfer_bytes(DEV_BUS, DEV_CS, false, data, NULL, len);

        spi_release(DEV_BUS);
        return MCP23X17_OK;
    }
    else {
#endif
#ifdef MODULE_MCP23017
        i2c_acquire(DEV_BUS);

        /* read the I/O direction configuration and GPIO pull-up register */
        if (i2c_write_regs(DEV_BUS, DEV_ADDR, reg, data, len, 0)) {
            DEBUG_DEV("I2C write error starting from register %02x", dev);
            i2c_release(DEV_BUS);
            return -MCP23X17_ERROR_I2C;
        }
        i2c_release(DEV_BUS);
        return MCP23X17_OK;
#endif
#if MODULE_MCP23S17
    }
#endif

    return -MCP23X17_ERROR_I2C;
}

static int _update_pin(mcp23x17_t *dev, uint8_t reg, gpio_t pin, int value)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %d, value %d, reg %02x", dev, pin, value, reg);

    uint8_t data;
    uint8_t port = pin >> 3;
    pin -= (port << 3);

    /* read the register */
    int res;
    if ((res = _read(dev, reg + port, &data, 1))) {
        DEBUG_DEV("error reading register %02x", dev, reg);
        return res;
    }

    if (value) {
        data |= (1 << pin);
    }
    else {
        data &= ~(1 << pin);
    }

    /* write back the register */
    if ((res = _write(dev, reg + port, &data, 1))) {
        DEBUG_DEV("error writing register %02x", dev, reg);
        return res;
    }

    i2c_release(DEV_BUS);

    return MCP23X17_OK;
}
