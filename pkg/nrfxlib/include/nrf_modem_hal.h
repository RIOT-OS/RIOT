/*
 * Copyright (C) 2021-2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup    pkg_nrf_modem
 * @{
 *
 * @file
 * @brief       RIOT internal header for nrf_modem glue logic
 *
 * @note        Content of this file was taken from Nordic SDK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef NRF_MODEM_HAL_H
#define NRF_MODEM_HAL_H

#include "cpu.h"
#ifdef __cplusplus
extern "C" {
#endif

/** @brief Base number of error codes. */
#define NRFX_ERROR_BASE_NUM         0x0BAD0000

/** @brief Base number of driver error codes. */
#define NRFX_ERROR_DRIVERS_BASE_NUM (NRFX_ERROR_BASE_NUM + 0x10000)

/** @brief Enumerated type for error codes. */
typedef enum {
    /**< Operation performed successfully. */
    NRFX_SUCCESS                    = (NRFX_ERROR_BASE_NUM + 0),
    /**< Internal error. */
    NRFX_ERROR_INTERNAL             = (NRFX_ERROR_BASE_NUM + 1),
    /**< No memory for operation. */
    NRFX_ERROR_NO_MEM               = (NRFX_ERROR_BASE_NUM + 2),
    /**< Not supported. */
    NRFX_ERROR_NOT_SUPPORTED        = (NRFX_ERROR_BASE_NUM + 3),
    /**< Invalid parameter. */
    NRFX_ERROR_INVALID_PARAM        = (NRFX_ERROR_BASE_NUM + 4),
     /**< Invalid state, operation disallowed in this state. */
    NRFX_ERROR_INVALID_STATE        = (NRFX_ERROR_BASE_NUM + 5),
    /**< Invalid length. */
    NRFX_ERROR_INVALID_LENGTH       = (NRFX_ERROR_BASE_NUM + 6),
    /**< Operation timed out. */
    NRFX_ERROR_TIMEOUT              = (NRFX_ERROR_BASE_NUM + 7),
    /**< Operation is forbidden. */
    NRFX_ERROR_FORBIDDEN            = (NRFX_ERROR_BASE_NUM + 8),
     /**< Null pointer. */
    NRFX_ERROR_NULL                 = (NRFX_ERROR_BASE_NUM + 9),
    /**< Bad memory address. */
    NRFX_ERROR_INVALID_ADDR         = (NRFX_ERROR_BASE_NUM + 10),
     /**< Busy. */
    NRFX_ERROR_BUSY                 = (NRFX_ERROR_BASE_NUM + 11),
    /**< Module already initialized. */
    NRFX_ERROR_ALREADY_INITIALIZED  = (NRFX_ERROR_BASE_NUM + 12),
    /**< TWI error: Overrun. */
    NRFX_ERROR_DRV_TWI_ERR_OVERRUN  = (NRFX_ERROR_DRIVERS_BASE_NUM + 0),
    /**< TWI error: Address not acknowledged. */
    NRFX_ERROR_DRV_TWI_ERR_ANACK    = (NRFX_ERROR_DRIVERS_BASE_NUM + 1),
    /**< TWI error: Data not acknowledged. */
    NRFX_ERROR_DRV_TWI_ERR_DNACK    = (NRFX_ERROR_DRIVERS_BASE_NUM + 2)
} nrfx_err_t;

/**
 * @brief IPC driver handler type.
 *
 * @param[in] event_mask Bitmask with events that triggered the interrupt.
 * @param[in] p_context  Context passed to the interrupt handler, set on initialization.
 */
typedef void (*nrfx_ipc_handler_t)(uint32_t event_mask, void * p_context);

/** @brief IPC configuration structure. */
typedef struct {
    /**< Configuration of the connection between signals and IPC channels. */
    uint32_t send_task_config[IPC_CONF_NUM];
    /**< Configuration of the connection between events and IPC channels. */
    uint32_t receive_event_config[IPC_CONF_NUM];
    /**< Bitmask with events to be enabled to generate interrupt. */
    uint32_t receive_events_enabled;
} nrfx_ipc_config_t;

#ifdef __cplusplus
}
#endif

#endif /* NRF_MODEM_HAL_H */
/** @} */