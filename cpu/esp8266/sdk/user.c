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
 * @brief       ESP8266 user defined SDK functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "c_types.h"

#include "common.h"
#include "cpu_conf.h"

#include "esp/uart_regs.h"
#include "spi_flash.h"
#include "sdk/rom.h"
#include "sdk/user.h"

uint32_t __attribute__((weak)) user_rf_cal_sector_set(void)
{
    uint32_t sec_num = flashchip->chip_size / flashchip->sector_size;
    return sec_num - 5;
}

#ifndef MODULE_ESP_SDK

void uart_tx_flush (uint32_t num)
{
    while ((UART(num).STATUS >> UART_STATUS_TXFIFO_COUNT_S) & UART_STATUS_TXFIFO_COUNT_M) {}
}

/**
 * Following code is completly or at least partially from
 * https://github.com/pvvx/esp8266web
 * (c) PV` 2015
 *
 * @{
 */

void IRAM system_set_pll (uint8_t crystal_26m_en)
{
    if(rom_i2c_readReg(103,4,1) != 136) { /* 8: 40MHz, 136: 26MHz */
        if (crystal_26m_en == 1) { /* 0: 40MHz, 1: 26MHz */
            /* set 80MHz PLL CPU */
            rom_i2c_writeReg(103,4,1,136);
            rom_i2c_writeReg(103,4,2,145);
        }
    }
}

/** @} */

#endif
