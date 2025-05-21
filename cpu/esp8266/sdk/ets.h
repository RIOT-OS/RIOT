/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp8266_sdk
 * @{
 *
 * @file
 * @brief       ESP8266 ETS ROM function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef DOXYGEN

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "rom/ets_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/* interrupts that are not defined in rom/ets_sys.h */
#define ETS_WDEV_INUM  0    /* WDEV process FIQ interrupt */
#define ETS_RTC_INUM   3    /* RTC interrupt */
#define ETS_CCOM_INUM  6    /* CCOMPARE0 match interrupt */
#define ETS_FRC2_INUM  10   /* SDK FRC2 timer interrupt */

/*
 * The following functions are mappings or dummies for source code
 * compatibility of NONOS-SDK and RTOS-SDK version
 */

#include "xtensa/xtensa_api.h"

#define ets_isr_mask(x)         xt_ints_off(x)
#define ets_isr_unmask(x)       xt_ints_on(x)
#define ets_isr_attach(i,f,a)   xt_set_interrupt_handler(i,f,a)

#define ETS_INTR_ENABLE(inum)   ets_isr_unmask((1<<inum))
#define ETS_INTR_DISABLE(inum)  ets_isr_mask((1<<inum))

#define _xtos_set_exception_handler(n,f)    xt_set_exception_handler(n,f)

extern uint8_t ets_get_cpu_frequency(void);
extern void ets_update_cpu_frequency(uint8_t);

extern int ets_vprintf(const char *fmt, va_list ap);
extern int ets_printf(const char *fmt, ...);
extern int ets_putc(int c);
extern void ets_install_putc1(void (*p)(char c));

extern void *ets_memcpy(void *dst, const void *src, size_t size);

extern void ets_wdt_disable(void);
extern void ets_wdt_enable (void);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
