/*
 * cc2538_wdt.h
 *
 *  Created on: 12.07.2017
 *      Author: geith
 */

#ifndef RIOT_CPU_CC2538_INCLUDE_CC2538_WDT_H_
#define RIOT_CPU_CC2538_INCLUDE_CC2538_WDT_H_

#include "cc2538.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SMWDTHROSC_WDCTL_EN (1 << 4)

typedef union {
  cc2538_reg_t WDT;
  struct{
    cc2538_reg_t INT        :  2; /**< timer interval */
    cc2538_reg_t RESERVED   :  1; /**< reserved */
    cc2538_reg_t EN         :  1; /**< enable */
    cc2538_reg_t CLR        :  4; /**< clear */
    cc2538_reg_t RESERVED_2 :  24; /**< reserved */
  } WDTbits;
} cc2538_wdt_t;

extern cc2538_wdt_t * const WDT;       /**< WDT Instance */

int cc2538_wdt_enable(uint32_t t_wdt);
int cc2538_wdt_enable_max(uint32_t t_wdt);

void cc2538_wdt_reset(void);


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* RIOT_CPU_CC2538_INCLUDE_CC2538_WDT_H_ */
