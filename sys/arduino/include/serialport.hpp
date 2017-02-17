/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @file
 * @brief       Definition of the Arduino 'Serial' interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

extern "C" {
#include "ringbuffer.h"
#include "periph/uart.h"
}

/**
 * @brief   Default RX buffer size - same as the original Arduino...
 */
#define SERIAL_RX_BUFSIZE       (64)

/**
 * @brief   Formatting options for Serial.print(int, format)
 */
enum SerialFormat {
    BIN,            /**< format to binary representation */
    OCT,            /**< format to octal representation */
    DEC,            /**< format to decimal representation */
    HEX             /**< format to hex representation */
};

/**
 * @brief   Arduino Serial Interface
 */
class SerialPort {

private:
    uart_t dev;
    char rx_mem[SERIAL_RX_BUFSIZE];
    ringbuffer_t rx_buf;

public:
    /**
     * @brief   Constructor maps the serial port to a RIOT UART device
     *
     * @param[in] dev       RIOT UART device
     */
    explicit SerialPort(uart_t dev);

    /**
     * @brief   Get the number of bytes (characters) available for reading from
     *          the serial port
     *
     * This is data that's already arrived and stored in the serial receive
     * buffer (which holds 64 bytes). available() inherits from the Stream
     * utility class.
     *
     * Copied from https://www.arduino.cc/en/Serial/Available
     *
     * @return  The number of bytes available to read
     */
    int available(void);

    /**
     * @brief   Sets the data rate in bits per second (baud) for serial data
     *          transmission
     *
     * For communicating with the computer, use one of these rates: 300, 600,
     * 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200. You
     * can, however, specify other rates - for example, to communicate over
     * pins 0 and 1 with a component that requires a particular baud rate.
     *
     * Copied from https://www.arduino.cc/en/Serial/Begin
     *
     * @param[in] speed     speed in bits per second (baud)
     */
    void begin(long speed);

    /**
     * @brief   Disables serial communication, allowing the RX and TX pins to be
     *          used for general input and output
     *
     * To re-enable serial communication, call @ref begin()
     *
     * Copied from https://www.arduino.cc/en/Serial/End
     */
    void end(void);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *
     * Prints data to the serial port as human-readable ASCII text. This command
     * can take many forms. Numbers are printed using an ASCII character for
     * each digit. Floats are similarly printed as ASCII digits, defaulting to
     * two decimal places. Bytes are sent as a single character. Characters and
     * strings are sent as is.
     *
     * Copied from https://www.arduino.cc/en/Serial/Print
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t print(int val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *
     * @see print()
     *
     * @param[in] val       the value to print
     * @param[in] format    specifies the number base
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t print(int val, SerialFormat format);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t print(float val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *
     * @see print()
     *
     * @param[in] val       the value to print
     * @param[in] format    number of decimal places
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t print(float val, int format);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *
     * @see print()
     *
     * @param[in] val       the value to print
     *
    * @return  the number of bytes written, reading that number is optional
     */
    size_t print(char val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *
     * @see print()
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t print(const char *val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *          followed by a carriage return character (ASCII 13, or "\r") and
     *          a newline character (ASCII 10, or "\n")
     *
     * This command takes the same forms as @ref print().
     *
     * Copied from https://www.arduino.cc/en/Serial/Println
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t println(int val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *          followed by a carriage return character (ASCII 13, or "\r") and
     *          a newline character (ASCII 10, or "\n")
     *
     * @see println()
     *
     * @param[in] val       the value to print
     * @param[in] format    specifies the number base
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t println(int val, SerialFormat format);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *          followed by a carriage return character (ASCII 13, or "\r") and
     *          a newline character (ASCII 10, or "\n")
     *
     * @see println()
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t println(float val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *          followed by a carriage return character (ASCII 13, or "\r") and
     *          a newline character (ASCII 10, or "\n")
     *
     * @see println()
     *
     * @param[in] val       the value to print
     * @param[in] format    number of decimal places
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t println(float val, int format);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *          followed by a carriage return character (ASCII 13, or "\r") and
     *          a newline character (ASCII 10, or "\n")
     *
     * @see println()
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t println(char val);

    /**
     * @brief   Prints data to the serial port as human-readable ASCII text
     *          followed by a carriage return character (ASCII 13, or "\r") and
     *          a newline character (ASCII 10, or "\n")
     *
     * @see println()
     *
     * @param[in] val       the value to print
     *
     * @return  the number of bytes written, reading that number is optional
     */
    size_t println(const char *val);

    /**
     * @brief   Reads incoming serial data
     *
     * Copied from https://www.arduino.cc/en/Serial/Read
     *
     * @return  the first byte of incoming serial data available
     * @return  -1 if no data is available
     */
    int read(void);

    /**
     * @brief   Writes binary data to the serial port
     *
     * This data is sent as a byte or series of bytes; to send the characters
     * representing the digits of a number use the @ref print() function instead.
     *
     * Copied from https://www.arduino.cc/en/Serial/Write
     *
     * @param[in] val       a value to send as a single byte
     *
     * @return  the number of bytes written, reading that number is optional
     */
    int write(int val);

    /**
     * @brief   Writes binary data to the serial port
     *
     * @see write()
     *
     * @param[in] str       a string to send as a series of bytes
     *
     * @return  the number of bytes written, reading that number is optional
     */
    int write(const char *str);

    /**
     * @brief   Writes binary data to the serial port
     * @details [long description]
     *
     * @param[in] buf       an array to send as a series of bytes
     * @param[in] len       the length of the buffer
     *
     * @return  the number of bytes written, reading that number is optional
     */
    int write(char *buf, int len);
};

#endif /* ARDUINO_SERIAL_H */
/** @} */
