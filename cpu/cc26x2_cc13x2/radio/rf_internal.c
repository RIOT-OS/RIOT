/*
 * Copyright (C) 2020 Locha Mesh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "cc26x2_cc13x2_rf_internal.h"
#include "cc26x2_cc13x2_rf_commands.h"

static RF_Object _rf_prop_obj;

#define INACTIVITY_TIMEOUT (2000) /**< Inactivity timeout: 2 ms */

RF_Handle cc26x2_cc13x2_rf_open(void)
{
    /* Initialize parameters structure */
    RF_Params prop_params;
    RF_Params_init(&prop_params);

    /* Change inactivity time */
    prop_params.nInactivityTimeout = INACTIVITY_TIMEOUT;

    /* Open a handle to the driver */
    RF_Handle handle = RF_open(&_rf_prop_obj, &rf_prop_mode,
                               (RF_RadioSetup*)&rf_cmd_prop_radio_div_setup,
                               &prop_params);

    return handle;
}
