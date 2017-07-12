/*
 * wdt.c
 *
 *  Created on: 12.07.2017
 *      Author: geith
 */

#include "irq.h"
#include "cc2538_wdt.h"

#define CC2538_WDT_CLK 32768

cc2538_wdt_t * const WDT = (cc2538_wdt_t *)&SMWDTHROSC_WDCTL;

// counter values supported by the cc2538
//const uint16_t counter_values[] = {64, 512, 8192, 32768 }; // values according to cc2538 user manual
const uint16_t counter_values[] = {32, 256, 4096, 16384 }; // values according to measurements on Zolertia Firefly

static uint16_t cc2538_wdt_usec_to_cnt(uint32_t t_wdt)
{
  uint32_t cnt;
  if(t_wdt > (1000 * 1000)){
      // max. WDT interval of 1s
      t_wdt = 1000 * 1000;
  }

  // scale down (still below min. resolution)
  t_wdt /= 10;

  cnt = (t_wdt * CC2538_WDT_CLK) / (100 * 1000);

  if(cnt == 0){
      cnt = 1;
  }

  return cnt;
}

int cc2538_wdt_enable(uint32_t t_wdt) {
  uint32_t cnt;
  uint16_t sel_cnt;
  uint8_t sel_cnt_idx;

  cnt = cc2538_wdt_usec_to_cnt(t_wdt);

  for(sel_cnt_idx=0;
      sel_cnt_idx<(sizeof(counter_values)/sizeof(counter_values[0]));
      sel_cnt_idx++){

      sel_cnt = counter_values[sel_cnt_idx];
      if(sel_cnt >= cnt){
	  break;
      }
  }

  WDT->WDTbits.INT = 3 - sel_cnt_idx; // invert index
  WDT->WDTbits.EN = 1;

  t_wdt = sel_cnt * (1000 * 10) / CC2538_WDT_CLK;
  t_wdt *= 100;
  return t_wdt;
}

int cc2538_wdt_enable_max(uint32_t t_wdt) {
  uint32_t cnt;
  uint16_t sel_cnt;
  uint8_t sel_cnt_idx;

  cnt = cc2538_wdt_usec_to_cnt(t_wdt);

  for(sel_cnt_idx=((sizeof(counter_values)/sizeof(counter_values[0]))-1);
      sel_cnt_idx>0;
      sel_cnt_idx--){

      sel_cnt = counter_values[sel_cnt_idx];
      if(sel_cnt <= cnt){
	  break;
      }
  }

  WDT->WDTbits.INT = 3 - sel_cnt_idx; // invert index
  WDT->WDTbits.EN = 1;

  t_wdt = sel_cnt * (1000 * 10) / CC2538_WDT_CLK;
  t_wdt *= 100;
  return t_wdt;
}

void cc2538_wdt_reset(void) {

  unsigned irq_state;

  irq_state = irq_disable();

  WDT->WDTbits.CLR = 0xa;
  WDT->WDTbits.CLR = 0x5;

  irq_restore(irq_state);
}
