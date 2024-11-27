/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio STDIO abstraction
 * @ingroup     sys
 *
 * @brief       Simple standard input/output (STDIO) abstraction for RIOT
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef STDIO_BASE_H
#define STDIO_BASE_H

#include <unistd.h>

#include "modules.h"
#include "isrpipe.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STDIO_RX_BUFSIZE
/**
 * @brief Buffer size for STDIO
 */
#define STDIO_RX_BUFSIZE    (64)
#endif

enum {
    STDIO_NULL,                 /**< dummy stdio */
    STDIO_UART,                 /**< stdio via UART */
    STDIO_RTT,                  /**< stdio via Segger RTT */
    STDIO_SEMIHOSTING,          /**< stdio via Semihosting */
    STDIO_USBUS_CDC_ACM,        /**< stdio via USB CDC ACM (usbus) */
    STDIO_TINYUSB_CDC_ACM,      /**< tdio via USB CDC ACM (TinyUSB) */
    STDIO_ESP32_SERIAL_JTAG,    /**< stdio via ESP32 debug Serial/JTAG */
    STDIO_NIMBLE,               /**< stdio via BLE (NimBLE) */
    STDIO_UDP,                  /**< stdio via UDP */
    STDIO_TELNET,               /**< stdio via telnet */
    STDIO_ETHOS,                /**< stdio via ethos (mutiplex) */
    STDIO_SLIP,                 /*<< stdio via SLIP (mutiplex) */
};

/**
 * @brief stdio provider struct
 */
typedef struct {
    /**
     * @brief   Initialize and attach the stdio provider
     */
    void (*open)(void);
    /**
     * @brief   Detach the stdio provider
     */
    void (*close)(void);
    /**
     * @brief   Write @p len bytes from @p src into stdout
     *
     * @param[in]   src     buffer to read from
     * @param[in]   len     nr of bytes to write
     *
     * @return nr of bytes written
     * @return <0 on error
     */
    ssize_t (*write)(const void *src, size_t len);
} stdio_provider_t;

/**
 * @brief   isrpipe for writing stdin input to
 */
extern isrpipe_t stdin_isrpipe;

/**
 * @brief initialize the module
 */
void stdio_init(void);

#if IS_USED(MODULE_STDIO_AVAILABLE) || DOXYGEN
/**
 * @brief   Get the number of bytes available for reading from stdio.
 *
 * @warning This function is only available if the implementation supports
 *          it and the @c stdio_available module is enabled.
 *
 * @return  number of available bytes
 */
int stdio_available(void);
#endif

/**
 * @brief   Clear the input buffer
 *
 * @note    Requires 'USEMODULE += stdin'
 *
 * @warning This function does only work if the stdio implementation supports it.
 */
void stdio_clear_stdin(void);

/**
 * @brief read @p len bytes from stdio uart into @p buffer
 *
 * @param[out]  buffer  buffer to read into
 * @param[in]   max_len nr of bytes to read
 *
 * @return nr of bytes read
 * @return <0 on error
 */
ssize_t stdio_read(void* buffer, size_t max_len);

/**
 * @brief write @p len bytes from @p buffer into STDOUT
 *
 * @note Depending on the stdio backend(s) used, not all bytes might
 * be written to stdout and accounted for if multiple backends are
 * active, as not all stdout backends will do a blocking write.
 *
 * @param[in]   buffer  buffer to read from
 * @param[in]   len     nr of bytes to write
 *
 * @return nr of bytes written
 * @return <0 on error
 */
ssize_t stdio_write(const void* buffer, size_t len);

/**
 * @brief Disable stdio and detach stdio providers
 */
void stdio_close(void);

#if defined(MODULE_STDIO_DISPATCH) || DOXYGEN
/**
 * @brief stdio implementation methods
 *
 * @param _type     stdio provider type, for identification
 * @param _open     attach / init function
 * @param _close    close / disable function
 * @param _write    write function
 */
#define STDIO_PROVIDER(_type, _open, _close, _write)        \
    XFA_CONST(stdio_provider_t, stdio_provider_xfa, 0) stdio_ ##_type = { \
        .open = _open,                                      \
        .close = _close,                                    \
        .write = _write,                                    \
    };
#else
#define STDIO_PROVIDER(_type, _open, _close, _write)        \
    void stdio_init(void) {                                 \
        void (*f)(void) = _open;                            \
        if (f != NULL) {                                    \
            f();                                            \
        }                                                   \
    }                                                       \
    void stdio_close(void) {                                \
        void (*f)(void) = _close;                           \
        if (f != NULL) {                                    \
            f();                                            \
        }                                                   \
    }                                                       \
    ssize_t stdio_write(const void* buffer, size_t len) {   \
        return _write(buffer, len);                         \
    }
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_BASE_H */
