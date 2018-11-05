/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef OSAPI_H
#define OSAPI_H

#ifndef DOXYGEN

#include <string.h>
#include "os_type.h"
#include "user_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void ets_bzero(void *s, size_t n);
void ets_delay_us(uint16_t us);
void ets_install_putc1(void (*p)(char c));

#define os_bzero ets_bzero
#define os_delay_us ets_delay_us
#define os_install_putc1 ets_install_putc1

int ets_memcmp(const void *str1, const void *str2, unsigned int nbyte);
void *ets_memcpy(void *dest, const void *src, unsigned int nbyte);
void *ets_memmove(void *dest, const void *src, unsigned int nbyte);
void *ets_memset(void *dest, int val, unsigned int nbyte);

int ets_strcmp(const char *s1, const char *s2);
char *ets_strcpy(char *s1, const char *s2);
int ets_strlen(const char *s);
int ets_strncmp(const char *s1, const char *s2, unsigned int n);
char *ets_strncpy(char *s1, const char *s2, unsigned int n);
char *ets_strstr(const char *s1, const char *s2);

#define os_memcmp ets_memcmp
#define os_memcpy ets_memcpy
#define os_memmove ets_memmove
#define os_memset ets_memset
#define os_strcat strcat
#define os_strchr strchr
#define os_strcmp ets_strcmp
#define os_strcpy ets_strcpy
#define os_strlen ets_strlen
#define os_strncmp ets_strncmp
#define os_strncpy ets_strncpy
#define os_strstr ets_strstr

void ets_timer_arm_new(os_timer_t *ptimer, uint32_t time, bool repeat_flag, bool ms_flag);
void ets_timer_disarm(os_timer_t *ptimer);
void ets_timer_setfn(os_timer_t *ptimer, os_timer_func_t *pfunction, void *parg);

#ifdef USE_US_TIMER
#define os_timer_arm_us(a, b, c) ets_timer_arm_new(a, b, c, 0)
#endif
#define os_timer_arm(a, b, c) ets_timer_arm_new(a, b, c, 1)
#define os_timer_disarm ets_timer_disarm
#define os_timer_setfn ets_timer_setfn

int ets_sprintf(char *str, const char *format, ...)  __attribute__ ((format (printf, 2, 3)));
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));

#define os_sprintf  ets_sprintf

#ifdef USE_OPTIMIZE_PRINTF
#define os_printf(fmt, ...) do {    \
    static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt;    \
    os_printf_plus(flash_str, ##__VA_ARGS__);    \
    } while(0)
#else
#define os_printf    os_printf_plus
#endif

unsigned long os_random(void);
int os_get_random(unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* OSAPI_H */
