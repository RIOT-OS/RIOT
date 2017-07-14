/*
 * wdt.c
 *
 *  Created on: 12.07.2017
 *      Author: geith
 */

#include "irq.h"
#include "cc2538.h"
#include "periph/wdt.h"

#define CC2538_WDT_CLK 32768

#ifdef __cplusplus
extern "C" {
#endif

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

// counter values supported by the cc2538
//const uint16_t counter_values[] = {64, 512, 8192, 32768 }; // values according to cc2538 user manual
const uint16_t counter_values[] = { 32, 256, 4096, 16384 }; // values according to measurements on Zolertia Firefly
#define NUM_CNT_VALUES (sizeof(counter_values) / sizeof(counter_values[0]))

static uint16_t cc2538_wdt_usec_to_cnt(uint32_t t_wdt) {
    uint32_t cnt;
    if (t_wdt > (1000 * 1000)) {
        /* max. WDT interval of 1s */
        t_wdt = 1000 * 1000;
    }

    /* scale down (still below min. resolution) */
    t_wdt /= 10;

    cnt = (t_wdt * CC2538_WDT_CLK) / (100 * 1000);

    if (cnt == 0) {
        cnt = 1;
    }

    return cnt;
}

int wdt_init(uint32_t t_wdt, wdt_mode_t mode) {
    uint32_t cnt;
    uint16_t sel_cnt = 0;
    uint8_t sel_cnt_idx = 0;
    uint8_t i;

    if (WDT->WDTbits.EN == 1) {
        return -1;
    }

    cnt = cc2538_wdt_usec_to_cnt(t_wdt);

    switch(mode){
    case WDT_EXACT:
        for (i = 0; i < NUM_CNT_VALUES; i++) {
            if(counter_values[i] == cnt){
                sel_cnt_idx = i;
                sel_cnt = counter_values[sel_cnt_idx];
                break;
            }
        }
        if(sel_cnt == 0) {
            /* no exact match found -> error */
            return -1;
        }
        break;
    case WDT_MIN:
        for (i = 0; i < NUM_CNT_VALUES; i++) {

            sel_cnt_idx = i;
            sel_cnt = counter_values[sel_cnt_idx];
            if (sel_cnt >= cnt) {
                break;
            }
        }
        break;
    case WDT_MAX:
        for (i = NUM_CNT_VALUES-1;
            i >= 0; i--) {

            sel_cnt_idx = i;
            sel_cnt = counter_values[sel_cnt_idx];
            if (sel_cnt <= cnt) {
                break;
            }
        }
        break;
    }

    WDT->WDTbits.INT = 3 - sel_cnt_idx; // invert index

    t_wdt = sel_cnt * (1000 * 10) / CC2538_WDT_CLK;
    t_wdt *= 100;
    return t_wdt;
}

int wdt_enable(void) {
    WDT->WDTbits.EN = 1;
    return 0;
}

int wdt_disable(void) {
    //WDT->WDTbits.EN = 0; // has no effect, WDT can't be disabled
    return -1;
}

int wdt_is_enabled(void) {
    return WDT->WDTbits.EN;
}

void wdt_reset(void) {

    unsigned irq_state;

    irq_state = irq_disable();

    WDT->WDTbits.CLR = 0xa;
    WDT->WDTbits.CLR = 0x5;

    irq_restore(irq_state);
}
