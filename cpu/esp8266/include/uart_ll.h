/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Low-level UART driver for source code compatibility with ESP-IDF
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef DOXYGEN

#include "esp8266/uart_struct.h"
#include "esp8266/uart_register.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_SCLK_DEFAULT       1

#define UART_LL_FIFO_DEF_LEN    (128)
#define UART_LL_INTR_MASK       ((uint32_t)~0)

typedef uart_data_bits_t uart_word_length_t;
typedef unsigned int soc_module_clk_t;

static inline void uart_ll_set_sclk(uart_dev_t *hw, soc_module_clk_t source)
{
    /* dummy function for source code compatibility with ESP32 */
    (void)hw;
    (void)source;
}

static inline void uart_ll_set_baudrate(uart_dev_t *hw, uint32_t baud, uint32_t sclk_freq)
{
    hw->clk_div.val = (sclk_freq / baud) & 0xFFFFF;
}

static inline void uart_ll_set_stop_bits(uart_dev_t *hw, uart_stop_bits_t bits)
{
    hw->conf0.stop_bit_num = bits;
}

static inline void uart_ll_set_data_bit_num(uart_dev_t *hw, uart_word_length_t data_bit)
{
    hw->conf0.bit_num = data_bit;
}

static inline void uart_ll_set_parity(uart_dev_t *hw, uart_parity_t parity_mode)
{
    hw->conf0.parity = (parity_mode & 0x1);
    hw->conf0.parity_en = ((parity_mode >> 1) & 0x1);
}

static inline uint32_t uart_ll_get_rxfifo_len(uart_dev_t *hw)
{
    return hw->status.rxfifo_cnt;
}

static inline uint32_t uart_ll_get_txfifo_len(uart_dev_t *hw)
{
    return UART_LL_FIFO_DEF_LEN - hw->status.txfifo_cnt;
}

static inline void uart_ll_read_rxfifo(uart_dev_t *hw, uint8_t *buf, uint32_t rd_len)
{
    for (int i = 0; i < (int)rd_len; i++) {
        buf[i] = hw->fifo.rw_byte;
    }
}

static inline void uart_ll_write_txfifo(uart_dev_t *hw, const uint8_t *buf, uint32_t wr_len)
{
    for (int i = 0; i < (int)wr_len; i++) {
        hw->fifo.rw_byte = (int)buf[i];
    }
}

static inline void uart_ll_rxfifo_rst(uart_dev_t *hw)
{
    hw->conf0.rxfifo_rst = 1;
    hw->conf0.rxfifo_rst = 0;
}

static inline void uart_ll_txfifo_rst(uart_dev_t *hw)
{
    hw->conf0.rxfifo_rst = 1;
    hw->conf0.rxfifo_rst = 0;
}

static inline void uart_ll_set_rxfifo_full_thr(uart_dev_t *hw, uint16_t full_thrhd)
{
    hw->conf1.rxfifo_full_thrhd = full_thrhd;
}

static inline uint32_t uart_ll_get_intsts_mask(uart_dev_t *hw)
{
    return hw->int_st.val;
}

static inline void uart_ll_ena_intr_mask(uart_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val = hw->int_ena.val | mask;
}

static inline void uart_ll_clr_intsts_mask(uart_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

static inline uint32_t uart_ll_get_intr_ena_status(uart_dev_t *hw)
{
    return hw->int_ena.val;
}

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
