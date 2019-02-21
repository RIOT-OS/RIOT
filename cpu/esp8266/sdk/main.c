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
 * @brief       ESP8266 SDK libmain function
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef MODULE_ESP_SDK

#include <stdio.h>

#include "c_types.h"

#include "common.h"
#include "cpu_conf.h"
#include "irq.h"
#include "irq_arch.h"
#include "log.h"

#include "esp/rtcmem_regs.h"
#include "spi_flash.h"
#include "sdk/ets.h"
#include "sdk/main.h"
#include "sdk/rom.h"

extern char* _printf_buf;

int IRAM os_printf_plus (const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);

    int ret = vsnprintf(_printf_buf, PRINTF_BUFSIZ, format, arglist);

    if (ret > 0) {
        ets_printf (_printf_buf);
    }

    va_end(arglist);

    return ret;
}

SpiFlashOpResult IRAM spi_flash_read (uint32_t faddr, uint32_t *dst, size_t size)
{
    /*
     * For simplicity, we use the ROM function. Since we need to disable the
     * IROM cache function for that purpose, we have to be IRAM.
     * Please note, faddr, src and size have to be aligned to 4 byte.
     */

    SpiFlashOpResult ret;

    CHECK_PARAM_RET (dst != NULL, SPI_FLASH_RESULT_ERR);
    CHECK_PARAM_RET (faddr + size <= flashchip->chip_size, SPI_FLASH_RESULT_ERR);

    critical_enter ();
    Cache_Read_Disable ();

    ret = SPIRead (faddr, dst, size);

    Cache_Read_Enable(0, 0, 1);
    critical_exit ();

    return ret;
}

SpiFlashOpResult IRAM spi_flash_write (uint32_t faddr, uint32_t *src, size_t size)
{
    /*
     * For simplicity, we use the ROM function. Since we need to disable the
     * IROM cache function for that purpose, we have to be in IRAM.
     * Please note, faddr, src and size have to be aligned to 4 byte
     */

    SpiFlashOpResult ret;

    CHECK_PARAM_RET (src != NULL, SPI_FLASH_RESULT_ERR);
    CHECK_PARAM_RET (faddr + size <= flashchip->chip_size, SPI_FLASH_RESULT_ERR);

    critical_enter ();
    Cache_Read_Disable ();

    ret = SPIWrite (faddr, src, size);

    Cache_Read_Enable(0, 0, 1);
    critical_exit ();

    return ret;
}

SpiFlashOpResult IRAM spi_flash_erase_sector(uint16_t sec)
{
    CHECK_PARAM_RET (sec < flashchip->chip_size / flashchip->sector_size, SPI_FLASH_RESULT_ERR);

    critical_enter ();
    Cache_Read_Disable();

    SpiFlashOpResult ret = SPIEraseSector (sec);

    Cache_Read_Enable(0, 0, 1);
    critical_exit ();

    return ret;
}

void system_deep_sleep(uint32_t time_in_us)
{
    /* TODO implement */
    (void)time_in_us;
    NOT_YET_IMPLEMENTED();
}

void system_restart(void)
{
    /* TODO it's just a hard reset at the moment */
    __asm__ volatile (" call0 0x40000080 ");
}

extern bool system_update_cpu_freq(uint8 freq)
{
    if (freq == 160) {
        DPORT.CPU_CLOCK |= DPORT_CPU_CLOCK_X2;
        ets_update_cpu_frequency(160);
    }
    else {
        DPORT.CPU_CLOCK &= ~DPORT_CPU_CLOCK_X2;
        ets_update_cpu_frequency(80);
    }
    return true;
}

/**
 * Following code is completly or at least partially from
 * https://github.com/pvvx/esp8266web
 * (c) PV` 2015
 *
 * @{
 */

uint8_t ICACHE_FLASH_ATTR system_get_checksum(uint8_t *ptr, uint32_t len)
{
    uint8_t checksum = 0xEF;
    while (len--) {
        checksum ^= *ptr++;
    }
    return checksum;
}

#define RTCMEM_SIZE 0x300      /* user RTC RAM 768 bytes, 192 dword registers */

static bool IRAM _system_rtc_mem_access (uint32_t src, void *dst, uint32_t size, bool write)
{
    /* src hast to be smaller than 192 */
    CHECK_PARAM_RET (src <= (RTCMEM_SIZE >> 2), false);
    /* src times 4 plus size must less than RTCMEM_SIZE */
    CHECK_PARAM_RET (((src << 2) + size) < RTCMEM_SIZE, false);
    /* des_addr has to be a multiple of 4 */
    CHECK_PARAM_RET (((uint32_t)dst & 0x3) == 0, false);

    /* align size to next higher multiple of 4 */
    if (size & 0x3) {
        size = (size + 4) & ~0x3;
    }

    for (uint32_t i = 0; i < (size >> 2); i++) {
        if (write) {
            RTCMEM_SYSTEM[src + i] = ((uint32_t*)dst)[i];
        }
        else {
            ((uint32_t*)dst)[i] = RTCMEM_SYSTEM[src + i];
        }
    }

    return true;
}

bool IRAM system_rtc_mem_read (uint32_t src_addr, void *des_addr, uint32_t save_size)
{
    return _system_rtc_mem_access (src_addr, des_addr, save_size, false);
}

bool IRAM system_rtc_mem_write (uint32_t src_addr, void *des_addr, uint32_t save_size)
{
    return _system_rtc_mem_access (src_addr, des_addr, save_size, true);
}

/** @} */

#endif /* MODULE_ESP_SDK */
