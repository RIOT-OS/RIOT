/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_silabs_drivers_aem
 * @{
 *
 * @file
 * @brief       Implementations of the advanced energy monitor driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "aem.h"

#include "em_dbg.h"
#include "em_gpio.h"

void aem_init(void)
{
    /* do not initialize if debugger is not connected */
    if (!DBG_Connected()) {
        return;
    }

    /* enable GPIO clock for configuring SWO pins */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    /* enable debug peripheral via SWO */
#if defined(_SILICON_LABS_32B_SERIES_0)
    DBG_SWOEnable(GPIO_ROUTE_SWLOCATION_LOC0);
#elif defined(_SILICON_LABS_32B_SERIES_1)
    DBG_SWOEnable(GPIO_ROUTELOC0_SWVLOC_LOC0);
#endif

    /* enable trace in core debug */
    CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* enable PC and IRQ sampling output */
    DWT->CTRL = AEM_DWT_CTRL;

    /* set TPIU prescaler to 16 */
    TPI->ACPR = AEM_TPI_ACPR;

    /* set protocol to NRZ */
    TPI->SPPR = AEM_TPI_SPPR;

    /* disable continuous formatting */
    TPI->FFCR = AEM_TPI_FFCR;

    /* unlock ITM and output data */
    ITM->LAR = AEM_ITM_LAR;
    ITM->TCR = AEM_ITM_TCR;
}
