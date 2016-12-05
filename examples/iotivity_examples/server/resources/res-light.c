/*
 * Copyright (C) 2016 CREATE-NET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     Examples
 * @{
 *
 * @file        res-light.c
 * @brief       Resource light definition. Handler are defined here.
 *
 * @author      Mattia Antonini <mattia.antonini1@studenti.unipr.it>
 *                              <m.antonini@create-net.org>
 *
 * @}
 */

#include "oc_api.h"
#include "board.h"


static oc_resource_t *res_light = NULL;
static bool res_light_state = false;

static void
res_light_set_led_state(bool light_state)
{
    if (light_state) {
        PRINT("server_oic: LED0 is ON\n");
        LED0_ON;
    }
    else {
        PRINT("server_oic: LED0 is OFF\n");
        LED0_OFF;
    }
}


static void
res_light_get_handler(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    PRINT("server_oic: GET request\n");
    oc_rep_start_root_object();
    switch (interface) {
        case OC_IF_BASELINE:
            oc_process_baseline_interface(request->resource);
        case OC_IF_RW:
            oc_rep_set_boolean(root, state, res_light_state);
            break;
        default:
            break;
    }
    oc_rep_end_root_object();
    oc_send_response(request, OC_STATUS_OK);
    PRINT("server_oic: Light state %d\n", res_light_state);
}

static void
res_light_put_handler(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    PRINT("server_oic: PUT request\n");
    bool state = false;
    oc_rep_t *rep = request->request_payload;

    while (rep != NULL) {
        PRINT("server_oic: key: %s ", oc_string(rep->name));
        switch (rep->type) {
            case BOOL:
                state = rep->value_boolean;
                PRINT("value: %d\n", state);
                break;
            default:
                oc_send_response(request, OC_STATUS_BAD_REQUEST);
                return;
                break;
        }
        rep = rep->next;
    }
    oc_send_response(request, OC_STATUS_CHANGED);
    res_light_state = state;
    res_light_set_led_state(res_light_state);
}

void
res_light_register(void)
{
    res_light = oc_new_resource("/light/1", 1, 0);
    oc_resource_bind_resource_type(res_light, "oic.r.light");
    oc_resource_bind_resource_interface(res_light, OC_IF_RW);
    oc_resource_set_default_interface(res_light, OC_IF_RW);

//TODO: Security is not supported by the pkg
#ifdef OC_SECURITY
    oc_resource_make_secure(res_light);
#endif

    oc_resource_set_discoverable(res_light, true);
    oc_resource_set_observable(res_light, true);
    oc_resource_set_request_handler(res_light, OC_GET, res_light_get_handler, NULL);
    oc_resource_set_request_handler(res_light, OC_PUT, res_light_put_handler, NULL);
    oc_add_resource(res_light);

    res_light_set_led_state(false); //RIMANE TRUE... PERCHe'
}
