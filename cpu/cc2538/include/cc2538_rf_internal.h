/*
 * Copyright (C) 2016 MUTEX NZ Ltd.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Internal interfaces for the cc2538_rf driver
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 */

#ifndef CC2538_RF_INTERNAL_H
#define CC2538_RF_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define CC2538_RX_FIFO_ADDR         0x40088000
#define CC2538_TX_FIFO_ADDR         0x40088200

/**
 * @brief   Read a single byte from the RX FIFO
 *
 * This function will return the first currently unread byte
 * from the RX FIFO, and advance the FIFO pointer by one. Hence,
 * subsequent calls to this function will return subsequent bytes
 * from the RX FIFO.
 *
 * @return  The first currently unread byte from the RX FIFO
 */
uint_fast8_t rfcore_read_byte(void);

/**
 * @brief   Peek a single byte from the RX FIFO
 *
 * Peeking, as opposed to reading, a byte from the RX FIFO
 * will not advance the FIFO pointer. Further, bytes may be
 * read from any position in the FIFO by providing an index.
 *
 * @param[in]  idx  The index of the byte to peek
 *
 * @return          The byte at index idx
 */
uint_fast8_t rfcore_peek_rx_fifo(int idx);

/**
 * @brief   Read the remaining unread data from the RX FIFO
 *
 * @param[out] buf          The buffer to read the data into
 * @param[in]  len          The maximum length of the buffer
 */
void rfcore_read_fifo(void *buf, uint_fast8_t len);

/**
 * @brief   Issue a command strobe from the CPU to the radio
 *
 * @param[in]  instr          The instruction to issue
 */
void rfcore_strobe(uint_fast8_t instr);

/**
 * @brief   Write a single byte to the next index of the TX FIFO
 *
 * @param[in]  byte          The byte to write
 */
void rfcore_write_byte(uint_fast8_t byte);

/**
 * @brief   Poke a single byte in the TX FIFO
 *
 * Poking, as opposed to writing, a byte to the TX FIFO
 * will not advance the FIFO pointer. Further, bytes may be
 * written to any position in the FIFO by providing an index.
 *
 * @param[in]  idx        The index of the byte to write to
 * @param[in]  byte       The byte to write
 */
void rfcore_poke_tx_fifo(int idx, uint_fast8_t byte);

/**
 * @brief   Write a string of bytes to the TX FIFO
 *
 * @param[in] buf          The buffer containing the data to write
 * @param[in] len          The length of the data to write
 */
void rfcore_write_fifo(const void *buf, uint_fast8_t len);

bool RFCORE_ASSERT_failure(const char *expr, const char *func, int line);

#ifdef __cplusplus
}
#endif

#endif /* CC2538_RF_INTERNAL_H */
/** @} */
