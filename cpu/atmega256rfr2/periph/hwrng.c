/*
 * Copyright (C) 2017 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "cpu.h"

#include "periph_conf.h"
#include "periph/hwrng.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void transition(void){
    /* Make sure transceiver is finished with transition */
    while( (TRX_STATUS&0x1f)==0x1f){}
}

void hwrng_init(void)
{
    /* Nothing to do here. */
}

/*
 * Random Number Generator
 *
 * The radio transceiver incorporates a 2-bit, noise observing, true random number
 * generator to be used to:
 *      Generate random seeds for CSMA-CA algorithm
 *      (see"Extended Operating Mode" on page 47)
 *
 *      Generate random values for AES key generation
 *      (see "Security Module (AES)" on page 99);
 *
 * The values are stored in bits RND_VALUE of register PHY_RSSI. The random number
 * is updated every tTR29 = 1 μs in Basic Operation Mode receive states with locked PLL.
 *
 * Note, if the PLL is not locked or unlocks in receive states or either antenna
 * diversity or RPC mode is enabled, the RND_VALUE is zero.
 */
void hwrng_read(void *buf, unsigned int num)
{
    DEBUG("Hardware random bytes to get: %d\n", num);

    /* Make sure transceiver is not in transition */
    transition();

    uint8_t prevRxSync = RX_SYN;
    uint8_t prevTrxRpc = TRX_RPC;
    uint8_t prevState = TRX_STATE;
    uint8_t prevTrxPr = TRXPR;
    uint8_t prevAntDiv = ANT_DIV;

    xtimer_ticks32_t wait= xtimer_ticks_from_usec(1);

    /* Manual p. 119 RX_PDT_DIS (Register RX_SYN) is set to 0 */
    RX_SYN &= ~(1 << RX_PDT_DIS);

    /* Manual p. 105 RPC/SRT has to be disabled. */
    TRX_RPC &= ~(1<<RX_RPC_CTRL1 | 1<<RX_RPC_CTRL0);
    /* randome ready time 2*1us + 24us (pll Ready) */

    /* Disable antenna diversity */
    ANT_DIV &= ~(1<<ANT_DIV_EN);

    /* Make sure transceiver is not in sleep */
    if (TRXPR & (1<<SLPTR)) {
        /* wake up the transceiver */
        TRXPR &= ~(1<<SLPTR);
    }

    transition();

    /* Manual p. 119 radio transceiver has to be in Basic Operating Mode receive state*/
    if ( (TRX_STATUS&0x1f)!= 0x06) {
        /* can not be forced to PLL_ON from TRX_OFF */
        if( (TRX_STATUS&0x1f)!= 0x08 ){
            /* Set to PLL_ON */
            TRX_STATE = 0x09;
        }else{
            /* Force to PLL_ON */
            TRX_STATE = 0x04;
        }

        transition();

        /* Set to RX_ON */
        TRX_STATE = 0x06;

        transition();
    }

    for (size_t byteCount = 0; byteCount < num; ++byteCount) {
        uint8_t rnd = 0;
        for (uint8_t i = 0; i < 4; ++i) {
            /* Random value update period 1us p. 47*/
            xtimer_spin(wait);

            /* bit 5 and 6 of the AT86RF2XX_REG__PHY_RSSI register contain the RND_VALUE */
            uint8_t regVal = PHY_RSSI & 0x60;
            /* shift the two random bits first to the right and then to the
             * correct position of the return byte */
            regVal = regVal >> 5;
            regVal = regVal << 2 * i;
            rnd |= regVal;
        }
        ((uint8_t *)buf)[byteCount] = rnd;
    }

    /* Set back to previous state */
    RX_SYN  = prevRxSync;
    TRX_RPC = prevTrxRpc;
    ANT_DIV = prevAntDiv;

    /* Make sure transceiver is back in sleep if it was, it is in RX_ON,
     * else set to previous state*/
    if (prevTrxPr & (1 << SLPTR)) {
        if ( (TRX_STATUS&0x1f) != 0x08) {
            TRX_STATE = 0x08;
        }

        transition();

        /* set to sleep */
        TRXPR |= (1 << SLPTR);
    }
    else {
        /* Only from PLL_ON all states can be reached */
        /* Force to PLL_ON */
        TRX_STATE = 0x04;

        transition();

        TRX_STATE = prevState;
    }
}
