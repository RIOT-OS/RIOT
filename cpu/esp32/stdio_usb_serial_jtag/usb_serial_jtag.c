/*
 * Copyright (C) 2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "stdio_base.h"

#include "irq_arch.h"
#include "esp_attr.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "hal/usb_serial_jtag_ll.h"
#include "soc/periph_defs.h"
#include "rom/ets_sys.h"

static tsrb_t serial_tx_rb;
static uint8_t serial_tx_rb_buf[USB_SERIAL_JTAG_PACKET_SZ_BYTES];

#define IRQ_MASK (USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY | \
                 (IS_USED(MODULE_STDIO_USB_SERIAL_JTAG_RX) * USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT))

static ssize_t _write(const void *buffer, size_t len)
{
    tsrb_add(&serial_tx_rb, buffer, len);
    USB_SERIAL_JTAG.int_ena.val = IRQ_MASK;

    return len;
}

IRAM_ATTR
static void _serial_intr_handler(void *arg)
{
    (void)arg;

    irq_isr_enter();

    uint32_t mask = usb_serial_jtag_ll_get_intsts_mask();

    /* read data if available */
    while (IS_USED(MODULE_STDIO_USB_SERIAL_JTAG_RX) &&
           usb_serial_jtag_ll_rxfifo_data_available()) {
        isrpipe_write_one(&stdin_isrpipe, USB_SERIAL_JTAG.ep1.rdwr_byte);
    }

    /* write data if there is a free stop */
    if (mask & USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY) {

        while (usb_serial_jtag_ll_txfifo_writable()) {
            int c = tsrb_get_one(&serial_tx_rb);
            if (c < 0) {
                /* no more data to send - disable interrupt */
                USB_SERIAL_JTAG.int_ena.val = IRQ_MASK & ~USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY;
                break;
            }
            USB_SERIAL_JTAG.ep1.rdwr_byte = c;
        }
    }

    /* clear all interrupt flags */
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_LL_INTR_MASK);
    usb_serial_jtag_ll_txfifo_flush();

    irq_isr_exit();
}

static void _init(void)
{
    tsrb_init(&serial_tx_rb, serial_tx_rb_buf, sizeof(serial_tx_rb_buf));

    /* enable RX interrupt */
    if (IS_USED(MODULE_STDIO_USB_SERIAL_JTAG_RX)) {
        USB_SERIAL_JTAG.int_ena.val = USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT;
    }

    /* clear all interrupt flags */
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_LL_INTR_MASK);

    /* route USB Serial/JTAG interrupt source to CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, ETS_USB_SERIAL_JTAG_INTR_SOURCE, CPU_INUM_SERIAL_JTAG);

    /* enable the CPU interrupt */
    intr_cntrl_ll_set_int_handler(CPU_INUM_SERIAL_JTAG, _serial_intr_handler, NULL);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_SERIAL_JTAG));

#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
    /* set interrupt level */
    intr_cntrl_ll_set_int_level(CPU_INUM_SERIAL_JTAG, 1);
#endif
}

static void _detach(void)
{
    intr_cntrl_ll_disable_interrupts(BIT(CPU_INUM_SERIAL_JTAG));
}

STDIO_PROVIDER(STDIO_ESP32_SERIAL_JTAG, _init, _detach, _write)
/**@}*/
