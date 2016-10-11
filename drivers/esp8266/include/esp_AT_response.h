/*
 * Copyright (C) 2016 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_esp8266 ESP8266 driver
 * @ingroup     drivers_netdev
 * @brief       High-level driver for the ESP8266 modem
 * @{
 *
 * @file
 * @brief       typedef neccesary for esp AT command; constant AT commands
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 */

#ifndef ESP_AT_RESPONSE_H_
#define ESP_AT_RESPONSE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* DEFINES */
#define OK     ("\r\nOK\r\n")
#define FAIL   ("\r\nFAIL\r\n")
#define ERROR  ("\r\nERROR\r\n")
#define CLOSED ("\r\nCLOSED\r\n")

/* TYPEDEFS */


/* return value for response outcome */
typedef enum {
    ok,
    fail,
    error,
    closed,
    ready,
    no_resp
} term_mode_t;


/* AT response terminator */
typedef struct {
    char *str;
    uint16_t len;
    term_mode_t term_mode;
} terminator_t;

/* AT response */

typedef struct {
    char *str;
    term_mode_t term_mode;
} response_t;

/** CONSTANT VALUES **/

/* AT command response termination strings */
static const terminator_t terminators[] = {
    { OK, sizeof(OK) - 1, ok },
    { FAIL, sizeof(FAIL) - 1, fail },
    { ERROR, sizeof(ERROR) - 1, error },
    { CLOSED, sizeof(CLOSED) - 1, closed },
    //{"ready\r\n", ready}
};

#ifdef __cplusplus
}
#endif

#endif /* ESP_AT_RESPONSE_H_ */
/** @} */
