/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_light_control Light Control
 * @brief       Light Control object implementation for LwM2M client using Wakaama
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
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef OBJECTS_LIGHT_CONTROL_H
#define OBJECTS_LIGHT_CONTROL_H

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
 * @brief Signature of the callback called when the light status or dimmer values are written.
 *
 * @param[in] object                Light Control object handle.
 * @param[in] instance_id           ID of the instance where the resource changed.
 * @param[in] status                Light status.
 * @param[in] dimmer                Dimmer value.
 * @param[in] arg                   Argument registered in
 *                                  @ref lwm2m_object_light_control_instance_create.
 */
typedef void (*lwm2m_obj_light_control_cb_t)(lwm2m_object_t *object, uint16_t instance_id,
                                             bool status, uint8_t dimmer, void *arg);

/**
 * @brief Arguments for the creation of a Light Control object instance.
 */
typedef struct lwm2m_obj_light_control_args {
    lwm2m_obj_light_control_cb_t cb; /**< Callback for status and dimmer changes. May be NULL. */
    void *cb_arg;                    /**< Argument to call cb with. May be NULL. */
    char *color;                     /**< Array of chars with the light color. May be NULL. */
    size_t color_len;                /**< Length of color */
    char *app_type;                  /**< Array of chars with the light app type. May be NULL. */
    size_t app_type_len;             /**< Length of app_type */
} lwm2m_obj_light_control_args_t;

/**
 * @brief   Get the Light Control object handle
 *
 * @return Pointer to the global handle of the Light Control object.
 */
lwm2m_object_t *lwm2m_object_light_control_get(void);

/**
 * @brief   Create a new Light Control instance and add it to the @p object list.
 *
 * @param[in, out] object           Light Control object handle.
 * @param[in]      instance_id      ID for the new instance.
 * @param[in]      args             Initialize structure with the parameter for the instance. May
 *                                  not be NULL.
 *
 * @return 0 on success
 * @return <0 otherwise
 */
int lwm2m_object_light_control_instance_create(lwm2m_object_t *object, uint16_t instance_id,
                                               const lwm2m_obj_light_control_args_t *args);

#ifdef __cplusplus
}
#endif

#endif /* OBJECTS_LIGHT_CONTROL_H */
/** @} */
