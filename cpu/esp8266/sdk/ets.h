/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef ETS_H
#define ETS_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#include "c_types.h"
#include "ets_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/* interrupts that are not defined in espressif/ets_sys.h */
#define ETS_WDEV_INUM  0    /* WDEV process FIQ interrupt */
#define ETS_RTC_INUM   3    /* RTC interrupt */
#define ETS_CCOM_INUM  6    /* CCOMPARE0 match interrupt */
#define ETS_SOFT_INUM  7    /* software interrupt */
#define ETS_WDT_INUM   8    /* SDK watchdog timer */
#define ETS_FRC2_INUM  10   /* SDK FRC2 timer interrupt */

#ifndef MODULE_ESP_SDK_INT_HANDLING
/*
 * The following functions are mappings or dummies for source code
 * compatibility of SDK and NON-SDK version
 */

#include "xtensa/xtensa_api.h"

#define ets_isr_mask(x)         xt_ints_off(x)
#define ets_isr_unmask(x)       xt_ints_on(x)
#define ets_isr_attach(i,f,a)   xt_set_interrupt_handler(i,f,a)

#define _xtos_set_exception_handler(n,f)    xt_set_exception_handler(n,f)

#else /* MODULE_ESP_SDK_INT_HANDLING */

extern void ets_isr_mask(uint32_t);
extern void ets_isr_unmask(uint32_t);

typedef void (_xtos_handler_t)(void*);
extern void _xtos_set_exception_handler(int n, _xtos_handler_t* f);

#endif /* MODULE_ESP_SDK_INT_HANDLING */

#ifndef MODULE_ESP_SDK
extern void ets_delay_us(uint16_t us);
extern void ets_timer_arm_new(ETSTimer *ptimer, uint32_t ms_us, int repeat_flag, int is_ms);
extern void ets_timer_disarm(ETSTimer *ptimer);
extern void ets_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *arg);
#endif

extern void ets_timer_arm(ETSTimer *ptimer, uint32_t ms, bool repeat_flag);
extern void ets_timer_handler_isr(void);

extern void ets_install_uart_printf(void);
extern void ets_install_putc1(void (*p)(char c));
extern int  ets_uart_printf(const char *format, ...);

extern int  ets_putc(int);
extern int  ets_printf(const char * format, ...);
extern int  ets_vprintf(void *function, const char *format, va_list arg);

extern uint8_t ets_get_cpu_frequency(void);
extern void    ets_update_cpu_frequency(uint8_t);

extern void *ets_memcpy(void *to, const void *from, size_t size);

extern void ets_wdt_disable(void);
extern void ets_wdt_enable (void);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ETS_H */
