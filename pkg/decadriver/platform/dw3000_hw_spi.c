/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_decadriver
 * @{
 *
 * @file
 * @brief       Hardware (GPIO) and SPI abstraction for decadriver in RIOT
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "dw3000_hw.h"
#include "dw3000_spi.h"
#include "deca_device_api.h"
#include "deca_interface.h"

#include "dw3000_params.h"

#include <stdint.h>

#define ENABLE_DEBUG    0
#include "debug.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "thread.h"
#include "ztimer.h"

/** Stack size of the user-space thread handling interrupts */
#ifndef CONFIG_DECADRIVER_STACK_SIZE
#  define CONFIG_DECADRIVER_STACK_SIZE          THREAD_STACKSIZE_MEDIUM
#endif

/**
 * @brief Priority of the user-space thread handling interrupts
 *
 * Value inspired from GNRC_NETIF_PRIO */
#ifndef CONFIG_DECADRIVER_ISR_THREAD_PRIO
#  define CONFIG_DECADRIVER_ISR_THREAD_PRIO     (THREAD_PRIORITY_MAIN - 5)
#endif


static spi_clk_t _spi_speed = DW3000_PARAM_SPI_SPEED_SLOW;
static bool _irq_active = false;
static char _rx_stack[CONFIG_DECADRIVER_STACK_SIZE];
static thread_t * _rx_thread_ptr = NULL;

#define RX_THREAD_FLAG  1

/* ------------------------------ dw3000_hw ------------------------------ */

int dw3000_hw_init(void)
{
    _spi_speed = dw3000_params.spi_speed_slow;

    if (gpio_is_valid(dw3000_params.reset)) {
        gpio_init(dw3000_params.reset, GPIO_IN);
    }

    if (gpio_is_valid(dw3000_params.wakeup)) {
        gpio_init(dw3000_params.wakeup, GPIO_OUT);
    }

    return dw3000_spi_init();
}

void dw3000_hw_reset(void)
{
    if (!gpio_is_valid(dw3000_params.reset)) {
        /* If no reset pin is configured, try to reset via SPI.
         * Note that this can lead to a NULL pointer access if this is called
         * before dwt_probe() being successful. So to call reset initially, a
         * reset pin is strongly recommended. */
        DEBUG_PUTS("decadriver: dw3000_hw_reset() called without reset pin.\n"
                   "            Attempting softreset...\n");
        dw3000_spi_speed_slow();
        dwt_softreset(0);
        return;
    }
    else {
        /* Interrupts need to be turned off before pulling reset low */
        decaIrqStatus_t irq = decamutexon();

        /* The timing values are taken from the Zephyr / nRF / ESP ports */
        gpio_init(dw3000_params.reset, GPIO_OUT);
        gpio_clear(dw3000_params.reset);
        ztimer_sleep(ZTIMER_MSEC, 1);
        gpio_init(dw3000_params.reset, GPIO_IN);
        ztimer_sleep(ZTIMER_MSEC, 2);

        decamutexoff(irq);
    }
}

static void _interrupt_callback(void* arg)
{
    (void) arg;
    thread_flags_set(_rx_thread_ptr, RX_THREAD_FLAG);
}

static void* _rx_thread(void* args)
{
    (void) args;

    while (1) {
        thread_flags_wait_any(RX_THREAD_FLAG);
        while (gpio_read(dw3000_params.irq)) {
            dwt_isr();
        }
    }
    return NULL;
}

int dw3000_hw_init_interrupt(void)
{
    if (!gpio_is_valid(dw3000_params.irq)) {
        DEBUG("decadriver: IRQ pin not configured");
        return -ENOENT;
    }

    if (_rx_thread_ptr == NULL) {
        kernel_pid_t pid = thread_create(_rx_stack, sizeof(_rx_stack),
                                         CONFIG_DECADRIVER_ISR_THREAD_PRIO, 0,
                                         _rx_thread, NULL, "deca rx");
        if (pid < 0) {
            return pid;
        }
        _rx_thread_ptr = thread_get(pid);
    }

    gpio_init_int(dw3000_params.irq, GPIO_IN, GPIO_RISING,
                    _interrupt_callback, NULL);
    _irq_active = true;
    return 0;
}

void dw3000_hw_fini(void)
{
    dw3000_hw_interrupt_disable();
    dw3000_spi_fini();
}

void dw3000_hw_wakeup(void)
{
    if (gpio_is_valid(dw3000_params.wakeup)) {
        gpio_set(dw3000_params.wakeup);
        ztimer_sleep(ZTIMER_MSEC, 1);
        gpio_clear(dw3000_params.reset);
    }
    else {
        dw3000_spi_wakeup();
    }
}

void dw3000_hw_wakeup_pin_low(void)
{
    if (gpio_is_valid(dw3000_params.wakeup)) {
        gpio_clear(dw3000_params.wakeup);
    }
}

void dw3000_hw_interrupt_enable(void)
{
    if (gpio_is_valid(dw3000_params.irq)) {
        gpio_irq_enable(dw3000_params.irq);
        _irq_active = true;
    }
}

void dw3000_hw_interrupt_disable(void)
{
    if (gpio_is_valid(dw3000_params.irq)) {
        gpio_irq_disable(dw3000_params.irq);
        _irq_active = false;
    }
}

bool dw3000_hw_interrupt_is_enabled(void)
{
    return _irq_active;
}

/* ------------------------------ dw3000_spi ------------------------------ */

int dw3000_spi_init(void)
{
    return spi_init_cs(dw3000_params.spi, dw3000_params.spi_cs);
}

void dw3000_spi_fini(void)
{
    return;
}

void dw3000_spi_wakeup(void)
{
    gpio_clear(dw3000_params.reset);
    ztimer_sleep(ZTIMER_MSEC, 1);
    gpio_set(dw3000_params.reset);
}

void dw3000_spi_speed_slow(void)
{
    _spi_speed = dw3000_params.spi_speed_slow;
}

void dw3000_spi_speed_fast(void)
{
    _spi_speed = dw3000_params.spi_speed_fast;
}

int32_t dw3000_spi_read(uint16_t headerLength, uint8_t* headerBuffer,
                        uint16_t readLength, uint8_t* readBuffer)
{
    spi_acquire(dw3000_params.spi, dw3000_params.spi_cs,
                dw3000_params.spi_mode, _spi_speed);
    spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, true,
                       headerBuffer, NULL, headerLength);
    spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, false,
                       NULL, readBuffer, readLength);
    spi_release(dw3000_params.spi);
    return DWT_SUCCESS;
}

int32_t dw3000_spi_write(uint16_t headerLength, const uint8_t* headerBuffer,
                         uint16_t bodyLength, const uint8_t* bodyBuffer)
{
    /* This function is also used for commands only consisting of a header.
     * So the body is full optional here. Only if it is given shall CS be held
     * and the second spi_transfer_bytes() be called, otherwise leading to an
     * assertion error in spi_transfer_bytes() */
    bool has_body = bodyLength > 0;

    spi_acquire(dw3000_params.spi, dw3000_params.spi_cs,
                dw3000_params.spi_mode, _spi_speed);
    spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, has_body,
                       headerBuffer, NULL, headerLength);
    if (has_body) {
        spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, false,
                           bodyBuffer, NULL, bodyLength);
    }

    spi_release(dw3000_params.spi);
    return DWT_SUCCESS;
}

int32_t dw3000_spi_write_crc(uint16_t headerLength, const uint8_t* headerBuffer,
                             uint16_t bodyLength, const uint8_t* bodyBuffer,
                             uint8_t crc8)
{
    /* See dw3000_spi_write() */
    bool has_body = bodyLength > 0;

    spi_acquire(dw3000_params.spi, dw3000_params.spi_cs,
                dw3000_params.spi_mode, _spi_speed);
    spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, true,
                       headerBuffer, NULL, headerLength);
    if (has_body) {
        spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, true,
                           bodyBuffer, NULL, bodyLength);
    }
    spi_transfer_bytes(dw3000_params.spi, dw3000_params.spi_cs, false,
                       &crc8, NULL, 1);
    spi_release(dw3000_params.spi);
    return DWT_SUCCESS;
}
