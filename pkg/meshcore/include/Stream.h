/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       Minimal Arduino Stream interface for MeshCore
 *
 * MeshCore reads and writes the identities and prints hexadecimal values with
 * a Stream.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

/**
 * @brief   Byte stream
 */
class Stream {
public:
    virtual ~Stream() = default;

    /**
     * @brief   Writes a byte
     *
     * @param[in] byte  byte to write
     *
     * @return  the number of bytes written
     */
    virtual size_t write(uint8_t byte) = 0;

    /**
     * @brief   Reads a byte
     *
     * @return  the byte read
     * @return  -1 if no byte is available
     */
    virtual int read() = 0;

    /**
     * @brief   Writes bytes
     *
     * @param[in] buf   bytes to write
     * @param[in] len   number of bytes to write
     *
     * @return  the number of bytes written
     */
    virtual size_t write(const uint8_t *buf, size_t len)
    {
        size_t n = 0;
        while ((n < len) && (write(buf[n]) == 1)) {
            n++;
        }
        return n;
    }

    /**
     * @brief   Reads bytes
     *
     * @param[out] buf  buffer for the bytes read
     * @param[in] len   number of bytes to read
     *
     * @return  the number of bytes read
     */
    size_t readBytes(uint8_t *buf, size_t len)
    {
        size_t n = 0;
        while (n < len) {
            int byte = read();
            if (byte < 0) {
                break;
            }
            buf[n++] = (uint8_t)byte;
        }
        return n;
    }

    /**
     * @brief   Prints a character
     *
     * @param[in] c     character to print
     *
     * @return  the number of bytes written
     */
    size_t print(char c)
    {
        return write((uint8_t)c);
    }

    /**
     * @brief   Prints a string
     *
     * @param[in] str   string to print
     *
     * @return  the number of bytes written
     */
    size_t print(const char *str)
    {
        return write((const uint8_t *)str, strlen(str));
    }

    /**
     * @brief   Prints a new line
     *
     * @return  the number of bytes written
     */
    size_t println()
    {
        return print("\r\n");
    }
};

/** @} */
