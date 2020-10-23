/*
 * Copyright (C) 2016 MUTEX NZ Ltd.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 *
 * @}
 */

#include "cpu.h"
#include "cc2538.h"
#include "cc2538_rf.h"
#include "cc2538_rf_netdev.h"
#include "cc2538_rf_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void isr_rfcoreerr(void)
{
    uint_fast8_t flags;

    /* Latch and clear the interrupt status */
    flags = RFCORE_SFR_RFERRF;
    RFCORE_SFR_RFERRF = 0;

    /* These conditions shouldn't happen if the driver is working properly */
    RFCORE_ASSERT(NOT(flags & RXUNDERF));
    RFCORE_ASSERT(NOT(flags & TXOVERF));
    RFCORE_ASSERT(NOT(flags & TXUNDERF));

    /* Fail in case of miscallaneous unexpected error conditions */
    RFCORE_ASSERT(NOT(flags & STROBE_ERR));

    DEBUG("%s(): RFERRF=0x%02x.\n", __FUNCTION__, flags);

    /* Flush the receive FIFO in case of a receive overflow */
    if (flags & RXOVERF) {
        DEBUG("%s(): RXOVERF detected!\n", __FUNCTION__);
        RFCORE_FLUSH_RECEIVE_FIFO();
    }

    /* Flush the receive FIFO in case of a receive overflow */
    if (flags & NLOCK) {
        /* The frequency synthesizer failed to achieve lock after time-out, or
         * lock is lost during reception. The receiver must be restarted to clear
         * this error situation. */
        DEBUG("%s(): NLOCK detected!\n", __FUNCTION__);
        RFCORE_FLUSH_RECEIVE_FIFO();
    }
}

void isr_rfcorerxtx(void)
{
    cc2538_irq_handler();

    cortexm_isr_end();
}

uint_fast8_t rfcore_read_byte(void)
{
    RFCORE_ASSERT(RFCORE_XREG_RXFIFOCNT > 0);
    return RFCORE_SFR_RFDATA;
}

uint_fast8_t rfcore_peek_rx_fifo(int idx)
{
    RFCORE_ASSERT(idx >= 0 && idx < CC2538_RF_FIFO_SIZE);
    return *(uint_fast8_t *)(CC2538_RX_FIFO_ADDR + 4 * idx);
}

void rfcore_read_fifo(void *buf, uint_fast8_t len)
{
    uint_fast8_t n;

    RFCORE_ASSERT(len <= RFCORE_XREG_RXFIFOCNT);

    for (n = 0; n < len; n++) {
        GET_BYTE(buf, n) = RFCORE_SFR_RFDATA;
    }
}

void rfcore_strobe(uint_fast8_t instr)
{
    RFCORE_SFR_RFST = instr;
}

void rfcore_write_byte(uint_fast8_t byte)
{
    RFCORE_ASSERT(RFCORE_XREG_TXFIFOCNT < CC2538_RF_FIFO_SIZE);
    RFCORE_SFR_RFDATA = byte;
}

void rfcore_poke_tx_fifo(int idx, uint_fast8_t byte)
{
    RFCORE_ASSERT(idx >= 0 && idx < CC2538_RF_FIFO_SIZE);
    *(uint_fast8_t *)(CC2538_TX_FIFO_ADDR + 4 * idx) = byte;
}

void rfcore_write_fifo(const void *buf, uint_fast8_t len)
{
    uint_fast8_t n;

    RFCORE_ASSERT(len <= CC2538_RF_FIFO_SIZE - RFCORE_XREG_TXFIFOCNT);

    for (n = 0; n < len; n++) {
        RFCORE_SFR_RFDATA = GET_BYTE(buf, n);
    }
}

bool RFCORE_ASSERT_failure(const char *expr, const char *func, int line)
{
#if (DEVELHELP || ENABLE_DEBUG)
    DEBUG_PRINT("RFCORE_ASSERT(%s) failed at line %u in %s()!\n", expr, line, func);
    DEBUG_PRINT("  RFCORE_SFR_RFERRF = 0x%02x\n", (unsigned int)RFCORE_SFR_RFERRF);
#else
    (void)expr;
    (void)func;
    (void)line;
#endif

    return false;
}
