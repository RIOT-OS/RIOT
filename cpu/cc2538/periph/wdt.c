/*
 * SPDX-FileCopyrightText: 2017 Technische Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file
 * @brief       WDT peripheral driver implementation for the cc2538
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <assert.h>

#include "cc2538.h"
#include "periph/wdt.h"

#define CC2538_WDT_CLK 32768
#define WTD_MS(cycles) (((cycles) * 1000 + CC2538_WDT_CLK/2) / CC2538_WDT_CLK)

typedef union {
    cc2538_reg_t WDT;
    struct {
        cc2538_reg_t INT :2;         /**< timer interval */
        cc2538_reg_t RESERVED :1;    /**< reserved */
        cc2538_reg_t EN :1;          /**< enable */
        cc2538_reg_t CLR :4;         /**< clear */
        cc2538_reg_t RESERVED_2 :24; /**< reserved */
    } WDTbits;
} cc2538_wdt_t;

cc2538_wdt_t * const WDT = (cc2538_wdt_t *) &SMWDTHROSC_WDCTL;

/*
 * supported clock counter values by the cc2538
 *
 * The array index corresponds to the value in the INT field of the WDT register.
 * According to the "CC2538 User's Guide" [1, pp. 370], the counter values and
 * corresponding timings are:
 * 64    -> 1.9 ms
 * 512   -> 15.6 ms
 * 8192  -> 250 ms
 * 32768 -> 1000 ms
 *
 * [1] http://www.ti.com/lit/ug/swru319c/swru319c.pdf
 */
const uint16_t clk_values_ms[] = { WTD_MS(32768), WTD_MS(8192), WTD_MS(512), WTD_MS(64) };

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time) {

    /* window mode is not supported */
    (void) min_time;
    assert(min_time == 0);

    /* check, if WDT is already enabled */
    assert(WDT->WDTbits.EN != 1);

    /* ensure valid range */
    if (max_time < NWDT_TIME_LOWER_LIMIT) {
        max_time = NWDT_TIME_LOWER_LIMIT;
    }

    /* get required clock count */
    unsigned i;
    for (i = 0; i < ARRAY_SIZE(clk_values_ms); ++i) {
        if (max_time >= clk_values_ms[i]) {
            break;
        }
    }

    /* select the interval */
    WDT->WDTbits.INT = i;
}

void wdt_start(void) {
    WDT->WDTbits.EN = 1;
}

void wdt_kick(void) {
    /* writing WDT clear sequence */
    WDT->WDTbits.CLR = 0xa;
    WDT->WDTbits.CLR = 0x5;
}
