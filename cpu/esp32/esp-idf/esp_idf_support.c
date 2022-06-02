/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Functions required for ESP-IDF compatibility
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <string.h>

#include "esp_common.h"
#include "log.h"
#include "syscalls.h"

#include "esp_attr.h"
#include "esp_log.h"

#define ENABLE_DEBUG  0
#include "debug.h"

#if IS_USED(MODULE_ESP_WIFI_ANY) || IS_USED(MODULE_ESP_ETH)
#include "esp_event_base.h"

ESP_EVENT_DEFINE_BASE(IP_EVENT);
#endif

/* Global variables required by ESP-IDF */
uint8_t *g_wpa_anonymous_identity;
int g_wpa_anonymous_identity_len;

uint8_t *g_wpa_username;
int g_wpa_username_len;

uint8_t *g_wpa_password;
int g_wpa_password_len;

uint8_t *g_wpa_new_password;
int g_wpa_new_password_len;

const uint8_t *g_wpa_client_cert;
int g_wpa_client_cert_len;

const uint8_t *g_wpa_private_key;
int g_wpa_private_key_len;

const uint8_t *g_wpa_private_key_passwd;
int g_wpa_private_key_passwd_len;

const uint8_t *g_wpa_ca_cert;
int g_wpa_ca_cert_len;

char *g_wpa_ttls_phase2_type;
bool g_wpa_suiteb_certification;

char *g_wpa_phase1_options;

uint8_t *g_wpa_pac_file;
int g_wpa_pac_file_len;

/*
 * provided by: /path/to/esp-idf/components/log/log_freertos.c
 */
uint32_t IRAM_ATTR esp_log_timestamp(void)
{
    return system_get_time() / USEC_PER_MSEC;
}

typedef struct {
    const char *tag;
    unsigned level;
} esp_log_level_entry_t;

static esp_log_level_entry_t _log_levels[] = {
    { .tag = "wifi", .level = LOG_INFO },
    { .tag = "*", .level = LOG_DEBUG },
};

static char _printf_buf[PRINTF_BUFSIZ];

/*
 * provided by: /path/to/esp-idf/component/log/log.c
 */
void IRAM_ATTR esp_log_write(esp_log_level_t level,
                             const char* tag, const char* format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_writev(level, tag, format, list);
    va_end(list);
}

/*
 * provided by: /path/to/esp-idf/component/log/log.c
 */
void IRAM_ATTR esp_log_writev(esp_log_level_t level,
                              const char *tag,
                              const char *format,
                              va_list args)
{
    /*
     * We use the log level set for the given tag instead of using
     * the given log level.
     */
    esp_log_level_t act_level = LOG_DEBUG;
    size_t i;
    for (i = 0; i < ARRAY_SIZE(_log_levels); i++) {
        if (strcmp(tag, _log_levels[i].tag) == 0) {
            act_level = _log_levels[i].level;
            break;
        }
    }

    /* If we didn't find an entry for the tag, we use the log level for "*" */
    if (i == ARRAY_SIZE(_log_levels)) {
        act_level = _log_levels[ARRAY_SIZE(_log_levels)-1].level;
    }

    /* Return if the log output has not the required level */
    if ((unsigned)act_level > CONFIG_LOG_DEFAULT_LEVEL) {
        return;
    }
#if 0   /* seems not to be required any longer */
    /*
     * The format of log output from ESP SDK libraries is "X (s) t: message\n"
     * where X is the log level, d the system time in milliseconds and t the
     * tag. To be able to enable these additional information by module
     * `esp_log_tagged`, we have to separate these information from the
     * message here.
     */
    const char* msg = (strchr (format, ':') + 2);

    va_list arglist;
    va_copy(arglist, args);

    /* remove time and tag argument from argument list */
    va_arg(arglist, unsigned);
    va_arg(arglist, const char*);
    vsnprintf(_printf_buf, PRINTF_BUFSIZ, msg, arglist);
    va_end(arglist);
#else
    va_list arglist;
    va_copy(arglist, args);
    vsnprintf(_printf_buf, PRINTF_BUFSIZ, format, arglist);
    va_end(arglist);
#endif

    switch (act_level) {
        case LOG_NONE   : return;
        case LOG_ERROR  : LOG_TAG_ERROR  (tag, "%s", _printf_buf); break;
        case LOG_WARNING: LOG_TAG_WARNING(tag, "%s", _printf_buf); break;
        case LOG_INFO   : LOG_TAG_INFO   (tag, "%s", _printf_buf); break;
        case LOG_DEBUG  : LOG_TAG_DEBUG  (tag, "%s", _printf_buf); break;
        case LOG_ALL    : LOG_TAG_ALL    (tag, "%s", _printf_buf); break;
    }
}

/*
 * provided by: /path/to/esp-idf/component/log/log.c
 */
void esp_log_level_set(const char* tag, esp_log_level_t level)
{
    size_t i;
    for (i = 0; i < ARRAY_SIZE(_log_levels); i++) {
        if (strcmp(tag, _log_levels[i].tag) == 0) {
            break;
        }
    }

    if (i == ARRAY_SIZE(_log_levels)) {
        LOG_DEBUG("Tag for setting log level not found\n");
        return;
    }

    _log_levels[i].level = level;
}
