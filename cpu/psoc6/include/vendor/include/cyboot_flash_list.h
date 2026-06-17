/***************************************************************************//**
* \file cyboot_flash_list.h
* \version 1.0.0
* Provides header file for a flash programming API.
********************************************************************************
* \copyright
* (c) 2024, Cypress Semiconductor Corporation (an Infineon company) or an
* affiliate of Cypress Semiconductor Corporation.  All rights reserved.
* This software, associated documentation and materials ("Software") is owned
* by Cypress Semiconductor Corporation or one of its affiliates ("Cypress") and
* is protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you obtained this
* Software ("EULA"). If no EULA applies, then any reproduction, modification,
* translation, compilation, or representation of this Software is prohibited
* without the express written permission of Cypress.
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice.
* Cypress does not assume any liability arising out of the application or use
* of the Software or any product or circuit described in the Software. Cypress
* does not authorize its products for use in any products where a malfunction
* or failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product").
* By including Cypress's product in a High Risk Product, the manufacturer of
* such system or application assumes all risk of such use and in doing so
* agrees to indemnify Cypress against all liability.
*******************************************************************************/

#ifndef CY_FB_FLASH_API_H
#define CY_FB_FLASH_API_H

#include <stdint.h>
#include <stdbool.h>
/** \cond INTERNAL */
/* PSOC C3 specific flash geometry */
#define CYBOOT_FLASH_SECTOR_SIZE        (128UL*1024UL)
#define CYBOOT_N_FLASH_SECTORS          (2UL)
#define CYBOOT_N_ROWS_PER_SECTOR        (CYBOOT_FLASH_SECTOR_SIZE / CY_FLASH_ROW_SIZE)
#define CYBOOT_N_SCRATCH_ROWS           (2UL)

#define CYBOOT_FLASH_REFRESH_TAG        (0xEF39A49AUL) /* A value from ./build/hamming_const/<...>.txt */
#define CY_FLASH_ROW_SIZE               (512UL)
#define CYBOOT_FLASH_COLUMN33_SIZE      (16UL)

#define SFLASH_BASE_ADDR                (0x33400000UL)
#define CYBOOT_SCRATCH_BASE_ADDR        (0x33800000UL)

#define CYBOOT_FLASH_COL33_BASE_ADDR    (0x33A00000UL)
#define CYBOOT_SFLASH_COL33_BASE_ADDR   (0x33B00000UL)
#define CYBOOT_SCRATCH_COL33_BASE_ADDR  (0x33B80000UL)

/* Bit-field values in cyboot_flash_context_t.flags */
#define CY_FLASH_MODE_BLOCKING          (1UL << 0UL)
#define CY_FLASH_MODE_STALL_READ        (1UL << 1UL)
#define CY_FLASH_MODE_REFRESH_MANUAL    (1UL << 2UL)

/* Flash API error codes */
#define CYBOOT_FLASH_SUCCESS            (0x0D50B002UL)  /* A success */
#define CYBOOT_FLASH_FAILED             (0xBAF00008UL)  /* A generic failure */
#define CYBOOT_FLASH_RECOVER_ERR        (0xBAF00009UL)  /* A flash recover failure */
#define CYBOOT_FLASH_INIT_FAILED        (0xBAF0004BUL) /** Flash init error */
#define CYBOOT_FLASH_ADDR_INVALID       (0xBAF0004CUL) /** Flash address is invalid */
#define CYBOOT_FLASH_PARAM_INVALID      (0xBAF0004DUL) /** Param is invalid */
#define CYBOOT_FAULT_UNEXPECTED         (0xBAF0004EUL) /** Flash ECC NC fault happened */

typedef struct
{
    uint32_t min_count;
    uint32_t max_count;
    uint32_t min_page_addr;
    uint32_t scratch_row_idx;
} cyboot_flash_refresh_t;

typedef void (*cyboot_flash_callback_t)(void * ctx);
typedef struct
{
    /*
    * [0] 0 -BLOCKING, 1-NON-BLOCKING
    * [1] RWW or STALL_READ, for BLOCKING
    * [2] REFRESH_CTL, determines who provides the data for Column 33.
    *   0- Flash API computes the data for column 33.
    *   1- column 33 is programmed as is. Up to a caller to provide the data.
    */
    uint32_t flags;
    uint32_t hv_params_addr; /* 0 for SFLASH, anything else for a user defined HV_PARAMS */
    cyboot_flash_refresh_t *refresh;

    /* params for non-blocking operations */
    cyboot_flash_callback_t callback_pre_irq;
    cyboot_flash_callback_t callback_post_irq;
    cyboot_flash_callback_t callback_complete;
    uint32_t callback_param;
    uint32_t state; /* A state machine - EMPTY, ERASE_0, PROGRAM_0, WRITE_0,1 */
    uint32_t flash_addr; /* Non-blocking WriteRow */
    uint32_t data_addr;  /* Non-blocking WriteRow */
    uint32_t reserved[2]; /* size to be 8 bytes aligned */
} cyboot_flash_context_t;

typedef uint32_t cyboot_flash_row_t[(CY_FLASH_ROW_SIZE + CYBOOT_FLASH_COLUMN33_SIZE)/ sizeof(uint32_t)];

/* Sub sector, sector, bulk functions */
typedef uint32_t (*cyboot_flash_erase_sub_sector_t)(uint32_t flash_address, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_erase_sector_t)(uint32_t flash_address, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_erase_all_t)(uint32_t flash_address, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_program_sub_sector_t)(uint32_t flash_address, uint32_t data, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_program_sector_t)(uint32_t flash_address, uint32_t data, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_program_all_t)(uint32_t flash_address, uint32_t data, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_margin_mode_screen_t)(uint32_t flash_address, uint32_t n_rows, uint32_t params,
                                                uint32_t *ram_buffer,
                                                cyboot_flash_context_t *ctx);

typedef void     (*cyboot_flash_refresh_init_t)(uint32_t sector_idx, cyboot_flash_refresh_t *refresh);
typedef void     (*cyboot_flash_refresh_init_all_t)(cyboot_flash_refresh_t *refresh);
typedef bool     (*cyboot_flash_refresh_test_t)(const cyboot_flash_refresh_t *refresh);
typedef uint32_t (*cyboot_flash_refresh_perform_t)(uint32_t sector_idx, cyboot_flash_refresh_t *refresh);
typedef uint32_t (*cyboot_flash_refresh_perform_start_t)(uint32_t sector_idx, cyboot_flash_refresh_t *refresh, const cyboot_flash_context_t *ctx);;
typedef uint32_t (*cyboot_flash_refresh_recover_t)(uint32_t sector_idx, cyboot_flash_refresh_t *refresh);
typedef uint32_t (*cyboot_flash_refresh_recover_all_t)(cyboot_flash_refresh_t *refresh);

typedef uint32_t (*cyboot_flash_read_scratch_col33_t)(uint32_t sector_idx, uint32_t row_idx, uint32_t *count, uint32_t *ref_addr);
typedef uint32_t (*cyboot_flash_read_main_col33_t)(uint32_t sector_idx, uint32_t row_idx, uint32_t *count);
typedef void     (*cyboot_flash_refresh_get_scratch_idx_t)(uint32_t sector_idx, cyboot_flash_refresh_t *refresh);
typedef void     (*cyboot_flash_refresh_get_min_max_t)(uint32_t sector_idx, cyboot_flash_refresh_t *refresh);
typedef uint32_t (*cyboot_flash_refresh_get_sector_idx_t)(uint32_t flash_addr, uint32_t *sector_idx, uint32_t *row_idx);

/* Non-blocking additional functions */
typedef uint32_t (*cyboot_flash_operation_t)(uint32_t func_id, uint32_t flash_addr, cyboot_flash_row_t data, cyboot_flash_context_t *ctx);
typedef void (*cyboot_flash_complete_t)(const cyboot_flash_context_t *ctx);
typedef bool (*cyboot_is_flash_ready_t)(const cyboot_flash_context_t *ctx);
typedef void (*cyboot_flash_irq_handler_t)(void);

/* Blocking functions */
typedef uint32_t (*cyboot_flash_erase_row_t)(uint32_t flash_address, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_program_row_t)(uint32_t flash_address, cyboot_flash_row_t data, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_write_row_t)(uint32_t flash_address, cyboot_flash_row_t data, cyboot_flash_context_t *ctx);

/* Non-blocking functions */
typedef uint32_t (*cyboot_flash_erase_row_start_t)(uint32_t flash_address, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_program_row_start_t)(uint32_t flash_address, cyboot_flash_row_t data, cyboot_flash_context_t *ctx);
typedef uint32_t (*cyboot_flash_write_row_start_t)(uint32_t flash_address, cyboot_flash_row_t data, cyboot_flash_context_t *ctx);

typedef struct
{
    cyboot_flash_erase_sub_sector_t         cyboot_flash_erase_sub_sector;
    cyboot_flash_erase_sector_t             cyboot_flash_erase_sector;
    cyboot_flash_erase_all_t                cyboot_flash_erase_all;
    cyboot_flash_program_sub_sector_t       cyboot_flash_program_sub_sector;
    cyboot_flash_program_sector_t           cyboot_flash_program_sector;
    cyboot_flash_program_all_t              cyboot_flash_program_all;
    cyboot_flash_margin_mode_screen_t       cyboot_flash_margin_mode_screen;

    cyboot_flash_refresh_init_t             cyboot_flash_refresh_init;
    cyboot_flash_refresh_init_all_t         cyboot_flash_refresh_init_all;
    cyboot_flash_refresh_test_t             cyboot_flash_refresh_test;
    cyboot_flash_refresh_perform_t          cyboot_flash_refresh_perform;
    cyboot_flash_refresh_perform_start_t    cyboot_flash_refresh_perform_start;
    cyboot_flash_refresh_recover_t          cyboot_flash_refresh_recover;
    cyboot_flash_refresh_recover_all_t      cyboot_flash_refresh_recover_all;
    cyboot_flash_read_scratch_col33_t       cyboot_flash_read_scratch_col33;
    cyboot_flash_read_main_col33_t          cyboot_flash_read_main_col33;
    cyboot_flash_refresh_get_scratch_idx_t  cyboot_flash_refresh_get_scratch_idx;
    cyboot_flash_refresh_get_min_max_t      cyboot_flash_refresh_get_min_max;
    cyboot_flash_refresh_get_sector_idx_t   cyboot_flash_refresh_get_sector_idx;

    uint32_t unused_2[6];

    cyboot_flash_operation_t                cyboot_flash_operation;
    cyboot_flash_complete_t                 cyboot_flash_complete;
    cyboot_is_flash_ready_t                 cyboot_is_flash_ready;
    cyboot_flash_irq_handler_t              cyboot_flash_irq_handler;

    cyboot_flash_erase_row_t                cyboot_flash_erase_row;
    cyboot_flash_program_row_t              cyboot_flash_program_row;
    cyboot_flash_write_row_t                cyboot_flash_write_row;

    cyboot_flash_erase_row_start_t          cyboot_flash_erase_row_start;
    cyboot_flash_program_row_start_t        cyboot_flash_program_row_start;
    cyboot_flash_write_row_start_t          cyboot_flash_write_row_start;
} rom_funct_list_t;

#define ROM_FUNC    ((rom_funct_list_t *)0x1080FF6CUL)

/*
* Usage:
* - ROM_FUNC->cyboot_flash_erase(addr, ctx);
* - ROM_FUNC->cyboot_flash_program(addr, data, ctx);
* - etc.
*/
/** \endcond */
#endif
/* [] END OF FILE */
