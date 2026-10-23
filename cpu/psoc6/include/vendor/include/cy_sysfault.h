/***************************************************************************//**
* \file cy_sysfault.h
* \version 1.30
*
* \brief
* Provides an API declaration of the SysFault driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2020-2022), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
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
* \addtogroup group_sysfault
* \{
*
* The SysFault driver provides an API to configure the Fault reporting structure.
* The functions and other declarations used in this driver are in cy_sysfault.h.
* You can include cy_pdl.h to get access to all functions and declarations in the PDL.
* This driver is only available for CAT1C, CAT1D devices.
*
* The Fault subsystem contains information about faults that occur in the system.
* The fault subsystem captures only faults and it does not take any action to correct it.
* The subsystem can cause a reset, give a pulse indication, or trigger another peripheral.
* CAT1C, CAT1D uses a centralized fault report structure. The centralized nature allows for a
* system-wide consistent handling of faults, which simplifies software development as
* follows only a single fault interrupt handler is required. The fault report structure
* provides the fault source and additional fault-specific information from a single set
* of memory mapped input/output (MMIO) registers, no iterative search is required for
* the fault source and fault information.
* All pending faults are available from a single set of MMIO registers. Below is the block
* diagram.
* \image html fault.png
*
* Fault IP provides fault report structure. Fault report structures capture faults.
* The number of fault report structures is specified by a design time configuration
* parameter (FAULT_NR). In CAT1C, CAT1D there are two instances of fault structures, each fault
* report structure has a dedicated set of MMIO control and status registers and captures
* a single fault. A fault report structure provides the fault source and additional fault
* specific information from a single set of MMIO registers. The fault structures capture
* faults like MPU/SMPU/PPI protection violations, peripheral specific errors, memory
* controller specific errors. E.g., SRAM controller ECC errors,
* FLASH controller read while program and ECC errors, Processor tightly coupled
* memory (TCM) ECC errors.
*
* System fault will be captured by fault report structures. A fault report structure
* provides the fault source and additional fault specific information from a single
* set of MMIO registers.
* The captured fault information includes:
* * A validity bit field that indicates a fault is captured.
* * A fault index that identifies the fault source.
* * Additional fault information describing fault specifics.
*
* \section group_sysfault_configuration Configuration Considerations
*
* Fault configuration includes clearing the existing fault status, enabling fault source,
* setting interrupt mask, and fault initialization.
* Below is the code snippet for the fault configuration.
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Config
*
* Once the configured fault occurs, the interrupt handler will be triggered where the
* fault information can be captured.
* Below is the code snippet that can be part of a interrupt handler.
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
* \section group_sysfault_section_more_information More Information
*
* For more information on the System Fault, refer to the technical reference
* manual (TRM).
*
* \section group_sysfault_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.30</td>
*     <td>Added enum \ref en_sysfault_source_t and new macros.</td>
*     <td>Support added for CAT1D and enabled sysfault compilation for PSoC C3 (CAT1B).</td>
*   </tr>
*   <tr>
*     <td>1.20</td>
*     <td>Updated \ref Cy_SysFault_GetErrorSource API and added new macro</td>
*     <td>MISRA 10.8 fix and code enhancement</td>
*   </tr>
*    <tr>
*     <td>1.10</td>
*     <td>Updated macro related to driver enablement for CAT1C without any functional impact</td>
*     <td>Code Enhancement</td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_sysfault_macros Macros
* \defgroup group_sysfault_enums Enums
* \defgroup group_sysfault_data_structures Data Structures
* \defgroup group_sysfault_functions Functions
*
*/

#if !defined(CY_SYSFAULT_H)
#define CY_SYSFAULT_H

#include "cy_device.h"

#if defined(CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT)
#include "cy_syslib.h"
#include "cy_device_headers.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined (CY_IP_MXS22SRSS)
#define cy_en_SysFault_source_t en_sysfault_source_t
#define CY_SYSFAULT_NO_FAULT (((uint8_t)M33SYSCPUSS_RRAMC_MMIO_PC_LOCK_FAULT) + 1U)
#endif

/*******************************************************************************
*       Function Constants
*******************************************************************************/

/**
* \addtogroup group_sysfault_macros
* \{
*/

/** Driver major version */
#define CY_SYSFAULT_DRV_VERSION_MAJOR    1

/** Driver minor version */
#define CY_SYSFAULT_DRV_VERSION_MINOR    30

/** Driver ID */
#define CY_SYSFAULT_ID CY_PDL_DRV_ID     (0x76U)

/** \cond INTERNAL */

/* Macro to validate parameters in Cy_SysFault_GetFaultData() function */
#define CY_SYSFAULT_IS_DATA_SET_VALID(dataSet)     (((dataSet) == CY_SYSFAULT_DATA0) || \
                                                    ((dataSet) == CY_SYSFAULT_DATA1) || \
                                                    ((dataSet) == CY_SYSFAULT_DATA2) || \
                                                    ((dataSet) == CY_SYSFAULT_DATA3))

/* Macro to validate parameters in Cy_SysFault_GetPendingFault() function */
#define CY_SYSFAULT_IS_FAULT_SET_VALID(pendingFault)  (((pendingFault) == CY_SYSFAULT_SET0) || \
                                                       ((pendingFault) == CY_SYSFAULT_SET1) || \
                                                       ((pendingFault) == CY_SYSFAULT_SET2))
#if defined (CY_IP_MXS40SSRSS)
#define cy_en_SysFault_source_t en_sysfault_source_t
#define CY_SYSFAULT_NO_FAULT (((uint8_t)SRSS_FAULT_SSV) + 1U)
#endif

/** \endcond */


/** \} group_sysfault_macros */


/**
* \addtogroup group_sysfault_enums
* \{
*/

/**
* SysFault driver error codes
*/
typedef enum
{
    CY_SYSFAULT_SUCCESS   = 0x0UL, /**< Returned successful */
    CY_SYSFAULT_BAD_PARAM = CY_SYSFAULT_ID | CY_PDL_STATUS_ERROR | 0x1UL, /**< Bad parameter was passed */
} cy_en_SysFault_status_t;

/**
* SysFault pending Fault source set.
*/
typedef enum
{
    CY_SYSFAULT_SET0 = 0UL, /**< Set of Faults in the range of 0-31 Fault ID */
    CY_SYSFAULT_SET1 = 1UL, /**< Set of Faults in the range of 32-63  Fault ID */
    CY_SYSFAULT_SET2 = 2UL, /**< Set of Faults in the range of 64-95 Fault ID */
} cy_en_SysFault_Set_t;

/**
* Instances of Fault data register.
*/
typedef enum 
{
    CY_SYSFAULT_DATA0 = 0UL, /**< Used to get the Fault data for DATA0 register */
    CY_SYSFAULT_DATA1=  1UL, /**< Used to get the Fault data for DATA1 register */
    CY_SYSFAULT_DATA2 = 2UL, /**< Used to get the Fault data for DATA2 register */
    CY_SYSFAULT_DATA3=  3UL, /**< Used to get the Fault data for DATA3 register */
} cy_en_SysFault_Data_t;

/** \cond INTERNAL */
/* Enumeration below is only for documentation purpose.
 * Actual enumeration values come from generated headers. */
/** \endcond */

#if defined(CY_DOXYGEN)
/**
* SysFault sources for CAT1D Family of devices.
*/
typedef enum
{   PERI_0_PERI_MS0_PPC_VIO         = 0x0000u, /**< PERI0 master interface PPC violation */
    PERI_0_PERI_MS1_PPC_VIO         = 0x0001u, /**< PERI0 master interface PPC violation */
    PERI_0_PERI_PPC_PC_MASK_VIO     = 0x0002u, /**< PERI0 peripheral PPC PC mask violation */
    PERI_0_PERI_GP1_TIMEOUT_VIO     = 0x0003u, /**< PERI0 peripheral group 1 AHB timeout */
    PERI_0_PERI_GP2_TIMEOUT_VIO     = 0x0004u, /**< PERI0 peripheral group 2 AHB timeout */
    PERI_0_PERI_GP3_TIMEOUT_VIO     = 0x0005u, /**< PERI0 peripheral group 3 AHB timeout */
    PERI_0_PERI_GP4_TIMEOUT_VIO     = 0x0006u, /**<    PERI0 peripheral group 4 AHB timeout */
    PERI_0_PERI_GP5_TIMEOUT_VIO     = 0x0007u, /**<    PERI0 peripheral group 5 AHB timeout */
    PERI_0_PERI_GP0_AHB_VIO         = 0x0008u, /**< PERI0 peripheral group 0 AHB violation */
    PERI_0_PERI_GP1_AHB_VIO         = 0x0009u, /**< PERI0 peripheral group 1 AHB violation */
    PERI_0_PERI_GP2_AHB_VIO         = 0x000Au, /**< PERI0 peripheral group 2 AHB violation */
    PERI_0_PERI_GP3_AHB_VIO         = 0x000Bu, /**< PERI0 peripheral group 3 AHB violation */
    PERI_0_PERI_GP4_AHB_VIO         = 0x000Cu, /**< PERI0 peripheral group 4 AHB violation */
    PERI_0_PERI_GP5_AHB_VIO         = 0x000Du, /**< PERI0 peripheral group 5 AHB violation */
    PERI_1_PERI_MS0_PPC_VIO         = 0x000Eu, /**< PERI1 master interface PPC violation */
    PERI_1_PERI_MS1_PPC_VIO         = 0x000Fu, /**< PERI1 master interface PPC violation */
    PERI_1_PERI_PPC_PC_MASK_VIO     = 0x0010u, /**< PERI1 peripheral PPC PC mask violation */
    PERI_1_PERI_GP1_TIMEOUT_VIO     = 0x0011u, /**< PERI1 peripheral group 1 AHB timeout */
    PERI_1_PERI_GP2_TIMEOUT_VIO     = 0x0012u, /**< PERI1 peripheral group 2 AHB timeout */
    PERI_1_PERI_GP3_TIMEOUT_VIO     = 0x0013u, /**< PERI1 peripheral group 3 AHB timeout */
    PERI_1_PERI_GP4_TIMEOUT_VIO     = 0x0014u, /**<    PERI1 peripheral group 4 AHB timeout */
    PERI_1_PERI_GP0_AHB_VIO         = 0x0015u, /**< PERI1 peripheral group 0 AHB violation */
    PERI_1_PERI_GP1_AHB_VIO         = 0x0016u, /**< PERI1 peripheral group 1 AHB violation */
    PERI_1_PERI_GP2_AHB_VIO         = 0x0017u, /**< PERI1 peripheral group 2 AHB violation */
    PERI_1_PERI_GP3_AHB_VIO         = 0x0018u, /**< PERI1 peripheral group 3 AHB violation */
    PERI_1_PERI_GP4_AHB_VIO         = 0x0019u, /**< PERI1 peripheral group 4 AHB violation */
    M33SYSCPUSS_RAMC0_MPC_FAULT_MMIO = 0x001Au, /**< RAMC-0 MPC Fault */
    M33SYSCPUSS_RAMC1_MPC_FAULT_MMIO = 0x001Bu, /**< RAMC-1 MPC Fault */
    M33SYSCPUSS_RRAMC_HOST_IF_MPC_FAULT = 0x001Cu, /**< RRAM Controller Host interface MPC Fault */
    M33SYSCPUSS_RRAMC_HOST_IF_OTP_WR_TAG_ERROR_FAULT = 0x001Du, /**<     RRAMC OTP tag bit violation*/
    M33SYSCPUSS_RRAMC_POST_READ_TAG_MISMATCH_FAULT = 0x001Eu, /**< RRAMC post-read tag bit violation */
    M33SYSCPUSS_RRAMC_POST_READ_ADDR_CHECKER_ALARM_FAULT = 0x001Fu, /**< RRAMC post-read address violation */
    M33SYSCPUSS_RRAMC_INCOMPLETE_WRITE_FAULT = 0x0020u, /**<     RRAMC NVM indirect sequence incomplete */
    M33SYSCPUSS_RRAMC_NVM_ADDRESS_MISMATCH_FAULT = 0x0021u, /**< RRAMC SFR NVM address mismatch */
    M33SYSCPUSS_RRAMC_MMIO_PROTECTED_LOCKABLE_FAULT = 0x0022u, /**< RRAMC protected NVM lock fault */
    M33SYSCPUSS_RRAMC_MMIO_UDS_CTRL_FAULT = 0x0023u, /**< RRAMC invalid unique device secret lock */
    M33SYSCPUSS_RRAMC_NVM_TRAP_FAULT = 0x0024u, /**< Trap signal from RRAM */
    M33SYSCPUSS_RRAMC_NVM_IR_ECCLOG_FAULT = 0x0025u, /**< RRAM ECC failure threshold exceeded */
    M33SYSCPUSS_EXP_MS_AHB_ERROR_MMIO = 0x0026u, /**< Fault generated due to AHB error on EXP bridge*/
    M55APPCPUSS_APP_SYS_BR_ERROR_MMIO = 0x0027u, /**< sys to app bridge error fault interface */
    M55APPCPUSS_SYS_APP_BR_ERROR_MMIO = 0x0028u, /**< app to sys bridge error fault interface */
    M0SECCPUSS_FAULT_M0SEC          = 0x0029u, /**< Secure Enclave Fault Interface */
    SRSS_FAULT_CSV                  = 0x002Au, /**< SRSS clock supervision fault */
    SMIF_FAULT_MXSMIF_TOP           = 0x002Du, /**< SMIF MPC violation */
    SOCMEM_SOCMEM_MPC               = 0x002Eu, /**< SOCMEM MPC violation */
    SOCMEM_SOCMEM_AHB_ERROR         = 0x002Fu, /**< SocMEM AHB error */
    M33SYSCPUSS_RRAMC_INVALID_LCS_FAULT = 0x0030u, /**< RRAMC invalid life cycle state */
    M33SYSCPUSS_RRAMC_MMIO_RECLAIMED_REGION_SIZE_FAULT = 0x0031u, /**< RRAMC invalid reclaimed size */
    M33SYSCPUSS_RRAMC_RRAM_SFR_NVM_HRESP_FAULT = 0x0032u, /**< RRAMC AHB bus error*/
    M33SYSCPUSS_RRAMC_BANK_MAPPING_FAULT = 0x0033u, /**< RRAMC invalid bank mapping */
    M33SYSCPUSS_RRAMC_MMIO_TB_GATING_CTL_FAULT = 0x0034u, /**< RRAMC invalid turn-off tag bit gating control */
    M33SYSCPUSS_RRAMC_MMIO_PC_LOCK_FAULT = 0x0035u /**< RRAMC invalid protection context lock */
} en_sysfault_source_t;
#endif

/** \} group_sysfault_enums */


/***************************************
*       Configuration Structure
***************************************/

/**
* \addtogroup group_sysfault_data_structures
* \{
*/

/**
* Configuration structure for a Fault control register.
*/
typedef struct {
    bool       TriggerEnable;    /**< Enables the trigger output when it is True */
    bool       OutputEnable;    /**< Enables the output signal when it is True */
    bool       ResetEnable;     /**< Enables the Reset request  when it is True */
} cy_stc_SysFault_t;

/** \} group_sysfault_data_structures */


/***************************************
*       Function Prototypes
***************************************/

/**
* \addtogroup group_sysfault_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_SysFault_Init
****************************************************************************//**
*
* \brief
* Initializes the SysFault for recording faults.
*
* \param base
* The pointer to a Fault structure instance.
*
* \param config
* The pointer to a Configuration structure.
*
* \return
* Fault status. Refer \ref cy_en_SysFault_status_t
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Config
*
*******************************************************************************/
cy_en_SysFault_status_t Cy_SysFault_Init(FAULT_STRUCT_Type *base, cy_stc_SysFault_t *config);


/*******************************************************************************
* Function Name: Cy_SysFault_ClearStatus
****************************************************************************//**
*
* \brief
* Clears status register.
*
* \param base
* The pointer to a Fault structure instance.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Config
*
*******************************************************************************/
void Cy_SysFault_ClearStatus(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_GetErrorSource
****************************************************************************//**
*
* \brief
* Returns the source of error for the Fault.
*
* \param  base
* The pointer to a Fault structure instance.
*
* \return 
* Fault source.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
cy_en_SysFault_source_t Cy_SysFault_GetErrorSource(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_GetFaultData
****************************************************************************//**
*
* \brief
* Returns the Fault information for the provided dataSet.
*
* \param base
* The pointer to a Fault structure instance.
*
* \param dataSet
* Instance of data register.
*
* \return
* Fault information.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
uint32_t Cy_SysFault_GetFaultData(FAULT_STRUCT_Type *base, cy_en_SysFault_Data_t dataSet);


/*******************************************************************************
* Function Name: Cy_SysFault_GetPendingFault
****************************************************************************//**
*
* \brief
* Returns the sources of pending fault that are not captured.
*
* \param base
* The pointer to a Fault structure instance.
*
* \param pendingFault
* Instance of PENDING register. PENDING0 returns the occurred pending Faults in the range of 0-31 Fault ID.
*
* \return
* The status of pending Faults.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_PendingFault
*
*******************************************************************************/
uint32_t Cy_SysFault_GetPendingFault(FAULT_STRUCT_Type *base, cy_en_SysFault_Set_t pendingFault);


/*******************************************************************************
* Function Name: Cy_SysFault_SetMaskByIdx
****************************************************************************//**
*
* \brief
* Enable the Fault to be captured.
*
* \param base
* The pointer to a Fault structure instance.
*
* \param idx
* The Fault id to be set in the mask register.
*
* \return
* None.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Config
*
*******************************************************************************/
void Cy_SysFault_SetMaskByIdx(FAULT_STRUCT_Type *base, cy_en_SysFault_source_t idx);


/*******************************************************************************
* Function Name: Cy_SysFault_ClearMaskByIdx
****************************************************************************//**
*
* \brief
* Disable the faults to be captured.
*
* \param base
* The pointer to a Fault structure instance.
*
* \param idx
* The fault id to be cleared in the mask register.
*
* \return
* None.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_PendingFault
*
*******************************************************************************/
void Cy_SysFault_ClearMaskByIdx(FAULT_STRUCT_Type *base, cy_en_SysFault_source_t idx);


/*******************************************************************************
* Function Name: Cy_SysFault_GetInterruptStatus
****************************************************************************//**
*
* \brief
* Returns the status of the interrupt.
*
* \param base
* The pointer to a Fault structure instance.
*
* \return
* Interrupt status.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
uint32_t Cy_SysFault_GetInterruptStatus(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_ClearInterrupt
****************************************************************************//**
*
* \brief
* Clears Active Interrupt Source.
*
* \param base
* The pointer to a Fault structure instance.
*
* \return
* None.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
void Cy_SysFault_ClearInterrupt(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_SetInterrupt
****************************************************************************//**
*
* \brief
* Triggers an interrupt via a software write.
*
* \param base
* The pointer to a Fault structure instance.
*
* \return
* None.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Config
*
*******************************************************************************/
void Cy_SysFault_SetInterrupt(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_SetInterruptMask
****************************************************************************//**
*
* \brief
* Sets an interrupt mask.
*
* \param base
* The pointer to a Fault structure instance.
*
* \return
* None.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Config
*
*******************************************************************************/
void Cy_SysFault_SetInterruptMask(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_ClearInterruptMask
****************************************************************************//**
*
* \brief
* Clears an interrupt mask.
*
* \param base
* The pointer to a Fault structure instance.
*
* \return
* None.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
void Cy_SysFault_ClearInterruptMask(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_GetInterruptMask
****************************************************************************//**
*
* \brief Returns the interrupt mask.
*
* \param  base
* The pointer to a Fault structure instance.
*
* \return
* Interrupt Mask.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
uint32_t Cy_SysFault_GetInterruptMask(FAULT_STRUCT_Type *base);


/*******************************************************************************
* Function Name: Cy_SysFault_GetInterruptStatusMasked
****************************************************************************//**
*
* \brief
* Returns whether masked interrupt triggered the interrupt.
*
* \param base
* The pointer to a Fault structure instance.
*
* \return
* Interrupt Mask.
*
* \funcusage
* \snippet sysfault/snippet/main.c snippet_Cy_SysFault_Handle_Fault
*
*******************************************************************************/
uint32_t Cy_SysFault_GetInterruptStatusMasked(FAULT_STRUCT_Type *base);

/** \} group_sysfault_functions */


#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_M7CPUSS */

#endif /* CY_SYSFAULT_H */

/** \} group_sysfault */

/* [] END OF FILE */
