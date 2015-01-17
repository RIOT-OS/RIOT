/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        random.c
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/random.h"

/* only compile this driver if enabled in the board's periph_conf.h */
#if RANDOM_NUMOF

void random_poweron(void)
{
    uint16_t seed = 0;
    int i;

    /* Make sure the RNG is on */
    SOC_ADC->ADCCON1bits.RCTRL = 0;

    /* Enable clock for the RF Core */
    SYS_CTRL_RCGCRFC = 1;

    /* Wait for the clock ungating to take effect */
    while (SYS_CTRL_RCGCRFC != 1);

    /* Infinite RX - FRMCTRL0[3:2] = 10. This will mess with radio operation */
    RFCORE_XREG_FRMCTRL0 = 0x00000008;

    /* Turn RF on */
    RFCORE_SFR_RFST = ISRXON;

    /*
     * Wait until "the chip has been in RX long enough for the transients to
     * have died out. A convenient way to do this is to wait for the RSSI-valid
     * signal to go high."
     */
    while (!RFCORE->XREG_RSSISTATbits.RSSI_VALID);

    /*
     * Form the seed by concatenating bits from IF_ADC in the RF receive path.
     * Keep sampling until we have read at least 16 bits AND the seed is valid
     *
     * Invalid seeds are 0 and 0x8003 and should not be used.
     */
    for (i = 0; (i < 8) || (seed == 0) || (seed == 0x8003); i++) {
        seed <<= 2;
        seed ^= RFCORE->XREG_RFRND;
    }

    /* Seed the high byte first: */
    SOC_ADC_RNDL = (seed >> 8) & 0xff;
    SOC_ADC_RNDL = seed & 0xff;

    /* Turn RF off: */
    RFCORE_SFR_RFST = ISRFOFF;
}

int random_read(char *buf, unsigned int num)
{
    int count;

    for (count = 0; count < num; ) {
        /* Clock the RNG LSFR once: */
        SOC_ADC->ADCCON1bits.RCTRL = 1;

        /* Read up to 2 bytes of random data: */
        buf[count++] = SOC_ADC_RNDL;
        if (count >= num) break;
        buf[count++] = SOC_ADC_RNDH;
    }

    return count;
}

void random_poweroff(void)
{
}

#endif /* RANDOM_NUMOF */
