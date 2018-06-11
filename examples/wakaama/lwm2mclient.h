/*******************************************************************************
*
* Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    Bosch Software Innovations GmbH - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/
/*
 * lwm2mclient.h
 *
 *  General functions of lwm2m test client.
 *
 *  Created on: 22.01.2015
 *  Author: Achim Kraus
 *  Copyright (c) 2015 Bosch Software Innovations GmbH, Germany. All rights reserved.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Header for main loop for Wakaama example client.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 */

#ifndef LWM2MCLIENT_H
#define LWM2MCLIENT_H

#include "liblwm2m.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LWM2M object IDs of supported devices
 *
 *       Also see IPSO Alliance Smart Object definitions:
 *       https://github.com/IPSO-Alliance/pub
 * @{
 */
#define LWM2M_GENERIC_SENSOR_ID       (3300u)
/* Sensors that return decimal values */
#define LWM2M_ILLUMINANCE_SENSOR_ID   (3301u)
#define LWM2M_ANALOG_SENSOR_ID        (3202u)
#define LWM2M_TEMPERATURE_SENSOR_ID   (3303u)
#define LWM2M_HUMIDITY_SENSOR_ID      (3304u)
#define LWM2M_BAROMETER_SENSOR_ID     (3315u)
#define LWM2M_VOLTAGE_SENSOR_ID       (3316u)
#define LWM2M_CURRENT_SENSOR_ID       (3317u)
#define LWM2M_FREQUENCY_SENSOR_ID     (3318u)
#define LWM2M_DEPTH_SENSOR_ID         (3319u)
#define LWM2M_PERCENTAGE_SENSOR_ID    (3320u)
#define LWM2M_ALTITUDE_SENSOR_ID      (3321u)
#define LWM2M_LOAD_SENSOR_ID          (3322u)
#define LWM2M_PRESSURE_SENSOR_ID      (3323u)
#define LWM2M_LOUDNESS_SENSOR_ID      (3324u)
#define LWM2M_CONCENTRATION_SENSOR_ID (3325u)
#define LWM2M_ACIDITY_SENSOR_ID       (3326u)
#define LWM2M_CONDUCTIVITY_SENSOR_ID  (3327u)
#define LWM2M_POWER_SENSOR_ID         (3328u)
#define LWM2M_POWER_FACTOR_SENSOR_ID  (3329u)
#define LWM2M_DISTANCE_SENSOR_ID      (3330u)
#define LWM2M_RATE_SENSOR_ID          (3346u)
/* Sensors that return digital values */
#define LWM2M_DIGITAL_SENSOR_ID       (3200u)
/* Actuators */
#define LWM2M_DIGITAL_ACTUATOR_ID     (3201u)
/** @} */

/**
 * @name IDs of used LwM2M attribute types.
 *
 *       Also see IPSO Alliance Smart Object definitions:
 *       https://github.com/IPSO-Alliance/pub
 * @{
 */
/* Float sensors */
#define LWM2M_SENSOR_VALUE_ATTR           (5700u) /* R Float */
#define LWM2M_SENSOR_UNITS_ATTR           (5701u) /* R Float */
#define LWM2M_SENSOR_ANALOG_INPUT_ATTR    (5600u) /* R Float */
#define LWM2M_MIN_MEASURED_ATTR           (5601u) /* R Float */
#define LWM2M_MAX_MEASURED_ATTR           (5602u) /* R Float */
#define LWM2M_MIN_RANGE_ATTR              (5603u) /* R Float */
#define LWM2M_MAX_RANGE_ATTR              (5604u) /* R Float */
#define LWM2M_RESET_MIN_MAX_MEASURED_ATTR (5605u) /* Execute */
/* Digital sensors */
#define LWM2M_DINPUT_ATTR                 (5500u) /* R Bool */
#define LWM2M_DINPUT_COUNTER_ATTR         (5501u) /* R Int */
#define LWM2M_DINPUT_POLARITY_ATTR        (5502u) /* RW Bool */
#define LWM2M_DINPUT_DEBOUNCE_ATTR        (5503u) /* RW Int */
#define LWM2M_DINPUT_EDGESELECT_ATTR      (5504u) /* RW INT (1-3) */
#define LWM2M_DINPUT_COUNTER_RESET_ATTR   (5505u) /* Execute */
/* Digital actuator */
#define LWM2M_DOUTPUT_ATTR                (5550u) /* RW Bool */
#define LWM2M_DOUTPUT_POLARITY_ATTR       (5551u) /* RW Bool */
/* Misc */
#define LWM2M_APP_TYPE_ATTR               (5750u) /* RW String */
#define LWM2M_SENSOR_TYPE_ATTR            (5751u) /* R String */
/** @} */

/*
 * lwm2mutil.c
 */

/**
 * @brief  Translates a SAUL device type into a corresponding LWM2M object ID.
 *
 * @param[in] saul_type  A SAUL device type ID.
 *
 * @return -1 if no LWM2M object ID could be found
 * @return one of the LWM2M_*_SENSOR_ID values otherwise
 */
int lwm2m_saul_type_to_obj_id(uint8_t saul_type);

/**
 * @brief  Converts an LWM2M object ID into a string.
 *
 * @param[in] id  ID of an LWM2M object.
 *
 * @return Name of the object if @p id is defined.
 * @return NULL otherwise.
 */
const char *lwm2m_object_id_to_str(const uint16_t id);

/*
 * object_digital_actuator.c
 */

/**
 * @brief  Creates a new object instance for a digital actuator object and optionally adds
 *         it to an existing instance list.
 *
 * @param[in/out] object    Pointer to pointer of a base sensor object. If the
 *                          inner pointer is NULL, a new base object will be allocated.
 * @param[in]     dev       Pointer to the SAUL device driver for the sensor.
 *
 * @return 0 on success
 * @return -EINVAL if @p object == NULL
 * @return -EINVAL if @p dev == NULL
 * @return -EINVAL if @p objId of @p object does not equal LWM2M_DIGITIAL_ACTUATOR_ID
 * @return -ENOMEM if no memory could be allocated for the base object or the instance.
 * @return -EEXIST if an instance for @p dev already exists in @p object.
 */
int lwm2m_get_bool_actuator(lwm2m_object_t **object, saul_reg_t *dev);

/**
 * @brief  Release memory of a base sensor object and all its instances.
 *
 * @param[in] obj  Pointer to the object to release.
 */
void lwm2m_free_bool_actuator(lwm2m_object_t *obj);

/**
 * @brief  Automatically adds objects all supported devices in the SAUL registry
 *         to an LWM2M client instance.
 *
 * @param[in] context  The LWM2M client context to add objects to.
 *
 * @return The number of objects added.
 */
int lwm2m_auto_add_bool_actuators(lwm2m_context_t *context);

/**
 * @brief  Notify observers about value changes.
 *
 * @param[in] lwm2mH  LWM2M context
 */
void lwm2m_poll_bool_actuators(lwm2m_context_t *lwm2mH);

/*
 * object_digital_sensor.c
 */

/**
 * @brief  Creates a new object instance for a digital sensor object and optionally adds
 *         it to an existing instance list.
 *
 * @param[in/out] object    Pointer to pointer of a base sensor object. If the
 *                          inner pointer is NULL, a new base object will be allocated.
 * @param[in]     dev       Pointer to the SAUL device driver for the sensor.
 *
 * @return 0 on success
 * @return -EINVAL if @p object == NULL
 * @return -EINVAL if @p dev == NULL
 * @return -EINVAL if @p objId of @p object does not equal LWM2M_DIGITAL_SENSOR_ID
 * @return -ENOMEM if no memory could be allocated for the base object or the instance.
 * @return -EEXIST if an instance for @p dev already exists in @p object.
 */
int lwm2m_get_bool_sensor(lwm2m_object_t **object, saul_reg_t *dev);

/**
 * @brief  Release memory of a base sensor object and all its instances.
 *
 * @param[in] obj  Pointer to the object to release.
 */
void lwm2m_free_bool_sensor(lwm2m_object_t *obj);

/**
 * @brief  Automatically adds objects all supported devices in the SAUL registry
 *         to an LWM2M client instance.
 *
 * @param[in] context  The LWM2M client context to add objects to.
 *
 * @return The number of objects added.
 */
int lwm2m_auto_add_bool_sensors(lwm2m_context_t *context);

/**
 * @update  Update the measured values for all supported sensor objects and notify
 *          observers if values changed.
 *
 * @param[in] lwm2mH  LWM2M context
 */
void lwm2m_poll_bool_sensors(lwm2m_context_t *lwm2mH);

/*
 * object_float_sensor.c
 */

/**
 * @brief  Creates a new object instance for an analog sensor object and optionally adds
 *         it to an existing instance list.
 *
 * @param[in/out] object     Pointer to pointer of a base sensor object. If the
 *                           inner pointer is NULL, a new base object will be allocated.
 * @param[in]     sensor_id  ID of the sensor type to use (See LWM2M_*_SENSOR_ID defines).
 * @param[in]     dev        Pointer to the SAUL device driver for the sensor.
 *
 * @return 0 on success
 * @return -EINVAL if @p object == NULL
 * @return -EINVAL if @p sensor_id is an unknown ID
 * @return -EINVAL if @p dev == NULL
 * @return -EINVAL if @p sensor_id does not match the objId of @p object.
 * @return -ENOMEM if no memory could be allocated for the base object or the instance.
 * @return -EEXIST if an instance for @p dev already exists in @p object.
 */
int lwm2m_get_float_sensor(lwm2m_object_t **object, int sensor_id, saul_reg_t *dev);

/**
 * @brief  Release memory of a base sensor object and all its instances.
 *
 * @param[in] obj  Pointer to the object to release.
 */
void lwm2m_free_float_sensor(lwm2m_object_t *obj);

/**
 * @brief  Automatically adds objects all supported devices in the SAUL registry
 *         to an LWM2M client instance.
 *
 * @param[in] context  The LWM2M client context to add objects to.
 *
 * @return The number of objects added.
 */
int lwm2m_auto_add_float_sensors(lwm2m_context_t *context);

/**
 * @update  Update the measured values for all supported sensor objects and notify
 *          observers if values changed.
 *
 * @param[in] lwm2mH  LWM2M context
 */
void lwm2m_poll_float_sensors(lwm2m_context_t *lwm2mH);

/*
 * object_device.c
 */

/**
 * @brief  Instantiate an lwm2m device object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *lwm2m_get_object_device(void);

/**
 * @brief  Release resouces of an lwm2m device object.
 *
 * @param[in] object  Pointer to the object.
 */
void lwm2m_free_object_device(lwm2m_object_t *object);

/**
 * @brief  Indicates when a reboot has been requested via an lwm2m device object.
 *
 * @return true   When reboot has been requested.
 * @return false  Otherwise
 */
bool lwm2m_device_reboot_requested(void);

/*
 * object_server.c
 */

/**
 * @brief  Instantiate an lwm2m server object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *get_server_object(int server_id, const char *binding, int lifetime, bool storing);

/**
 * @brief  Release resouces of an lwm2m server object.
 *
 * @param[in] object  Pointer to the object.
 */
void clean_server_object(lwm2m_object_t *object);

/**
 * @brief  Prints a string representation of an lwm2m server object to stdout.
 *
 * @param[in] object  Pointer to the object.
 */
void display_server_object(lwm2m_object_t *object);

/**
 * @brief  Creates a copy of an lwm2m server object.
 *
 * @param[out] object_dest  Destination object to write to.
 * @param[in]  object_src   Source object to copy from.
 */
void copy_server_object(lwm2m_object_t *object_dest, lwm2m_object_t *object_src);

/*
 * object_access_control.c
 */

/**
 * @brief  Instantiate an lwm2m access control object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *acc_ctrl_create_object(void);

/**
 * @brief  Release resouces of an lwm2m access control object.
 *
 * @param[in] object  Pointer to the object.
 */
void acl_ctrl_free_object(lwm2m_object_t *object);

/**
 * @brief  Add an instance of an lwm2m access control object to a base object.
 *
 * @param[out] accCtrlObjP  The access control base object to add an instance to.
 * @param[in]  instId       The instance ID of the new instance.
 * @param[in]  acObjectId   The object ID of the target object.
 * @param[in]  acObjInstId  The instance ID of the target object.
 * @param[in]  acOwner      The owner
 *
 * @return     true         When the instance was successfully created.
 * @return     false        Otherwise.
 */
bool  acc_ctrl_obj_add_inst(lwm2m_object_t *accCtrlObjP, uint16_t instId,
                            uint16_t acObjectId, uint16_t acObjInstId, uint16_t acOwner);

/**
 * @brief  Add an access control list entry to an lwm2m access control object instance.
 *
 * @param[out] accCtrlObjP  The access control base object to modify.
 * @param[in]  instId       The instance ID of the object to modify.
 * @param[in]  aclResId     The resource ID to control access to.
 * @param[in]  acValue      The permissions to set.
 */
bool  acc_ctrl_oi_add_ac_val(lwm2m_object_t *accCtrlObjP, uint16_t instId,
                             uint16_t aclResId, uint16_t acValue);
/*
 * object_security.c
 */

/**
 * @brief  Instantiate an lwm2m security object.
 *
 * @param[in] server_id     Short ID of the server
 * @param[in] server_uri    URI of the server
 * @param[in] bs_psk_id     PSK identity
 * @param[in] psk           PSK
 * @param[in] psk_len       Length of @p psk
 * @param[in] is_bootstrap  Server is bootstrap-server
 *
 * @return  Pointer to object on success.
 * @return  NULL on error.
 */
lwm2m_object_t *get_security_object(int server_id, const char *server_uri, char *bs_psk_id, char *psk, uint16_t psk_len, bool is_bootstrap);

/**
 * @brief  Release resouces of an lwm2m security object.
 *
 * @param[in] object  Pointer to the object.
 */
void clean_security_object(lwm2m_object_t *object);

/**
 * @brief  Get the URI of the corresponding server from an lwm2m security object.
 *
 * @param[in] object           The security base object to read from.
 * @param[in] sec_obj_inst_id  The instance ID of the object to read from.
 *
 * @return    URI string    On success.
 * @retuen    NULL          On error.
 */
char *get_server_uri(lwm2m_object_t *object, uint16_t sec_obj_inst_id);

/**
 * @brief  Prints a string representation of an lwm2m security object to stdout.
 *
 * @param[in] object  Pointer to the object.
 */
void display_security_object(lwm2m_object_t *object);

/**
 * @brief  Creates a copy of an lwm2m security object.
 *
 * @param[out] object_dest  Destination object to write to.
 * @param[in]  object_src   Source object to copy from.
 */
void copy_security_object(lwm2m_object_t *object_dest, lwm2m_object_t *object_src);

/*
 * Run the wakaama process.
 *
 * @param[in] arg Ignored.
 */
void *lwm2m_run_server(void *arg);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LWM2MCLIENT_H */
