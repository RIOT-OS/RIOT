/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 OTA keys S.A.
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_gsm     GSM
 * @ingroup     drivers
 * @brief       A generic implementation of the GSM API
 *
 * @{
 *
 * @file
 * @brief   GSM-independent driver
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Vincent Dupont <vincent@otakeys.com>
 * @author  Max van Kessel <maxvankessel@betronic.nl>
 */
#ifndef GSM_H
#define GSM_H

#include <stdint.h>

#include "rmutex.h"
#include "thread.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "at.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   gsm thread stack size
 */
#ifndef GSM_THREAD_STACKSIZE
#define GSM_THREAD_STACKSIZE    (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   gsm thread priority
 */
#ifndef GSM_THREAD_PRIO
#define GSM_THREAD_PRIO         (THREAD_PRIORITY_MAIN + 1)
#endif

/**
 * @brief   at device buffer size
 */
#ifndef GSM_UART_BUFSIZE
#define GSM_UART_BUFSIZE        (1024U)
#endif

/**
 * @brief   at device default timeout in micro seconds
 */
#ifndef GSM_SERIAL_TIMEOUT_US
#define GSM_SERIAL_TIMEOUT_US   (1000000U)
#endif

/**
 * @brief   small line buffer size
 */
#ifndef GSM_AT_LINEBUFFER_SIZE_SMALL
#define GSM_AT_LINEBUFFER_SIZE_SMALL    (32)
#endif

/**
 * @brief   normal linebuffer size
 */
#ifndef GSM_AT_LINEBUFFER_SIZE
#define GSM_AT_LINEBUFFER_SIZE          (64)
#endif

/**
 * @brief States of gsm modem
 */
enum {
    GSM_OFF,    /**< GSM_OFF */
    GSM_BOOT,   /**< GSM_BOOT */
    GSM_ON,     /**< GSM_ON */
};

/**
 * @brief   Default gsm_driver_t type definition
 */
typedef struct gsm_driver gsm_driver_t;

/**
 * @brief   gsm device parameters
 *
 * Common GSM parameters.
 */
typedef struct gsm_params {
    uart_t      uart;       /**< communication interface of modem */
    uint32_t    baudrate;   /**< initial baudrate */
    gpio_t      ri_pin;     /**< ring indicator */
} gsm_params_t;

typedef struct gsm {
    const gsm_driver_t  *driver;                    /**< gsm driver */
    const gsm_params_t  *params;                    /**< gsm parameters */

    rmutex_t        mutex;                          /**< lock */
    kernel_pid_t    pid;                            /**< thread pid */
    char            stack[GSM_THREAD_STACKSIZE];    /**< stack */
    int             state;                          /**< gsm state */

    at_dev_t        at_dev;                         /**< at parser */
    char            buffer[GSM_UART_BUFSIZE];       /**< at buffer */
} gsm_t;

struct gsm_driver {

    /**
     * @brief   Initialize the device driver
     *
     * @param[in]   dev     device driver to initialize
     *
     * @return 0 on success
     */
    int (*init_base)(gsm_t *dev);

    /**
     * @brief   Power on the module
     *
     * @param[in]   dev     device to act on
     *
     * @return 0 on success
     */
    int (*power_on)(gsm_t *dev);

    /**
     * @brief   Power off the module
     *
     * @param[in]   dev     device to act on
     *
     * @return 0 on success
     */
    int (*power_off)(gsm_t *dev);

    /**
     * @brief   Set module to sleep
     *
     * @param[in]   dev     device to act on
     *
     */
    void (*sleep)(gsm_t *dev);

    /**
     * @brief   Reset module
     *
     * @param[in]   dev     device to act on
     *
     */
    void (*reset)(gsm_t *dev);
};

/**
 * @brief   Initialize gsm module and base module.
 *
 * @param[in] dev       Device to initialize
 * @param[in] params
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_init(gsm_t *dev, gsm_params_t *params);

/**
 * @brief   Powers the gsm module.
 *
 * @param[in] dev   Device to operate on
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_power_on(gsm_t *dev);

/**
 * @brief   Turns the gsm module off.
 *
 * @param[in] dev  Device to operate on
 *
 * @return  none
 */
void gsm_power_off(gsm_t *dev);

/**
 * @brief   Enables the gsm radio.
 *
 * @param[in] dev   Device to enable
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_enable_radio(gsm_t *dev);

/**
 * @brief   Disable the gsm radio.
 *
 * @param[in] dev   Device to disable
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_disable_radio(gsm_t *dev);

/**
 * @brief   Checks if gsm is alive
 * @note    Time between retries is GSM_SERIAL_TIMEOUT_US
 *
 * @param[in] dev       Device to operate on
 * @param[in] retries   Number of retries
 *
 * @return    true for success, otherwise false
 */
bool gsm_is_alive(gsm_t *dev, uint8_t retries);

/**
 * @brief   Set sim puk into modem and pin
 *
 * @param[in] dev   Device to write to
 * @param[in] puk   Puk to set
 * @param[in] pin   Set new pin
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_set_puk(gsm_t *dev, const char *puk, const char *pin);

/**
 * @brief   Set sim pin into modem
 *
 * @param[in] dev   Device to write to
 * @param[in] pin   Pin to set
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_set_pin(gsm_t *dev, const char *pin);

/**
 * @brief   Checks if modem is already unlocked or needs unlocking
 *
 * @param[in] dev   Device to write to
 *
 * @return  > 0 for pin required
 * @return    0 for unlocked
 * @return  < 0 for failure
 */
int gsm_check_pin(gsm_t *dev);

/**
 * @brief   Checks if an operator is available
 *
 * @param[in] dev   Device to operate on
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_check_operator(gsm_t *dev);

/**
 * @brief   Gets a list of operators
 *
 * @param[in]   dev     Device to operate on
 * @param[out]  outbuf  Buffer to store operator data in
 * @param[in]   len     Length of buffer
 *
 * @return  Length of data written into @p outbuf.
 * @return  < 0 for failure
 */
size_t gsm_get_operator(gsm_t *dev, char *outbuf, size_t len);

/**
 * @brief   gets International Mobile Equipment Identity (IMEI)
 *
 * @param[in]   dev    Device to write to
 * @param[out]  buf    Buffer to store imei data in
 * @param[in]   len    Length of buffer
 *
 * @return  Length of data written into @p buf.
 * @return  < 0 for failure
 */
ssize_t gsm_get_imei(gsm_t *dev, char *buf, size_t len);

/**
 * @brief   Gets International Mobile Subscriber Identity (IMSI)
 *
 * @param[in]   dev    Device to write to
 * @param[out]  buf    Buffer to store imsi data in
 * @param[in]   len    Length of buffer
 *
 * @return  Length of data written into @p buf.
 * @return  < 0 for failure
 */
ssize_t gsm_get_imsi(gsm_t *dev, char *buf, size_t len);

/**
 * @brief   Gets simcard identification
 *
 * @param[in]   dev    Device to write to
 * @param[out]  outbuf Buffer to store ccid data in
 * @param[in]   len    Length of buffer
 *
 * @return  Length of data written into @p outbuf.
 * @return  < 0 for failure
 */
ssize_t gsm_get_simcard_identification(gsm_t *dev, char *outbuf, size_t len);

/**
 * @brief   Gets modem identification
 *
 * @param[in] dev   Device to write to
 *
 * @return  Length of data written into @p buf.
 * @return  < 0 for failure
 */
ssize_t gsm_get_identification(gsm_t *dev, char *buf, size_t len);

/**
 * @brief   Gets modem signal quality
 *
 * @param[in]   dev     Device to write to
 * @param[out]  rssi    RSSI value
 * @param[out]  ber     Bearer value
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_get_signal(gsm_t *dev, unsigned *rssi, unsigned *ber);

/**
 * @brief   Get registration value
 *
 * @param[in]   dev  device to operate on
 *
 * @return  positive CREG value on success
 * @return  < 0 on error
 */
int gsm_get_registration(gsm_t *dev);

/**
 * @brief   Gets modem local time
 *
 * @param[in]   dev    Device to write to
 * @param[out]  outbuf Buffer to store time in
 * @param[in]   len    Length of buffer
 *
 * @return  Length of data written into @p outbuf.
 * @return  < 0 for failure
 */
ssize_t gsm_get_local_time(gsm_t *dev, char * outbuf, size_t len);


/**
 * @brief   Send a 'raw' command to the modem and get the result
 *
 * @param[in]   dev         device to operate on
 * @param[in]   cmd         command to send
 * @param[out]  buf         response buffer
 * @param[in]   len         length of @p buf
 * @param[in]   timeout_sec command timeout in seconds
 *
 * @return  the length of the response on success
 * @return  < 0 on error
 */
ssize_t gsm_cmd(gsm_t *dev, const char *cmd, uint8_t *buf, size_t len,
        unsigned timeout_sec);

/**
 * @brief   Print modem status on stdout
 *
 * @param[in]   dev  device to operate on
 */
void gsm_print_status(gsm_t *dev);

/**
 * @brief Register a unsolicited result code
 *
 * @param[in] dev   device to operate on
 * @param[in] urc   unsolicited result code to act on
 * @param[in] cb    callback function
 * @param[in] args  arguments for the callback
 */
void gsm_register_urc_callback(gsm_t *dev, const char * urc,
                                    at_oob_cb_t cb, void * args);

#ifdef __cplusplus
}
#endif

#endif /* GSM_H */
