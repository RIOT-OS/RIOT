/*
 * Copyright (C) 2018 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 * @file
 * @brief       NXP KW41Z XCVR module initialization and calibration of kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "log.h"

#include "kw41zrf.h"
#include "bit.h"
#include "vendor/XCVR/MKW41Z4/fsl_xcvr.h"
#include "vendor/XCVR/MKW41Z4/ifr_radio.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

int kw41zrf_xcvr_init(kw41zrf_t *dev)
{
    (void) dev;
    uint8_t radio_id = ((RSIM->MISC & RSIM_MISC_RADIO_VERSION_MASK) >> RSIM_MISC_RADIO_VERSION_SHIFT);
    switch (radio_id) {
        case 0x3: /* KW41/31/21 v1 */
        case 0xb: /* KW41/31/21 v1.1 */
            break;
        default:
            return -ENODEV;
    }

    RSIM->RF_OSC_CTRL = (RSIM->RF_OSC_CTRL &
        ~(RSIM_RF_OSC_CTRL_RADIO_EXT_OSC_OVRD_MASK)) | /* Set EXT_OSC_OVRD value to zero */
        RSIM_RF_OSC_CTRL_RADIO_EXT_OSC_OVRD_EN_MASK; /* Enable over-ride with zero value */
    bit_set32(&SIM->SCGC5, SIM_SCGC5_PHYDIG_SHIFT); /* Enable PHY clock gate */

    /* Load IFR trim values */
    IFR_SW_TRIM_TBL_ENTRY_T sw_trim_tbl[] =
    {
        {TRIM_STATUS, 0, 0}, /*< Fetch the trim status word if available.*/
        {TRIM_VERSION, 0, 0} /*< Fetch the trim version number if available.*/
    };
    handle_ifr(&sw_trim_tbl[0], sizeof(sw_trim_tbl) / sizeof(sw_trim_tbl[0]));
    DEBUG("[kw41zrf] sw_trim_tbl:\n");

    for (unsigned k = 0; k < sizeof(sw_trim_tbl) / sizeof(sw_trim_tbl[0]); ++k) {
        DEBUG("[kw41zrf] [%u] id=0x%04x ", k, (unsigned)sw_trim_tbl[k].trim_id);
        if (sw_trim_tbl[k].trim_id == TRIM_STATUS) {
            DEBUG("(TRIM_STATUS)  ");
        }
        else if (sw_trim_tbl[k].trim_id == TRIM_VERSION) {
            DEBUG("(TRIM_VERSION) ");
        }
        DEBUG("value=%" PRIu32 ", valid=%u\n", sw_trim_tbl[k].trim_value,
            (unsigned)sw_trim_tbl[k].valid);
    }

    /* We only use 802.15.4 mode in this driver */
    xcvrStatus_t status = XCVR_Configure(&xcvr_common_config, &zgbe_mode_config,
        &xcvr_ZIGBEE_500kbps_config, &xcvr_802_15_4_500kbps_config, 25, XCVR_FIRST_INIT);

    if (status != gXcvrSuccess_c) {
        return -EIO;
    }
    return 0;
}
