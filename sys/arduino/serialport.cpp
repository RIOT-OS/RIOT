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

#include "assert.h"
#include "fmt.h"
#include "irq.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
#include "stdio_base.h"
#endif
}

#include "serialport.hpp"

void rx_cb(void *arg, uint8_t c)
{
    ringbuffer_t *buf = (ringbuffer_t *)arg;
    ringbuffer_add_one(buf, (char)c);
}

SerialPort::SerialPort(uart_t dev)
{
#if !IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    assert(dev != UART_UNDEF);
#endif
    this->dev = dev;
}

int SerialPort::available(void)
{
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
#if IS_USED(MODULE_STDIO_AVAILABLE)
        return stdio_available();
#else /* IS_USED(MODULE_STDIO_AVAILABLE) */
        return 0;
#endif /* IS_USED(MODULE_STDIO_AVAILABLE) */
    }
#endif /* IS_USED(MODULE_ARDUINO_SERIAL_STDIO) */
    return (int)rx_buf.avail;
}

void SerialPort::begin(long baudrate)
{
    /* this clears the contents of the ringbuffer... */
    ringbuffer_init(&rx_buf, rx_mem, SERIAL_RX_BUFSIZE);
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
        return;
    }
#endif
    uart_init(dev, (uint32_t)baudrate, rx_cb, (void *)&rx_buf);
}

void SerialPort::end(void)
{
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
        return;
    }
#endif
    uart_poweroff(dev);
}

size_t SerialPort::print(int val)
{
    return print((long)val, DEC);
}

size_t SerialPort::print(int val, SerialFormat format)
{
    if (format == DEC) {
        return print((long)val, format);
    }

    /* Propagation to long of negative number changes binary representation.
     * This cast should add leading zeros to the binary representation, that
     * will be skipped by print(unsigned long, SerialFormat)
     */
    return print((unsigned long)((unsigned)val), format);
}

size_t SerialPort::print(unsigned int val)
{
    return print((unsigned long)val, DEC);
}

size_t SerialPort::print(unsigned int val, SerialFormat format)
{
    return print((unsigned long)val, format);
}

size_t SerialPort::print(long val)
{
    return print(val, DEC);
}

size_t SerialPort::print(long val, SerialFormat format)
{
    if (format == DEC) {
        char buf[64];
        size_t len;
        len = sprintf(buf, "%li", val);
        write(buf, len);
        return len;
    }

    return print((unsigned long)val, format);
}

size_t SerialPort::print(unsigned long val)
{
    return print(val, DEC);
}

size_t SerialPort::print(unsigned long val, SerialFormat format)
{
    char buf[64];
    size_t len;
    switch (format) {
        case BIN:
            len = 0;
            {
                static const unsigned long mask = 1UL << (8 * sizeof(unsigned long) - 1);
                unsigned pos = 0;
                while (!(val & (mask >> pos))) {
                    pos ++;
                }
                for (; pos < 8 * sizeof(unsigned long); pos++) {
                    buf[len++] = (val & (mask >> pos)) ? '1' : '0';
                }
            }
            break;
        case OCT:
            len = sprintf(buf, "%lo", val);
            break;
        case DEC:
            len = sprintf(buf, "%lu", val);
            break;
        case HEX:
            len = sprintf(buf, "%lx", val);
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
    size_t len = fmt_float(buf, val, format);
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

template<typename T> size_t SerialPort::_println(T val)
{
    size_t res = print(val);
    write("\r\n");
    return res + 2;
}

template<typename T> size_t SerialPort::_println(T val, SerialFormat format)
{
    size_t res = print(val, format);
    write("\r\n");
    return res + 2;
}

size_t SerialPort::println(int val)
{
    return _println<int>(val);
}

size_t SerialPort::println(int val, SerialFormat format)
{
    return _println<int>(val, format);
}

size_t SerialPort::println(unsigned int val)
{
    return _println<unsigned int>(val);
}

size_t SerialPort::println(unsigned int val, SerialFormat format)
{
    return _println<unsigned int>(val, format);
}

size_t SerialPort::println(long val)
{
    return _println<long>(val);
}

size_t SerialPort::println(long val, SerialFormat format)
{
    return _println<long>(val, format);
}

size_t SerialPort::println(unsigned long val)
{
    return _println<unsigned long>(val);
}

size_t SerialPort::println(unsigned long val, SerialFormat format)
{
    return _println<unsigned long>(val, format);
}

size_t SerialPort::println(float val)
{
    return _println<float>(val);
}

size_t SerialPort::println(float val, int format)
{
    /* cannot use template here, second parameter differs in type */
    size_t res = print(val, format);
    write("\r\n");
    return res + 2;
}

size_t SerialPort::println(char val)
{
    return _println<char>(val);
}

size_t SerialPort::println(const char *val)
{
    return _println<const char *>(val);
}

size_t SerialPort::println(void)
{
    write("\r\n");
    return 2;
}

int SerialPort::read(void)
{
    int res = -1;

    irq_disable();
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
        char chr;
        if (this->available()) {
            res = (stdio_read((void *)&chr, 1) == 1) ? chr : -1;
        }
        irq_enable();
        return res;
    }
#endif
    if (rx_buf.avail > 0) {
        res = ringbuffer_get_one(&rx_buf);
    }
    irq_enable();

    return res;
}

int SerialPort::write(int val)
{
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
        stdio_write((const void *)&val, 1);
        return 1;
    }
#endif
    uart_write(dev, (uint8_t *)&val, 1);
    return 1;
}

int SerialPort::write(const char *str)
{
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
        stdio_write((const void *)str, strlen(str));
        return strlen(str);
    }
#endif
    uart_write(dev, (uint8_t *)str, strlen(str));
    return strlen(str);
}

int SerialPort::write(char *buf, int len)
{
#if IS_USED(MODULE_ARDUINO_SERIAL_STDIO)
    if (this->dev == UART_UNDEF) {
        stdio_write((const void *)buf, len);
        return len;
    }
#endif
    uart_write(dev, (uint8_t *)buf, len);
    return len;
}
