/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx RF Core common commands definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_RFC_COMMON_CMD_H
#define CC26XX_CC13XX_RFC_COMMON_CMD_H

#include <stdint.h>

#include "cc26xx_cc13xx_rfc_mailbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Direct commands
 * @{
 */
#define RFC_CMD_STOP (0x0402) /**< Stop a running command */
/** @} */

/**
 * @brief   Radio operation start trigger
 */
typedef struct {
    uint8_t type:4; /**< The type of trigger */
    uint8_t ena_cmd:1; /**< 0h = No alternative trigger command,
                            1h = CMD_TRIGGER can be used as an alternative
                            trigger */
    uint8_t trigger_no:2; /**< The trigger number of the CMD_TRIGGER command that
                               triggers this action */
    uint8_t past_trig:1; /**< 0: A trigger in the past is never triggered, or for
                              start of commands, give an error */
} rfc_trigger_t;
/**
 * @brief   Condition for running next operation
 */
typedef struct {
    uint8_t rule:4; /**< Rule for how to proceed */
    uint8_t skip_no:4; /**< Skip number + 1 if the rule involves skipping */
} rfc_cond_t;

/**
 * @brief   General radio operation.
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
} rfc_op_t;

/**
 * @brief   CMD_FS
 * @{
 */
#define RFC_CMD_FS (0x0803) /**< CMD_FS command ID */
/**
 * @brief   Tune frequency synthesizer
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    uint16_t frequency; /**< The frequency in MHz to tune to*/
    uint16_t fract_freq; /**< Fractional part of the frequency to tune to */
    struct {
        uint8_t txmode:1; /**< 0h = Start synth in RX mode, 1h = Start synth in
                               TX mode */
        uint8_t ref_freq:6; /**< 0h = Use default reference frequenc, others =
                                 Use reference frequency 48 MHz */
    } synth_conf;
    uint8_t __dummy0; /**< Reserved, always write 0 */
    uint8_t __dummy1; /**< Reserved */
    uint8_t __dummy2; /**< Reserved */
    uint16_t __dummy3; /**< Reserved */
} rfc_cmd_fs_t;
/** @} */

/**
 * @brief   CMD_FS
 * @{
 */
#define RFC_CMD_FS_POWERDOWN (0x080D) /**< CMD_FS command ID */
/**
 * @brief   Powerdown frequency synthesizer
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
} rfc_cmd_fs_powerdown_t;
/** @} */

/**
 * @brief   CMD_SYNC_STOP_RAT
 * @{
 */
#define RFC_CMD_SYNC_STOP_RAT (0x0809) /**< CMD_SYNC_STOP_RAT command ID */
/**
 * @brief   Synchronize and Stop Radio Timer Command
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    uint16_t __dummy0; /**< Reserved */
    rfc_ratmr_t rat0; /**< The returned RAT timer value corresponding to the
                           value the RAT would have had when the RTC was zero */
} rfc_cmd_sync_stop_rat_t;
/** @} */

/**
 * @brief   CMD_SYNC_START_RAT
 * @{
 */
#define RFC_CMD_SYNC_START_RAT (0x080A) /**< CMD_SYNC_STOP_RAT command ID */
//! Synchrously Start Radio Timer Command
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    uint16_t __dummy0; /**< Reserved */
    rfc_ratmr_t rat0; /**< The desired RAT timer value corresponding to the
                           value the RAT would have had when the RTC was zero.
                           This parameter is returned by CMD_SYNC_STOP_RAT */
} rfc_cmd_sync_start_rat_t;
/** @} */

/**
 * @brief   CMD_SET_TX_POWER
 * @{
 */
#define RFC_CMD_SET_TX_POWER (0x0010)
/**
 * @brief   Set Transmit Power
 */
typedef struct  {
   uint16_t command_no; /**< The command ID number */
   uint16_t tx_power; /**< New TX power setting */
} rfc_cmd_set_tx_power_t;
/** @} */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* CC26XX_CC13XX_RFC_COMMON_CMD_H */

/*@}*/
