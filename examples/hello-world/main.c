/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "periph/dma.h"
#include "xtimer.h"
#include "board.h"
#include "mutex.h"

static mutex_t m = MUTEX_INIT_LOCKED;
static uint8_t buff[1024] = "hello world 12345";
static uint8_t buff2[1024] = "bleblebleblebleblebleblebl";
static uint8_t buff3[1024] = "aaaaaaaaaaaaaaaaaaaaaaaaaa";
static dma_t dma = DMA_UNIT(3);

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    dma_poweron(dma);
    dma_isr_enable(dma);
    printf("ISR: 0x%X\n", (0x2 << (1 << 2)));
    puts("transfer");

    while(1) {
        dma_transfer(dma, MEMORY_TO_MEMORY, buff, buff3, sizeof(buff));
        mutex_lock(&m);
        printf("%s\n", buff3);
        dma_transfer(dma, MEMORY_TO_MEMORY, buff2, buff3, sizeof(buff));
        mutex_lock(&m);
        printf("%s\n\n", buff3);
        xtimer_usleep(1000000);
    }


    return 0;
}

void isr_dma1_ch2_3(void)
{
    LED0_TOGGLE;
    printf("ISR: 0x%X\n", (unsigned)DMA1->ISR);
    dma_end_isr(dma);
    mutex_unlock(&m);
}
