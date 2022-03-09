/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nrf_modem
 * @{
 *
 * @file
 * @brief       RIOT specific implementation of nrf_modem_os_* functions
 *              used by nrf modem
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "nrf_modem_os.h"
#include "nrf_errno.h"
#include "nrf_modem_platform.h"
#include "nrf_modem_limits.h"
#include "ztimer.h"
#include "tlsf.h"
#include "sema.h"

#include "cpu.h"
#include "periph_cpu.h"
#include <errno.h>
#include <stdlib.h>

/* Enabling debug may break timing, use it with care */
#define ENABLE_DEBUG 0
#include "debug.h"

#define TRACE_IRQ          EGU2_IRQn
#define TRACE_IRQ_PRIORITY 6
#define TRACE_IRQ_HANDLER  EGU2_IRQHandler

extern void *__real_malloc(size_t size);
extern void __real_free(void *ptr);
extern void *__real_realloc(void *ptr, size_t size);

ztimer_t wait_timer;
bool time_exp;
ztimer_now_t started;
extern tlsf_t tlsf;
static mutex_t _lock;

static void _timeout(void* arg)
{
    (void)arg;
    unsigned irq = irq_disable();
    time_exp = true;
    irq_restore(irq);
}

static void _nrf_modem_irq_enable(void)
{
    NVIC_SetPriority(NRF_MODEM_APPLICATION_IRQ, NRF_MODEM_APPLICATION_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(NRF_MODEM_APPLICATION_IRQ);
    NVIC_EnableIRQ(NRF_MODEM_APPLICATION_IRQ);
}

static void _nrf_modem_trace_enable(void)
{
    NVIC_SetPriority(TRACE_IRQ, TRACE_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(TRACE_IRQ);
    NVIC_EnableIRQ(TRACE_IRQ);
}

void nrf_modem_os_init(void) {
    _nrf_modem_irq_enable();
    _nrf_modem_trace_enable();
    wait_timer.callback = _timeout;
    wait_timer.arg = 0;
}

int nrf_modem_os_sem_init(void **sem, unsigned int initial_count,
                          unsigned int limit)
{
    (void)limit;
    /* Check if a sem is given or if we must initialize one */
    if (*sem == NULL) {
        *sem = malloc(sizeof(sema_t));
        if (*sem == NULL) {
            return -ENOMEM;
        }
    }

    sema_create((sema_t*)*sem, initial_count);
    return 0;
}

void nrf_modem_os_sem_give(void *sem)
{
    int ret;
    sema_t* sema = (sema_t*)sem;
    ret = sema_post(sema);
    if ( ret != 0) {
       assert(0);
    }
}

int nrf_modem_os_sem_take(void *sem, int timeout)
{
    sema_t* sema = (sema_t*)sem;
    int ret = 0;
    if (timeout == NRF_MODEM_OS_NO_WAIT)
    {
        ret = sema_wait_timed_ztimer(sema, ZTIMER_MSEC, 0);
    }
    else if (timeout == NRF_MODEM_OS_FOREVER) {
        /* Wait for semaphore to be released */
        do {
            ret = sema_wait_timed_ztimer(sema, ZTIMER_MSEC, UINT32_MAX);
        } while (ret != 0);
    }
    else {
        ret = sema_wait_timed_ztimer(sema, ZTIMER_MSEC, timeout);
    }

    if (ret != 0) {
        return -NRF_ETIMEDOUT;
    } else {
        return 0;
    }
}

void *nrf_modem_os_shm_tx_alloc(size_t bytes) {
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    return tlsf_malloc(tlsf, bytes);
}

void nrf_modem_os_shm_tx_free(void *mem) {
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    tlsf_free(tlsf, mem);
}

/* malloc can be done in ISR context with Nordic Modem,
   but RIOT's malloc implementation is thread safe and
   prevent it from being call, thus, avoid it if we are
   from ISR context for compatibility */
void *nrf_modem_os_alloc(size_t bytes)
{
    void* ptr;

    DEBUG("[nrf_modem_os]:%s:%d bytes\n", __FUNCTION__, bytes);
    if (irq_is_in()) {
        if (mutex_trylock(&_lock)) {
            ptr = __real_malloc(bytes);
            mutex_unlock(&_lock);
        }
        else {
            return NULL;
        }
    }
    else {
        ptr = malloc(bytes);
    }

    return ptr;
}

void nrf_modem_os_free(void *mem)
{
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    if (irq_is_in()) {
        if (mutex_trylock(&_lock)) {
            __real_free(mem);
            mutex_unlock(&_lock);
        }
        else {
            assert(0);
        }
    }
    else {
        free(mem);
    }
}

void nrf_modem_os_busywait(int32_t usec) {
    /* Cannot use timer based as this function can be
       call from ISR context */
    for (unsigned i=0; i < (usec * (CLOCK_CORECLOCK / 1000000));i++)
    {
        __asm("nop");
    }
}

int32_t nrf_modem_os_timedwait(uint32_t context, int32_t* timeout)
{
    (void)context;

    unsigned irq = irq_disable();
    if (!ztimer_is_set(ZTIMER_MSEC, &wait_timer)) {
        ztimer_set(ZTIMER_MSEC, &wait_timer, *timeout);
        started = ztimer_now(ZTIMER_MSEC);
        time_exp = false;
    }

    if (time_exp == true) {
        *timeout = 0;
        irq_restore(irq);
        return NRF_ETIMEDOUT;
    }
    else {
        *timeout = *timeout - (ztimer_now(ZTIMER_MSEC) - started);
    }
    irq_restore(irq);
    return 0;
}

void nrf_modem_os_errno_set(int errno_val)
{
    (void)errno_val;
    DEBUG("[nrf_modem_os]:%s:%d\n", __FUNCTION__, errno_val);
    /* TODO: implement this */
}

bool nrf_modem_os_is_in_isr(void) {
    return irq_is_in();
}

void nrf_modem_os_application_irq_set(void) {
    NVIC_SetPendingIRQ(NRF_MODEM_APPLICATION_IRQ);
}

void nrf_modem_os_application_irq_clear(void) {
    NVIC_ClearPendingIRQ(NRF_MODEM_APPLICATION_IRQ);
}

void isr_egu1(void) {
    nrf_modem_os_application_irq_handler();
}
void isr_egu2(void) {
    nrf_modem_os_trace_irq_handler();
}

void nrf_modem_os_trace_irq_set(void) {
    NVIC_SetPendingIRQ(TRACE_IRQ);
}

void nrf_modem_os_trace_irq_clear(void) {
    NVIC_ClearPendingIRQ(TRACE_IRQ);
}

int32_t nrf_modem_os_trace_put(const uint8_t * const p_buffer, uint32_t buf_len) {

    (void)p_buffer;
    (void)buf_len;
    /* TODO implement trace */

    return 0;
}
