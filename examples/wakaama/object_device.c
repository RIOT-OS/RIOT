/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liblwm2m.h"
#include "lwm2mconfig.h"

#ifndef LWM2M_DEVICE_MANUFACTURER
#define LWM2M_DEVICE_MANUFACTURER "A RIOT maker"
#endif

#ifndef LWM2M_DEVICE_MODEL
#define LWM2M_DEVICE_MODEL RIOT_BOARD
#endif

#ifndef LWM2M_DEVICE_SERIAL
#define LWM2M_DEVICE_SERIAL "undefined"
#endif

#ifndef LWM2M_DEVICE_FW_VERSION
#define LWM2M_DEVICE_FW_VERSION RIOT_VERSION
#endif

/* U (UDP) or UQ (UDP queue) and S (SMS) or SQ (SMS queue) */
#ifndef LWM2M_DEVICE_BINDINGS
#define LWM2M_DEVICE_BINDINGS "U"
#endif

#ifndef LWM2M_DEVICE_TYPE
#define LWM2M_DEVICE_TYPE "RIOT device"
#endif

#ifndef LWM2M_DEVICE_HW_VERSION
#define LWM2M_DEVICE_HW_VERSION RIOT_BOARD
#endif

#ifndef LWM2M_DEVICE_SW_VERSION
#define LWM2M_DEVICE_SW_VERSION RIOT_VERSION
#endif

enum {
    LWM2M_RES_MANUFACTURER = 0,
    LWM2M_RES_MODEL_NO,
    LWM2M_RES_SERIAL,
    LWM2M_RES_FW_VER,
    LWM2M_RES_REBOOT,           /* exec */
    LWM2M_RES_FRESET,           /* exec */
    LWM2M_RES_POWER_SRC,        /* multiplea */
    LWM2M_RES_POWER_VOL,        /* multiple */
    LWM2M_RES_POWER_AMP,        /* multiple */
    LWM2M_RES_BATTERY_LEVEL,    /* 0-100 (percentage) */
    LWM2M_RES_MEM_FREE,         /* kB */
    LWM2M_RES_ERROR_CODE,       /* 0-8; multiple */
    LWM2M_RES_ERROR_CODE_RESET, /* exec */
    LWM2M_RES_TIME,             /* rw, string */
    LWM2M_RES_TIME_OFFSET,      /* rw, string; from UTC */
    LWM2M_RES_TIME_ZONE,        /* rw, string */
    LWM2M_RES_BINDINGS,
    LWM2M_RES_TYPE,
    LWM2M_RES_HW_VERSION,
    LWM2M_RES_SW_VERSION,
    LWM2M_RES_BATTERY_STATUS,
    LWM2M_RES_MEM_TOTAL,        /* kB */
    LWM2M_RES_EXT_DEV_INFO,     /* objlink; multiple */
};

typedef struct {
    uint8_t *power_sources;      /* 0-7 */
    uint16_t *power_voltage;     /* mV */
    uint16_t *power_current;     /* mA */
    uint8_t battery_status;      /* 0-6 */
    uint32_t mem_total;          /* kB */
    uint16_t(*ext_dev_info)[2];  /* Network byte order; most significant half ->
                                  * object, least significant -> instance
                                  */
    uint8_t ext_dev_info_len;
    /*
     * Possible values (according to LwM2M specification):
     *    0 No error
     *    1 Low battery power
     *    2 External power supply off
     *    3 GPS module failure
     *    4 Low received signal strength
     *    5 Out of memory
     *    6 SMS failure
     *    7 IP connectivity failure
     *    8 Peripheral malfunction
     */
    uint8_t error_code[7];
    /* How many values used in error_code[] */
    uint8_t error_code_used;
} dev_data_t;

/* Set to true if reboot requested. */
static bool reboot;

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
        int len = sizeof(res) / sizeof(uint16_t);

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
    uint8_t result;
    dev_data_t *data = (dev_data_t *)objectP->userData;

    (void)data;

    /* Single instance object */
    if (instance_id != 0) {
        return COAP_404_NOT_FOUND;
    }

    /* Full object requested */
    if (*num_dataP == 0) {
        /* This list must contain all readable resources */
        uint16_t resList[] = {
            LWM2M_RES_MANUFACTURER, LWM2M_RES_MODEL_NO,   LWM2M_RES_SERIAL,
            LWM2M_RES_FW_VER,       LWM2M_RES_HW_VERSION, LWM2M_RES_SW_VERSION,
            LWM2M_RES_BINDINGS,     LWM2M_RES_TYPE,       LWM2M_RES_ERROR_CODE,
        };
        int cnt = sizeof(resList) / sizeof(uint16_t);
        *data_arrayP = lwm2m_data_new(cnt);
        if (*data_arrayP == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
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
                break;
            /* Statically defined resources */
            case LWM2M_RES_MANUFACTURER:
                lwm2m_data_encode_string(LWM2M_DEVICE_MANUFACTURER,
                                         *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_MODEL_NO:
                lwm2m_data_encode_string(LWM2M_DEVICE_MODEL, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_SERIAL:
                lwm2m_data_encode_string(LWM2M_DEVICE_SERIAL, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_FW_VER:
                lwm2m_data_encode_string(LWM2M_DEVICE_FW_VERSION, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_HW_VERSION:
                lwm2m_data_encode_string(LWM2M_DEVICE_HW_VERSION, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_SW_VERSION:
                lwm2m_data_encode_string(LWM2M_DEVICE_SW_VERSION, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_BINDINGS:
                lwm2m_data_encode_string(LWM2M_DEVICE_BINDINGS, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            case LWM2M_RES_TYPE:
                lwm2m_data_encode_string(LWM2M_DEVICE_TYPE, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            /* Error code is mandatory */
            case LWM2M_RES_ERROR_CODE:
                /* TODO: Do this properly */
                lwm2m_data_encode_int(0, *data_arrayP + i);
                result = COAP_205_CONTENT;
                break;
            /* Time resources */
            case LWM2M_RES_TIME:
            case LWM2M_RES_TIME_OFFSET:
            case LWM2M_RES_TIME_ZONE:
            /* What to do with this? */
            case LWM2M_RES_POWER_SRC:
            case LWM2M_RES_POWER_VOL:
            case LWM2M_RES_POWER_AMP:
            case LWM2M_RES_BATTERY_LEVEL:
            case LWM2M_RES_MEM_FREE:
            case LWM2M_RES_BATTERY_STATUS:
            case LWM2M_RES_MEM_TOTAL:
            case LWM2M_RES_EXT_DEV_INFO:
            default:
                result = COAP_404_NOT_FOUND;
        }

        if (result != COAP_205_CONTENT) {
            break;
        }
    }

    return result;
}

static uint8_t prv_device_write(uint16_t instance_id, int num_data,
                                lwm2m_data_t *data_array,
                                lwm2m_object_t *objectP)
{
    int i;
    uint8_t result = COAP_404_NOT_FOUND;
    dev_data_t *data = (dev_data_t *)objectP->userData;

    (void)data;

    (void)instance_id;
    (void)num_data;
    (void)data_array;

    for (i = 0; i < num_data; i++) {
        switch (data_array[i].id) {
            /* Writable */
            case LWM2M_RES_TIME:
            case LWM2M_RES_TIME_OFFSET:
            case LWM2M_RES_TIME_ZONE:
            /* TODO: Update stuff here. */
            /* Non-Writable */
            case LWM2M_RES_MANUFACTURER:
            case LWM2M_RES_MODEL_NO:
            case LWM2M_RES_SERIAL:
            case LWM2M_RES_FW_VER:
            case LWM2M_RES_REBOOT:
            case LWM2M_RES_FRESET:
            case LWM2M_RES_POWER_SRC:
            case LWM2M_RES_POWER_VOL:
            case LWM2M_RES_POWER_AMP:
            case LWM2M_RES_BATTERY_LEVEL:
            case LWM2M_RES_MEM_FREE:
            case LWM2M_RES_ERROR_CODE:
            case LWM2M_RES_ERROR_CODE_RESET:
            case LWM2M_RES_BINDINGS:
            case LWM2M_RES_TYPE:
            case LWM2M_RES_HW_VERSION:
            case LWM2M_RES_SW_VERSION:
            case LWM2M_RES_BATTERY_STATUS:
            case LWM2M_RES_MEM_TOTAL:
            case LWM2M_RES_EXT_DEV_INFO:
                result = COAP_405_METHOD_NOT_ALLOWED;
                break;
            default:
                result = COAP_404_NOT_FOUND;
        }
    }

    return result;
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
