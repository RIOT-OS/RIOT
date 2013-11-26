/**
 * Auto initialization for used modules
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup auto_init
 * @{
 * @file    auto_init_c
 * @brief   initializes any used module that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */
#include <stdint.h>
#include <stdio.h>

#include "auto_init.h"

#ifdef MODULE_UART0
#include "board_uart0.h"
#endif

#ifdef MODULE_MCI
#include "diskio.h"
#endif

#ifdef MODULE_VTIMER
#include "vtimer.h"
#endif

#ifdef MODULE_RTC
#include "rtc.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

extern int main(void);

void auto_init(void)
{
#ifdef MODULE_BOARD_DISPLAY
    extern void lcd_init();
    lcd_init();
    DEBUG("DISP OK");
#endif
#ifdef MODULE_DISPLAY_PUTCHAR
    extern void init_display_putchar();
    init_display_putchar();
    DEBUG("DISP OK");
#endif
#ifdef MODULE_HWTIMER
    DEBUG("Auto init hwtimer module.\n");
    hwtimer_init();
#endif
#ifdef MODULE_VTIMER
    DEBUG("Auto init vtimer module.\n");
    vtimer_init();
#endif
#ifdef MODULE_UART0
    DEBUG("Auto init uart0 module.\n");
    board_uart0_init();
#endif
#ifdef MODULE_RTC
    DEBUG("Auto init rtc module.\n");
    rtc_init();
    rtc_enable();
#endif
#ifdef MODULE_SHT11
    DEBUG("Auto init SHT11 module.\n");
    sht11_init();
#endif
#ifdef MODULE_GPIOINT
    DEBUG("Auto init gpioint module.\n");
    gpioint_init();
#endif
#ifdef MODULE_CC110X
    DEBUG("Auto init CC1100 module.\n");
#ifndef MODULE_TRANSCEIVER
    cc1100_init();
#endif
#endif
#ifdef MODULE_LTC4150
    DEBUG("Auto init ltc4150 module.\n");
    ltc4150_init();
#endif
#ifdef MODULE_MCI
    DEBUG("Auto init mci module.\n");
    MCI_initialize();
#endif
#ifdef MODULE_PROFILING
    extern void profiling_init(void);
    profiling_init();
#endif
    main();
}
