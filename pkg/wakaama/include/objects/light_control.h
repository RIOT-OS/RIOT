/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_light_control Light Control
 * @brief       Light Control object implementation for LwM2M client using Wakaama
 *
 * @experimental This API is considered experimental and may change in future releases without
 *               deprecation process.
 *
 * This implements the LwM2M Light Control object (ID 3311) as specified in the LwM2M registry.
 *
 * This Object is used to control a light source, such as a LED or other light. It allows a light to
 * be turned on or off and dim it.
 *
 * To use this object add `USEMODULE += wakaama_objects_light_control` to the application Makefile.
 *
 * ## Resources
 *
 * For an XML description of the object see
 * https://raw.githubusercontent.com/OpenMobileAlliance/lwm2m-registry/prod/3311.xml
 *
| Name                    |  ID  | Mandatory |   Type  |  Range  | Units | Implemented |
|-------------------------|:----:|:---------:|:-------:|:-------:|:-----:|:-----------:|
| On/Off                  | 5850 |    Yes    | Boolean |    -    |   -   |     Yes[1]  |
| Dimmer                  | 5851 |     No    | Integer | 0 - 100 |  /100 |     Yes[1]  |
| On time                 | 5852 |     No    | Integer |    -    |   s   |     Yes     |
| Cumulative active power | 5805 |     No    |  Float  |    -    |   Wh  |      No     |
| Power factor            | 5820 |     No    |  Float  |    -    |   -   |      No     |
| Colour                  | 5706 |     No    |  String |    -    |   -   |     Yes     |
| Sensor Units            | 5701 |     No    |  String |    -    |   -   |      No     |
| Application Type        | 5750 |     No    |  String |    -    |   -   |     Yes     |
 *
 * [1]: The handling of these resources are implemented, but its actual impact on the light state
 *      depends on the application.
 * ## Usage
 *
 * 1. Initialize the LwM2M client with @ref lwm2m_object_light_control_init, by passing a pointer
 *    to the LwM2M client data.
 *
 * 2. Now you can create instances of the Light Control object with
 *    @ref lwm2m_object_light_control_instance_create. As part of the arguments,
 *    you can pass a callback that will be called when the light resources are updated
 *    (i.e. status, dimmer, color, app_type), as well as an user argument.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #define LIGHT_COLOR "FFFFFF"
 * #define LIGHT_APP_TYPE "LED 0"
 *
 * // ... ///
 *
 * void _light_cb(lwm2m_object_t *object, uint16_t instance_id, bool status, uint8_t dimmer,
 *                const char* color, const char* app_type, void *arg)
 * {
 *     (void)object;
 *     (void)instance_id;
 *     (void)arg;
 *
 *     printf("%s is now %s, ", app_type, status? "ON":"OFF");
 *     printf("with color %s and intensity %d%%\n", color, dimmer);
 * }
 *
 * // ... ///
 *
 * lwm2m_object_t *light_control;
 * lwm2m_client_data_t client_data;
 *
 * lwm2m_client_init(&client_data);
 * light_control = lwm2m_object_light_control_init(&client_data);
 *
 * lwm2m_obj_light_control_args_t args = {
 *    .cb = _light_cb,
 *    .cb_arg = NULL,
 *    .color = LIGHT_COLOR,
 *    .color_len = sizeof(LIGHT_COLOR) - 1,
 *    .app_type = LIGHT_APP_TYPE,
 *    .app_type_len = sizeof(LIGHT_APP_TYPE) - 1
 * };
 *
 * int result = lwm2m_object_light_control_instance_create(&args, 0);
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 3. You can update the status, dimmer, color and app_type of the light control instance with
 *    @ref lwm2m_object_light_control_update_status, @ref lwm2m_object_light_control_update_dimmer,
 *    @ref lwm2m_object_light_control_update_color and
 *    @ref lwm2m_object_light_control_update_app_type respectively. This will make sure to send
 *    notifications to servers that may be observing these resources.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * lwm2m_object_light_control_update_status(0, true, false);
 * lwm2m_object_light_control_update_dimmer(0, 50, false);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "liblwm2m.h"

/**
 * @defgroup lwm2m_objects_light_control_config LwM2M Light Control object compile configurations
 * @ingroup lwm2m_client_config
 * @{
 */
/**
 * @brief Maximum number of instances of the object
 */
#ifndef CONFIG_LWM2M_LIGHT_INSTANCES_MAX
#define CONFIG_LWM2M_LIGHT_INSTANCES_MAX                (3U)
#endif

/**
 * @brief Maximum size for the color string
 */
#ifndef CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE
#define CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE       (16U)
#endif

/**
 * @brief Maximum size for the application type string
 */
#ifndef CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE
#define CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE    (16U)
#endif
/** @} */

/**
 * @brief Light Control object ID.
 */
#define LWM2M_LIGHT_CONTROL_OBJECT_ID         3311

/**
 * @name Light Control object resource's IDs.
 * @{
 */
/**
 * @brief Light status resource ID.
 */
#define LWM2M_LIGHT_CONTROL_ON_OFF_ID         5850
/**
 * @brief Dimmer value resource ID.
 */
#define LWM2M_LIGHT_CONTROL_DIMMER_ID         5851
/**
 * @brief On Time resource ID.
 */
#define LWM2M_LIGHT_CONTROL_ON_TIME_ID        5852
/**
 * @brief Light active power resource ID.
 */
#define LWM2M_LIGHT_CONTROL_ACT_PWR_ID        5805
/**
 * @brief Light power factor resource ID.
 */
#define LWM2M_LIGHT_CONTROL_PWR_FACTOR_ID     5820
/**
 * @brief Light color resource ID.
 */
#define LWM2M_LIGHT_CONTROL_COLOUR_ID         5706
/**
 * @brief Units of the power sensing resource ID.
 */
#define LWM2M_LIGHT_CONTROL_UNITS_ID          5701
/**
 * @brief Light application type resource ID.
 */
#define LWM2M_LIGHT_CONTROL_APP_TYPE_ID       5750
/** @} */

/**
 * @brief Signature of the callback called when the light resources are updated.
 *
 * @param[in] object                Light Control object handle.
 * @param[in] instance_id           ID of the instance where the resource changed.
 * @param[in] status                Light status.
 * @param[in] dimmer                Dimmer value.
 * @param[in] color                 Light color NULL-terminated string.
 * @param[in] app_type              Light application type NULL-terminated string.
 * @param[in] arg                   Argument registered in
 *                                  @ref lwm2m_object_light_control_instance_create.
 */
typedef void (*lwm2m_obj_light_control_cb_t)(lwm2m_object_t *object, uint16_t instance_id,
                                             bool status, uint8_t dimmer, const char* color,
                                             const char* app_type, void *arg);

/**
 * @brief Arguments for the creation of a Light Control object instance.
 */
typedef struct lwm2m_obj_light_control_args {
    lwm2m_obj_light_control_cb_t cb; /**< Callback for status and dimmer changes. May be NULL. */
    void *cb_arg;                    /**< Argument to call cb with. May be NULL. */
    const char *color;               /**< Array of chars with the light color. May be NULL. */
    size_t color_len;                /**< Length of color */
    const char *app_type;            /**< Array of chars with the light app type. May be NULL. */
    size_t app_type_len;             /**< Length of app_type */
} lwm2m_obj_light_control_args_t;

/**
 * @brief Initialize the Light Control object.
 *
 * @param[in] client_data  LwM2M client data.
 *
 * @return Pointer to the Light Control object on success
 */
lwm2m_object_t *lwm2m_object_light_control_init(lwm2m_client_data_t *client_data);

/**
 * @brief   Create a new Light Control instance and add it to the @p object list.
 *
 * @param[in]      args             Initialize structure with the parameter for the instance. May
 *                                  not be NULL.
 * @param[in]      instance_id      ID for the new instance. It must be between 0 and
 *                                  (UINT16_MAX - 1), if -1 the next available ID will be used.
 *
 * @return 0 on success
 * @return -EINVAL if an invalid @p instance_id is given
 * @return -ENOMEM if no memory is available to create a new instance
 */
int lwm2m_object_light_control_instance_create(const lwm2m_obj_light_control_args_t *args,
                                               int32_t instance_id);

/**
 * @brief Update the status of a light control instance.
 *
 * @param[in] instance_id  ID of the instance to update.
 * @param[in] status       New status of the light.
 * @param[in] call_cb      If true, the callback @ref lwm2m_obj_light_control_args_t::cb will be
 *                         called.
 *
 * @return 0 on success
 * @return -EINVAL if the instance does not exist
 */
int lwm2m_object_light_control_update_status(uint16_t instance_id, bool status, bool call_cb);

/**
 * @brief Update the dimmer value of a light control instance.
 *
 * @param[in] instance_id  ID of the instance to update.
 * @param[in] dimmer       New dimmer value.
 * @param[in] call_cb     If true, the callback @ref lwm2m_obj_light_control_args_t::cb will be
 *                         called.
 *
 * @return 0 on success
 * @return -EINVAL if the instance does not exist
 */
int lwm2m_object_light_control_update_dimmer(uint16_t instance_id, uint8_t dimmer, bool call_cb);

/**
 * @brief Update the color of a light control instance.
 *
 * @param[in] instance_id  ID of the instance to update.
 * @param[in] color        New color of the light.
 * @param[in] len          Length of the color string.
 * @param[in] call_cb     If true, the callback @ref lwm2m_obj_light_control_args_t::cb will be
 *                         called.
 *
 * @return 0 on success
 * @return -EINVAL if the instance does not exist
 * @return -ENOBUFS if the color string is too long
 */
int lwm2m_object_light_control_update_color(uint16_t instance_id, const char *color, size_t len,
                                            bool call_cb);

/**
 * @brief Update the application type of a light control instance.
 *
 * @param[in] instance_id  ID of the instance to update.
 * @param[in] app_type     New application type of the light.
 * @param[in] len          Length of the app_type string.
 * @param[in] call_cb     If true, the callback @ref lwm2m_obj_light_control_args_t::cb will be
 *                         called.
 *
 * @return 0 on success
 * @return -EINVAL if the instance does not exist
 * @return -ENOBUFS if the app_type string is too long
 */
int lwm2m_object_light_control_update_app_type(uint16_t instance_id, const char *app_type,
                                               size_t len, bool call_cb);

#ifdef __cplusplus
}
#endif

/** @} */
