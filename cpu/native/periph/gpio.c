/*
 * Copyright (C) 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     native_cpu
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       empty GPIO implementation
 *
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "periph/gpio.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "async_read.h"

#define NATIVE_GPIO_NUMOF   42

char buf[128];


static void _gpio_isr(int fd, void* arg);

typedef struct {
    int active;
    gpio_t pin;
    gpio_cb_t cb;
    void* arg;
} native_gpio_cb_t;

native_gpio_cb_t gpio_list[NATIVE_GPIO_NUMOF];

int gpio_init(gpio_t pin, gpio_mode_t mode) {
    int fd, len;
    fd = open(GPIO_SYSFS_DIR "/export", O_WRONLY);
    if (fd < 0) {
        return fd;
    }
    len = snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, len);
    close(fd);

    len = snprintf(buf, sizeof(buf), GPIO_SYSFS_DIR "/gpio%d/direction", pin);
    fd = open(buf, O_WRONLY);
    if (fd < 0 ) {
        return fd;
    }
    int ret = 0;
    switch(mode) {
        case GPIO_IN:
        case GPIO_IN_PD:
        case GPIO_IN_PU:
            write(fd, "in", 3);
            break;
        case GPIO_OUT:
            write(fd, "out", 4);
            break;
        default:
            ret = -1;
    }
    close(fd);
    return ret;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg){

    int fd;

    native_gpio_cb_t *gpio = &(gpio_list[pin]);
    gpio->active = 1;
    gpio->pin = pin;
    gpio->cb = cb;
    gpio->arg = arg;

    gpio_init(pin, mode);

    snprintf(buf, sizeof(buf), GPIO_SYSFS_DIR "/gpio%d/edge", pin);
    fd = open(buf, O_WRONLY);
    if (fd < 0 ) {
        return fd;
    }
    switch(flank) {
        case GPIO_FALLING:
            write(fd, "falling", 8);
            break;
        case GPIO_RISING:
            write(fd, "rising", 7);
            break;
        case GPIO_BOTH:
            write(fd, "both", 5);
            break;
        default:
            break;
    }
    close(fd);

    snprintf(buf, sizeof(buf), GPIO_SYSFS_DIR "/gpio%d/value", pin);

    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        return fd;
    }

    native_async_read_setup();
    native_async_add_int_handler(fd, gpio, _gpio_isr);
    return -1;
}

void gpio_irq_enable(gpio_t pin) {
  (void) pin;
}

void gpio_irq_disable(gpio_t pin) {
  (void) pin;
}

int gpio_read(gpio_t pin) {
    int fd, val;
    char c;

    snprintf(buf, sizeof(buf), GPIO_SYSFS_DIR "/gpio%d/value", pin);

    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        return fd;
    }

    read(fd, &c, 1);
    if (c != '0') {
        val = 1;
    } else {
        val = 0;
    }
    close(fd);

    return val;
}

void gpio_set(gpio_t pin) {
    gpio_write(pin, 1);
}

void gpio_clear(gpio_t pin) {
    gpio_write(pin, 0);
}

void gpio_toggle(gpio_t pin) {
    if(gpio_read(pin) == 1) {
        gpio_clear(pin);
    } else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value) {
    int fd;

    snprintf(buf, sizeof(buf), GPIO_SYSFS_DIR "/gpio%d/value", pin);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        return;
    }

    if (value) {
        write(fd, "1", 2);
    } else {
        write(fd, "0", 2);
    }
    close(fd);
}

static void _gpio_isr(int fd, void* arg)
{
    (void)arg;
    lseek(fd, 0, SEEK_SET);  // Read from the start of the file
    read(fd, buf, 128);
    printf("\npoll() GPIO interrupt occurred\n");
    native_async_read_continue(fd);
}
/** @} */
