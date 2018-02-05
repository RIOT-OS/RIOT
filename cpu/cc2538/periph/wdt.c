/*
 * Copyright (C) 2017 Technische Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
 *
 * @}
 */

#include "irq.h"
#include "cpu.h"
#include "cc2538.h"
#include "periph/wdt.h"

#define CC2538_WDT_CLK 32768

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
 * Own measurements showed exactly half that times. In consequence, the counter
 * values have been halved too:
 * 32    -> 0.95 ms
 * 256   -> 7.8 ms
 * 4096  -> 125 ms
 * 16384 -> 500 ms
 *
 * [1] http://www.ti.com/lit/ug/swru319c/swru319c.pdf
 */
const uint16_t clk_counter_values[] = { 32, 256, 4096, 16384 };

#define NUM_CNT_VALUES (int8_t)(sizeof(clk_counter_values) / sizeof(clk_counter_values[0]))

static uint16_t cc2538_wdt_usec_to_clk_cnt(uint32_t t_wdt) {
    uint32_t clk_cnt;
    if (t_wdt > (1000 * 1000)) {
        /* max. WDT interval of 1s */
        t_wdt = 1000 * 1000;
    }

    /* scale down to stay in valid value range for next calculation
     * (still below min. resolution)
     */
    t_wdt /= 10;

    /* calculate the required clock count */
    clk_cnt = (t_wdt * CC2538_WDT_CLK) / (100 * 1000);

    /* ensure minimal clock count */
    if (clk_cnt == 0) {
        clk_cnt = 1;
    }

    return clk_cnt;
}

int wdt_init(uint32_t t_wdt, wdt_timing_t timing) {
    uint32_t clk_cnt;
    uint16_t sel_clk_cnt = 0;
    uint8_t sel_clk_cnt_idx = 0;
    int8_t i;

    /* check, if WDT is already enabled */
    if (WDT->WDTbits.EN == 1) {
        return WDT_ERR;
    }

    /* get required clock count */
    clk_cnt = cc2538_wdt_usec_to_clk_cnt(t_wdt);

    /* searching the best matching timing according to selected timing mode */
    switch(timing){
    case WDT_EXACT:
        for (i = 0; i < NUM_CNT_VALUES; i++) {
            if(clk_counter_values[i] == clk_cnt){
                sel_clk_cnt_idx = i;
                sel_clk_cnt = clk_counter_values[sel_clk_cnt_idx];
                break;
            }
        }
        if(sel_clk_cnt == 0) {
            /* no exact match found -> error */
            return WDT_ERR;
        }
        break;
    case WDT_MIN:
        for (i = 0; i < NUM_CNT_VALUES; i++) {

            sel_clk_cnt_idx = i;
            sel_clk_cnt = clk_counter_values[sel_clk_cnt_idx];
            if (sel_clk_cnt >= clk_cnt) {
                break;
            }
        }
        break;
    case WDT_MAX:
        for (i = NUM_CNT_VALUES-1;
            i >= 0; i--) {

            sel_clk_cnt_idx = i;
            sel_clk_cnt = clk_counter_values[sel_clk_cnt_idx];
            if (sel_clk_cnt <= clk_cnt) {
                break;
            }
        }
        break;
    }

    /* invert array index and write into register */
    WDT->WDTbits.INT = 3 - sel_clk_cnt_idx;

    /* calculate return value based on selected timing */
    t_wdt = sel_clk_cnt * (1000 * 10) / CC2538_WDT_CLK;
    t_wdt *= 100;

    return t_wdt;
}

int wdt_init_cb(uint32_t t_wdt, wdt_timing_t timing, wdt_cb_t wdt_cb, void* arg) {
    /* cc2538 does not support WDT IRQ */
    (void) t_wdt;
    (void) timing;
    (void) wdt_cb;
    (void) arg;

    return WDT_ERR_NO_IRQ;
}

int wdt_enable(void) {
    WDT->WDTbits.EN = 1;
    return WDT_OK;
}

int wdt_disable(void) {
    /* cc2538 doesn't allow disabling the WDT */
    return WDT_ERR;
}

int wdt_is_enabled(void) {
    /* writing WDT enable flag into register */
    return WDT->WDTbits.EN;
}

void wdt_reset(void) {
    unsigned irq_state;

    /* disabling IRQs */
    irq_state = irq_disable();

    /* writing WDT clear sequence */
    WDT->WDTbits.CLR = 0xa;
    WDT->WDTbits.CLR = 0x5;

    /* restoring IRQ state */
    irq_restore(irq_state);
}
