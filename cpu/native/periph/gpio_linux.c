/*
 * Copyright (C) 2019 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       native GPIO implementation
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "native_internal.h"
#include "periph/gpio.h"

typedef struct {
    gpio_cb_t cb;
    void *arg;
    bool enabled;
} native_gpio_cb_t;

typedef struct {
    int fd;
    unsigned num_pins;
    int *pins;
#ifdef MODULE_PERIPH_GPIO_IRQ
    native_gpio_cb_t **cbs;
#endif
} native_port_t;

static native_port_t *ports;
static unsigned port_numof;

#define _port(p)    (p >> GPIO_PORT_SHIFT)
#define _pin(p)     (p & ((1 << GPIO_PORT_SHIFT) - 1))

int gpio_linux_setup(const char* gpiochip)
{
    struct gpiochip_info info;
    int fd = real_open(gpiochip, O_RDWR);

    if (fd < 0) {
        real_printf("GPIO: cannot open %s\n", gpiochip);
        return fd;
    }

    if (real_ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
        real_close(fd);
        real_printf("GPIO: can not query %s\n", gpiochip);
        return -ENODEV;
    }

    printf("GPIO: found %d pins on %s [%s]\n", info.lines, info.name, info.label);

    void *tmp = reallocarray(ports, port_numof + 1, sizeof(*ports));

    if (tmp == NULL) {
        real_close(fd);
        real_printf("GPIO: out of memory\n");
        return -ENOMEM;
    }

    ports = tmp;
    ports[port_numof].fd = fd;
    ports[port_numof].num_pins = info.lines;
    ports[port_numof].pins = calloc(info.lines, sizeof(*ports[0].pins));
#ifdef MODULE_PERIPH_GPIO_IRQ
    ports[port_numof].cbs = calloc(info.lines, sizeof(*ports[0].cbs));
#endif
    ++port_numof;

    return 0;
}

void gpio_linux_teardown(void)
{
    for (unsigned i = 0; i < port_numof; ++i) {
        for (unsigned j = 0; j < ports[i].num_pins; ++j) {

            if (ports[i].pins[j] > 0) {
                real_close(ports[i].pins[j]);
            }
#ifdef MODULE_PERIPH_GPIO_IRQ
            if (ports[i].cbs[j]) {
                real_free(ports[i].cbs[j]);
            }
#endif
        }

        if (ports[i].pins) {
            real_free(ports[i].pins);
        }

        if (ports[i].fd) {
            real_close(ports[i].fd);
        }
#ifdef MODULE_PERIPH_GPIO_IRQ
        if (ports[i].cbs) {
            real_free(ports[i].cbs);
        }
#endif
    }

    if (ports) {
        real_free(ports);
        ports = NULL;
    }

    port_numof = 0;
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    int res;
    const unsigned p = _pin(pin);
    struct gpiohandle_request req = {
        .lineoffsets[0] = p,
        .flags          = mode,
        .lines          = 1
    };
    native_port_t *port;

    if (mode == 0xFF) {
        return -EINVAL;
    }

    if (_port(pin) >= port_numof) {
        return -EINVAL;
    }

    port = &ports[_port(pin)];

    if (p >= port->num_pins) {
        return -EINVAL;
    }

    /* if the pin is already configured, close it first */
    if (port->pins[p] > 0) {
        real_close(port->pins[p]);
        port->pins[p] = 0;
    }

    res = real_ioctl(port->fd, GPIO_GET_LINEHANDLE_IOCTL, &req);

    if (res < 0) {
        return res;
    }

    port->pins[p] = req.fd;
    return 0;
}

bool gpio_read(gpio_t pin)
{
    struct gpiohandle_data data;

    if (_port(pin) >= port_numof) {
        return -EINVAL;
    }

    int fd = ports[_port(pin)].pins[_pin(pin)];

    if (fd <= 0) {
        return -EINVAL;
    }

    real_ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);

    return data.values[0];
}

static void _set(gpio_t pin, uint8_t val)
{
    int fd = ports[_port(pin)].pins[_pin(pin)];

    if (fd > 0) {
        real_ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &val);
    }
}

void gpio_set(gpio_t pin)
{
    _set(pin, 1);
}

void gpio_clear(gpio_t pin)
{
    _set(pin, 0);
}

void gpio_toggle(gpio_t pin)
{
    _set(pin, !gpio_read(pin));
}

void gpio_write(gpio_t pin, bool value)
{
    _set(pin, value);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
#include "async_read.h"

static void _async_read_wrapper(int fd, void *arg) {
    native_gpio_cb_t *cb = arg;

    struct gpioevent_data event;
    real_read(fd, &event, sizeof(event));

    if (cb->cb && cb->enabled) {
        cb->cb(cb->arg);
    }

    native_async_read_continue(fd);
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int res;
    const unsigned p = _pin(pin);
    struct gpioevent_request req = {
        .lineoffset  = p,
        .handleflags = mode,
        .eventflags  = flank
    };
    native_port_t *port;

    if (mode == 0xFF) {
        return -EINVAL;
    }

    if (_port(pin) >= port_numof) {
        return -EINVAL;
    }

    port = &ports[_port(pin)];

    if (p >= port->num_pins) {
        return -EINVAL;
    }

    /* if the pin is already configured, close it first */
    if (port->pins[p] > 0) {
        real_close(port->pins[p]);
        port->pins[p] = 0;
    }

    res = real_ioctl(port->fd, GPIO_GET_LINEEVENT_IOCTL, &req);

    if (res < 0) {
        return res;
    }

    port->pins[p] = req.fd;

    if (port->cbs[p] == NULL) {
        port->cbs[p] = malloc(sizeof(native_gpio_cb_t));
    }

    port->cbs[p]->cb  = cb;
    port->cbs[p]->arg = arg;
    port->cbs[p]->enabled = true;

    native_async_read_setup();
    native_async_read_add_int_handler(req.fd, port->cbs[p], _async_read_wrapper);

    return 0;
}

static void _set_irq_enabled(gpio_t pin, bool enabled)
{
    native_port_t *port;
    const unsigned p = _pin(pin);

    if (_port(pin) >= port_numof) {
        return;
    }

    port = &ports[_port(pin)];

    if (p >= port->num_pins) {
        return;
    }

    port->cbs[p]->enabled = enabled;
}

void gpio_irq_enable(gpio_t pin)
{
    _set_irq_enabled(pin, true);
}

void gpio_irq_disable(gpio_t pin)
{
    _set_irq_enabled(pin, false);
}

#endif /* MODULE_PERIPH_GPIO_IRQ */

/** @} */
