/***************************************************************************//**
* \file cy_ppc.h
* \version 1.0
*
* \brief
* The header file of the PPC driver.
*
********************************************************************************
* \copyright
* Copyright 2016-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/**
* \addtogroup group_ppc 
* \{
* Peripheral Protection Controller (PPC) replaces the PPU to provide peripheral
* access based on protection context (PC)
* and ARM's TrustZone-M. PPC checks the security attribute of the peripheral region
* for the transaction from the respective masters for security violation. If the
* transaction has security violation, the transaction is blocked. The master's security
* attributes are setup by Master Security Control (MSC) and is part of CPUSS which is
* out of context of this document. The main difference between PPU and PPC is
* on the programming model of the security attribute and privilege attribute.
* PPC encompass the protection context on top of the ARM's TrustZone-M secure methodology.
*
* The functions and other declarations used in this driver are in cy_ppc.h.
* You can include cy_pdl.h (ModusToolbox only) to get access to all functions
* and declarations in the PDL.
*
* You can use this driver to configure peripheral access as secure/non-secure.
*
* \section group_ppc_more_information More Information
*
* For more information on the PPC , refer to
* the technical reference manual (TRM).
*
* \section group_ppc_MISRA MISRA-C Compliance
* The ppc driver does not have any specific deviations.
*
* \section group_ppc_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_ppc_macros Macros
* \defgroup group_ppc_functions Functions
* \defgroup group_ppc_data_structures Data Structures
* \defgroup group_ppc_enums Enumerated Types
*/

#if !defined (CY_PPC_H)
#define CY_PPC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cy_syslib.h"
#include "cy_device_headers.h"
#include "cy_device.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined (CY_IP_M33SYSCPUSS)

/**
* \addtogroup group_ppc_macros
* \{
*/
/** The PPC driver identifier */
/** PPC driver ID */
#define CY_PPC_ID                      (CY_PDL_DRV_ID(0x61U))
/** Driver major version */
#define CY_PPC_DRV_VERSION_MAJOR       1
/** Driver minor version */
#define CY_PPC_DRV_VERSION_MINOR       0
/** \} group_ppc_macros */

/**
* Prot Driver error codes
*/
/**
* \addtogroup group_ppc_enums
* \{
*/
/** PPC API return status */
typedef enum 
{
    CY_PPC_SUCCESS       = 0x00U,                                   /**< Returned successful */
    CY_PPC_BAD_PARAM     = CY_PPC_ID | CY_PDL_STATUS_ERROR | 0x01U, /**< Bad parameter was passed */
    CY_PPC_INVALID_STATE = CY_PPC_ID | CY_PDL_STATUS_ERROR | 0x02U, /**< The operation is not setup */
    CY_PPC_FAILURE       = CY_PPC_ID | CY_PDL_STATUS_ERROR | 0x03U, /**< The resource is locked */
    CY_PPC_UNAVAILABLE   = CY_PPC_ID | CY_PDL_STATUS_ERROR | 0x04U  /**< The resource is unavailable */
} cy_en_ppc_status_t;

/** PPC response configuration */
typedef enum
{
    CY_PPC_RZWI     = 0, /**< Read Zero and Write ignore */
    CY_PPC_BUS_ERR = 1  /**< Generate bus error  */
} cy_en_ppc_resp_cfg_t;


/** PPC security attributes */
typedef enum 
{
    CY_PPC_SECURE     = 0, /**< PPC region is Secure */
    CY_PPC_NON_SECURE = 1  /**< PPC region is Non-secure */
} cy_en_ppc_sec_attribute_t;

/** PPC Non-secure privilege attributes */
typedef enum 
{
    CY_PPC_NON_SEC_PRIV     = 0, /**< Privilege access to non-secure region */
    CY_PPC_NON_SEC_NONPRIV = 1   /**< Privilege and non-privilege access to non-secure region  */
} cy_en_ppc_nspriv_attribute_t;

/** PPC Secure privilege attributes */
typedef enum 
{
    CY_PPC_SEC_PRIV     = 0, /**< Privilege access to secure region */
    CY_PPC_SEC_NONPRIV = 1   /**< Privilege and non-privilege access to secure region  */
} cy_en_ppc_secpriv_attribute_t;

/** \} group_ppc_enums */

/**
* \addtogroup group_ppc_data_structures
* \{
*/
/** PPC initialization structure */
typedef struct 
{
    cy_en_ppc_resp_cfg_t respConfig;  /**< Starting peripheral region ID */
} cy_stc_ppc_init_t;


/** PPC PC mask initialization structure */
typedef struct 
{
    cy_en_prot_region_t startRegion; /**< Starting peripheral region ID */
    cy_en_prot_region_t endRegion;   /**< Ending peripheral region ID */
    uint32_t pcMask;                 /**< PC mask */
} cy_stc_ppc_pc_mask_t;

/** PPC attribute initialization structure */
typedef struct 
{
    cy_en_prot_region_t startRegion;                /**< Starting peripheral region ID */
    cy_en_prot_region_t endRegion;                  /**< Ending peripheral region ID. To apply policy to a single peripheral region, assign the same value to startRegion and endRegion */
    cy_en_ppc_sec_attribute_t secAttribute;         /**< Security attribute */
    cy_en_ppc_secpriv_attribute_t secPrivAttribute; /**< Secure Privilege attribute */
    cy_en_ppc_nspriv_attribute_t nsPrivAttribute;   /**< Non-secure Privilege attribute */
} cy_stc_ppc_attribute_t;

/** \} group_ppc_data_structures */

/**
* \addtogroup group_ppc_functions
* \{
*/
cy_en_ppc_status_t Cy_Ppc_InitPpc(PPC_Type* base, const cy_stc_ppc_init_t* ppcInit);

cy_en_ppc_status_t Cy_Ppc_ConfigAttrib(PPC_Type* base, cy_stc_ppc_attribute_t* attribute);

cy_en_ppc_status_t Cy_Ppc_SetPcMask(PPC_Type* base, cy_stc_ppc_pc_mask_t* pcMaskConfig);

cy_en_ppc_status_t Cy_Ppc_SetLockMask(PPC_Type* base, uint32_t lockMask);

uint32_t Cy_Ppc_GetLockMask(PPC_Type* base);

/** \} group_ppc_functions */

#if defined(__cplusplus)
}
#endif

#endif

#endif /* #if !defined (CY_PPC_H) */

/** \} group_flash */


/* [] END OF FILE */
