/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
// Include all headers to check for compiler warnings
#include "hardware/adc.h"
#include "hardware/claim.h"
#include "hardware/clocks.h"
#include "hardware/divider.h"
#include "hardware/dma.h"
#include "hardware/exception.h"
#include "hardware/flash.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/interp.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "hardware/resets.h"
#include "hardware/rtc.h"
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"
#include "hardware/xosc.h"
#include "pico/binary_info.h"
#include "pico/bit_ops.h"
#include "pico/bootrom.h"
#include "pico/divider.h"
#include "pico/double.h"
#include "pico/fix/rp2040_usb_device_enumeration.h"
#include "pico/float.h"
#include "pico/int64_ops.h"
#include "pico/malloc.h"
#include "pico/multicore.h"
#include "pico/printf.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "pico/time.h"
#include "pico/unique_id.h"

bi_decl(bi_block_device(
                           BINARY_INFO_MAKE_TAG('K', 'S'),
                           "foo",
                           0x80000,
                           0x40000,
                           NULL,
                           BINARY_INFO_BLOCK_DEV_FLAG_READ | BINARY_INFO_BLOCK_DEV_FLAG_WRITE |
                                   BINARY_INFO_BLOCK_DEV_FLAG_PT_UNKNOWN));

//#pragma GCC push_options
//#pragma GCC optimize ("O3")

uint32_t *foo = (uint32_t *) 200;

uint32_t dma_to = 0;
uint32_t dma_from = 0xaaaa5555;

void spiggle(void) {
    dma_channel_config c = dma_channel_get_default_config(1);
    channel_config_set_bswap(&c, true);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_ring(&c, true, 13);
    dma_channel_set_config(1, &c, false);
    dma_channel_transfer_from_buffer_now(1, foo, 23);
}

void __isr dma_handler_a(void) {
    printf("HELLO A\n");
    if (dma_hw->ints1 & 1) {
        dma_hw->ints1 = 1;
        printf("A WINS DMA_TO %08x\n", (uint) dma_to);
        irq_remove_handler(DMA_IRQ_1, dma_handler_a);
    }
}

void __isr dma_handler_b(void) {
    printf("HELLO B\n");
    if (dma_hw->ints1 & 1) {
        dma_hw->ints1 = 1;
        printf("B WINS DNA_TO %08x\n", (uint) dma_to);
//        irq_remove_handler(DMA_IRQ_1, dma_handler_b);
    }
}

//#pragma GCC pop_options

int main(void) {
    spiggle();

    stdio_init_all();

    printf("HI %d\n", (int)time_us_32());
    puts("Hello Everything!");
    puts("Hello Everything2!");

    irq_add_shared_handler(DMA_IRQ_1, dma_handler_a, 0x80);
    irq_add_shared_handler(DMA_IRQ_1, dma_handler_b, 0xC0);

    dma_channel_config config = dma_channel_get_default_config(0);
//    set_exclusive_irq_handler(DMA_IRQ_1, dma_handler_a);
    dma_channel_set_irq1_enabled(0, true);
    irq_set_enabled(DMA_IRQ_1, true);
    dma_channel_configure(0, &config, &dma_to, &dma_from, 1, true);
    dma_channel_set_config(0, &config, false);

    // note this loop expects to cause a breakpoint!!
    for (int i = 0; i < 20; i++) {
        puts("sleepy");
        sleep_ms(1000);
        dma_channel_configure(0, &config, &dma_to, &dma_from, 1, true);
        if (i==3) {
            irq_remove_handler(DMA_IRQ_1, dma_handler_a);
        }
        if (i==2) {
            irq_remove_handler(DMA_IRQ_1, dma_handler_b);
        }
    }
    // this should compile as we are Cortex M0+
    __asm volatile("SVC #3");
}
