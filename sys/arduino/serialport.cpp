/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Implementation of the Arduino 'Serial' interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
extern "C" {
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "irq.h"
}

#include "serialport.hpp"

void rx_cb(void *arg, uint8_t c)
{
    ringbuffer_t *buf = (ringbuffer_t *)arg;
    ringbuffer_add_one(buf, (char)c);
}

SerialPort::SerialPort(uart_t dev)
{
    this->dev = dev;
}

int SerialPort::available(void)
{
    return (int)rx_buf.avail;
}

void SerialPort::begin(long baudrate)
{
    /* this clears the contents of the ringbuffer... */
    ringbuffer_init(&rx_buf, rx_mem, SERIAL_RX_BUFSIZE);
    uart_init(dev, (uint32_t)baudrate, rx_cb, (void *)&rx_buf);
}

void SerialPort::end(void)
{
    uart_poweroff(dev);
}

size_t SerialPort::print(int val)
{
    return print(val, DEC);
}

size_t SerialPort::print(int val, SerialFormat format)
{
    char buf[64];
    size_t len;
    switch (format) {
        case BIN:
            /* TODO */
            return 0;
        case OCT:
            len = sprintf(buf, "%o", (unsigned)val);
            break;
        case DEC:
            len = sprintf(buf, "%i", val);
            break;
        case HEX:
            len = sprintf(buf, "%x", (unsigned)val);
            break;
        default:
            return 0;
    }
    write(buf, len);
    return len;
}

size_t SerialPort::print(float val)
{
    return print(val, 2);
}

size_t SerialPort::print(float val, int format)
{
    char buf[64];
    size_t len = sprintf(buf, "%.*f", format, (double)val);
    write(buf, len);
    return len;
}

size_t SerialPort::print(char val)
{
    return (size_t)write((int)val);
}

size_t SerialPort::print(const char *val)
{
    return (size_t)write(val);
}

size_t SerialPort::println(int val)
{
    size_t res = print(val);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(int val, SerialFormat format)
{
    size_t res = print(val, format);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(float val)
{
    size_t res = print(val);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(float val, int format)
{
    size_t res = print(val, format);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(char val)
{
    size_t res = print(val);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(const char *val)
{
    size_t res = print(val);
    write("\r\n");
    return (res + 2);
}

int SerialPort::read(void)
{
    int res = -1;

    irq_disable();
    if (rx_buf.avail > 0) {
        res = ringbuffer_get_one(&rx_buf);
    }
    irq_enable();

    return res;
}

int SerialPort::write(int val)
{
    uart_write(dev, (uint8_t *)&val, 1);
    return 1;
}

int SerialPort::write(const char *str)
{
    uart_write(dev, (uint8_t *)str, strlen(str));
    return strlen(str);
}

int SerialPort::write(char *buf, int len)
{
    uart_write(dev, (uint8_t *)buf, len);
    return len;
}
