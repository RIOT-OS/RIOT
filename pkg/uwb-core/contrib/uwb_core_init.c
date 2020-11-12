/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core bootstrapping  core
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "thread.h"

#include "dpl/dpl.h"
#include "uwb_core.h"

#include "uwb_dw1000.h"
#include "uwb_dw1000_params.h"

#include "twr_ss/twr_ss.h"
#include "twr_ss_ext/twr_ss_ext.h"
#include "twr_ss_ack/twr_ss_ack.h"
#include "twr_ds/twr_ds.h"
#include "twr_ds_ext/twr_ds_ext.h"

static dw1000_dev_instance_t dev;
static uint8_t _dw1000_rx_buffer[MYNEWT_VAL(UWB_RX_BUFFER_SIZE)];
static uint8_t _dw1000_tx_buffer[MYNEWT_VAL(DW1000_HAL_SPI_BUFFER_SIZE)];

void uwb_core_init(void)
{
    /* this will start the thread handling the event queue for uwb-core */
    uwb_core_riot_init();
    /* inits the dw1000 devices linked list */
    uwb_dw1000_init();
    /* set preallocated buffers to avoid malloc/calloc */
    uwb_dw1000_set_buffs(&dev, _dw1000_tx_buffer, _dw1000_rx_buffer);
    /* setup dw1000 device */
    uwb_dw1000_setup(&dev, (void *) &dw1000_params[0]);
    /* this will start a thread handling dw1000 device*/
    uwb_dw1000_config_and_start(&dev);

    /* init uwb pkg's */
    if (IS_USED(MODULE_UWB_CORE_RNG)) {
        extern void uwb_rng_pkg_init(void);
        uwb_rng_pkg_init();
    }

    /* uwb configuration module */
    if (IS_USED(MODULE_UWB_CORE_UWBCFG)) {
        extern int uwbcfg_pkg_init(void);
        uwbcfg_pkg_init();
    }

    /* ranging algorithms */
    if (IS_USED(MODULE_UWB_CORE_RNG)) {
        twr_ss_pkg_init();
    }
    if (IS_USED(MODULE_UWB_CORE_RNG)) {
        twr_ss_ack_pkg_init();
    }
    if (IS_USED(MODULE_UWB_CORE_RNG)) {
        twr_ss_ext_pkg_init();
    }
    if (IS_USED(MODULE_UWB_CORE_RNG)) {
        twr_ds_pkg_init();
    }
    if (IS_USED(MODULE_UWB_CORE_RNG)) {
        twr_ds_ext_pkg_init();
    }
}
