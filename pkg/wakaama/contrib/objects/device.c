/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *               2021 HAW Hamburg
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
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_defines.h"
#include "liblwm2m.h"
#include "objects/device.h"
#include "lwm2m_client_config.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/* Set to true if reboot requested. */
static bool reboot;

/**
 * @brief 'Execute' callback for the Device object.
 *
 * @param[in] instance_id       Instance ID. Should be 0 as a single instance exists.
 * @param[in] resource_id       ID of the resource to execute.
 * @param[in] buffer            Information needed for the execution.
 * @param[in] length            Length of @p buffer.
 * @param[in] object            Device object pointer
 *
 * @return COAP_204_CHANGED             on success
 * @return COAP_404_NOT_FOUND           when wrong instance specified
 * @return COAP_400_BAD_REQUEST         when wrong information has been sent
 * @return COAP_405_METHOD_NOT_ALLOWED  when trying to execute a resource that is not supported
 */
static uint8_t _execute_cb(uint16_t instance_id, uint16_t resource_id, uint8_t *buffer, int length,
                           lwm2m_object_t *object);

/**
 * @brief 'Read' callback for the Device object.
 *
 * @param[in] instance_id       Instance ID. Should be 0 as a single instance exists.
 * @param[in, out] num_data     Number of resources requested. 0 means all.
 * @param[in, out] data_array   Initialized data array to output the values,
 *                              when @p num_data != 0. Uninitialized otherwise.
 * @param[in] object            Device object pointer
 *
 * @return COAP_205_CONTENT                 on success
 * @return COAP_404_NOT_FOUND               when resource can't be found
 * @return COAP_500_INTERNAL_SERVER_ERROR   otherwise
 */
static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object);

/**
 * @brief 'Discover' callback for the Device object.
 *
 * @param[in] instance_id       Instance ID. Should be 0 as a single instance exists.
 * @param[in, out] num_data     Number of resources requested. 0 means all.
 * @param[in, out] data_array   Initialized data array to determine if the resource exists,
 *                              when @p num_data != 0. Uninitialized otherwise.
 * @param[in] object            Device object pointer
 *
 * @return COAP_205_CONTENT                 on success
 * @return COAP_404_NOT_FOUND               when a resource is not supported
 * @return COAP_500_INTERNAL_SERVER_ERROR   otherwise
 */
static uint8_t _discover_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                            lwm2m_object_t *object);

typedef struct {
    lwm2m_list_t list;              /**< Linked list handle */
} lwm2m_obj_device_inst_t;

static const lwm2m_obj_device_inst_t _instance;

/**
 * @brief Implementation of the object interface for the Device Object.
 */
static lwm2m_object_t _device_object = {
    .next           = NULL,
    .objID          = LWM2M_DEVICE_OBJECT_ID,
    .instanceList   = (lwm2m_list_t *)&_instance,
    .readFunc       = _read_cb,
    .executeFunc    = _execute_cb,
    .discoverFunc   = _discover_cb,
    .writeFunc      = NULL,
    .deleteFunc     = NULL,
    .createFunc     = NULL,
    .userData       = NULL
};

static uint8_t _discover_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                            lwm2m_object_t *object)
{
    uint8_t result;
    int i;

    (void)object;

    if (instance_id != 0) {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    if (*num_data == 0) {
        /* This list must contain all available resources */
        uint16_t res[] = {
            LWM2M_RES_MANUFACTURER, LWM2M_RES_MODEL_NO, LWM2M_RES_SERIAL,
            LWM2M_RES_FW_VER, LWM2M_RES_REBOOT, LWM2M_RES_ERROR_CODE,
            /* LWM2M_RES_ERROR_CODE_RESET, TODO */
            LWM2M_RES_BINDINGS, LWM2M_RES_TYPE, LWM2M_RES_HW_VERSION,
            LWM2M_RES_SW_VERSION,
        };
        int len = ARRAY_SIZE(res);

        *data_array = lwm2m_data_new(len);
        *num_data = len;

        if (*data_array == NULL) {
            DEBUG("[lwm2m:device:discover] could not allocate data array\n");
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        for (i = 0; i < len; i++) {
            (*data_array)[i].id = res[i];
        }
    }
    else {
        /* Check if each given resource is present */
        for (i = 0; i < *num_data && result == COAP_205_CONTENT; i++) {
            switch ((*data_array)[i].id) {
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

static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object)
{
    (void)object;
    int i;
    DEBUG("[lwm2m:device:read]\n");

    /* Single instance object */
    if (instance_id != 0) {
        return COAP_404_NOT_FOUND;
    }

    /* Full object requested */
    if (*num_data == 0) {
        DEBUG("[lwm2m:device:read] all resources are requested\n");

        /* This list must contain all readable resources */
        uint16_t resList[] = {
            LWM2M_RES_MANUFACTURER, LWM2M_RES_MODEL_NO,   LWM2M_RES_SERIAL,
            LWM2M_RES_FW_VER,       LWM2M_RES_HW_VERSION, LWM2M_RES_SW_VERSION,
            LWM2M_RES_BINDINGS,     LWM2M_RES_TYPE,       LWM2M_RES_ERROR_CODE,
        };
        int cnt = ARRAY_SIZE(resList);
        *data_array = lwm2m_data_new(cnt);
        *num_data = cnt;

        if (*data_array == NULL) {
            DEBUG("[lwm2m:device:read] could not allocate data array\n");
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        for (i = 0; i < cnt; i++) {
            (*data_array)[i].id = resList[i];
        }
    }

    for (i = 0; i < *num_data; i++) {
        lwm2m_data_t *data = &(*data_array)[i];

        DEBUG("[lwm2m:device:read] reading resource %d\n", data->id);
        switch (data->id) {
        /* Exec resources, can't be read */
        case LWM2M_RES_REBOOT:
        case LWM2M_RES_FRESET:
        case LWM2M_RES_ERROR_CODE_RESET:
            return COAP_405_METHOD_NOT_ALLOWED;
            break;

        case LWM2M_RES_ERROR_CODE:
            /* TODO: Here some error reporting should be implemented. For now returning no error */
            lwm2m_data_encode_int(LWM2M_DEVICE_ERR_NO_ERR, data);
            break;

        case LWM2M_RES_MANUFACTURER:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_MANUFACTURER, data);
            break;

        case LWM2M_RES_MODEL_NO:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_MODEL, data);
            break;

        case LWM2M_RES_SERIAL:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_SERIAL, data);
            break;

        case LWM2M_RES_FW_VER:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_FW_VERSION, data);
            break;

        case LWM2M_RES_BINDINGS:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_BINDINGS, data);
            break;

        case LWM2M_RES_TYPE:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_TYPE, data);
            break;

        case LWM2M_RES_HW_VERSION:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_HW_VERSION, data);
            break;

        case LWM2M_RES_SW_VERSION:
            lwm2m_data_encode_string(CONFIG_LWM2M_DEVICE_SW_VERSION, data);
            break;

        case LWM2M_DEVICE_RESOURCES:
            lwm2m_data_encode_string(NULL, data);
            break;

        /* The rest are not defined resources */
        default:
            return COAP_404_NOT_FOUND;
        }
    }

    return COAP_205_CONTENT;
}

static uint8_t _execute_cb(uint16_t instance_id, uint16_t resource_id, uint8_t *buffer, int length,
                           lwm2m_object_t *object)
{
    (void)buffer;
    (void)object;

    /* single instance object */
    if (instance_id != 0) {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) {
        return COAP_400_BAD_REQUEST;
    }

    /* for now only rebooting is available */
    if (resource_id == LWM2M_RES_REBOOT) {
        reboot = true;
        return COAP_204_CHANGED;
    }

    return COAP_405_METHOD_NOT_ALLOWED;
}

/*
 * Call this from the main loop to check whether a reboot was requested.
 */
bool lwm2m_device_reboot_requested(void)
{
    return reboot;
}

lwm2m_object_t *lwm2m_object_device_init(lwm2m_client_data_t *client_data)
{
    assert(client_data);

    _device_object.userData = client_data;
    return &_device_object;
}
