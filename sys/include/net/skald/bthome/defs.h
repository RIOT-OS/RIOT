/*
 * SPDX-FileCopyrightText: 2024 Martine S. Lenders
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    ble_skald_bthome_defs BTHome definitions
 * @ingroup     ble_skald_bthome
 * @brief       Numbers for BTHome
 *
 * @{
 *
 * @file
 * @brief   BTHome definitions interface
 *
 * @author  Martine S. Lenders <mail@martine-lenders.eu>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   BTHome object IDs
 *
 * _v_ is the value sent, with the type in parenthesis.
 */
typedef enum {
    /* ==================== 0x00 ==================== */
    BTHOME_ID_PACKET_ID = 0x00,                 /**< Packet ID as _v_ (uint8_t) */
    BTHOME_ID_BATTERY = 0x01,                   /**< Battery in _v_ % (uint8_t) */
    BTHOME_ID_TEMPERATURE_FACTOR_0_01 = 0x02,   /**< Temperature in _v_ * 0.01 °C (int16_t) */
    BTHOME_ID_HUMIDITY_FACTOR_0_01 = 0x03,      /**< Humidity in _v_ * 0.01 % (uint16_t) */
    BTHOME_ID_PRESSURE_FACTOR_0_01 = 0x04,      /**< Pressure in _v_ * 0.01 hPa (uint24_t) */
    BTHOME_ID_ILLUMINANCE_FACTOR_0_01 = 0x05,   /**< Illuminance in _v_ * 0.01 lux (uint24_t) */
    BTHOME_ID_MASS_KG_FACTOR_0_01 = 0x06,       /**< Mass in _v_ * 0.01 kg (uint16_t) */
    BTHOME_ID_MASS_LB_FACTOR_0_01 = 0x07,       /**< Mass in _v_ * 0.01 lb (uint16_t) */
    BTHOME_ID_DEWPOINT_0_01 = 0x08,             /**< Dewpoint in _v_ * 0.01 °C (int16_t) */
    BTHOME_ID_COUNT_UINT8 = 0x09,               /**< Count as _v_ (uint8_t) */
    BTHOME_ID_ENERGY_3B_FACTOR_0_001 = 0x0a,    /**< Energy in _v_ * 0.001 kWh (uint24_t) */
    BTHOME_ID_POWER_UINT_FACTOR_0_01 = 0x0b,    /**< Power in _v_ * 0.01 W (uint24_t) */
    BTHOME_ID_VOLTAGE_FACTOR_0_001 = 0x0c,      /**< Voltage in _v_ * 0.001 V (uint16_t) */
    /**
     * @brief PM2.5 (very fine particular matter) in _v_ ug/m3 (uint16_t)
     */
    BTHOME_ID_PM_2_5 = 0x0d,
    /**
     * @brief PM10 (fine particular matter) in _v_ ug/m3 (uint16_t)
     */
    BTHOME_ID_PM_10 = 0x0e,
    /**
     * @brief Generic boolean (uint8_t)
     *
     * 0 = False, 1 = True
     */
    BTHOME_ID_BOOLEAN = 0x0f,
    /* ==================== 0x10 ==================== */
    /**
     * @brief Power as binary state (uint8_t)
     *
     * 0 = Off, 1 = On
     */
    BTHOME_ID_POWER_BINARY = 0x10,
    /**
     * @brief Opening as binary state (uint8_t)
     *
     * 0 = Closed, 1 = Open
     */
    BTHOME_ID_OPENING_BINARY = 0x11,
    BTHOME_ID_CO2 = 0x12,                       /**< CO2 in _v_ ppm (uint16_t) */
    BTHOME_ID_TVOC = 0x13,                      /**< TVOC in _v_ ug/m3 (uint16_t) */
    BTHOME_ID_MOISTURE_FACTOR_0_01 = 0x14,      /**< Moisture in _v_ * 0.01 % (uint16_t) */
    /**
     * @brief Battery as binary state (uint8_t)
     *
     * 0 = Normal, 1 = Low
     */
    BTHOME_ID_BATTERY_BINARY = 0x15,
    /**
     * @brief Battery charging as binary state (uint8_t)
     *
     * 0 = Not charging, 1 = Charging
     */
    BTHOME_ID_BATTERY_CHARGING_BINARY = 0x16,
    /**
     * @brief Carbon monoxid as binary state (uint8_t)
     *
     * 0 = Not detected, 1 = Detected
     */
    BTHOME_ID_CARBON_MONOXIDE_BINARY = 0x17,
    /**
     * @brief Cold as binary state (uint8_t)
     *
     * 0 = Normal, 1 = Cold
     */
    BTHOME_ID_COLD_BINARY = 0x18,
    /**
     * @brief Connectivity as binary state (uint8_t)
     *
     * 0 = Disconnected, 1 = Connected
     */
    BTHOME_ID_CONNECTIVITY_BINARY = 0x19,
    /**
     * @brief Door as binary state (uint8_t)
     *
     * 0 = Closed, 1 = Open
     */
    BTHOME_ID_DOOR_BINARY = 0x1a,
    /**
     * @brief Garage door as binary state (uint8_t)
     *
     * 0 = Closed, 1 = Open
     */
    BTHOME_ID_GARAGE_DOOR_BINARY = 0x1b,
    /**
     * @brief Gas as binary state (uint8_t)
     *
     * 0 = Clear, 1 = Detected
     */
    BTHOME_ID_GAS_BINARY = 0x1c,
    /**
     * @brief Heat as binary state (uint8_t)
     *
     * 0 = Normal, 1 = Hot
     */
    BTHOME_ID_HEAT_BINARY = 0x1d,
    /**
     * @brief Light as binary state (uint8_t)
     *
     * 0 = No light, 1 = Light detected
     */
    BTHOME_ID_LIGHT_BINARY = 0x1e,
    /**
     * @brief Lock as binary state (uint8_t)
     *
     * 0 = Locked, 1 = Unlocked
     */
    BTHOME_ID_LOCKED_BINARY = 0x1f,
    /* ==================== 0x20 ==================== */
    /**
     * @brief Moisture as binary state (uint8_t)
     *
     * 0 = Dry, 1 = Wet
     */
    BTHOME_ID_MOISTURE_BINARY = 0x20,
    /**
     * @brief Motion as binary state (uint8_t)
     *
     * 0 = Clear, 1 = Detected
     */
    BTHOME_ID_MOTION_BINARY = 0x21,
    /**
     * @brief Moving as binary state (uint8_t)
     *
     * 0 = Not moving, 1 = Moving
     */
    BTHOME_ID_MOVING_BINARY = 0x22,
    /**
     * @brief Occupancy as binary state (uint8_t)
     *
     * 0 = Clear, 1 = Detected
     */
    BTHOME_ID_OCCUPANCY_BINARY = 0x23,
    /**
     * @brief Plug as binary state (uint8_t)
     *
     * 0 = Unplugged, 1 = Plugged in
     */
    BTHOME_ID_PLUG_BINARY = 0x24,
    /**
     * @brief Presence as binary state (uint8_t)
     *
     * 0 = Away, 1 = Home
     */
    BTHOME_ID_PRESENCE_BINARY = 0x25,
    /**
     * @brief Problem as binary state (uint8_t)
     *
     * 0 = OK, 1 = Problem
     */
    BTHOME_ID_PROBLEM_BINARY = 0x26,
    /**
     * @brief Running as binary state (uint8_t)
     *
     * 0 = Not running, 1 = Running
     */
    BTHOME_ID_RUNNING_BINARY = 0x27,
    /**
     * @brief Safety as binary state (uint8_t)
     *
     * 0 = Unsafe, 1 = Safe
     */
    BTHOME_ID_SAFETY_BINARY = 0x28,
    /**
     * @brief Smoke as binary state (uint8_t)
     *
     * 0 = Clear, 1 = Detected
     */
    BTHOME_ID_SMOKE_BINARY = 0x29,
    /**
     * @brief Sound as binary state (uint8_t)
     *
     * 0 = Clear, 1 = Detected
     */
    BTHOME_ID_SOUND_BINARY = 0x2a,
    /**
     * @brief Tamper as binary state (uint8_t)
     *
     * 0 = Off, 1 = On
     */
    BTHOME_ID_TAMPER_BINARY = 0x2b,
    /**
     * @brief Vibration as binary state (uint8_t)
     *
     * 0 = Clear, 1 = Detected
     */
    BTHOME_ID_VIBRATION_BINARY = 0x2c,
    /**
     * @brief Window as binary state (uint8_t)
     *
     * 0 = Closed, 1 = Open
     */
    BTHOME_ID_WINDOW_BINARY = 0x2d,
    BTHOME_ID_HUMIDITY_FACTOR_1 = 0x2e,         /**< Humidity in _v_ % (uint8_t) */
    BTHOME_ID_MOISTURE_FACTOR_1 = 0x2f,         /**< Moisture in _v_ % (uint8_t) */
    /* ==================== 0x30 ==================== */
    /**
     * @brief Button as _v_ (uint8_t)
     *
     * @see @ref skald_bthome_btn_t for value of _v_
     */
    BTHOME_ID_BUTTON = 0x3a,
    /**
     * @brief Dimmer as _v_ (uint16_t)
     *
     * 0x00XX for none, 0x01XX for XX steps left, 0x02XX for XX steps right
     */
    BTHOME_ID_DIMMER = 0x3c,
    BTHOME_ID_COUNT_UINT16 = 0x3d,              /**< Count as _v_ (uint16_t) */
    BTHOME_ID_COUNT_UINT32 = 0x3e,              /**< Count as _v_ (uint32_t) */
    BTHOME_ID_ROTATION_FACTOR_0_1 = 0x3f,       /**< Rotation in _v_ (int16_t) */
    /* ==================== 0x40 ==================== */
    BTHOME_ID_DISTANCE_MM = 0x40,               /**< Distance in _v_ mm (uint16_t) */
    BTHOME_ID_DISTANCE_M = 0x41,                /**< Distance in _v_ m (uint16_t) */
    BTHOME_ID_DURATION_FACTOR_0_001 = 0x42,     /**< Distance in _v_ m (uint16_t) */
    BTHOME_ID_CURRENT_UINT_FACTOR_0_001 = 0x43, /**< Current in _v_ * 0.001 A (uint16_t) */
    BTHOME_ID_SPEED_FACTOR_0_01 = 0x44,         /**< Speed in _v_ * 0.01 m/s (uint16_t) */
    BTHOME_ID_TEMPERATURE_FACTOR_0_1 = 0x45,    /**< Temperature in _v_ * 0.1 °C (int16_t) */
    BTHOME_ID_UV_INDEX_FACTOR_0_1 = 0x44,       /**< Volume in _v_ * 0.1 (uint8_t) */
    BTHOME_ID_VOLUME_L_FACTOR_0_1 = 0x47,       /**< Volume in _v_ * 0.1 l (uint16_t) */
    BTHOME_ID_VOLUME_ML_FACTOR_0_1 = 0x48,      /**< Volume in _v_ * 0.1 ml (uint16_t) */
    /**
     * @brief Volume flow rate in _v_ * 0.001 m3/h (uint16_t)
     */
    BTHOME_ID_VOLUME_FLOW_RATE_FACTOR_0_001 = 0x49,
    BTHOME_ID_VOLTAGE_FACTOR_0_1 = 0x4a,        /**< Voltage in _v_ * 0.1 V (uint16_t) */
    BTHOME_ID_GAS_3B_FACTOR_0_001 = 0x4b,       /**< Gas in _v_ * 0.001 m3 (uint24_t) */
    BTHOME_ID_GAS_4B_FACTOR_0_001 = 0x4c,       /**< Gas in _v_ * 0.001 m3 (uint32_t) */
    BTHOME_ID_ENERGY_4B_FACTOR_0_001 = 0x4d,    /**< Energy in _v_ * 0.001 kWh (uint32_t) */
    BTHOME_ID_VOLUME_L_FACTOR_0_001 = 0x4e,     /**< Volume in _v_ * 0.001 l (uint32_t) */
    BTHOME_ID_WATER_FACTOR_0_001 = 0x4f,        /**< Water in _v_ * 0.001 l (uint32_t) */
    /* ==================== 0x50 ==================== */
    BTHOME_ID_TIMESTAMP = 0x50,                 /**< Timestamp in _v_ s since UNIX epoch (uint48) */
    BTHOME_ID_ACCELERATION_FACTOR_0_001 = 0x51, /**< Acceleration in _v_ m/s² */
    BTHOME_ID_GYROSCOPE_FACTOR_0_001 = 0x52,    /**< Gyroscope in _v_ °/s */
    /**
     * @brief Volume storage in _v_ * 0.001 l (uint32_t)
     */
    BTHOME_ID_VOLUME_STORAGE_FACTOR_0_001 = 0x55,
    BTHOME_ID_TEMPERATURE_FACTOR_1 = 0x57,      /**< Temperature in _v_ °C (int8_t) */
    BTHOME_ID_TEMPERATURE_FACTOR_0_35 = 0x58,   /**< Temperature in _v_ * 0.35 °C (int8_t) */
    BTHOME_ID_COUNT_SINT8 = 0x59,               /**< Count as _v_ (int8_t) */
    BTHOME_ID_COUNT_SINT16 = 0x5a,              /**< Count as _v_ (int16_t) */
    BTHOME_ID_COUNT_SINT32 = 0x5b,              /**< Count as _v_ (int32_t) */
    BTHOME_ID_POWER_SINT_FACTOR_0_01 = 0x5c,    /**< Power in _v_ * 0.01 W (int32_t) */
    BTHOME_ID_CURRENT_SINT_FACTOR_0_001 = 0x5d, /**< Current in _v_ * 0.001 A (int16_t) */
    BTHOME_ID_TEXT = 0x53,                      /**< Text as _v_ (bytes) */
    BTHOME_ID_RAW = 0x54,                       /**< Raw as _v_ (bytes) */
    BTHOME_ID_CONDUCTIVITY = 0x56,              /**< Conductivity in _v_ uS/cm (uint16_t) */
    /* ==================== 0xf0 ==================== */
    BTHOME_ID_DEVICE_TYPE_ID = 0xf0,            /**< Device type id as _v_ (uint16_t) */
    /**
     * @brief Firmware version as _v_ (4 bytes, uint32_t)
     *
     * _v_ is interpreted as v[3].v[2].[1].[0]
     */
    BTHOME_ID_FIRMWARE_VERSION_4B_ID = 0xf1,
    /**
     * @brief Firmware version as _v_ (3 bytes, uint32_t)
     *
     * _v_ is interpreted as v[2].[1].[0]
     */
    BTHOME_ID_FIRMWARE_VERSION_3B_ID = 0xf2,
} skald_bthome_id_t;

/**
 * @brief Value for button.
 *
 * @see @ref BTHOME_ID_BUTTON.
 */
typedef enum {
    BTHOME_BTN_NONE = 0x00,                 /**< No button press */
    BTHOME_BTN_PRESS = 0x01,                /**< Press */
    BTHOME_BTN_DOUBLE_PRESS = 0x02,         /**< Double press */
    BTHOME_BTN_TRIPLE_PRESS = 0x03,         /**< Triple press */
    BTHOME_BTN_LONG_PRESS = 0x04,           /**< Long press */
    BTHOME_BTN_LONG_DOUBLE_PRESS = 0x05,    /**< Long double press */
    BTHOME_BTN_LONG_TRIPLE_PRESS = 0x05,    /**< Long triple press */
    BTHOME_BTN_HOLD_PRESS = 0x80,           /**< Hold press */
} skald_bthome_btn_t;

#ifdef __cplusplus
}
#endif

/** @} */
