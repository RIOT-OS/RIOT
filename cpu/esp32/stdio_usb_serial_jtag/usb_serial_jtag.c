/*
 * SPDX-FileCopyrightText: 2023 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    cpu_esp32_usb_serial_jtag  ESP32 USB Serial/JTAG interface
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       stdio via USB Serial JTAG debug interface
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>

#include "irq.h"
#include "stdio_base.h"
#include "syscalls.h"
#include "time_units.h"

#include "periph_cpu.h"

#include "irq_arch.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "hal/usb_serial_jtag_ll.h"
#include "soc/periph_defs.h"
#include "rom/ets_sys.h"

/* time of the last successful write to the TX FIFO */
static volatile uint32_t _last_tx;

static bool _wait_tx_fifo_writable(void)
{
    while (!usb_serial_jtag_ll_txfifo_writable()) {
        /* read _last_tx before the current time, otherwise the difference
         * becomes negative if another writer updates _last_tx in between */
        uint32_t last_tx = _last_tx;
        if ((system_get_time() - last_tx) > CONFIG_STDIO_USB_SERIAL_JTAG_TX_TIMEOUT_US) {
            /* the host didn't fetch data (no terminal connected?) */
            return false;
        }
    }
    return true;
}

static ssize_t _write(const void *buffer, size_t len)
{
    const uint8_t *buf = buffer;
    size_t n = 0;

    /* The TX FIFO is sent automatically when it is full. Remaining data is
     * dropped if the host doesn't fetch the data in time. */
    while ((n < len) && _wait_tx_fifo_writable()) {
        /* Checking for free space and writing to the TX FIFO must not be
         * interrupted by another writer, otherwise data gets lost. At most
         * 64 bytes are written with IRQs disabled. */
        unsigned state = irq_disable();
        n += usb_serial_jtag_ll_write_txfifo(buf + n, len - n);
        _last_tx = system_get_time();
        irq_restore(state);
    }

    /* Send the remaining data in the TX FIFO. If it has been sent
     * automatically because it was full, this sends a zero-length packet
     * that is required to complete the transfer on the host side. */
    if (_wait_tx_fifo_writable()) {
        usb_serial_jtag_ll_txfifo_flush();
    }

    return len;
}

IRAM_ATTR
static void _serial_intr_handler(void *arg)
{
    (void)arg;

    irq_isr_enter();

    /* read data if available */
    while (IS_USED(MODULE_STDIO_USB_SERIAL_JTAG_RX) &&
           usb_serial_jtag_ll_rxfifo_data_available()) {
        stdio_rx_write_one(USB_SERIAL_JTAG.ep1.rdwr_byte);
    }

    /* clear all interrupt flags */
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_LL_INTR_MASK);

    irq_isr_exit();
}

static void _init(void)
{
    /* enable RX interrupt */
    if (IS_USED(MODULE_STDIO_USB_SERIAL_JTAG_RX)) {
        USB_SERIAL_JTAG.int_ena.val = USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT;
    }

    /* clear all interrupt flags */
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_LL_INTR_MASK);

    /* route USB Serial/JTAG interrupt source to CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, ETS_USB_SERIAL_JTAG_INTR_SOURCE, CPU_INUM_SERIAL_JTAG);

    /* enable the CPU interrupt */
    esp_cpu_intr_set_handler(CPU_INUM_SERIAL_JTAG, _serial_intr_handler, NULL);
    esp_cpu_intr_enable(BIT(CPU_INUM_SERIAL_JTAG));

#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
    /* set interrupt level */
    esp_cpu_intr_set_priority(CPU_INUM_SERIAL_JTAG, 1);
#endif
}

static void _detach(void)
{
    esp_cpu_intr_disable(BIT(CPU_INUM_SERIAL_JTAG));
}

STDIO_PROVIDER(STDIO_ESP32_SERIAL_JTAG, _init, _detach, _write)
/**@}*/
