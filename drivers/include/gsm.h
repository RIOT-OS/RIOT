/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GSM_H
#define GSM_H

#include "at.h"
#include "rmutex.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GSM_UART_BUFSIZE
/**
 * @brief   AT device buffer size
 */
#define GSM_UART_BUFSIZE        (1024U)
#endif

#ifndef GSM_SERIAL_TIMEOUT
/**
 * @brief   AT device default timeout
 */
#define GSM_SERIAL_TIMEOUT      (1000000U)
#endif

#ifndef GSM_THREAD_STACKSIZE
/**
 * @brief   GSM thread stack size
 */
#define GSM_THREAD_STACKSIZE    (THREAD_STACKSIZE_DEFAULT)
#endif

#ifndef GSM_THREAD_PRIO
/**
 * @brief   GSM thread priority
 */
#define GSM_THREAD_PRIO         (THREAD_PRIORITY_MAIN + 1)
#endif

typedef struct gsm_driver gsm_driver_t;

typedef void (*gsm_sms_cb_t)(void *arg, char *sms, char *sender, char *date);
typedef void (*gsm_wkup_cb_t)(void *arg);

enum {
    GSM_OFF,
    GSM_ON,
};

typedef enum {
    GSM_CT_APPLICATION_X_WWW_URL_ENCODED,
    GSM_CT_TEXT_PLAIN,
    GSM_CT_APPLICATION_OCTET_STREAM,
    GSM_CT_MULTIPART_FORM_DATA,
    GSM_CT_APPLICATION_JSON,
    GSM_CT_APPLICATION_XML,
    /* Add new content types here */
    GSM_CT_MAX,
} gsm_content_type_t;

typedef struct {
    uint16_t port;
    gsm_content_type_t content_type;
    const char *user_agent;
} gsm_http_params_t;

/**
 * @brief   GSM device parameters
 *
 * GSM device common parameters.
 * GSM implementations can extend this structure with device specific fields
 */
typedef struct {
    uart_t uart;
    uint32_t baudrate;
    uint32_t baudrate_to_set;
    gpio_t ri_pin;
} gsm_params_t;

/**
 * @brief   GSM device descriptor
 */
typedef struct {
    /**
     * GSM driver
     */
    const gsm_driver_t *driver;
    /**
     * GSM parameters
     */
    gsm_params_t params;
    /**
     * AT device
     */
    at_dev_t at_dev;
    /**
     * GSM lock
     */
    rmutex_t mutex;
    /**
     * AT device buffer
     */
    char buf[GSM_UART_BUFSIZE];
    /**
     * GSM thread stack
     */
    char stack[GSM_THREAD_STACKSIZE];
    /**
     * GSM thread PID
     */
    kernel_pid_t pid;
    /**
     * GSM state (on / off)
     */
    int state;
    /**
     * SMS callback
     */
    gsm_sms_cb_t sms_cb;
    /**
     * SMS callback parameter
     */
    void *sms_arg;
    /**
     * Wake up callback
     */
    gsm_wkup_cb_t wkup_cb;
    /**
     * Wake up callback parameter
     */
    void *wkup_arg;
} gsm_t;

/**
 * @brief   GSM driver
 */
struct gsm_driver {
    /**
     * @brief   Initialize the device driver
     *
     * Initialize the internal structure with the given @p param. This function
     * is called once and should not act on the device itself.
     *
     * @param[in]   dev     device driver to initialize
     * @param[in]   params  device driver parameters
     *
     * @return 0 on success
     */
    int (*init_drv)(gsm_t *dev, const gsm_params_t *params);

    /**
     * @brief   Perform a hardware reset on the device
     *
     * @param[in]   dev     device driver
     *
     * @return 0 on success
     */
    int (*reset)(gsm_t *dev);

    /**
     * @brief   Initialize the device
     *
     * This function actually initialize the hardware
     *
     * @param[in]   dev     device driver
     *
     * @return 0 on success
     */
    int (*init)(gsm_t *dev);

    /**
     * @brief   Change device baudrate
     *
     * @param[in]   dev     device driver
     *
     * @return 0 on success
     * @return < 0 on error
     */
    int (*change_baudrate)(gsm_t *dev, uint32_t baudrate);

    /**
     * @brief   Put the device to sleep
     *
     * After this funtion is called successfully, the AT device UART may be
     * powered off. If the RI pin is defined, it is used as interrupt source for
     * wake up.
     *
     * @param[in]   dev     device driver
     *
     * @return 0 on success
     * @return < 0 on error, or if device can not sleep
     */
    int (*sleep)(gsm_t *dev);

    /**
     * @brief   Wake up the device
     *
     * Before this funtion is called, the AT device UART is powered on.
     *
     * @param[in]   dev     device driver
     *
     * @return 0 on success
     * @return < 0 on errir, or if device can not wake up
     */
    int (*wake_up)(gsm_t *dev);

    /**
     * @brief   Initialize GPRS data connection
     *
     * @param[in]   gsmdev  device driver
     * @param[in]   apn     APN name to use
     * @param[in]   user    APN user if authentication is needed
     * @param[in]   passwd  APN password it authentication is needed
     *
     * @return 0 on success
     * @return < 0 on error
     */
    int (*gprs_init)(gsm_t *gsmdev, const char *apn, const char *user, const char *passwd);

    /**
     * @brief   Send an HTTP GET request and return a buffer
     *
     * @param[in]   gsmdev  device driver
     */
    ssize_t (*http_get)(gsm_t *gsmdev, const char *url,
                        uint8_t *resultbuf, size_t len,
                        const gsm_http_params_t *params);

    /**
     * @brief   Send an HTTP GET request and return a file
     *
     * @param[in]   gsmdev  device driver
     */
    ssize_t (*http_get_file)(gsm_t *gsmdev, const char *url, const char *filename,
                             const gsm_http_params_t *params);

    /**
     * @brief   Send an HTTP POST request from a buffer an return a buffer
     *
     * @param[in]   gsmdev  device driver
     */
    ssize_t (*http_post)(gsm_t *gsmdev, const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len,
                         const gsm_http_params_t *params);

    /**
     * @brief   Send an HTTP POST request from a file and return a buffer
     *
     * @param[in]   gsmdev  device driver
     */
    ssize_t (*http_post_file)(gsm_t *gsmdev, const char *url,
                              const char *filename,
                              uint8_t *resultbuf, size_t result_len,
                              const gsm_http_params_t *params);

    /**
     * @brief   Send an HTTP POST request from a file and return a file
     *
     * @param[in]   gsmdev  device driver
     */
    ssize_t (*http_post_file_2)(gsm_t *gsmdev, const char *url,
                                const char *filename,
                                const char *result_filename,
                                const gsm_http_params_t *params);

    int(*get_loc)(gsm_t *dev, char *lon, char *lat);
    int(*cnet_scan)(gsm_t *dev, char *buf, size_t len);

    int(*time_sync)(gsm_t *dev);
    ssize_t (*get_nmea)(gsm_t *gsmdev, char *nmea, size_t len);
};

/**
 * @brief   Initialize GSM device
 *
 * @param[in]   gsmdev  device struct to operate on
 * @param[in]   params  parameter struct used to initialize @p gsmdev
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int gsm_init(gsm_t *gsmdev, const gsm_params_t *params);

/**
 * @brief   Set SIM card PIN
 *
 * @param[in]   gsmdev  device struct to operate on
 * @param[in]   pin     PIN to set
 *
 * @returns     0 on success
 * @returns     -1 on error
 */
int gsm_set_pin(gsm_t *gsmdev, const char *pin);

/**
 * @brief   Test if SIM card needs PIN
 *
 * @param[in]   gsmdev  device struct to operate on
 *
 * @returns     0 if SIM doesn't need pin
 * @returns     1 if SIM needs to be unlocked with PIN
 */
int gsm_check_pin(gsm_t *gsmdev);

/**
 * @brief   Initialize GPRS connection
 *
 * @param[in]   gsmdev  device struct to operate on
 * @param[in]   apn     APN used to connect
 * @param[in]   user    APN user if authentication is needed
 * @param[in]   passwd  APN password if authentication is needed
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
static inline int gsm_gprs_init(gsm_t *gsmdev, const char *apn, const char *user, const char *passwd)
{
    return gsmdev->driver->gprs_init(gsmdev, apn, user, passwd);
}

/**
 * @brief   Check if modem is registered to network
 *
 * @param[in]   gsmdev  device to operate on
 *
 * @returns     0 if registered
 * @returns     1 otherwise
 */
int gsm_reg_check(gsm_t *gsmdev);

/**
 * @brief   Get network registration state
 *
 * @param[in]   gsmdev  device to operate on
 * @param[out]  buf     response buffer
 * @param[in]   len     response buffer length
 *
 * @return  number of bytes in @buf on success
 * @return  -1 on error
 */
size_t gsm_reg_get(gsm_t *gsmdev, char *buf, size_t len);

/**
 * @brief   Get CREG value
 *
 * @param[in]   gsmdev  device to operate on
 *
 * @return  positive CREG value on success
 * @return  < 0 on error
 */
int gsm_creg_get(gsm_t *gsmdev);

/**
 * @brief   Get CSQ values
 *
 * @param[in]   gsmdev  device to operate on
 * @param[out]  csq     CSQ signal quality
 * @param[out]  ber     Bit Error Rate from CSQ
 *
 * @return  0 on succes
 * @return  < 0 on error
 */
int gsm_signal_get(gsm_t *gsmdev, unsigned *csq, unsigned *ber);

/**
 * @brief   Get modem IMEI
 *
 * @param[in]   gsmdev  device to operate on
 * @param[out]  buf     buffer to write IMEI in
 * @param[in]   len     length of @p buf
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_imei_get(gsm_t *gsmdev, char *buf, size_t len);

/**
 * @brief   Get SIM ICCID
 *
 * @param[in]   gsmdev  device to operate on
 * @param[out]  buf     buffer to write ICCID in
 * @param[in]   len     length of @p buf
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_iccid_get(gsm_t *gsmdev, char *buf, size_t len);

/**
 * @brief   Get IP address
 *
 * @param[in]   gsmdev  device to operate on
 *
 * @return  modem IP address
 */
uint32_t gsm_gprs_getip(gsm_t *gsmdev);

/**
 * @brief   Print modem status on stdout
 *
 * @param[in]   gsmdev  device to operate on
 */
void gsm_print_status(gsm_t *gsmdev);

/**
 * @brief   Enable SMS reception
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   cb          SMS callback
 * @param[in]   arg         SMS callback argument
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_receive_sms(gsm_t *gsmdev, gsm_sms_cb_t cb, void *arg);

/**
 * @brief   Put the modem in sleep mode
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   cb          wake up callback, called when the modem requests a wakeup
 * @param[in]   arg         wake up callback argument
 */
void gsm_sleep(gsm_t *gsmdev, gsm_wkup_cb_t cb, void *arg);

/**
 * @brief   Send a HTTP GET request
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   url         URL to contact, must contain http(s)://
 * @param[out]  resultbuf   buffer to hold HTTP response
 * @param[in]   len         length of @p resultbuf
 *
 * @return  length of the response on success
 * @return  -1 on error
 */
ssize_t gsm_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len,
                     const gsm_http_params_t *params);

/**
 * @brief   Send a HTTP GET request and save the response in a file
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   url         URL to contact, must contain http(s)://
 * @param[in]   filename    file where the response will be saved
 *
 * @return  length of the response on success
 * @return  -1 on error
 */
ssize_t gsm_http_get_file(gsm_t *gsmdev, const char *url, const char *filename,
                          const gsm_http_params_t *params);

/**
 * @brief   Send a HTTP POST request
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   url         URL to contact, must contain http(s)://
 * @param[in]   data        POST data to send, without HTTP header
 * @param[in]   data_len    length of @p post_data
 * @param[out]  resultbuf   buffer to hold HTTP response
 * @param[in]   result_len  length of @p resultbuf
 *
 * @return  length of the response on success
 * @return  -1 on error
 */
ssize_t gsm_http_post(gsm_t *gsmdev,
                      const char *url,
                      const uint8_t *data, size_t data_len,
                      uint8_t *resultbuf, size_t result_len,
                      const gsm_http_params_t *params);

/**
 * @brief   Send a file as HTTP POST request
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   url         URL to contact, must contain http(s)://
 * @param[in]   filename    file containing data to send
 * @param[out]  resultbuf   buffer to hold HTTP response
 * @param[in]   result_len  length of @p resultbuf
 *
 * @return  length of the response on sucess
 * @retuen  -1 on error
 */
ssize_t gsm_http_post_file(gsm_t *gsmdev, const char *url,
                           const char *filename,
                           uint8_t *resultbuf, size_t result_len,
                           const gsm_http_params_t *params);

/**
 * @brief   Send a file as HTTP POST request and save the response in a file
 *
 * @param[in]   gsmdev      device to operate on
 * @param[in]   url         URL to contact, must contain http(s)://
 * @param[in]   filename    file containing data to send
 * @param[in]   result_filename file where the response will be saved
 *
 * @return  length of the response on sucess
 * @retuen  -1 on error
 */
ssize_t gsm_http_post_file_2(gsm_t *gsmdev, const char *url,
                             const char *filename,
                             const char *result_filename,
                             const gsm_http_params_t *params);

/**
 * @brief
 * @param gsmdev
 * @param buf
 * @param len
 * @return
 */
int gsm_cnet_scan(gsm_t *gsmdev, char *buf, size_t len);

/**
 * @brief   Send a 'raw' command to the modem and get the result
 *
 * @param[in]   gsmdev  device to operate on
 * @param[in]   cmd     command to send
 * @param[out]  buf     response buffer
 * @param[in]   len     length of @p buf
 * @param[in]   timeout command timeout
 *
 * @return  the length of the response on success
 * @return  < 0 on error
 */
ssize_t gsm_cmd(gsm_t *gsmdev, const char *cmd, uint8_t *buf, size_t len, unsigned timeout);

/**
 * @brief gsm_gps_get_loc
 * @param gsmdev
 * @param buf
 * @param len
 * @return
 */
int gsm_gps_get_loc(gsm_t *gsmdev, uint8_t *buf, size_t len);

/**
 * @brief gsm_get_loc
 * @param gsmdev
 * @param lon
 * @param lat
 * @return
 */
int gsm_get_loc(gsm_t *gsmdev, char *lon, char *lat);

/**
 * @brief gsm_off
 * @param gsmdev
 * @return
 */
int gsm_off(gsm_t *gsmdev);

/**
 * @brief gsm_get_nmea
 * @param gsmdev
 * @param nmea
 * @param len
 * @return
 */
ssize_t gsm_get_nmea(gsm_t *gsmdev, char *nmea, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* GSM_H */
