/*******************************************************************************
*
* Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    David Navarro, Intel Corporation - initial API and implementation
*    Christian Manal - Ported to RIOT OS
*******************************************************************************/
/**
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *               2019 HAW Hamburg
 * @{
 * @ingroup         pkg_wakaama
 *
 * @file
 * @brief           Platform adaption for Wakaama package
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <liblwm2m.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ztimer.h"
#include "tlsf.h"

#include "lwm2m_platform.h"
#include "lwm2m_client_config.h"

static uint32_t _tlsf_heap[(CONFIG_LWM2M_TLSF_BUFFER / sizeof(uint32_t))];
static tlsf_t _tlsf;

typedef struct {
    unsigned free;          /**< total free size */
    unsigned used;          /**< total used size */
} _tlsf_size_container_t;

 static void _tlsf_size_walker(void* ptr, size_t size, int used, void* user)
{
    printf("\t%p %s size: %" PRIuSIZE " (%p)\n", ptr, used ? "used" : "free", size, ptr);

     if (used) {
        ((_tlsf_size_container_t *)user)->used += (unsigned int)size;
    }
    else {
        ((_tlsf_size_container_t *)user)->free += (unsigned int)size;
    }
}

void lwm2m_tlsf_status(void)
{
    puts("\nTLSF usage:");
    _tlsf_size_container_t sizes = { .free = 0, .used = 0 };
    tlsf_walk_pool(tlsf_get_pool(_tlsf), _tlsf_size_walker, &sizes);
    printf("\tTotal free size: %u\n", sizes.free);
    printf("\tTotal used size: %u\n", sizes.used);
}

void lwm2m_platform_init(void)
{
    _tlsf = tlsf_create_with_pool(_tlsf_heap, sizeof(_tlsf_heap));
}

void *lwm2m_malloc(size_t s)
{
    return tlsf_malloc(_tlsf, s);
}

void lwm2m_free(void *p)
{
    tlsf_free(_tlsf, p);
}

char *lwm2m_strdup(const char *str)
{
    size_t len = strlen(str) + 1;
    void *new = lwm2m_malloc(len);

    if (new == NULL) {
        return NULL;
    }

    return strncpy(new, str, len);
}

int lwm2m_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

time_t lwm2m_gettime(void)
{
    return (time_t)(ztimer_now(ZTIMER_SEC));
}

/* For clang we need to specify that the first argument will be a format string
 * for print
 */
__attribute__((__format__ (__printf__, 1, 0)))
void lwm2m_printf(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);

    vfprintf(stderr, format, ap);

    va_end(ap);
}
