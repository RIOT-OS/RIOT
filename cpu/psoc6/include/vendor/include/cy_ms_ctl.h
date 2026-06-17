/***************************************************************************//**
* \file cy_ms_ctl.h
* \version 1.2
*
* \brief
* The header file of the MSC driver.
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
* \addtogroup group_ms_ctl
* \{
* MS CTL is the Master Security Controller which helps to protect the transactions
* initiated on the bus.
*
* MSC is instantiated on non-CPU paths to provide security extension attributes
* and to add TrustZone-M capability. MSC Driver provides APIs to configure
* access rights to the given master for each Protection Context. Maximum of 32 masters
* are supported. It also provides API to set the active PC.
*
* The functions and other declarations used in this driver are in cy_ms_ctl.h.
* You can include cy_pdl.h (ModusToolbox only) to get access to all functions
* and declarations in the PDL.
*
* You can use this driver to protect the transactions initiated on the bus.
*
* CAT1D devices support multiple CPU subsystems, viz. SYSCPUSS and APPCPUSS. Each subsystem
* has its own version of MS_CTL IP within it. In order to support MS_CTL IP in the
* APPCPUSS, new set of APIs with suffix V1 are added.
*
* Following APIs are used for MS_CTL IP in CAT1D device SYSCPUSS <br>
* Cy_Ms_Ctl_ConfigBusMaster <br>
* Cy_Ms_Ctl_ConfigMscAcgResp <br>
* Cy_Ms_Ctl_SetActivePC <br>
* Cy_Ms_Ctl_GetActivePC <br>
* Cy_Ms_Ctl_SetPcHandler <br>
* Cy_Ms_Ctl_GetPcHandler <br>
*
* Following APIs are used for MS_CTL IP in CATD device APPCPUSS <br>
* Cy_Ms_Ctl_ConfigBusMasterV1 <br>
* Cy_Ms_Ctl_ConfigMscAcgRespV1 <br>
* Cy_Ms_Ctl_SetActivePCV1 <br>
* Cy_Ms_Ctl_GetActivePCV1 <br>
*
* \section group_ms_ctl_more_information More Information
*
* For more information on the MSC , refer to
* the technical reference manual (TRM).
*
* \section group_ms_ctl_MISRA MISRA-C Compliance
* The mSc driver does not have any specific deviations.
*
* \section group_ms_ctl_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.2</td>
*     <td>Updated API \ref Cy_Ms_Ctl_SetPcHandler.</td>
*     <td>Defect fix.</td>
*   </tr>
*   <tr>
*     <td>1.1</td>
*     <td>Added support for CAT1D devices, Corrected typo in API</td>
*     <td>Support for CAT1D devices</td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_ms_ctl_macros Macros
* \defgroup group_ms_ctl_functions Functions
* \defgroup group_ms_ctl_enums Enumerated Types
*/

#if !defined (CY_MS_CTL_H)
#define CY_MS_CTL_H

#include "cy_device.h"

#if defined (CY_IP_M33SYSCPUSS) || defined(CY_DOXYGEN)

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cy_syslib.h"
#include "cy_device_headers.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_ms_ctl_macros
* \{
*/
/** The MSC driver identifier */
/** MSC driver ID */
#define CY_MS_CTL_ID                      (CY_PDL_DRV_ID(0x62U))

/** Driver major version */
#define CY_MS_CTL_DRV_VERSION_MAJOR       1

/** Driver minor version */
#define CY_MS_CTL_DRV_VERSION_MINOR       2

/** \} group_ms_ctl_macros */

/**
* \addtogroup group_ms_ctl_enums
* \{
*/
/** MSC API return status */
typedef enum
{
    CY_MS_CTL_SUCCESS       = 0x00U,                                   /**< Returned successful */
    CY_MS_CTL_BAD_PARAM     = CY_MS_CTL_ID | CY_PDL_STATUS_ERROR | 0x01U, /**< Bad parameter was passed */
    CY_MS_CTL_INVALID_STATE = CY_MS_CTL_ID | CY_PDL_STATUS_ERROR | 0x02U, /**< The operation is not setup */
    CY_MS_CTL_FAILURE       = CY_MS_CTL_ID | CY_PDL_STATUS_ERROR | 0x03U, /**< The resource is locked */
    CY_MS_CTL_UNAVAILABLE   = CY_MS_CTL_ID | CY_PDL_STATUS_ERROR | 0x04U  /**< The resource is unavailable */
} cy_en_ms_ctl_status_t;

/** Bus masters */
#if (CY_IP_M33SYSCPUSS_VERSION > 1) || defined (CY_IP_M55APPCPUSS)
typedef enum
{
    CPUSS_MS_ID_CM33_0            = 0,
    CPUSS_MS_ID_CM33_1            = 1,
    CPUSS_MS_ID_DMAC0_MS          = 2,
    CPUSS_MS_ID_DMAC1_MS          = 3,
    CPUSS_MS_ID_DW0_MS            = 4,
    CPUSS_MS_ID_DW1_MS            = 5,
    CPUSS_MS_ID_CODE_MS_0         = 6,
    CPUSS_MS_ID_SYS_MS_0          = 7,
    CPUSS_MS_ID_SYS_MS_1          = 8,
    CPUSS_MS_ID_EXP_SYSCPUSS_MS_0 = 9,
    CPUSS_MS_ID_EXP_SYSCPUSS_MS_1 = 10,
    CPUSS_MS_ID_EXP_SYSCPUSS_MS_2 = 11,
    CPUSS_MS_ID_EXP_SYSCPUSS_MS_3 = 12,
    CPUSS_MS_ID_EXP_APPCPUSS_MS_0 = 13,
    CPUSS_MS_ID_EXP_APPCPUSS_MS_1 = 14,
    CPUSS_MS_ID_EXP_APPCPUSS_MS_2 = 15,
    CPUSS_MS_ID_EXP_APPCPUSS_MS_3 = 16,
    CPUSS_MS_ID_SYS_MS_0_NVM      = 17,
    CPUSS_MS_ID_SYS_MS_1_NVM      = 18,
    CPUSS_MS_ID_CM55_MS_0         = 19,
    CPUSS_MS_ID_CM55_MS_1         = 20,
    CPUSS_MS_ID_CM55_MS_2         = 21,
    CPUSS_MS_ID_CM55_MS_3         = 22,
    CPUSS_MS_ID_AXIDMAC0_MS       = 23,
    CPUSS_MS_ID_AXIDMAC1_MS       = 24,
    CPUSS_MS_ID_U55AXI0_MS        = 25,
    CPUSS_MS_ID_U55AXI1_MS        = 26,
    CPUSS_MS_ID_AXI_SYS_MS2       = 27,
    CPUSS_MS_ID_AXI_SYS_MS3       = 28,
    CPUSS_MS_ID_BISR_MS           = 29,
    CPUSS_MS_ID_RESERVED          = 30,
    CPUSS_MS_ID_TC_MS             = 31
} en_ms_ctl_master_t;
#else
typedef enum
{
    CPUSS_MS_ID_CM33_0       = 0,
    CPUSS_MS_ID_CM33_1       = 1,
    CPUSS_MS_ID_DMAC0_MS     = 2,
    CPUSS_MS_ID_DMAC1_MS     = 3,
    CPUSS_MS_ID_DW0          = 4,
    CPUSS_MS_ID_DW1          = 5,
    CPUSS_MS_ID_CODE_MS_0    = 6,
    CPUSS_MS_ID_SYS_MS_0     = 7,
    CPUSS_MS_ID_SYS_MS_1     = 8,
    CPUSS_MS_ID_EXP_MS_0     = 9,
    CPUSS_MS_ID_EXP_MS_1     = 10,
    CPUSS_MS_ID_EXP_MS_2     = 11,
    CPUSS_MS_ID_EXP_MS_3     = 12,
    CPUSS_MS_ID_EXP_MS_4     = 13,
    CPUSS_MS_ID_EXP_MS_5     = 14,
    CPUSS_MS_ID_EXP_MS_6     = 15,
    CPUSS_MS_ID_EXP_MS_7     = 16,
    CPUSS_MS_ID_SYS_MS_0_NVM = 17,
    CPUSS_MS_ID_SYS_MS_1_NVM = 18,
    CPUSS_MS_ID_TC_MS        = 31
} en_ms_ctl_master_t;
#endif

/** Bus masters security controller and ACG config */
typedef enum
{
    CODE_MS0_MSC = 0,
    SYS_MS0_MSC  = 1,
    SYS_MS1_MSC  = 2,
    EXP_MS_MSC   = 3,
    DMAC0_MSC    = 4,
    DMAC1_MSC    = 5
} en_ms_ctl_master_sc_acg_t;

#if defined (CY_IP_M55APPCPUSS)

/** Bus masters security controller and ACG config */
typedef enum
{
    APP_SYS_MS0_MSC  = 0,
    APP_SYS_MS1_MSC  = 1,
    APP_AXIDMAC0_MSC = 2,
    APP_AXIDMAC1_MSC = 3,
    APP_AXI_MS0_MSC  = 4,
    APP_AXI_MS1_MSC  = 5,
    APP_AXI_MS2_MSC  = 6,
    APP_AXI_MS3_MSC  = 7,
    APP_EXP_MS0_MSC  = 8,
    APP_EXP_MS1_MSC  = 9,
    APP_EXP_MS2_MSC  = 10,
    APP_EXP_MS3_MSC  = 11,
} en_ms_ctl_master_sc_acg_v1_t;
#endif

/** Response type when ACG blocks incoming transfers */
typedef enum
{
    CY_MS_CTL_GATE_RESP_WAITED_TRFR = 0, /**< Waited transfer */
    CY_MS_CTL_GATE_RESP_ERR         = 1  /**< Error response */
} cy_en_ms_ctl_cfg_gate_resp_t;

/** Response type when MSC blocks transfers */
typedef enum
{
    CY_MS_CTL_SEC_RESP_RAZ_WI = 0, /**< Read as zero and write ignored */
    CY_MS_CTL_SEC_RESP_ERR    = 1  /**< Error response */
} cy_en_ms_ctl_sec_resp_t;

/** Protection context mask (PC_MASK) */
typedef enum
{
    CY_MS_CTL_PCMASK0  = 0x0001U,  /**< Mask to allow PC = 0 */
    CY_MS_CTL_PCMASK1  = 0x0002U,  /**< Mask to allow PC = 1 */
    CY_MS_CTL_PCMASK2  = 0x0004U,  /**< Mask to allow PC = 2 */
    CY_MS_CTL_PCMASK3  = 0x0008U,  /**< Mask to allow PC = 3 */
    CY_MS_CTL_PCMASK4  = 0x0010U,  /**< Mask to allow PC = 4 */
    CY_MS_CTL_PCMASK5  = 0x0020U,  /**< Mask to allow PC = 5 */
    CY_MS_CTL_PCMASK6  = 0x0040U,  /**< Mask to allow PC = 6 */
    CY_MS_CTL_PCMASK7  = 0x0080U,  /**< Mask to allow PC = 7 */
    CY_MS_CTL_PCMASK8  = 0x0100U,  /**< Mask to allow PC = 8 */
    CY_MS_CTL_PCMASK9  = 0x0200U,  /**< Mask to allow PC = 9 */
    CY_MS_CTL_PCMASK10 = 0x0400U,  /**< Mask to allow PC = 10 */
    CY_MS_CTL_PCMASK11 = 0x0800U,  /**< Mask to allow PC = 11 */
    CY_MS_CTL_PCMASK12 = 0x1000U,  /**< Mask to allow PC = 12 */
    CY_MS_CTL_PCMASK13 = 0x2000U,  /**< Mask to allow PC = 13 */
    CY_MS_CTL_PCMASK14 = 0x4000U,  /**< Mask to allow PC = 14 */
    CY_MS_CTL_PCMASK15 = 0x8000U   /**< Mask to allow PC = 15 */
} cy_en_prot_pcmask_t;

/** \} group_ms_ctl_enums */

/**
* \addtogroup group_ms_ctl_functions
* \{
*/
/*******************************************************************************
* Function Name: Cy_Ms_Ctl_ConfigBusMaster
****************************************************************************//**
*
* \brief Configures the referenced bus master  by setting the privilege , non-secure
* and PC mask settings
*
* \param busMaster
* Bus master being initialized
*
* \param privileged
* privileged setting
*
* \param pcMask
* pcMask
*
* \param nonSecure
* Bus master security  setting
*
* \return
* Initialization status
*
*******************************************************************************/
cy_en_ms_ctl_status_t Cy_Ms_Ctl_ConfigBusMaster(en_ms_ctl_master_t busMaster, bool privileged, bool nonSecure, uint32_t pcMask);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_ConfigMscAcgResp
****************************************************************************//**
*
* \brief Response configuration for ACG and MSC for the referenced bus master
*
* \param busMaster
* Bus master for which response is being initialized
*
* \param gateResp
* Response type when the ACG is blocking the incoming transfers:
*
* \param secResp
* Bust master privileged setting
*
* \return
* Initialization status
*
*******************************************************************************/

cy_en_ms_ctl_status_t Cy_Ms_Ctl_ConfigMscAcgResp(en_ms_ctl_master_sc_acg_t busMaster, cy_en_ms_ctl_cfg_gate_resp_t gateResp, cy_en_ms_ctl_sec_resp_t secResp);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_SetActivePC
****************************************************************************//**
*
* \brief Set active protection context (PC)for the referenced bus master
*
* \param busMaster
* Bus master being initialized
*
* \param pc
* PC value
*
* \return
* Initialization status
*
*******************************************************************************/
cy_en_ms_ctl_status_t Cy_Ms_Ctl_SetActivePC(en_ms_ctl_master_t busMaster, uint32_t pc);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_GetActivePC
****************************************************************************//**
*
* \brief Reads the active protection context (PC) for the referenced bus master
*
* \param busMaster
* Bus master for which the PC value is being read
*
* \return
* PC value
*
*******************************************************************************/

uint32_t Cy_Ms_Ctl_GetActivePC(en_ms_ctl_master_t busMaster);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_SetSavedPC
****************************************************************************//**
*
* \brief Set saved protection context (PC) for the referenced bus master
*
* \param busMaster
* Bus master being initialized
*
* \param savedPc
* PC value
*
* \return
* Initialization status
*
*******************************************************************************/
cy_en_ms_ctl_status_t Cy_Ms_Ctl_SetSavedPC(en_ms_ctl_master_t busMaster, uint32_t savedPc);


/*******************************************************************************
* Function Name: Cy_Ms_Ctl_GetSavedPC
****************************************************************************//**
*
* \brief Reads the saved protection context (PC) for the referenced bus master
*
* \param busMaster
* Bus master for which the saved PC value is being read
*
* \return
* PC value
*
*******************************************************************************/

uint32_t Cy_Ms_Ctl_GetSavedPC(en_ms_ctl_master_t busMaster);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_SetPcHandler
****************************************************************************//**
*
* \brief Sets the handler address for the given PC. This is used to detect entry to Cypress
* "trusted" code through an exception/interrupt.
*
* \note The function can't update the handler address for the PC lower than the current application's PC.
* For example, if the application is running in PC2 it can't update handler address for PC0 or PC1.
*
* \param pc
* Protection context for which the handler is being set
*
* \param handler
* Address of the protection context  handler
*
* \return
* Initialization status
*
*******************************************************************************/

cy_en_ms_ctl_status_t Cy_Ms_Ctl_SetPcHandler(uint32_t pc, cy_israddress handler);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_GetPcHandler
****************************************************************************//**
*
* \brief Reads the handler address for the given PC
*
* \param pc
* Protection context for which the handler is being read
*
* \return
* Address of the protection context  handler
*
*******************************************************************************/

cy_israddress Cy_Ms_Ctl_GetPcHandler(uint32_t pc);

#if defined (CY_IP_M55APPCPUSS)


/*******************************************************************************
* Function Name: Cy_Ms_Ctl_ConfigBusMasterV1
****************************************************************************//**
*
* \brief Configures the referenced bus master  by setting the privilege , non-secure
* and PC mask settings in APPCPUSS in CAT1D devices.
*
* \param busMaster
* Bus master being initialized
*
* \param privileged
* privileged setting
*
* \param pcMask
* pcMask
*
* \param nonSecure
* Bus master security  setting
*
* \return
* Initialization status
*
*******************************************************************************/
cy_en_ms_ctl_status_t Cy_Ms_Ctl_ConfigBusMasterV1(en_ms_ctl_master_t busMaster, bool privileged, bool nonSecure, uint32_t pcMask);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_ConfigMscAcgRespV1
****************************************************************************//**
*
* \brief Response configuration for ACG and MSC for the referenced bus master in APPCPUSS
* in CAT1D devices.
*
* \param busMaster
* Bus master for which response is being initialized
*
* \param gateResp
* Response type when the ACG is blocking the incoming transfers:
*
* \param secResp
* Bust master privileged setting
*
* \return
* Initialization status
*
*******************************************************************************/

cy_en_ms_ctl_status_t Cy_Ms_Ctl_ConfigMscAcgRespV1(en_ms_ctl_master_sc_acg_v1_t busMaster, cy_en_ms_ctl_cfg_gate_resp_t gateResp, cy_en_ms_ctl_sec_resp_t secResp);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_SetActivePCV1
****************************************************************************//**
*
* \brief Set active protection context (PC)for the referenced bus master in APPCPUSS
* in CAT1D devices
*
* \param busMaster
* Bus master being initialized
*
* \param pc
* PC value
*
* \return
* Initialization status
*
*******************************************************************************/
cy_en_ms_ctl_status_t Cy_Ms_Ctl_SetActivePCV1(en_ms_ctl_master_t busMaster, uint32_t pc);

/*******************************************************************************
* Function Name: Cy_Ms_Ctl_AppCpuSsGetActivePC
****************************************************************************//**
*
* \brief Reads the active protection context (PC) for the referenced bus master in APPCPUSS
* in CAT1D devices
*
* \param busMaster
* Bus master for which the PC value is being read
*
* \return
* PC value
*
*******************************************************************************/
uint32_t Cy_Ms_Ctl_GetActivePCV1(en_ms_ctl_master_t busMaster);

#endif
/** \} group_ms_ctl_functions */

#if defined(__cplusplus)
}
#endif

#endif /* #if defined (CY_IP_M55APPCPUSS) */

#endif /* #if defined (CY_IP_M33SYSCPUSS) */

/** \} group_ms_ctl */

/* [] END OF FILE */
