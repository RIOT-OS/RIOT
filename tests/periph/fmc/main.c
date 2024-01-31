/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Test for memories connected to the STM32 FMC/FSMC peripheral
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdio.h>
#include <string.h>

#include "benchmark.h"
#include "od.h"
#include "periph_conf.h"

#ifndef FMC_BANK
#define FMC_BANK    0
#endif

#ifndef MEM_BUFSIZ
#define MEM_BUFSIZ  1024
#endif

uint8_t mem_buf[MEM_BUFSIZ] = {};

void bench_mem_write(uint32_t addr, uint32_t len)
{
    uint8_t *mem = (uint8_t *)addr;
    uint32_t i = 0;

    while (i < len) {
        memcpy(mem + i, mem_buf, MEM_BUFSIZ);
        i += MEM_BUFSIZ;
    }
}

int main(void)
{
    printf("FMC HCLK freq %lu MHz\n", CLOCK_AHB/MHZ(1));

    uint8_t *data8  = (uint8_t *)(fmc_bank_config[FMC_BANK].address);
    uint16_t *data16 = (uint16_t *)((fmc_bank_config[FMC_BANK].address) + 256);
    uint32_t *data32 = (uint32_t *)((fmc_bank_config[FMC_BANK].address) +
                                    (fmc_bank_config[FMC_BANK].size) - 256);

    printf("8-bit data @%p, 16-bit data @%p, 32-bit data @%p\n",
           data8, data16, data32);
    puts("------------------------------------------------------------------------");

    for (unsigned i = 0; i < 256; i++) {
        data8[i] = i;
    }
    od_hex_dump_ext(data8, 256, 16, fmc_bank_config[FMC_BANK].address);

    for (unsigned i = 0; i < 256; i++) {
        if (data8[i] != i) {
            printf("ERROR: memory content did not match @%p\n", &data8[i]);
            return 1;
        }
    }
    puts("------------------------------------------------------------------------");

    for (unsigned i = 0; i < 128; i++) {
        data16[i] = ((128 + i) << 8) + i;
    }
    od_hex_dump_ext(data16, 256, 16, fmc_bank_config[FMC_BANK].address + 256);

    for (unsigned i = 0; i < 128; i++) {
        if (data16[i] != ((128 + i) << 8) + i) {
            printf("ERROR: memory content did not match @%p\n", &data16[i]);
            return 1;
        }
    }
    puts("------------------------------------------------------------------------");

    for (unsigned i = 0; i < 64; i++) {
        data32[i] = ((192 + i) << 24) + ((128 + i) << 16) + ((64 + i) << 8) + i;
    }
    od_hex_dump_ext(data32, 256, 16, (fmc_bank_config[FMC_BANK].address) +
                                     (fmc_bank_config[FMC_BANK].size) - 256);

    for (unsigned i = 0; i < 64; i++) {
        if (data32[i] != ((192 + i) << 24) + ((128 + i) << 16) + ((64 + i) << 8) + i) {
            printf("ERROR: memory content did not match @%p\n", &data32[i]);
            return 1;
        }
    }
    puts("------------------------------------------------------------------------");

    printf("fill complete memory of %lu kByte, a . represents a 16 kByte block\n",
           fmc_bank_config[FMC_BANK].size >> 10);

    data32 = (uint32_t *)(fmc_bank_config[FMC_BANK].address);

    for (uint32_t i = 0; i < (fmc_bank_config[FMC_BANK].size >> 2); i++) {
        *data32 = (uint32_t)data32;
        data32++;
        if (((i << 2) % KiB(16)) == 0) {
            printf(".");
        }
    }
    puts("\nready");
    puts("check memory content, a + represents one 16 kByte block of matching data");

    data32 = (uint32_t *)(fmc_bank_config[FMC_BANK].address);

    for (uint32_t i = 0; i < (fmc_bank_config[FMC_BANK].size >> 2); i++) {
        if (*data32 != (uint32_t)data32) {
            printf("ERROR: memory content did not match @%p, "
                   "should be %p but was 0x%08"PRIx32"\n",
                   data32, data32, *data32);
            return 1;
        }
        data32++;
        if (((i << 2) % KiB(16)) == 0) {
            printf("+");
        }
    }
    puts("\nready");
    puts("------------------------------------------------------------------------");

    puts("Doing some benchmarks\n");

    volatile uint8_t val8 = 0xf0;
    volatile uint16_t val16 = 0x5555;
    volatile uint32_t val32 = 0xaaaaaaaa;

    BENCHMARK_FUNC("write 8 bit", fmc_bank_config[FMC_BANK].size,
                   ((uint8_t *)(fmc_bank_config[FMC_BANK].address))[i] = val8);
    BENCHMARK_FUNC("write 16 bit", fmc_bank_config[FMC_BANK].size / 2,
                   ((uint16_t *)(fmc_bank_config[FMC_BANK].address))[i] = val16);
    BENCHMARK_FUNC("write 32 bit", fmc_bank_config[FMC_BANK].size / 4,
                   ((uint32_t *)(fmc_bank_config[FMC_BANK].address))[i] = val32);

    BENCHMARK_FUNC("read 8 bit", fmc_bank_config[FMC_BANK].size,
                   val8 = ((uint8_t *)(fmc_bank_config[FMC_BANK].address))[i]);
    BENCHMARK_FUNC("read 16 bit", fmc_bank_config[FMC_BANK].size / 2,
                   val16 = ((uint16_t *)(fmc_bank_config[FMC_BANK].address))[i]);
    BENCHMARK_FUNC("read 32 bit", fmc_bank_config[FMC_BANK].size / 4,
                   val32 = ((uint32_t *)(fmc_bank_config[FMC_BANK].address))[i]);

    puts("\nready");
    puts("------------------------------------------------------------------------");

    puts("print first and last memory block after benchmark, should be 0xaa\n");
    od_hex_dump_ext(data8, 256, 16, fmc_bank_config[FMC_BANK].address);
    puts("");

    data8 = (uint8_t *)((fmc_bank_config[FMC_BANK].address) +
                        (fmc_bank_config[FMC_BANK].size) - 256);
    od_hex_dump_ext(data8, 256, 16, (fmc_bank_config[FMC_BANK].address) +
                                    (fmc_bank_config[FMC_BANK].size) - 256);
    puts("------------------------------------------------------------------------");

    puts("\n[SUCCESS]\n");

    return 0;
}
