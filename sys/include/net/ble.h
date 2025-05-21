/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    ble_defs Generic BLE defines
 * @ingroup     ble
 * @brief       General values defined by the BT standard
 * @{
 *
 * @file
 * @brief       General BLE values as defined by the BT standard
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    BT version constants
 * @{
 */
#define BLE_VERSION_40                      (0x06)
#define BLE_VERSION_41                      (0x07)
#define BLE_VERSION_42                      (0x08)
#define BLE_VERSION_50                      (0x09)
/** @} */

/**
 * @name    Collections of general BLE constants
 * @{
 */
#define BLE_AA_LEN              (4U)    /**< access address length */
#define BLE_ADDR_LEN            (6U)    /**< link layer address length */
#define BLE_CRC_LEN             (3U)    /**< CRC length */
#define BLE_CHANMAP_LEN         (5U)    /**< channel map length */
#define BLE_CHAN_NUMOF          (40U)   /**< number of available channels */
#define BLE_CHAN_ADV_NUMOF      (3U)    /**< number of advertising channels */
#define BLE_CHAN_DAT_NUMOF      (37U)   /**< number of data channels */
#define BLE_ADV_PDU_LEN         (31U)   /**< max size of legacy ADV packets */
#define BLE_ADV_PDU_LEN_EXT     (251U)  /**< max size of extended ADV packets */
/** @} */

/**
 * @name    GATT Declaration UUIDs
 *
 * @see https://www.bluetooth.com/specifications/gatt/declarations
 * @{
 */
#define BLE_DECL_PRI_SERVICE                (0x2800)
#define BLE_DECL_SEC_SERVICE                (0x2801)
#define BLE_DECL_INCLUDE                    (0x2802)
#define BLE_DECL_CHAR                       (0x2803)
/** @} */

/**
 * @name    GATT descriptor UUIDs
 *
 * @see https://www.bluetooth.com/specifications/gatt/descriptors
 * @{
 */
#define BLE_DESC_AGGR_FMT                   (0x2905)
#define BLE_DESC_EXT_PROP                   (0x2900)
#define BLE_DESC_PRES_FMT                   (0x2904)
#define BLE_DESC_USER_DESC                  (0x2901)
#define BLE_DESC_CLIENT_CONFIG              (0x2902)
#define BLE_DESC_ENV_CONFIG                 (0x290b)
#define BLE_DESC_ENV_MEASUREMENT            (0x290c)
#define BLE_DESC_ENV_TRIGGER_SETTING        (0x290d)
#define BLE_DESC_EXT_REPORT_REF             (0x2907)
#define BLE_DESC_NUMOF_DIGITS               (0x2909)
#define BLE_DESC_REPORT_REF                 (0x2908)
#define BLE_DESC_SERVER_CONFIG              (0x2903)
#define BLE_DESC_TIME_TRIGGER_SETTING       (0x290e)
#define BLE_DESC_VALID_RANGE                (0x2906)
#define BLE_DESC_VALUE_TRIGGER_SETTING      (0x290a)
/** @} */

/**
 * @name    Selected GATT service UUIDs (16-bit)
 *
 * @see https://www.bluetooth.com/specifications/gatt/services
 * @{
 */
#define BLE_GATT_SVC_GAP                    (0x1800)    /**< GAP service */
#define BLE_GATT_SVC_GATT                   (0x1801)    /**< GATT service */
#define BLE_GATT_SVC_DEVINFO                (0x180a)    /**< device info */
#define BLE_GATT_SVC_HRS                    (0x180d)    /**< heart rate service */
#define BLE_GATT_SVC_BAS                    (0x180f)    /**< battery service */
#define BLE_GATT_SVC_IPSS                   (0x1820)    /**< IP protocol support */
/* XXX: custom services not defined by the BT SIG (random values) */
#define BLE_GATT_SVC_NDNSS                  (0x7e5e)    /**< NDN support service */
/* add more on demand */
/** @} */

/**
 * @name    Selected GATT characteristic UUIDs (16-bit)
 *
 * @see https://www.bluetooth.com/specifications/gatt/characteristics/
 * @{
 */
#define BLE_GATT_CHAR_BATTERY_LEVEL         (0x2a19)    /**< battery level */
#define BLE_GATT_CHAR_SYSTEM_ID             (0x2a23)    /**< system ID */
#define BLE_GATT_CHAR_MODEL_NUMBER_STR      (0x2a24)    /**< model number */
#define BLE_GATT_CHAR_SERIAL_NUMBER_STR     (0x2a25)    /**< serial number */
#define BLE_GATT_CHAR_FW_REV_STR            (0x2a26)    /**< firmware revision */
#define BLE_GATT_CHAR_HW_REV_STR            (0x2a27)    /**< hardware revision */
#define BLE_GATT_CHAR_SW_REV_STR            (0x2a28)    /**< software revision */
#define BLE_GATT_CHAR_MANUFACTURER_NAME     (0x2a29)    /**< manufacturer name */
#define BLE_GATT_CHAR_HEART_RATE_MEASURE    (0x2a37)    /**< heart rate measurement */
#define BLE_GATT_CHAR_BODY_SENSE_LOC        (0x2a38)    /**< body sensor location */
/** @} */

/**
 * @name    Characteristic format types (8-bit)
 *
 * @see https://www.bluetooth.com/specifications/assigned-numbers/format-types
 * @{
 */
#define BLE_UNIT_BLE_FMT_BOOL               (0x01)
#define BLE_UNIT_BLE_FMT_2bit               (0x02)
#define BLE_UNIT_BLE_FMT_NIBBLE             (0x03)
#define BLE_UNIT_BLE_FMT_UINT8              (0x04)
#define BLE_UNIT_BLE_FMT_UINT12             (0x05)
#define BLE_UNIT_BLE_FMT_UINT16             (0x06)
#define BLE_UNIT_BLE_FMT_UINT24             (0x07)
#define BLE_UNIT_BLE_FMT_UINT32             (0x08)
#define BLE_UNIT_BLE_FMT_UINT48             (0x09)
#define BLE_UNIT_BLE_FMT_UINT64             (0x0A)
#define BLE_UNIT_BLE_FMT_UINT128            (0x0B)
#define BLE_UNIT_BLE_FMT_SINT8              (0x0C)
#define BLE_UNIT_BLE_FMT_SINT12             (0x0D)
#define BLE_UNIT_BLE_FMT_SINT16             (0x0E)
#define BLE_UNIT_BLE_FMT_SINT24             (0x0F)
#define BLE_UNIT_BLE_FMT_SINT32             (0x10)
#define BLE_UNIT_BLE_FMT_SINT48             (0x11)
#define BLE_UNIT_BLE_FMT_SINT64             (0x12)
#define BLE_UNIT_BLE_FMT_SINT128            (0x13)
#define BLE_UNIT_BLE_FMT_FLOAT32            (0x14)
#define BLE_UNIT_BLE_FMT_FLOAT64            (0x15)
#define BLE_UNIT_BLE_FMT_SFLOAT             (0x16)
#define BLE_UNIT_BLE_FMT_FLOAT              (0x17)
#define BLE_UNIT_BLE_FMT_DUINT16            (0x18)
#define BLE_UNIT_BLE_FMT_UTF8               (0x19)
#define BLE_UNIT_BLE_FMT_UTF16              (0x1A)
#define BLE_UNIT_BLE_FMT_STRUCT             (0x1B)
/** @} */

/**
 * @name    Units (16-bit)
 *
 * @see https://www.bluetooth.com/specifications/assigned-numbers/units
 * @{
 */
#define BLE_UNIT_NONE                       (0x2700)    /**< no unit */
#define BLE_UNIT_BLE_UNIT_METRE             (0x2701)    /**< length [metre] */
#define BLE_UNIT_KILOGRAM                   (0x2702)    /**< mass [kilogram] */
#define BLE_UNIT_SECOND                     (0x2703)    /**< time [second] */
#define BLE_UNIT_AMPERE                     (0x2704)    /**< electric_current [ampere] */
#define BLE_UNIT_KELVIN                     (0x2705)    /**< thermodynamic_temperature [kelvin] */
#define BLE_UNIT_MOLE                       (0x2706)    /**< amount_of_substance [mole] */
#define BLE_UNIT_CANDELA                    (0x2707)    /**< luminous_intensity [candela] */
#define BLE_UNIT_SQUARE_METRES              (0x2710)    /**< area [square_metres] */
#define BLE_UNIT_CUBIC_METRES               (0x2711)    /**< volume [cubic_metres] */
#define BLE_UNIT_METRES_PER_SECOND          (0x2712)    /**< velocity [metres_per_second] */
#define BLE_UNIT_METRES_PER_SECOND_SQUARED  (0x2713)    /**< acceleration [metres_per_second_squared] */
#define BLE_UNIT_RECIPROCAL_METRE           (0x2714)    /**< wavenumber [reciprocal_metre] */
#define BLE_UNIT_KG_PER_CUBIC_METRE         (0x2715)    /**< density [kilogram_per_cubic_metre] */
#define BLE_UNIT_KG_PER_SQUARE_METRE        (0x2716)    /**< surface_density [kilogram_per_square_metre] */
#define BLE_UNIT_CUBIC_METRE_PER_KILOGRAM   (0x2717)    /**< specific_volume [cubic_metre_per_kilogram] */
#define BLE_UNIT_AMPERE_PER_SQUARE_METRE    (0x2718)    /**< current_density [ampere_per_square_metre] */
#define BLE_UNIT_AMPERE_PER_METRE           (0x2719)    /**< magnetic_field_strength [ampere_per_metre] */
#define BLE_UNIT_MOLE_PER_CUBIC_METRE       (0x271a)    /**< amount_concentration [mole_per_cubic_metre] */
#define BLE_UNIT_KILOGRAM_PER_CUBIC_METRE   (0x271b)    /**< mass_concentration [kilogram_per_cubic_metre] */
#define BLE_UNIT_CANDELA_PER_SQUARE_METRE   (0x271c)    /**< luminance [candela_per_square_metre] */
#define BLE_UNIT_REFRACTIVE_INDEX           (0x271d)    /**< refractive index */
#define BLE_UNIT_RELATIVE_PERMEABILITY      (0x271e)    /**< relative permeability */
#define BLE_UNIT_RADIAN                     (0x2720)    /**< plane_angle [radian] */
#define BLE_UNIT_STERADIAN                  (0x2721)    /**< solid_angle [steradian] */
#define BLE_UNIT_HERTZ                      (0x2722)    /**< frequency [hertz] */
#define BLE_UNIT_NEWTON                     (0x2723)    /**< force [newton] */
#define BLE_UNIT_PASCAL                     (0x2724)    /**< pressure [pascal] */
#define BLE_UNIT_JOULE                      (0x2725)    /**< energy [joule] */
#define BLE_UNIT_WATT                       (0x2726)    /**< power [watt] */
#define BLE_UNIT_COULOMB                    (0x2727)    /**< electric_charge [coulomb] */
#define BLE_UNIT_VOLT                       (0x2728)    /**< electric_potential_difference [volt] */
#define BLE_UNIT_FARAD                      (0x2729)    /**< capacitance [farad] */
#define BLE_UNIT_OHM                        (0x272a)    /**< electric_resistance [ohm] */
#define BLE_UNIT_SIEMENS                    (0x272b)    /**< electric_conductance [siemens] */
#define BLE_UNIT_WEBER                      (0x272c)    /**< magnetic_flux [weber] */
#define BLE_UNIT_TESLA                      (0x272d)    /**< magnetic_flux_density [tesla] */
#define BLE_UNIT_HENRY                      (0x272e)    /**< inductance [henry] */
#define BLE_UNIT_DEGREE_CELSIUS             (0x272f)    /**< thermodynamic_temperature [degree_celsius] */
#define BLE_UNIT_LUMEN                      (0x2730)    /**< luminous_flux [lumen] */
#define BLE_UNIT_LUX                        (0x2731)    /**< illuminance [lux] */
#define BLE_UNIT_BECQUEREL                  (0x2732)    /**< activity_referred_to_a_radionuclide [becquerel] */
#define BLE_UNIT_GRAY                       (0x2733)    /**< absorbed_dose [gray] */
#define BLE_UNIT_SIEVERT                    (0x2734)    /**< dose_equivalent [sievert] */
#define BLE_UNIT_KATAL                      (0x2735)    /**< catalytic_activity [katal] */
#define BLE_UNIT_PASCAL_SECOND              (0x2740)    /**< dynamic_viscosity [pascal_second] */
#define BLE_UNIT_NEWTON_METRE               (0x2741)    /**< moment_of_force [newton_metre] */
#define BLE_UNIT_NEWTON_PER_METRE           (0x2742)    /**< surface_tension [newton_per_metre] */
#define BLE_UNIT_RADIAN_PER_SECOND          (0x2743)    /**< angular_velocity [radian_per_second] */
#define BLE_UNIT_RADIAN_PER_SECOND_SQUARED  (0x2744)    /**< angular_acceleration [radian_per_second_squared] */
#define BLE_UNIT_HEAT_FLUX_WATT_PER_M2      (0x2745)    /**< heat_flux_density [watt_per_square_metre] */
#define BLE_UNIT_JOULE_PER_KELVIN           (0x2746)    /**< heat_capacity [joule_per_kelvin] */
#define BLE_UNIT_JOULE_PER_KG_KELVIN        (0x2747)    /**< specific_heat_capacity [joule_per_kilogram_kelvin] */
#define BLE_UNIT_JOULE_PER_KG               (0x2748)    /**< specific_energy [joule_per_kilogram] */
#define BLE_UNIT_WATT_PER_METRE_KELVIN      (0x2749)    /**< thermal_conductivity [watt_per_metre_kelvin] */
#define BLE_UNIT_JOULE_PER_CUBIC_METRE      (0x274a)    /**< energy_density [joule_per_cubic_metre] */
#define BLE_UNIT_VOLT_PER_METRE             (0x274b)    /**< electric_field_strength [volt_per_metre] */
#define BLE_UNIT_COULOMB_PER_CUBIC_METRE    (0x274c)    /**< electric_charge_density [coulomb_per_cubic_metre] */
#define BLE_UNIT_COULOMB_PER_M2             (0x274d)    /**< surface_charge_density [coulomb_per_square_metre] */
#define BLE_UNIT_FLUX_COULOMB_PER_M2        (0x274e)    /**< electric_flux_density [coulomb_per_square_metre] */
#define BLE_UNIT_FARAD_PER_METRE            (0x274f)    /**< permittivity [farad_per_metre] */
#define BLE_UNIT_HENRY_PER_METRE            (0x2750)    /**< permeability [henry_per_metre] */
#define BLE_UNIT_JOULE_PER_MOLE             (0x2751)    /**< molar_energy [joule_per_mole] */
#define BLE_UNIT_JOULE_PER_MOLE_KELVIN      (0x2752)    /**< molar_entropy [joule_per_mole_kelvin] */
#define BLE_UNIT_COULOMB_PER_KG             (0x2753)    /**< exposure [coulomb_per_kilogram] */
#define BLE_UNIT_GRAY_PER_SECOND            (0x2754)    /**< absorbed_dose_rate [gray_per_second] */
#define BLE_UNIT_WATT_PER_STERADIAN         (0x2755)    /**< radiant_intensity [watt_per_steradian] */
#define BLE_UNIT_WATT_PER_M2_STERADIAN      (0x2756)    /**< radiance [watt_per_square_metre_steradian] */
#define BLE_UNIT_KATAL_PER_CUBIC_METRE      (0x2757)    /**< catalytic_activity_concentration [katal_per_cubic_metre] */
#define BLE_UNIT_MINUTE                     (0x2760)    /**< time [minute] */
#define BLE_UNIT_HOUR                       (0x2761)    /**< time [hour] */
#define BLE_UNIT_DAY                        (0x2762)    /**< time [day] */
#define BLE_UNIT_ANGLE_DEGREE               (0x2763)    /**< plane_angle [degree] */
#define BLE_UNIT_ANGLE_MINUTE               (0x2764)    /**< plane_angle [minute] */
#define BLE_UNIT_ANGLE_SECOND               (0x2765)    /**< plane_angle [second] */
#define BLE_UNIT_HECTARE                    (0x2766)    /**< area [hectare] */
#define BLE_UNIT_LITRE                      (0x2767)    /**< volume [litre] */
#define BLE_UNIT_TONNE                      (0x2768)    /**< mass [tonne] */
#define BLE_UNIT_BAR                        (0x2780)    /**< pressure [bar] */
#define BLE_UNIT_MILLIMETRE_OF_MERCURY      (0x2781)    /**< pressure [millimetre_of_mercury] */
#define BLE_UNIT_NGSTRM                     (0x2782)    /**< length [ngstrm] */
#define BLE_UNIT_NAUTICAL_MILE              (0x2783)    /**< length [nautical_mile] */
#define BLE_UNIT_BARN                       (0x2784)    /**< area [barn] */
#define BLE_UNIT_KNOT                       (0x2785)    /**< velocity [knot] */
#define BLE_UNIT_NEPER                      (0x2786)    /**< logarithmic_radio_quantity [neper] */
#define BLE_UNIT_BEL                        (0x2787)    /**< logarithmic_radio_quantity [bel] */
#define BLE_UNIT_YARD                       (0x27a0)    /**< length [yard] */
#define BLE_UNIT_PARSEC                     (0x27a1)    /**< length [parsec] */
#define BLE_UNIT_INCH                       (0x27a2)    /**< length [inch] */
#define BLE_UNIT_FOOT                       (0x27a3)    /**< length [foot] */
#define BLE_UNIT_MILE                       (0x27a4)    /**< length [mile] */
#define BLE_UNIT_POUND_FORCE_PER_SQU_INCH   (0x27a5)    /**< pressure [pound_force_per_square_inch] */
#define BLE_UNIT_KILOMETRE_PER_HOUR         (0x27a6)    /**< velocity [kilometre_per_hour] */
#define BLE_UNIT_MILE_PER_HOUR              (0x27a7)    /**< velocity [mile_per_hour] */
#define BLE_UNIT_REVOLUTION_PER_MINUTE      (0x27a8)    /**< angular_velocity [revolution_per_minute] */
#define BLE_UNIT_GRAM_CALORIE               (0x27a9)    /**< energy [gram_calorie] */
#define BLE_UNIT_KG_CALORIE                 (0x27aa)    /**< energy [kilogram_calorie] */
#define BLE_UNIT_KILOWATT_HOUR              (0x27ab)    /**< energy [kilowatt_hour] */
#define BLE_UNIT_DEGREE_FAHRENHEIT          (0x27ac)    /**< thermodynamic_temperature [degree_fahrenheit] */
#define BLE_UNIT_PERCENTAGE                 (0x27ad)    /**< percentage */
#define BLE_UNIT_PER_MILLE                  (0x27ae)    /**< per mille */
#define BLE_UNIT_BEATS_PER_MINUTE           (0x27af)    /**< period [beats_per_minute] */
#define BLE_UNIT_AMPERE_HOURS               (0x27b0)    /**< electric_charge [ampere_hours] */
#define BLE_UNIT_MILLIGRAM_PER_DECILITRE    (0x27b1)    /**< mass_density [milligram_per_decilitre] */
#define BLE_UNIT_MILLIMOLE_PER_LITRE        (0x27b2)    /**< mass_density [millimole_per_litre] */
#define BLE_UNIT_YEAR                       (0x27b3)    /**< time [year] */
#define BLE_UNIT_MONTH                      (0x27b4)    /**< time [month] */
#define BLE_UNIT_COUNT_PER_CUBIC_METRE      (0x27b5)    /**< concentration [count_per_cubic_metre] */
#define BLE_UNIT_WATT_PER_SQUARE_METRE      (0x27b6)    /**< irradiance [watt_per_square_metre] */
#define BLE_UNIT_MLIT_PER_KG_PER_MINUTE     (0x27b7)    /**< transfer_rate [milliliter_per_kilogram_per_minute] */
#define BLE_UNIT_POUND                      (0x27b8)    /**< mass [pound] */
#define BLE_UNIT_METABOLIC_EQU              (0x27b9)    /**< metabolic equivalent */
#define BLE_UNIT_STEP_PER_MINUTE            (0x27ba)    /**< steps per minute */
#define BLE_UNIT_STROKE_PER_MINUTE          (0x27bc)    /**< strokes per minute */
#define BLE_UNIT_KILOMETER_PER_MINUTE       (0x27bd)    /**< velocity [kilometer_per_minute] */
#define BLE_UNIT_LUMEN_PER_WATT             (0x27be)    /**< luminous_efficacy [lumen_per_watt] */
#define BLE_UNIT_LUMEN_HOUR                 (0x27bf)    /**< luminous_energy [lumen_hour] */
#define BLE_UNIT_LUX_HOUR                   (0x27c0)    /**< luminous_exposure [lux_hour] */
#define BLE_UNIT_GRAM_PER_SECOND            (0x27c1)    /**< mass_flow [gram_per_second] */
#define BLE_UNIT_LITRE_PER_SECOND           (0x27c2)    /**< volume_flow [litre_per_second] */
/** @} */

/**
 * @brief   Channel identifiers defined for L2CAP LE-U link layers
 * @{
 */
#define BLE_L2CAP_CID_IPSP                  (0x0023)
#define BLE_L2CAP_CID_CB_MIN                (0x0040)
#define BLE_L2CAP_CID_CB_MAX                (0x007f)
/** @} */

/**
 * @name    ATT protocol opcodes
 * @{
 */
#define BLE_ATT_ERROR_RESP                  (0x01)
#define BLE_ATT_MTU_REQ                     (0x02)
#define BLE_ATT_MTU_RESP                    (0x03)
#define BLE_ATT_FIND_INFO_REQ               (0x04)
#define BLE_ATT_FIND_INFO_RESP              (0x05)
#define BLE_ATT_FIND_BY_VAL_REQ             (0x06)
#define BLE_ATT_FIND_BY_VAL_RESP            (0x07)
#define BLE_ATT_READ_BY_TYPE_REQ            (0x08)
#define BLE_ATT_READ_BY_TYPE_RESP           (0x09)
#define BLE_ATT_READ_REQ                    (0x0a)
#define BLE_ATT_READ_RESP                   (0x0b)
#define BLE_ATT_READ_BLOB_REQ               (0x0c)
#define BLE_ATT_READ_BLOB_RESP              (0x0d)
#define BLE_ATT_READ_MUL_REQ                (0x0e)
#define BLE_ATT_READ_MUL_RESP               (0x0f)
#define BLE_ATT_READ_BY_GROUP_TYPE_REQ      (0x10)
#define BLE_ATT_READ_BY_GROUP_TYPE_RESP     (0x11)
#define BLE_ATT_WRITE_REQ                   (0x12)
#define BLE_ATT_WRITE_RESP                  (0x13)
#define BLE_ATT_WRITE_COMMAND               (0x52)
#define BLE_ATT_PREP_WRITE_REQ              (0x16)
#define BLE_ATT_PREP_WRITE_RESP             (0x17)
#define BLE_ATT_EXEC_WRITE_REQ              (0x18)
#define BLE_ATT_EXEC_WRITE_RESP             (0x19)
#define BLE_ATT_VAL_NOTIFICATION            (0x1b)
#define BLE_ATT_VAL_INDICATION              (0x1d)
#define BLE_ATT_VAL_CONFIRMATION            (0x1e)
#define BLE_ATT_SIGNED_WRITE_CMD            (0xd2)
/** @} */

/**
 * @name    ATT protocol error codes
 * @{
 */
#define BLE_ATT_INVALID_HANDLE              (0x01)
#define BLE_ATT_READ_NOT_PERMITTED          (0x02)
#define BLE_ATT_WRITE_NOT_PERMITTED         (0x03)
#define BLE_ATT_INVALID_PDU                 (0x04)
#define BLE_ATT_INSUFFICIENT_AUTHEN         (0x05)
#define BLE_ATT_REQUEST_NOT_SUP             (0x06)
#define BLE_ATT_INVALID_OFFSET              (0x07)
#define BLE_ATT_INSUFFICIENT_AUTHOR         (0x08)
#define BLE_ATT_PREPARE_QUEUE_FULL          (0x09)
#define BLE_ATT_ATTRIBUTE_NOT_FOUND         (0x0a)
#define BLE_ATT_ATTRIBUTE_NOT_LONG          (0x0b)
#define BLE_ATT_INSUFFICENT_KEY_SIZE        (0x0c)
#define BLE_ATT_INVALID_ATTR_VAL_LEN        (0x0d)
#define BLE_ATT_ULIKELY_ERROR               (0x0e)
#define BLE_ATT_INSUFFICIENT_ENCRYPTION     (0x0f)
#define BLE_ATT_UNSUPPORTED_GROUP_TYPE      (0x10)
#define BLE_ATT_INSUFFICIENT_RESSOURCES     (0x11)
/** @} */

/**
 * @name    ATT property flags
 * @{
 */
#define BLE_ATT_BROADCAST                   (0x01)
#define BLE_ATT_READ                        (0x02)
#define BLE_ATT_WRITE_WO_RESP               (0x04)
#define BLE_ATT_WRITE                       (0x08)
#define BLE_ATT_NOTIFY                      (0x10)
#define BLE_ATT_INDICATE                    (0x20)
#define BLE_ATT_AUTH_SIGNED_WRITES          (0x40)
#define BLE_ATT_EXT_PROPERTIES              (0x80)
/** @} */

/**
 * @name    Flags used in certain types of ATT PDUs
 * @{
 */
#define BLE_ATT_FORMAT_U16      (0x01)  /**< used in FIND_INFO_RESP */
#define BLE_ATT_FORMAT_U128     (0x02)  /**< used in FIND_INFO_RESP */
/** @} */

/**
 * @name    GAP advertisement data type values
 *
 * @see https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
 * @{
 */
#define BLE_GAP_AD_FLAGS                    (0x01)
#define BLE_GAP_AD_UUID16_INCOMP            (0x02)
#define BLE_GAP_AD_UUID16_COMP              (0x03)
#define BLE_GAP_AD_UUID32_INCOMP            (0x04)
#define BLE_GAP_AD_UUID32_COMP              (0x05)
#define BLE_GAP_AD_UUID128_INCOMP           (0x06)
#define BLE_GAP_AD_UUID128_COMP             (0x07)
#define BLE_GAP_AD_NAME_SHORT               (0x08)
#define BLE_GAP_AD_NAME                     (0x09)
#define BLE_GAP_AD_TX_POWER_LEVEL           (0x0a)
#define BLE_GAP_AD_CLASS_OF_DEVICE          (0x0d)
#define BLE_GAP_AD_PAIRING_HASH_192         (0x0e)
#define BLE_GAP_AD_PAIRING_RAND_192         (0x0f)
#define BLE_GAP_AD_DEVICE_ID                (0x10)
#define BLE_GAP_AD_SEC_MANAGER_TK_VAL       (0x10)
#define BLE_GAP_AD_SEC_MANAGER_OOB_FLAGS    (0x11)
#define BLE_GAP_AD_SLAVE_CON_INTERVAL       (0x12)
#define BLE_GAP_AD_LIST_SOL_UUID_16         (0x14)
#define BLE_GAP_AD_LIST_SOL_UUID_128        (0x15)
#define BLE_GAP_AD_SERVICE_DATA             (0x16)
#define BLE_GAP_AD_SERVICE_DATA_UUID16      (0x16)
#define BLE_GAP_AD_ADDR_PUBLIC              (0x17)
#define BLE_GAP_AD_ADDR_RANDOM              (0x18)
#define BLE_GAP_AD_APPEARANCE               (0x19)
#define BLE_GAP_AD_ADV_INTERVAL             (0x1a)
#define BLE_GAP_AD_LE_DEVICE_ADDR           (0x1b)
#define BLE_GAP_AD_LE_ROLE                  (0x1c)
#define BLE_GAP_AD_PAIRING_HASH_256         (0x1d)
#define BLE_GAP_AD_PAIRING_RAND_256         (0x1e)
#define BLE_GAP_AD_LIST_SOL_UUID_32         (0x1f)
#define BLE_GAP_AD_SERVICE_DATA_32          (0x20)
#define BLE_GAP_AD_SERVICE_DATA_128         (0x21)
#define BLE_GAP_AD_LE_SEC_CON_CONF_VAL      (0x22)
#define BLE_GAP_AD_LE_SEC_CON_RAND_VAL      (0x23)
#define BLE_GAP_AD_URI                      (0x24)
#define BLE_GAP_AD_INDOOR_POSITIONING       (0x25)
#define BLE_GAP_AD_TRANSPORT_DISC_DATA      (0x26)
#define BLE_GAP_AD_LE_SUP_FEATURES          (0x27)
#define BLE_GAP_AD_CHAN_MAP_UPDATE_IND      (0x28)
#define BLE_GAP_AD_3D_INFO_DATA             (0x3d)
#define BLE_GAP_AD_VENDOR                   (0xff)
/** @} */

/**
 * @name    Flags used in GAP advertisement packets
 * @{
 */
#define BLE_GAP_DISCOVER_LIM                (0x01)
#define BLE_GAP_DISCOVERABLE                (0x02)
#define BLE_GAP_FLAG_BREDR_NOTSUP           (0x04)
/** @} */

/**
 * @name    BLE advertising packet types
 * @{
 */
#define BLE_PDU_MASK                        (0x0f)
#define BLE_ADV_IND                         (0x00)
#define BLE_DIRECT_IND                      (0x01)
#define BLE_ADV_NONCON_IND                  (0x02)
#define BLE_SCAN_REQ                        (0x03)
#define BLE_AUX_SCAN_REQ                    (0x03)
#define BLE_SCAN_RESP                       (0x04)
#define BLE_CONNECT_IND                     (0x05)
#define BLE_AUX_CONNECT_REQ                 (0x05)
#define BLE_ADV_SCAN_IND                    (0x06)
#define BLE_ADV_EXT_IND                     (0x07)
#define BLE_AUX_ADV_IND                     (0x07)
#define BLE_AUX_SCAN_RSP                    (0x07)
#define BLE_AUX_SYNC_IND                    (0x07)
#define BLE_AUX_CHAIN_IND                   (0x07)
#define BLE_CONNECT_RESP                    (0x08)
/** @} */

/**
 * @name    Advertising packet flags
 * @{
 */
#define BLE_LL_FLAG_CHSEL                   (0x20)
#define BLE_LL_FLAG_TXADD                   (0x40)
#define BLE_LL_FLAG_RXADD                   (0x80)
/** @} */

/**
 * @name    Link layer control message opcodes
 * @{
 */
#define BLE_LL_CONN_UPDATE_IND              (0x00)
#define BLE_LL_CHANNEL_MAP_IND              (0x01)
#define BLE_LL_TERMINATE_IND                (0x02)
#define BLE_LL_ENC_REQ                      (0x03)
#define BLE_LL_ENC_RSP                      (0x04)
#define BLE_LL_START_ENC_REQ                (0x05)
#define BLE_LL_START_ENC_RSP                (0x06)
#define BLE_LL_UNKNOWN_RSP                  (0x07)
#define BLE_LL_FEATURE_REQ                  (0x08)
#define BLE_LL_FEATURE_RSP                  (0x09)
#define BLE_LL_PAUSE_ENC_REQ                (0x0a)
#define BLE_LL_PAUSE_ENC_RSP                (0x0b)
#define BLE_LL_VERSION_IND                  (0x0c)
#define BLE_LL_REJECT_IND                   (0x0d)
#define BLE_LL_SLAVE_FEATURE_REQ            (0x0e)
#define BLE_LL_CONN_PARAM_REQ               (0x0f)
#define BLE_LL_CONN_PARAM_RSP               (0x10)
#define BLE_LL_REJECT_EXT_IND               (0x11)
#define BLE_LL_PING_REQ                     (0x12)
#define BLE_LL_PING_RSP                     (0x13)
#define BLE_LL_LENGTH_REQ                   (0x14)
#define BLE_LL_LENGTH_RSP                   (0x15)
#define BLE_LL_PHY_REQ                      (0x16)
#define BLE_LL_PHY_RSP                      (0x17)
#define BLE_LL_PHY_UPDATE_IND               (0x18)
#define BLE_LL_MIN_USED_CHAN_IND            (0x19)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
