/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "ztimer.h"

#include "mcp23x17.h"
#include "mcp23x17_regs.h"

#if IS_USED(MODULE_MCP23X17_IRQ)
#include "event/thread.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#if ENABLE_DEBUG

#include <string.h>

#define DEBUG_DEV(m, d, ...) \
        DEBUG("[mcp23x17] %s dev=%" PRIxPTR " addr=%02x: " m "\n", \
              __func__, (unsigned int)d, d->params.addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define _ADDR       (MCP23X17_BASE_ADDR + dev->params.addr)

#if IS_USED(MODULE_MCP23X17_SPI)
#define _SPI_DEV    (dev->params.if_params.spi.dev)
#define _SPI_CS     (dev->params.if_params.spi.cs)
#define _SPI_CLK    (dev->params.if_params.spi.clk)
#endif

#if IS_USED(MODULE_MCP23X17_I2C)
#define _I2C_DEV    (dev->params.if_params.i2c.dev)
#endif

#if IS_USED(MODULE_MCP23X17_IRQ)

#if IS_USED(MODULE_MCP23X17_IRQ_MEDIUM)
#define  MCP23X17_EVENT_PRIO    EVENT_PRIO_MEDIUM
#elif IS_USED(MODULE_MCP23X17_IRQ_HIGHEST)
#define  MCP23X17_EVENT_PRIO    EVENT_PRIO_HIGHEST
#endif

/* interrupt service routine for IRQs */
static void _irq_isr(void *arg);

/* declaration of IRQ handler function */
static void _irq_handler(event_t *event);

#endif /* MODULE_MCP23X17_IRQ */

/* forward declarations of internal functions */
static void _acquire(const mcp23x17_t *dev);
static void _release(const mcp23x17_t *dev);
static int _read(const mcp23x17_t *dev, uint8_t reg, uint8_t *data, size_t len);
static int _write(const mcp23x17_t *dev, uint8_t reg, const uint8_t *data, size_t len);
static int _update_pin(const mcp23x17_t *dev, uint8_t reg, gpio_t pin, int value);

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

    int res = MCP23X17_OK;

#if IS_USED(MODULE_MCP23X17_SPI)
    if (params->if_params.type == MCP23X17_SPI) {
        /* CS pin has to be defined and has to be initialized */
        assert(gpio_is_valid(_SPI_CS));
        if (spi_init_cs(_SPI_DEV, _SPI_CS) != SPI_OK) {
            DEBUG_DEV("CS pin defined but could not be initialized\n", dev);
            return -MCP23X17_ERROR_NO_DEV;
        }
    }
#endif

#if IS_USED(MODULE_MCP23X17_RESET)
    /* GPIO pin for the RESET signal has to be define and initialized */
    assert(gpio_is_valid(params->reset_pin));

    /* initialize the low active RESET pin */
    if (gpio_init(params->reset_pin, GPIO_OUT)) {
        DEBUG_DEV("RESET pin defined but could not be initialized", dev);
        return -MCP23X17_ERROR_RESET_PIN;
    }
    /* hardware reset impuls for at least 10 us is required, we use 1 ms */
    gpio_clear(params->reset_pin);
    ztimer_sleep(ZTIMER_MSEC, 1);
    gpio_set(params->reset_pin);
#endif

#if IS_USED(MODULE_MCP23X17_IRQ)
    /* GPIO pin for combined interrupt signal INTA/INTB has to be defined */
    assert(gpio_is_valid(params->int_pin));

    /* initialize the IRQ event object used for delaying interrupts */
    dev->irq_event.event.handler = _irq_handler;
    dev->irq_event.dev = dev;

    for (unsigned i = 0; i < MCP23X17_GPIO_PIN_NUM; i++) {
        dev->isr[i].cb = NULL;
        dev->isr[i].arg = NULL;
    }

    /* GPIO for interrupt signal has to be initialized */
    if (gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING,
                      _irq_isr, (void*)dev)) {
        DEBUG_DEV("INT pin defined but could not be initialized", dev);
        return -MCP23X17_ERROR_INT_PIN;
    }
#endif /* MODULE_MCP23X17_IRQ */

    _acquire(dev);

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
        _release(dev);
        return -MCP23X17_ERROR_NO_DEV;
    }

    /*
     * After power on reset or hardware reset, the configuration of GPIOs is:
     *
     * - GPIO pins are defined as inputs
     * - GPIO registers reflects the same logic state of the input pin
     * - GPIO pin value is compared against previous pin value for
     *   interrupt-on-change
     * - GPIO input pins are disabled for interrupts on change
     * - GPIO pull-ups are disabled
     * - GPIO default output values are 0
     *
     * If hardware reset is not used, we have to restore this configuration
     * after system reboots
     */
    res |= _write(dev, MCP23X17_REG_IODIR, _reset_conf, ARRAY_SIZE(_reset_conf));

#if IS_USED(MODULE_MCP23X17_IRQ)
    /* INT is configured as push/pull and is active low */
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
     * Disable all interrupts.
     *
     * Since this corresponds to the power on reset configuration written
     * before, we have not execute it here.
     */
#if 0
    uint8_t zero = 0;
    res |= _write(dev, MCP23X17_REG_INTCONA, &zero, 1);
    res |= _write(dev, MCP23X17_REG_INTCONB, &zero, 1);
    res |= _write(dev, MCP23X17_REG_GPINTENA, &zero, 1);
    res |= _write(dev, MCP23X17_REG_GPINTENB, &zero, 1);
#endif

#endif /* MODULE_MCP23X17_IRQ */

    iocon &= ~MCP23X17_IOCON_SEQOP;     /* sequential operation mode enabled */
    iocon &= ~MCP23X17_IOCON_DISSLW;    /* slew rate control enabled */
    iocon |=  MCP23X17_IOCON_HAEN;      /* hardware addressing enabled */

    /* write back configuration registers */
    res |= _write(dev, MCP23X17_REG_IOCONA, &iocon, 1);
    res |= _write(dev, MCP23X17_REG_IOCONB, &iocon, 1);

    _release(dev);

    return res;
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

    _acquire(dev);
    /* read the I/O direction configuration and GPIO pull-up register */
    if ((res = _read(dev, MCP23X17_REG_IODIR + port, &iodir, 1)) ||
        (res = _read(dev, MCP23X17_REG_GPPU + port, &gppu, 1)) ||
        (res = _read(dev, MCP23X17_REG_GPINTEN + port, &gpinten, 1))) {
        DEBUG_DEV("error reading IODIR, GPPU or GPINTEN register", dev);
        _release(dev);
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
                         /* intentionally falls through */
        case GPIO_OD:    dev->od_pins |= (1 << pin); /* set open-drain flag */
                         _update_pin(dev, MCP23X17_REG_GPIO, pin, 0); /* clear pin */
                         break;
        case GPIO_IN_PU: gppu |= pin_bit;            /* enable pull-up */
                         /* intentionally falls through */
        case GPIO_IN:    break;

        default: DEBUG_DEV("invalid pin mode", dev);
                 _release(dev);
                 return -MCP23X17_ERROR_INV_MODE;
    }

    /* write back the I/O direction configuration and GPIO pull-up register */
    if ((res = _write(dev, MCP23X17_REG_GPINTEN + port, &gpinten, 1)) ||
        (res = _write(dev, MCP23X17_REG_IODIR + port, &iodir, 1)) ||
        (res = _write(dev, MCP23X17_REG_GPPU + port, &gppu, 1))) {
        DEBUG_DEV("error writing IODIR, GPPU or GPINTEN register", dev);
        _release(dev);
        return res;
    }

    _release(dev);
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
    _acquire(dev);
    int res = _read(dev, MCP23X17_REG_GPIO + port, &gpio, 1);
    _release(dev);

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

    DEBUG_DEV("pin %u, value %d", dev, pin, value);

    _acquire(dev);

    /* check whether it is an emulated OD pin */
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

    _release(dev);
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

#if IS_USED(MODULE_MCP23X17_IRQ)

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

    _acquire(dev);

    /* delete pending interrupts */
    uint8_t regs[4];
    /* read the GPIO port register */
    if (_read(dev, MCP23X17_REG_INTF, regs, ARRAY_SIZE(regs))) {
        DEBUG_DEV("error reading INTF and INTCAP registers", dev);
        _release(dev);
        return;
    }

    _update_pin(dev, MCP23X17_REG_GPINTEN, pin, 1);
    _release(dev);

}

void mcp23x17_gpio_irq_disable(mcp23x17_t *dev, gpio_t pin)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %u", dev, pin);

    _acquire(dev);
    _update_pin(dev, MCP23X17_REG_GPINTEN, pin, 0);
    _release(dev);
}

/* interrupt service routine for IRQs */
static void _irq_isr(void *arg)
{
    /* some parameter sanity checks */
    assert(arg != NULL);

    /* just indicate that an interrupt occurred and return */
    event_post(MCP23X17_EVENT_PRIO, (event_t*)&((mcp23x17_t*)arg)->irq_event);
}

/* handle one IRQ event of device referenced by the event */
static void _irq_handler(event_t* event)
{
    mcp23x17_irq_event_t* irq_event = (mcp23x17_irq_event_t*)event;

    assert(irq_event != NULL);
    assert(irq_event->dev);

    mcp23x17_t *dev = irq_event->dev;

    DEBUG_DEV("event %p", dev, event);

    uint8_t regs[4];

    /* read the GPIO port register */
    _acquire(dev);
    if (_read(dev, MCP23X17_REG_INTF, regs, ARRAY_SIZE(regs))) {
        DEBUG_DEV("error reading INTF and INTCAP registers", dev);
        _release(dev);
        return;
    }
    _release(dev);

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
}

#endif /* MODULE_MCP23X17_IRQ */

/* internal functions */

static void _acquire(const mcp23x17_t *dev)
{
#if IS_USED(MODULE_MCP23X17_SPI)
    if (dev->params.if_params.type == MCP23X17_SPI) {
        spi_acquire(_SPI_DEV, _SPI_CS, SPI_MODE_0, _SPI_CLK);
    }
#endif
#if IS_USED(MODULE_MCP23X17_I2C)
    if (dev->params.if_params.type == MCP23X17_I2C) {
        i2c_acquire(_I2C_DEV);
    }
#endif
}

static void _release(const mcp23x17_t *dev)
{
#if IS_USED(MODULE_MCP23X17_SPI)
    if (dev->params.if_params.type == MCP23X17_SPI) {
        spi_release(_SPI_DEV);
    }
#endif
#if IS_USED(MODULE_MCP23X17_I2C)
    if (dev->params.if_params.type == MCP23X17_I2C) {
        i2c_release(_I2C_DEV);
    }
#endif
}

static int _read(const mcp23x17_t *dev, uint8_t reg, uint8_t *data, size_t len)
{
    DEBUG_DEV("reg=%02x data=%p len=%d", dev, reg, data, len);

    int res = MCP23X17_OK;

#if IS_USED(MODULE_MCP23X17_SPI)
    if (dev->params.if_params.type == MCP23X17_SPI) {
        spi_transfer_byte(_SPI_DEV, _SPI_CS, true, (_ADDR << 1) | 1);
        spi_transfer_byte(_SPI_DEV, _SPI_CS, true, reg);
        spi_transfer_bytes(_SPI_DEV, _SPI_CS, false, NULL, data, len);
    }
#endif

#if IS_USED(MODULE_MCP23X17_I2C)
    if (dev->params.if_params.type == MCP23X17_I2C) {
        int res = i2c_read_regs(_I2C_DEV, _ADDR, reg, data, len, 0);
        if (res) {
            DEBUG_DEV("I2C read error: %d (%s)", dev, res, strerror(res * -1));
            return -MCP23X17_ERROR_I2C;
        }
        else {
            return MCP23X17_OK;
        }
    }
#endif

    if (ENABLE_DEBUG) {
        printf("[mcp23x17] %s dev=%" PRIxPTR " addr=%02x reg=%02x read: ",
               __func__, (uintptr_t)dev, dev->params.addr, reg);
        for (uint8_t i = 1; i < len + 1; i++) {
             printf("%02x ", data[i]);
        }
        printf("\n");
    }

    return res;
}

static int _write(const mcp23x17_t *dev, uint8_t reg,
                                         const uint8_t *data, size_t len)
{
    DEBUG_DEV("reg=%02x data=%p len=%d", dev, reg, data, len);

    if (ENABLE_DEBUG) {
        printf("[mcp23x17] %s dev=%" PRIxPTR " addr=%02x reg=%02x write: ",
               __func__, (uintptr_t)dev, dev->params.addr, reg);
        for (uint8_t i = 1; i < len + 1; i++) {
             printf("%02x ", data[i]);
        }
        printf("\n");
    }

#if IS_USED(MODULE_MCP23X17_SPI)
    if (dev->params.if_params.type == MCP23X17_SPI) {
        spi_transfer_byte(_SPI_DEV, _SPI_CS, true, (_ADDR << 1));
        spi_transfer_byte(_SPI_DEV, _SPI_CS, true, reg);
        spi_transfer_bytes(_SPI_DEV, _SPI_CS, false, data, NULL, len);
        return MCP23X17_OK;
    }
#endif

#if IS_USED(MODULE_MCP23X17_I2C)
    if (dev->params.if_params.type == MCP23X17_I2C) {
        int res = i2c_write_regs(_I2C_DEV, _ADDR, reg, data, len, 0);
        if (res) {
            DEBUG_DEV("I2C write error: %d (%s)", dev, res, strerror(res * -1));
            return -MCP23X17_ERROR_I2C;
        }
        else {
            return MCP23X17_OK;
        }
    }
#endif

    return -MCP23X17_ERROR_NO_DEV;
}

static int _update_pin(const mcp23x17_t *dev, uint8_t reg, gpio_t pin, int value)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < MCP23X17_GPIO_PIN_NUM);

    DEBUG_DEV("pin %d, value %d, reg %02x", dev, pin, value, reg);

    uint8_t data;
    uint8_t port = pin >> 3;
    pin -= (port << 3);

    /* read the register */
    int res = _read(dev, reg + port, &data, 1);

    if (value) {
        data |= (1 << pin);
    }
    else {
        data &= ~(1 << pin);
    }

    /* write back the register */
    res |= _write(dev, reg + port, &data, 1);

    return res;
}
