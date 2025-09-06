/*
 * SPDX-FileCopyrightText: 2014 Loci Controls Inc.
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       HWRNG peripheral driver interface implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "vendor/hw_memmap.h"
#include "vendor/hw_soc_adc.h"

#include "cpu.h"
#include "cc2538_rfcore.h"
#include "periph/hwrng.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static cc2538_soc_adc_t *soc_adc = (cc2538_soc_adc_t *)SOC_ADC_BASE;

void hwrng_init(void)
{
    uint16_t seed = 0;
    int i;

    /* Make sure the RNG is on */
    uint32_t reg32 = soc_adc->ADCCON1 & ~(SOC_ADC_ADCCON1_RCTRL_M);
    soc_adc->ADCCON1 = reg32;

    /* Enable clock for the RF Core */
    SYS_CTRL_RCGCRFC = 1;

    /* Wait for the clock ungating to take effect */
    while (SYS_CTRL_RCGCRFC != 1) {}

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
    soc_adc->RNDH = (seed >> 8) & 0xff;
    soc_adc->RNDL = seed & 0xff;

    /* Turn RF off: */
    RFCORE_SFR_RFST = ISRFOFF;
}

void hwrng_read(void *buf, unsigned int num)
{
    uint8_t *b = (uint8_t *)buf;

    for (unsigned count = 0; count < num; count++) {
        /* Clock the RNG LSFR once: */
        soc_adc->ADCCON1 = soc_adc->ADCCON1 | (1UL << SOC_ADC_ADCCON1_RCTRL_S);
        /* Read up to 2 bytes of hwrng data: */
        b[count] = soc_adc->RNDL;
        count++;
        if (count < num) {
            b[count] = soc_adc->RNDH;
        }
    }
}
