/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       printf functions for SDK libraries
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file contains library-specific printf functions used by ESP SDK
 * libraries. These functions are used to intercept the output generated
 * by the SDK libraries and redirect it to RIOT's LOG macros.
 *
 * @}
 */

#include <stdarg.h>

#include "cpu_conf.h"
#include "esp_common_log.h"

#define LIB_PRINTF(name, level, tag) \
    int name ## _printf(const char *format, ...) { \
        va_list arg; \
        va_start(arg, format); \
        int res = _lib_printf(level, tag, format, arg); \
        va_end(arg); \
        return res; \
    }

static char _printf_buf[PRINTF_BUFSIZ];

static int _lib_printf(int level, const char* tag, const char* format, va_list arg)
{
    if (level > LOG_LEVEL) {
        return 0;
    }

    int len = vsnprintf(_printf_buf, PRINTF_BUFSIZ - 1, format, arg);

    if (len < 0) {
        ESP_EARLY_LOGI(tag, "Failed to format print");
        return 0;
    }

    /* Did the output get truncated? */
    if ((unsigned) len > PRINTF_BUFSIZ - 1) {
        len = PRINTF_BUFSIZ - 1;
    }

    /*
     * Since ESP_EARLY_LOG macros add a line break at the end, a terminating
     * line break in the output must be removed if there is one.
     */
    _printf_buf[PRINTF_BUFSIZ - 1] = 0;
    int i;
    for (i = len - 1; i >= 0; --i) {
        if (_printf_buf[i] != '\n' && _printf_buf[i] != '\r' && _printf_buf[i] != ' ') {
            break;
        }
        _printf_buf[i] = 0;
    }
    if (i > 0) {
        ESP_EARLY_LOGI(tag, "%s", _printf_buf);
    }
    va_end(arg);
    return len;
}

LIB_PRINTF(coexist, LOG_DEBUG, "coexist")
LIB_PRINTF(core, LOG_DEBUG, "core")
LIB_PRINTF(espnow, LOG_DEBUG, "espnow")
LIB_PRINTF(net80211, LOG_DEBUG, "net80211")
LIB_PRINTF(phy, LOG_DEBUG, "phy")
LIB_PRINTF(pp, LOG_DEBUG, "pp")
LIB_PRINTF(sc, LOG_DEBUG, "smartconfig")
/* The ESP8266 SDK uses smartconfig_printf but the ESP32 one uses sc_printf. */
int smartconfig_printf(const char *format, ...)
    __attribute__((alias("sc_printf")));
LIB_PRINTF(ssc, LOG_DEBUG, "ssc")
LIB_PRINTF(wpa, LOG_DEBUG, "wpa")
LIB_PRINTF(wpa2, LOG_DEBUG, "wpa")
LIB_PRINTF(wps, LOG_DEBUG, "wps")

LIB_PRINTF(core_ets, LOG_DEBUG, "core")
LIB_PRINTF(espnow_ets, LOG_DEBUG, "espnow")
LIB_PRINTF(pp_ets, LOG_DEBUG, "pp")
LIB_PRINTF(wpa_ets, LOG_DEBUG, "wpa")

int rtc_printf(const char* format, ...)
{
    /* librtc.a printf temporary disabled due to UART baud rate switching bug. */
    return 0;
}
