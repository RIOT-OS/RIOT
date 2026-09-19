/*
 * Copyright (C) 2016 Antonio Galea <antonio.galea@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_ata8510
 * @{
 *
 * @file
 * @brief       Internal interfaces for ATA8510 driver
 *
 * @author      Antonio Galea <antonio.galea@gmail.com>
 * @author      Roberto Asquini <bobasquins@gmail.com>
 */

#ifndef ATA8510_INTERNALS_H
#define ATA8510_INTERNALS_H

#include <stdint.h>

#include "ata8510.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATA8510_MAX_PACKET_LEN  (255)

/**
 * @brief   Constant part number of the ATA8510
 * @{
 */
#define ATA8510_PARTNUM       (0x21)
/** @} */

/**
 * @brief SPI commands for ATA8510
 * @{
 */
#define ATA8510_CMD_READ_FILL_LEVEL_RX_FIFO     (0x01)
#define ATA8510_CMD_READ_FILL_LEVEL_TX_FIFO     (0x02)
#define ATA8510_CMD_READ_FILL_LEVEL_RSSI_FIFO   (0x03)
#define ATA8510_CMD_GET_EVENT_BYTES             (0x04)
#define ATA8510_CMD_READ_RSSI_FIFO              (0x05)
#define ATA8510_CMD_READ_RX_FIFO                (0x06)
#define ATA8510_CMD_WRITE_SRAM_REGISTER         (0x07)
#define ATA8510_CMD_READ_SRAM_REGISTER          (0x08)
#define ATA8510_CMD_WRITE_EEPROM                (0x09)
#define ATA8510_CMD_READ_EEPROM                 (0x0A)
#define ATA8510_CMD_WRITE_TX_FIFO               (0x0B)
#define ATA8510_CMD_WRITE_TX_PREAMBLE_FIFO      (0x0C)
#define ATA8510_CMD_SET_SYSTEM_MODE             (0x0D)
#define ATA8510_CMD_GET_VERSION_FLASH           (0x13)
#define ATA8510_CMD_SYSTEM_RESET                (0x15)
#define ATA8510_CMD_OFF_COMMAND                 (0x18)
/** @} */

/**
 * @brief   FIFO lengths
 * @{
 */
#define ATA8510_SFIFO_LENGTH        (16)
#define ATA8510_DFIFO_RX_LENGTH     (32)
#define ATA8510_DFIFO_TX_LENGTH     (32)
/** @} */

/**
 * @brief   Event bytes - system flags
 * @{
 */
typedef union {
    struct {
        uint8_t dfifo_tx : 1,
                dfifo_rx : 1,
                sfifo : 1,
                lowbatt : 1,
                avcclow : 1,
                sys_rdy : 1,
                cmd_rdy : 1,
                sys_err : 1;
    } val;                      /**< byte access */
    uint8_t bits;               /**< bit access */
} ata8510_events_system_t;
/** @} */

/**
 * @brief   Event bytes - events flags
 * @{
 */
typedef union {
    struct {
        uint8_t eotb : 1,
                sotb : 1,
                wcokb : 1,
                idchkb : 1,
                eota : 1,
                sota : 1,
                wcoka : 1,
                idchka : 1;
    } val;                      /**< byte access */
    uint8_t bits;               /**< bit access */
} ata8510_events_events_t;
/** @} */

/**
 * @brief   Event bytes - power flags
 * @{
 */
typedef union {
    struct {
        uint8_t npwron1 : 1,
                npwron2 : 1,
                npwron3 : 1,
                npwron4 : 1,
                npwron5 : 1,
                npwron6 : 1,
                : 1,
                pwron : 1;
    } val;                      /**< byte access */
    uint8_t bits;               /**< bit access */
} ata8510_events_power_t;
/** @} */

/**
 * @brief   Event bytes - config flags
 * @{
 */
typedef union {
    struct {
        uint8_t ser : 3,
                : 1,
                ch : 2,
                patha : 1,
                pathb : 1;
    } val;                      /**< byte access */
    uint8_t bits;               /**< bit access */
} ata8510_events_config_t;
/** @} */

/**
 * @brief   Event bytes structure - internal radio state
 * @{
 */
typedef struct __attribute__((packed)) {
    ata8510_events_system_t system; /**< system flags */
    ata8510_events_events_t events; /**< events flags */
    ata8510_events_power_t power;   /**< power flags */
    ata8510_events_config_t config; /**< config flags */
} ata8510_events_t;
/** @} */

/**
 * @brief   ATA8510_CMD_SET_SYSTEM_MODE
 *          systemModeConfig
 * @{
 */
typedef struct {
    uint8_t opm : 2,                /**< operating mode */
            tmden : 1,              /**< transparent mode data enable */
            : 1,
            idle_mode_selector : 1, /**< idle mode selector */
            vco_tune : 1,           /**< VCO tune */
            ant_tune : 1,           /**< antenna tune */
            rf_cal : 1;             /**< RF calibration */
} ata8510_system_mode_config_t;

typedef enum {
    ATA8510_MODE_IDLE,
    ATA8510_MODE_TX,
    ATA8510_MODE_RX,
    ATA8510_MODE_RX_POLLING,
} ata8510_system_mode_config_opm_t;
/** @} */

/**
 * @brief   ATA8510_CMD_SET_SYSTEM_MODE
 *          serviceChannelConfig
 * @{
 */
typedef struct {
    uint8_t ser : 3,                /**< service */
            : 1,
            ch : 2,                 /**< channel */
            ena_path_a : 1,         /**< enable path A */
            ena_path_b : 1;         /**< enable path B */
} ata8510_system_channel_config_t;

typedef enum {
    ATA8510_CHANNEL_0,
    ATA8510_CHANNEL_1,
    ATA8510_CHANNEL_2,
} ata8510_channel_t;

typedef enum {
    ATA8510_SERVICE_0,
    ATA8510_SERVICE_1,
    ATA8510_SERVICE_2,
    ATA8510_SERVICE_3,
} ata8510_service_t;
/** @} */

/**
 * @brief RSSI measures
 * @{
 */
typedef struct {
    uint8_t data[ATA8510_SFIFO_LENGTH];  /**< samples for computing RSSI */
    uint8_t len;                         /**< count of acquired samples */
} ata8510_rssi_t;
#define ATA8150_DBM_UNDEF  (999)
/** @} */

/**
 * @brief internal registers used to regulate output power
 * @{
 */
#define ATA8510_EEPROM_FEPAC        (0x00CE)
#define ATA8510_SRAM_PA_DAMPING     (0x02F4)
/** @} */

/**
 * @brief delay to let the radio perform its initialization at poweron
 * @{
 */
#define ATA8510_POWERON_DELAY 310

#ifdef __cplusplus
}
#endif

#endif /* ATA8510_INTERNALS_H */
