/**
 * Native CPU Linux sysfs GPIO interface driver
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "native_internal.h"

#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/********************************************************************************
 * internal API declaration
 *******************************************************************************/

#define GPIO_SYSFS_PATH "/sys/class/gpio"

typedef enum {
    _NATIVE_GPIO_CONF_NONE,
    _NATIVE_GPIO_CONF_IN,
    _NATIVE_GPIO_CONF_OUT,
    _NATIVE_GPIO_CONF_BOTH,
    _NATIVE_GPIO_CONF_INT,
    _NATIVE_GPIO_CONF_INT_OUT,
} _native_gpio_conf_t;

typedef struct {
    _native_gpio_conf_t conf;
    int pin_state;
    int int_enabled;
    gpio_pp_t pp;
    gpio_flank_t flank;
    gpio_cb_t cb;
    void *cb_arg;
} _native_gpio_state_t;

/*
 * all return >= 0 on success, -1 otherwise
 */
static int sysfs_gpio_conf(gpio_t dev, _native_gpio_conf_t conf);
static int sysfs_gpio_read(gpio_t dev);

_native_gpio_state_t gpio_device_state[GPIO_NUMOF];

/********************************************************************************
 * public API implementation
 *******************************************************************************/

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    int ret = sysfs_gpio_conf(dev, _NATIVE_GPIO_CONF_OUT);
    if (ret != 0) {
        warnx("gpio_init_out(%i)", dev);
        return -1;
    }

    gpio_device_state[dev].pp = pullup;
    switch (gpio_device_state[dev].conf) {
        case _NATIVE_GPIO_CONF_IN:
            gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_BOTH;
            break;
        default:
            gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_OUT;
            break;
    }
    return ret;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    int ret = sysfs_gpio_conf(dev, _NATIVE_GPIO_CONF_IN);
    if (ret != 0) {
        warnx("gpio_init_in(%i)", dev);
        return -1;
    }

    gpio_device_state[dev].pp = pullup;
    switch (gpio_device_state[dev].conf) {
        case _NATIVE_GPIO_CONF_OUT:
            gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_BOTH;
            break;
        default:
            gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_IN;
            break;
    }
    return ret;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int ret = sysfs_gpio_conf(dev, _NATIVE_GPIO_CONF_IN);
    if (ret != 0) {
        warnx("gpio_init_int(%i)", dev);
        return -1;
    }

    /* TODO: configure interrupt + pollfds */

    gpio_device_state[dev].pp = pullup;
    gpio_device_state[dev].flank = flank;
    gpio_device_state[dev].cb = cb;
    gpio_device_state[dev].cb_arg = arg;
    switch (gpio_device_state[dev].conf) {
        case _NATIVE_GPIO_CONF_OUT:
            gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_INT_OUT;
            break;
        default:
            gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_INT;
            break;
    };
    gpio_irq_enable(dev);
    return ret;
}

void gpio_irq_enable(gpio_t dev)
{
    gpio_device_state[dev].int_enabled = 1;
}

void gpio_irq_disable(gpio_t dev)
{
    gpio_device_state[dev].int_enabled = 0;
}

int gpio_read(gpio_t dev)
{
    int ret = sysfs_gpio_read(dev);
    if (ret == -1) {
        warnx("gpio_read(%i): error reading", dev);
    }
    gpio_device_state[dev].pin_state = ret;
    return gpio_device_state[dev].pin_state;
}

void gpio_set(gpio_t dev)
{
    printf("Native GPIO device %i is now HIGH\n", dev);
    gpio_device_state[dev].pin_state = 1;
}

void gpio_clear(gpio_t dev)
{
    printf("Native GPIO device %i is now LOW\n", dev);
    gpio_device_state[dev].pin_state = 0;
}

void gpio_toggle(gpio_t dev)
{
    gpio_device_state[dev].pin_state = !gpio_device_state[dev].pin_state;
    printf("Native GPIO device %i is now %s\n", dev,
            (gpio_device_state[dev].pin_state == 0) ? "LOW" : "HIGH");
}

void gpio_write(gpio_t dev, int value)
{
    gpio_device_state[dev].pin_state = (value == 0) ? 0 : 1;
    printf("Native GPIO device %i is now %s\n", dev,
            (gpio_device_state[dev].pin_state == 0) ? "LOW" : "HIGH");
}


/********************************************************************************
 * internal API implementation
 *******************************************************************************/

static int sysfs_gpio_conf(gpio_t dev, _native_gpio_conf_t conf)
{
    char gpio_path_name[255];

    int len = snprintf(gpio_path_name, sizeof(gpio_path_name), GPIO_SYSFS_PATH  "/gpio%hi/direction", dev);
    if (len < 0) {
        warnx("sysfs_gpio_conf(%i): snprintf: fail", dev);
        return -1;
    }
 
    _native_syscall_enter();
    int fd = real_open(gpio_path_name, O_WRONLY);
    if (fd < 0) {
        warn("sysfs_gpio_conf(%i)", dev);
        _native_syscall_leave();
        return -1;
    }
 
    int ret = 0;
    switch (conf) {
        case (_NATIVE_GPIO_CONF_IN):
            ret = real_write(fd, "in", 3);
            if (ret == -1) {
                warn("sysfs_gpio_conf(%i, _NATIVE_GPIO_CONF_IN)", dev);
            }
            break;
        case (_NATIVE_GPIO_CONF_OUT):
            ret = real_write(fd, "out", 4);
            if (ret == -1) {
                warn("sysfs_gpio_conf(%i, _NATIVE_GPIO_CONF_OUT)", dev);
            }
            break;
        default:
            warnx("sysfs_gpio_conf(%i, %i): configuration mode not implemented", dev, conf);
            ret = -1;
            break;
    }
    real_close(fd);
    _native_syscall_leave();
    return ret;
}

static int sysfs_gpio_read(gpio_t dev)
{
    char gpio_path_name[255];

    int len = snprintf(gpio_path_name, sizeof(gpio_path_name), GPIO_SYSFS_PATH  "/gpio%hi/value", dev);
    if (len < 0) {
        warnx("sysfs_gpio_read(%i): snprintf: fail", dev);
        return -1;
    }
 
    _native_syscall_enter();
    int fd = real_open(gpio_path_name, O_WRONLY);
    if (fd < 0) {
        warn("sysfs_gpio_read(%i)", dev);
        _native_syscall_leave();
        return -1;
    }
 
    char val;
    int ret = _native_read(fd, &val, 1);
    if (ret == -1) {
        warn("sysfs_gpio_read(%i)", dev);
    }
    else {
        switch (val) {
            case ('0'):
                ret = 0;
                break;
            case ('1'):
                ret = 1;
                break;
            default:
                warnx("sysfs_gpio_read(%i): internal error", dev);
                ret = -1;
        }
    }
 
    if (real_close(fd) == -1) {
        warn("sysfs_gpio_read");
    }
    _native_syscall_leave();
    return ret;
}
