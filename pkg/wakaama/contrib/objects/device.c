/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_device
 *
 * @file
 * @brief       Device object implementation for LwM2M client using Wakaama
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_defines.h"
#include "liblwm2m.h"
#include "objects/device.h"
#include "lwm2m_client_config.h"

/* Set to true if reboot requested. */
static bool reboot;

/* Lookup table for static resources of device object */
static const char *_static_resources[] = {
    [LWM2M_RES_MANUFACTURER] = CONFIG_LWM2M_DEVICE_MANUFACTURER,
    [LWM2M_RES_MODEL_NO]     = CONFIG_LWM2M_DEVICE_MODEL,
    [LWM2M_RES_SERIAL]       = CONFIG_LWM2M_DEVICE_SERIAL,
    [LWM2M_RES_FW_VER]       = CONFIG_LWM2M_DEVICE_FW_VERSION,
    [LWM2M_RES_BINDINGS]     = CONFIG_LWM2M_DEVICE_BINDINGS,
    [LWM2M_RES_TYPE]         = CONFIG_LWM2M_DEVICE_TYPE,
    [LWM2M_RES_HW_VERSION]   = CONFIG_LWM2M_DEVICE_HW_VERSION,
    [LWM2M_RES_SW_VERSION]   = CONFIG_LWM2M_DEVICE_SW_VERSION,
    [LWM2M_DEVICE_RESOURCES] = NULL
};

/*Descriptor of a LwM2M device object instance */
typedef struct {
    uint8_t *power_sources;      /**< types of power sources (0-7) */
    uint16_t *power_voltage;     /**< voltage of power sources in mV */
    uint16_t *power_current;     /**< current of power sources in mA */
    uint8_t battery_status;      /**< battery status (0-6) */
    uint32_t mem_total;          /**< amount of memory on the device in kB */
    uint16_t(*ext_dev_info)[2];  /**< external devices information */
    uint8_t ext_dev_info_len;    /**< amount of external devices information */
    uint8_t error_code[7];       /**< error codes */
    uint8_t error_code_used;     /**< amount of error codes used */
} dev_data_t;

static uint8_t prv_device_discover(uint16_t instance_id, int *num_dataP,
                                   lwm2m_data_t **data_arrayP,
                                   lwm2m_object_t *objectP)
{
    uint8_t result;
    int i;

    (void)objectP;

    if (instance_id != 0) {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    if (*num_dataP == 0) {
        /* This list must contain all available resources */
        uint16_t res[] = {
            LWM2M_RES_MANUFACTURER, LWM2M_RES_MODEL_NO, LWM2M_RES_SERIAL,
            LWM2M_RES_FW_VER, LWM2M_RES_REBOOT, LWM2M_RES_ERROR_CODE,
            /* LWM2M_RES_ERROR_CODE_RESET, TODO */
            LWM2M_RES_BINDINGS, LWM2M_RES_TYPE, LWM2M_RES_HW_VERSION,
            LWM2M_RES_SW_VERSION,
        };
        int len = ARRAY_SIZE(res);

        *data_arrayP = lwm2m_data_new(len);
        if (*data_arrayP == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
        *num_dataP = len;
        for (i = 0; i < len; i++) {
            (*data_arrayP)[i].id = res[i];
        }
    }
    else {
        /* Check if each given resource is present */
        for (i = 0; i < *num_dataP && result == COAP_205_CONTENT; i++) {
            switch ((*data_arrayP)[i].id) {
                case LWM2M_RES_MANUFACTURER:
                case LWM2M_RES_MODEL_NO:
                case LWM2M_RES_SERIAL:
                case LWM2M_RES_FW_VER:
                case LWM2M_RES_REBOOT:
                case LWM2M_RES_ERROR_CODE:
                /* case LWM2M_RES_ERROR_CODE_RESET: TODO */
                case LWM2M_RES_BINDINGS:
                case LWM2M_RES_TYPE:
                case LWM2M_RES_HW_VERSION:
                case LWM2M_RES_SW_VERSION:
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_device_read(uint16_t instance_id, int *num_dataP,
                               lwm2m_data_t **data_arrayP,
                               lwm2m_object_t *objectP)
{
    int i;
    uint8_t result = COAP_404_NOT_FOUND;
    dev_data_t *data = (dev_data_t *)objectP->userData;

    (void)data;

    /* Single instance object */
    if (instance_id != 0) {
        goto out;
    }

    /* Full object requested */
    if (*num_dataP == 0) {
        /* This list must contain all readable resources */
        uint16_t resList[] = {
            LWM2M_RES_MANUFACTURER, LWM2M_RES_MODEL_NO,   LWM2M_RES_SERIAL,
            LWM2M_RES_FW_VER,       LWM2M_RES_HW_VERSION, LWM2M_RES_SW_VERSION,
            LWM2M_RES_BINDINGS,     LWM2M_RES_TYPE,       LWM2M_RES_ERROR_CODE,
        };
        int cnt = ARRAY_SIZE(resList);
        *data_arrayP = lwm2m_data_new(cnt);
        if (*data_arrayP == NULL) {
            result = COAP_500_INTERNAL_SERVER_ERROR;
            goto out;
        }
        *num_dataP = cnt;
        for (i = 0; i < cnt; i++) {
            (*data_arrayP)[i].id = resList[i];
        }
    }

    for (i = 0; i < *num_dataP; i++) {
        switch ((*data_arrayP)[i].id) {
            /* Exec resources */
            case LWM2M_RES_REBOOT:
            case LWM2M_RES_FRESET:
            case LWM2M_RES_ERROR_CODE_RESET:
                result = COAP_405_METHOD_NOT_ALLOWED;
                goto out;
                break;
            case LWM2M_RES_ERROR_CODE:
                /* TODO: Here some error reporting should be implemented. */
                lwm2m_data_encode_int(LWM2M_DEVICE_ERR_NO_ERR, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            /* The rest are either static or not defined resources */
            default:
                if (_static_resources[(*data_arrayP)[i].id]) {
                    lwm2m_data_encode_string(
                        _static_resources[(*data_arrayP)[i].id],
                        *data_arrayP + i);
                    result = COAP_205_CONTENT;
                }
                else {
                    result = COAP_404_NOT_FOUND;
                    goto out;
                }
        }
    }

out:
    return result;
}

static uint8_t prv_device_write(uint16_t instance_id, int num_data,
                                lwm2m_data_t *data_array,
                                lwm2m_object_t *objectP)
{
    dev_data_t *data = (dev_data_t *)objectP->userData;

    (void)data;
    (void)instance_id;
    (void)num_data;
    (void)data_array;

    if (data_array[0].id < LWM2M_DEVICE_RESOURCES) {
        /* for now not writing resources */
        return COAP_405_METHOD_NOT_ALLOWED;
    }
    else {
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_device_execute(uint16_t instance_id, uint16_t resource_id,
                                  uint8_t *buffer, int length,
                                  lwm2m_object_t *objectP)
{
    uint8_t result;
    dev_data_t *data = (dev_data_t *)objectP->userData;

    (void)data;

    (void)buffer;
    (void)length;
    (void)objectP;

    /* single instance object */
    if (instance_id != 0) {
        result = COAP_404_NOT_FOUND;
        goto err_out;
    }

    if (length != 0) {
        result = COAP_400_BAD_REQUEST;
        goto err_out;
    }

    switch (resource_id) {
        case LWM2M_RES_REBOOT:
            reboot = true;
            result = COAP_204_CHANGED;
            break;
        case LWM2M_RES_ERROR_CODE_RESET:
        /* TODO */
        case LWM2M_RES_FRESET:
        /* TODO Callback? */
        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
    }

err_out:
    return result;
}

/*
 * Call this from the main loop to check whether a reboot was requested.
 */
bool lwm2m_device_reboot_requested(void)
{
    return reboot;
}

lwm2m_object_t *lwm2m_get_object_device(void)
{
    lwm2m_object_t *obj;

    obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (obj == NULL) {
        goto err_out;
    }

    memset(obj, 0, sizeof(lwm2m_object_t));
    obj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));

    if (obj->instanceList == NULL) {
        goto free_obj;
    }

    memset(obj->instanceList, 0, sizeof(lwm2m_list_t));

    obj->objID = LWM2M_DEVICE_OBJECT_ID;

    obj->readFunc = prv_device_read;
    obj->writeFunc = prv_device_write;
    obj->executeFunc = prv_device_execute;
    obj->discoverFunc = prv_device_discover;

    /* Don't allocate memory for stuff that isn't used at the moment */
    /* obj->userData = lwm2m_malloc(sizeof(dev_data_t)); */
    /* if (obj->userData == NULL) { */
    /*    goto free_ilist; */
    /* } */
    /*  */
    /* memset(obj->userData, 0, sizeof(dev_data_t)); */
    /* INT USER DATA HERE */

    return obj;

/* free_ilist: */
/*    lwm2m_free(obj->instanceList); */

free_obj:
    lwm2m_free(obj);

err_out:
    return NULL;
}

void lwm2m_free_object_device(lwm2m_object_t *obj)
{
    if (obj == NULL) {
        return;
    }
    if (obj->userData) {
        lwm2m_free(obj->userData);
    }
    if (obj->instanceList) {
        lwm2m_free(obj->instanceList);
    }
    lwm2m_free(obj);
}
