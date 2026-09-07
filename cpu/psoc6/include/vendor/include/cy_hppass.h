/***************************************************************************//**
* \file cy_hppass.h
* \version 1.30.1
*
* Header file for the High Performance Programmable Analog Sub-System driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2024), Cypress Semiconductor Corporation (an Infineon company) or
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
* \addtogroup group_hppass
* \{
* The High Performance Programmable Analog Sub-System (HPPASS) PDL driver provides API to use the
* High Performance Programmable Analog Sub-System (HPPASS) HW block.
*
* \section group_hppass_glossary Glossary
* * AC - Autonomous Controller, state machine engine for High Performance Programmable Analog Sub-System
* * TC - Timer/Counter, the sub-block of the AC
* * STT - State Transition Table, the table of state machine states for High Performance Programmable Analog Sub-System,
*         essentially the 'program' for AC.
* * CSG - Comparator Slope Generator
*
********************************************************************************
* \section group_hppass_configuration Configuration Considerations
********************************************************************************
*
* In order to start the HPPASS, following functions should be called in order:
* * \ref Cy_HPPASS_Init - initializes entire HPPASS block registers, including STT,
*   SAR and CSG. Refer to config struct \ref cy_stc_hppass_cfg_t for details.
* * \ref Cy_HPPASS_AC_Start - starts the AC, which starts the HPPASS block operation.
*   Refer to \ref group_hppass_ac for details.
*
* \note When using the HPPASS personality in the Device Configurator, the personality
* includes "Initialize During Startup" checkbox (enabled by default). When enabled,
* the \ref Cy_HPPASS_Init function will be called inside of the init_cycfg_all() function.
*
********************************************************************************
* \section group_hppass_more_information More Information
********************************************************************************
* For more information on the HPPASS peripheral, refer to the technical reference
* manual (TRM).
*
*******************************************************************************
* \section group_hppass_changelog Changelog
*******************************************************************************
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td rowspan="1">1.30.1</td>
*     <td>Updated CSG documentation.</td>
*     <td>Documentation enhancement.</td>
*   </tr>
*   <tr>
*     <td rowspan="2"> 1.30</td>
*     <td>Added \ref Cy_HPPASS_SAR_CrossTalkAdjust function.</td>
*     <td>Driver enhancement.</td>
*   </tr>
*   <tr>
*     <td>Documentation update.</td>
*     <td>Documentation enhancement.</td>
*   </tr>
*   <tr>
*     <td rowspan="3"> 1.20</td>
*     <td>Updated \ref Cy_HPPASS_SAR_SetTempSensorCurrent function interface.</td>
*     <td>Usability improvement.</td>
*   </tr>
*   <tr>
*     <td>Updated \ref Cy_HPPASS_TEMP_Calc function input parameters range check.</td>
*     <td>Defect fixing.</td>
*   </tr>
*   <tr>
*     <td>Documentation update.</td>
*     <td>Documentation enhancement.</td>
*   </tr>
*   <tr>
*     <td rowspan="4"> 1.10</td>
*     <td>Added personality DRCs.</td>
*     <td>Defect fixing.</td>
*   </tr>
*   <tr>
*     <td>Fixed offset calculation for Cy_HPPASS_SAR_CountsTo_Volts function.</td>
*     <td>Defect fixing.</td>
*   </tr>
*   <tr>
*     <td>Added \ref Cy_HPPASS_SetFwTriggerPulse, \ref Cy_HPPASS_SetFwTriggerLevel,
*         and \ref Cy_HPPASS_ClearFwTriggerLevel functions.</td>
*     <td>Driver enhancement.</td>
*   </tr>
*   <tr>
*     <td>Documentation update.</td>
*     <td>Documentation enhancement.</td>
*   </tr>
*   <tr>
*     <td rowspan="1">1.0</td>
*     <td>Initial version.</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_hppass_common Common
* \{
*   \defgroup group_hppass_macros Macros
*   \{
*     \defgroup group_hppass_triggers_indexes Trigger Indexes
*     \defgroup group_hppass_triggers_out_level_masks Trigger Out Level Masks
*     \defgroup group_hppass_triggers_masks Trigger Masks
*     \defgroup group_hppass_interrupts Interrupt Masks
*   \}
*   \defgroup group_hppass_functions Functions
*   \{
*     \defgroup group_hppass_cfg_functions Static configuration functions
*     \defgroup group_hppass_rt_functions Runtime operations functions
*   \}
*   \defgroup group_hppass_data_structures Data Structures
*   \defgroup group_hppass_enums Enumerated Types
* \}
* \defgroup group_hppass_ac     AC (Autonomous Controller)
* \defgroup group_hppass_csg    CSG (Comparator Slope Generator)
* \defgroup group_hppass_sar    SAR (Successive-Approximation Register Analogue to Digital Converter)
*/

#if !defined(CY_HPPASS_H)
#define CY_HPPASS_H

#include "cy_device.h"

#ifdef CY_IP_MXS40MCPASS

#include "cy_hppass_ac.h"
#include "cy_hppass_csg.h"
#include "cy_hppass_sar.h"
#include "cy_syspm.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** \addtogroup group_hppass_macros
* \{
*/

#define CY_HPPASS_TRIG_NUM                (8U)  /**< The number of triggers */

#define CY_HPPASS_DEINIT                  (0UL) /**< De-init value for most HPPASS registers */
#define CY_HPPASS_CLK_STARTUP_DIV_DEINIT  (7UL) /**< De-init value for CLOCK_STARTUP_DIV register */
/** \} group_hppass_macros */

/** \addtogroup group_hppass_triggers_indexes
 *  To be used with \ref Cy_HPPASS_TriggerInConfig,
 *                  \ref Cy_HPPASS_TriggerOutLevelConfig,
 *                  \ref Cy_HPPASS_TriggerOutPulseConfig
 *   \{
 */
#define CY_HPPASS_TRIG_0_IDX (0U) /**< Trigger #0 index */
#define CY_HPPASS_TRIG_1_IDX (1U) /**< Trigger #1 index */
#define CY_HPPASS_TRIG_2_IDX (2U) /**< Trigger #2 index */
#define CY_HPPASS_TRIG_3_IDX (3U) /**< Trigger #3 index */
#define CY_HPPASS_TRIG_4_IDX (4U) /**< Trigger #4 index */
#define CY_HPPASS_TRIG_5_IDX (5U) /**< Trigger #5 index */
#define CY_HPPASS_TRIG_6_IDX (6U) /**< Trigger #6 index */
#define CY_HPPASS_TRIG_7_IDX (7U) /**< Trigger #7 index */
/** \} group_hppass_triggers_indexes */

/** \addtogroup group_hppass_triggers_out_level_masks
 *  To be used with \ref Cy_HPPASS_TriggerOutLevelConfig
 *   \{
 */
#define CY_HPPASS_TRIG_CMP_0         (0x01) /**< CSG comparator #0 output mask */
#define CY_HPPASS_TRIG_CMP_1         (0x02) /**< CSG comparator #1 output mask */
#define CY_HPPASS_TRIG_CMP_2         (0x04) /**< CSG comparator #2 output mask */
#define CY_HPPASS_TRIG_CMP_3         (0x08) /**< CSG comparator #3 output mask */
#define CY_HPPASS_TRIG_CMP_4         (0x10) /**< CSG comparator #4 output mask */

#define CY_HPPASS_TRIG_SAR_LIM_0     (0x01) /**< SAR limit detector #0 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_1     (0x02) /**< SAR limit detector #1 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_2     (0x04) /**< SAR limit detector #2 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_3     (0x08) /**< SAR limit detector #3 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_4     (0x10) /**< SAR limit detector #4 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_5     (0x20) /**< SAR limit detector #5 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_6     (0x40) /**< SAR limit detector #6 output mask */
#define CY_HPPASS_TRIG_SAR_LIM_7     (0x80) /**< SAR limit detector #7 output mask */
/** \} group_hppass_triggers_out_level_masks */

/** \addtogroup group_hppass_triggers_masks
 *  To be used with \ref Cy_HPPASS_SetFwTrigger,
 *                  \ref Cy_HPPASS_SetFwTriggerPulse,
 *                  \ref Cy_HPPASS_SetFwTriggerLevel,
 *                  \ref Cy_HPPASS_ClearFwTriggerLevel,
 *                  \ref Cy_HPPASS_ClearFwTrigger
 *   \{
 */
#define CY_HPPASS_TRIG_0_MSK (1U << CY_HPPASS_TRIG_0_IDX) /**< Trigger #0 mask */
#define CY_HPPASS_TRIG_1_MSK (1U << CY_HPPASS_TRIG_1_IDX) /**< Trigger #1 mask */
#define CY_HPPASS_TRIG_2_MSK (1U << CY_HPPASS_TRIG_2_IDX) /**< Trigger #2 mask */
#define CY_HPPASS_TRIG_3_MSK (1U << CY_HPPASS_TRIG_3_IDX) /**< Trigger #3 mask */
#define CY_HPPASS_TRIG_4_MSK (1U << CY_HPPASS_TRIG_4_IDX) /**< Trigger #4 mask */
#define CY_HPPASS_TRIG_5_MSK (1U << CY_HPPASS_TRIG_5_IDX) /**< Trigger #5 mask */
#define CY_HPPASS_TRIG_6_MSK (1U << CY_HPPASS_TRIG_6_IDX) /**< Trigger #6 mask */
#define CY_HPPASS_TRIG_7_MSK (1U << CY_HPPASS_TRIG_7_IDX) /**< Trigger #7 mask */
/** \} group_hppass_triggers_masks */

/** \addtogroup group_hppass_interrupts
 *  To be used with \ref Cy_HPPASS_GetInterruptStatus,
 *                  \ref Cy_HPPASS_GetInterruptStatusMasked,
 *                  \ref Cy_HPPASS_ClearInterrupt,
 *                  \ref Cy_HPPASS_SetInterrupt,
 *                  \ref Cy_HPPASS_SetInterruptMask,
 *                  \ref Cy_HPPASS_GetInterruptMask
 *  \{
 */
#define CY_HPPASS_INTR_FIFO_0_OVERFLOW       (0x01UL) /**< Result interrupt mask for the FIFO #0 overflow */
#define CY_HPPASS_INTR_FIFO_1_OVERFLOW       (0x02UL) /**< Result interrupt mask for the FIFO #1 overflow */
#define CY_HPPASS_INTR_FIFO_2_OVERFLOW       (0x04UL) /**< Result interrupt mask for the FIFO #2 overflow */
#define CY_HPPASS_INTR_FIFO_3_OVERFLOW       (0x08UL) /**< Result interrupt mask for the FIFO #3 overflow */
#define CY_HPPASS_INTR_FIFO_0_UNDERFLOW      (0x10UL) /**< Result interrupt mask for the FIFO #0 underflow */
#define CY_HPPASS_INTR_FIFO_1_UNDERFLOW      (0x20UL) /**< Result interrupt mask for the FIFO #1 underflow */
#define CY_HPPASS_INTR_FIFO_2_UNDERFLOW      (0x40UL) /**< Result interrupt mask for the FIFO #2 underflow */
#define CY_HPPASS_INTR_FIFO_3_UNDERFLOW      (0x80UL) /**< Result interrupt mask for the FIFO #3 underflow */

#define CY_HPPASS_INTR_FIFO_OVERFLOW         (HPPASS_MMIO_HPPASS_INTR_FIFO_OVERFLOW_Msk) /**< FIFO overflow interrupt mask */
#define CY_HPPASS_INTR_FIFO_UNDERFLOW        (HPPASS_MMIO_HPPASS_INTR_FIFO_UNDERFLOW_Msk) /**< FIFO underflow interrupt mask */
#define CY_HPPASS_INTR_RESULT_OVERFLOW       (HPPASS_MMIO_HPPASS_INTR_RESULT_OVERFLOW_Msk) /**< Result overflow interrupt mask */
#define CY_HPPASS_INTR_GROUP_TR_COLLISION    (HPPASS_MMIO_HPPASS_INTR_ENTRY_TR_COLLISION_Msk) /**< Sequencer Group Trigger Collision interrupt mask */
#define CY_HPPASS_INTR_GROUP_HOLD_VIOLATION  (HPPASS_MMIO_HPPASS_INTR_ENTRY_HOLD_VIOLATION_Msk) /**< Sequencer Group Hold Violation interrupt mask */
#define CY_HPPASS_INTR_AC_INT                (HPPASS_MMIO_HPPASS_INTR_AC_INT_Msk) /**< Autonomous Controller interrupt mask */

/** Combined interrupt mask */
#define CY_HPPASS_INTR                 (HPPASS_MMIO_HPPASS_INTR_FIFO_OVERFLOW_Msk | \
                                        HPPASS_MMIO_HPPASS_INTR_FIFO_UNDERFLOW_Msk | \
                                        HPPASS_MMIO_HPPASS_INTR_RESULT_OVERFLOW_Msk | \
                                        HPPASS_MMIO_HPPASS_INTR_ENTRY_TR_COLLISION_Msk | \
                                        HPPASS_MMIO_HPPASS_INTR_ENTRY_HOLD_VIOLATION_Msk | \
                                        HPPASS_MMIO_HPPASS_INTR_AC_INT_Msk)
/** \} group_hppass_interrupts */

/** \cond Macros for conditions used by CY_ASSERT calls */
#define CY_HPPASS_TRIGPULSE_VALID(trigPulse)          ((CY_HPPASS_DISABLED     == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_0  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_1  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_2  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_3  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_4  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_5  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_6  == (trigPulse)) || \
                                                       (CY_HPPASS_SAR_GROUP_7  == (trigPulse)) || \
                                                       (CY_HPPASS_FIR_0        == (trigPulse)) || \
                                                       (CY_HPPASS_FIR_1        == (trigPulse)) || \
                                                       (CY_HPPASS_AC_TR        == (trigPulse)))

#define CY_HPPASS_TRIGIN_VALID(inSel)          ((CY_HPPASS_TR_DISABLED  == (inSel)) || \
                                                (CY_HPPASS_TR_HW_A      == (inSel)) || \
                                                (CY_HPPASS_TR_HW_B      == (inSel)) || \
                                                (CY_HPPASS_TR_FW_PULSE  == (inSel)) || \
                                                (CY_HPPASS_TR_FW_LEVEL  == (inSel)))

#define CY_HPPASS_TRIGHW_VALID(hwMode)         ((CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC  == (hwMode)) || \
                                                (CY_HPPASS_PULSE_ON_NEG_DOUBLE_SYNC  == (hwMode)) || \
                                                (CY_HPPASS_PULSE_ON_BOTH_DOUBLE_SYNC == (hwMode)) || \
                                                (CY_HPPASS_LEVEL_DOUBLE_SYNC         == (hwMode)) || \
                                                (CY_HPPASS_PULSE_ON_POS_SINGLE_SYNC  == (hwMode)) || \
                                                (CY_HPPASS_PULSE_ON_NEG_SINGLE_SYNC  == (hwMode)) || \
                                                (CY_HPPASS_PULSE_ON_BOTH_SINGLE_SYNC == (hwMode)) || \
                                                (CY_HPPASS_LEVEL_SINGLE_SYNC         == (hwMode)))

#define CY_HPPASS_COMPMASK_VALID(compEn)          (0U == ((compEn) & ((uint8_t) ~(CY_HPPASS_INTR_CSG_0_CMP | \
                                                                                  CY_HPPASS_INTR_CSG_1_CMP | \
                                                                                  CY_HPPASS_INTR_CSG_2_CMP | \
                                                                                  CY_HPPASS_INTR_CSG_3_CMP | \
                                                                                  CY_HPPASS_INTR_CSG_4_CMP))))
/** \endcond */

/***************************************
*       Enumerated Types
***************************************/

/**
* \addtogroup group_hppass_enums
* \{
*/

/** Trigger type selection */
typedef enum
{
    CY_HPPASS_TR_DISABLED = 0U, /**< Trigger is off */
    CY_HPPASS_TR_HW_A     = 1U, /**< Select an external HW trigger from A trigger inputs */
    CY_HPPASS_TR_HW_B     = 2U, /**< Select the external HW trigger from B trigger inputs */
    CY_HPPASS_TR_FW_PULSE = 3U, /**< Select FW PULSE trigger */
    CY_HPPASS_TR_FW_LEVEL = 4U, /**< Select FW LEVEL trigger */
} cy_en_hppass_trig_t;


/** HW trigger mode */
typedef enum
{
    CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC  = 0U, /**< Async in FC, double synced and converted to pulse on pos edge */
    CY_HPPASS_PULSE_ON_NEG_DOUBLE_SYNC  = 1U, /**< Async in FC, double synced and converted to sync pulse on neg edge */
    CY_HPPASS_PULSE_ON_BOTH_DOUBLE_SYNC = 2U, /**< Async in FC, double synced and converted to sync pulse on 2 edges */
    CY_HPPASS_LEVEL_DOUBLE_SYNC         = 3U, /**< Async in FC, double synced and used as a level trigger */
    CY_HPPASS_PULSE_ON_POS_SINGLE_SYNC  = 4U, /**< Sync in FC then converted to pulse on pos edge */
    CY_HPPASS_PULSE_ON_NEG_SINGLE_SYNC  = 5U, /**< Sync in FC then converted to pulse on neg edge*/
    CY_HPPASS_PULSE_ON_BOTH_SINGLE_SYNC = 6U, /**< Sync in FC then converted to pulse on 2 edges */
    CY_HPPASS_LEVEL_SINGLE_SYNC         = 7U, /**< Async in FC, used as a level trigger */
} cy_en_hppass_trig_hw_t;


/** PULSE trigger out select */
typedef enum
{
    CY_HPPASS_DISABLED    = 0U,  /**< Disable trigger output */
    CY_HPPASS_SAR_GROUP_0 = 1U,  /**< Enable the SAR Sequencer Group 0 Done trigger output */
    CY_HPPASS_SAR_GROUP_1 = 2U,  /**< Enable the SAR Sequencer Group 1 Done event trigger output */
    CY_HPPASS_SAR_GROUP_2 = 3U,  /**< Enable the SAR Sequencer Group 2 Done event trigger output */
    CY_HPPASS_SAR_GROUP_3 = 4U,  /**< Enable the SAR Sequencer Group 3 Done event trigger output */
    CY_HPPASS_SAR_GROUP_4 = 5U,  /**< Enable the SAR Sequencer Group 4 Done event trigger output */
    CY_HPPASS_SAR_GROUP_5 = 6U,  /**< Enable the SAR Sequencer Group 5 Done event trigger output */
    CY_HPPASS_SAR_GROUP_6 = 7U,  /**< Enable the SAR Sequencer Group 6 Done event trigger output */
    CY_HPPASS_SAR_GROUP_7 = 8U,  /**< Enable the SAR Sequencer Group 7 Done event trigger output */
    CY_HPPASS_FIR_0       = 9U,  /**< Enable the FIR 0 Done event trigger output */
    CY_HPPASS_FIR_1       = 10U, /**< Enable the FIR 1 Done event trigger output */
    CY_HPPASS_AC_TR       = 11U, /**< Enable AC trigger output */
} cy_en_hppass_trig_out_pulse_t;


/** \} group_hppass_enums */

/***************************************
*       Configuration Structures
***************************************/

/**
* \addtogroup group_hppass_data_structures
* \{
*/

/** HW input trigger configuration structure */
typedef struct
{
    cy_en_hppass_trig_t      type; /**< Trigger type selection */
    cy_en_hppass_trig_hw_t hwMode; /**< HW trigger mode */
} cy_stc_hppass_trig_in_t;


/** Output Level Trigger configuration structure */
typedef struct
{
    bool  syncBypass; /**< LEVEL trigger operation type */
    uint8_t  compMsk; /**< Comparator mask, each bit represents separate CSG comparator output,
                       *   all selected outputs are then combined (ORed) together and
                       *   routed to associated trigger output
                       */
    uint8_t limitMsk; /**< SAR limit detector mask, each bit represents separate SAR limit detector output,
                       *   all selected outputs are then combined (ORed) together and
                       *   routed to associated trigger output
                       */
} cy_stc_hppass_trig_out_level_t;


/**
* Configuration structure to set up the entire High Performance Programmable Analog Sub-System
* to be used with \ref Cy_HPPASS_Init.
*/
typedef struct
{
    cy_stc_hppass_ac_t                                        ac; /**< Autonomous Controller */
    cy_stc_hppass_csg_t const                              * csg; /**< Pointer to the CSG configuration structure.
                                                                   *   Can be NULL if CSG is not used.
                                                                   */
    cy_stc_hppass_sar_t const                              * sar; /**< Pointer to the SAR configuration structure.
                                                                   *   Can be NULL if SAR is not used.
                                                                   */
    cy_stc_hppass_trig_in_t           trigIn[CY_HPPASS_TRIG_NUM]; /**< Array of Input Triggers */
    cy_en_hppass_trig_out_pulse_t  trigPulse[CY_HPPASS_TRIG_NUM]; /**< Array of Output Pulse Triggers */
    cy_stc_hppass_trig_out_level_t trigLevel[CY_HPPASS_TRIG_NUM]; /**< Array of Output Level Triggers */
} cy_stc_hppass_cfg_t;


/** \} group_hppass_data_structures */


/***************************************
*        Function Prototypes
***************************************/

/** \addtogroup group_hppass_cfg_functions
*   \{ */

/*******************************************************************************
* Function Name: Cy_HPPASS_Init
****************************************************************************//**
*
* Configures the entire HPPASS block including AC, STT, CSG and SAR according to the
* provided settings.
*
* \note If AC is running or SAR or CSG is already enabled, the initialization of
* the overall HPPASS is not allowed (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
* The only way to reinitialize the entire HPPASS is only after \ref Cy_HPPASS_DeInit call.
* Despite the 'block ready' status, the particular SAR or SCG subsystem,
* which is not running at the moment, can be reinitialized using
* \ref Cy_HPPASS_SAR_Init or \ref Cy_HPPASS_CSG_Init.
*
* \param cfg
* Pointer to structure containing configuration data
* for the entire High Performance Programmable Analog Sub-System, \ref cy_stc_hppass_cfg_t.
* The pointer cannot be NULL.
*
* \return
* Combined value of all the subsystem statuses, \ref cy_en_hppass_status_t.
*
* \funcusage
* \snippet test_apps/hppass_sar_fir/main.c SNIPPET_HPPASS_INIT_CFG
* \snippet test_apps/hppass_sar_fir/main.c SNIPPET_HPPASS_INIT
*
*******************************************************************************/
cy_rslt_t Cy_HPPASS_Init(const cy_stc_hppass_cfg_t * cfg);


/*******************************************************************************
* Function Name: Cy_HPPASS_DeInit
****************************************************************************//**
*
* De-initializes the AC, STT, AREF, SAR, CSG,
* and triggers register values to default.
*
* \note This function stops and shuts down entire HPPASS including all the trims,
* calibrations, resets AC and STT flow and subsystems.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_DE_INIT
*
*******************************************************************************/
void Cy_HPPASS_DeInit(void);


/*******************************************************************************
* Function Name: Cy_HPPASS_DeepSleepCallback
****************************************************************************//**
*
* DeepSleep callback saves the SAR ADC calibration values before switch to
* deep sleep power mode and restore them after the wakeup.
*
* \param callbackParams
* Callback parameters, see \ref cy_stc_syspm_callback_params_t
*
* \param mode
* Callback mode, see \ref cy_en_syspm_callback_mode_t
*
* \return
* \ref cy_en_syspm_status_t
*
* \funcusage
* \snippet hppass/snippet/hppass_common.c SNIPPET_HPPASS_DEEP_SLEEP_CLBK
*
*******************************************************************************/
cy_en_syspm_status_t Cy_HPPASS_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);


/** \cond Internal Trigger-related  macros */
#define CY_HPPASS_TRIG_IDX_VALID(trigIdx)      (CY_HPPASS_TRIG_7_IDX >= (trigIdx))
#define CY_HPPASS_TRIG_IN_BIT_POS(trigIdx)  (4UL * ((uint32_t)trigIdx)) /**< 4 is a width of TRIG_IN_SEL bitfield */
#define CY_HPPASS_TRIG_IN_BIT_VAL(val, trigIdx) ((val) << (CY_HPPASS_TRIG_IN_BIT_POS(trigIdx))) /**< Bitfield value calculation */
/** \endcond */


/*******************************************************************************
* Function Name: Cy_HPPASS_TriggerInConfig
****************************************************************************//**
*
* Sets the input trigger type and operation mode.
*
* \param trigIdx
* Trigger index.
* See \ref group_hppass_triggers_indexes.
*
* \param trig
* Pointer to the input trigger structure.
* See \ref cy_stc_hppass_trig_in_t. The pointer cannot be NULL.
*
* \return
* Status of operation, \ref cy_en_hppass_status_t.
*
* \funcusage
* \snippet hppass/snippet/hppass_common.c SNIPPET_HPPASS_TRIG_IN_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_TriggerInConfig(uint8_t trigIdx, const cy_stc_hppass_trig_in_t * trig)
{
    cy_en_hppass_status_t result = Cy_HPPASS_AC_CondRunCheck();

    CY_ASSERT_L3(CY_HPPASS_TRIGIN_VALID(trig->type));
    CY_ASSERT_L3(CY_HPPASS_TRIGHW_VALID(trig->hwMode));

    if ((NULL != trig) && (CY_HPPASS_TRIG_IDX_VALID(trigIdx)))
    {
        uint32_t trInSel_Pos = CY_HPPASS_TRIG_IN_BIT_POS(trigIdx);
        uint32_t trInSel_Msk = CY_HPPASS_TRIG_IN_BIT_VAL(HPPASS_INFRA_TR_IN_SEL_TR0_SEL_Msk, trigIdx);
        uint32_t trInMode_Pos = CY_HPPASS_TRIG_IN_BIT_POS(trigIdx);
        uint32_t trInMode_Msk = CY_HPPASS_TRIG_IN_BIT_VAL(HPPASS_INFRA_HW_TR_MODE_HW_TR0_MODE_Msk, trigIdx);

        CY_REG32_CLR_SET(HPPASS_INFRA_TR_IN_SEL(HPPASS_BASE), trInSel, trig->type);
        CY_REG32_CLR_SET(HPPASS_INFRA_HW_TR_MODE(HPPASS_BASE), trInMode, trig->hwMode);
    }
    else
    {
        result = CY_HPPASS_BAD_PARAM;
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_TriggerOutLevelConfig
****************************************************************************//**
*
* Sets the LEVEL trigger output configuration.
*
* \param trigIdx
* Trigger index.
* See \ref group_hppass_triggers_indexes.
*
* \param trig
* Pointer to the output LEVEL trigger structure.
* See \ref cy_stc_hppass_trig_out_level_t. The pointer cannot be NULL.
*
* \funcusage
* \snippet hppass/snippet/hppass_common.c SNIPPET_HPPASS_TRIG_OUT_LEVEL_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_TriggerOutLevelConfig(uint8_t trigIdx, const cy_stc_hppass_trig_out_level_t * trig)
{
    cy_en_hppass_status_t result = Cy_HPPASS_AC_CondRunCheck();

    if (CY_HPPASS_TRIG_IDX_VALID(trigIdx) &&
        (NULL != trig) &&
        CY_HPPASS_COMPMASK_VALID(trig->compMsk))
    {
        uint32_t trLevelCfg_Pos = trigIdx;
        uint32_t trLevelCfg_Msk = 1UL << trLevelCfg_Pos;

        CY_REG32_CLR_SET(HPPASS_MMIO_TR_LEVEL_CFG(HPPASS_BASE), trLevelCfg, trig->syncBypass);
        HPPASS_MMIO_TR_LEVEL_OUT(HPPASS_BASE, trigIdx) = _VAL2FLD(HPPASS_MMIO_TR_LEVEL_OUT_CMP_TR, trig->compMsk) |
                                                         _VAL2FLD(HPPASS_MMIO_TR_LEVEL_OUT_SAR_RANGE_TR, trig->limitMsk);
    }
    else
    {
        result = CY_HPPASS_BAD_PARAM;
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_TriggerOutPulseConfig
****************************************************************************//**
*
* Sets the PULSE trigger output configuration.
*
* \param trigIdx
* Trigger index.
* See \ref group_hppass_triggers_indexes.
*
* \param trig
* The PULSE trigger out select.
* See \ref cy_en_hppass_trig_out_pulse_t.
*
* \funcusage
* \snippet hppass/snippet/hppass_common.c SNIPPET_HPPASS_TRIG_OUT_PULSE_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_TriggerOutPulseConfig(uint8_t trigIdx, cy_en_hppass_trig_out_pulse_t trig)
{
    cy_en_hppass_status_t result = Cy_HPPASS_AC_CondRunCheck();

    CY_ASSERT_L3(CY_HPPASS_TRIGPULSE_VALID(trig));

    if (CY_HPPASS_TRIG_IDX_VALID(trigIdx))
    {
        HPPASS_MMIO_TR_PULSE_OUT(HPPASS_BASE, trigIdx) = _VAL2FLD(HPPASS_MMIO_TR_PULSE_OUT_SEL, trig);
    }
    else
    {
        result = CY_HPPASS_BAD_PARAM;
    }

    return result;
}

/** \} group_hppass_cfg_functions */


/** \addtogroup group_hppass_rt_functions
*   \{ */


/*******************************************************************************
* Function Name: Cy_HPPASS_GetInterruptStatus
****************************************************************************//**
*
* Returns the interrupt register status.
*
* \return
* The status of combined interrupt requests \ref group_hppass_interrupts.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_ISR
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_GetInterruptStatus(void)
{
    return HPPASS_MMIO_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_ClearInterrupt
****************************************************************************//**
*
* Clears the interrupt for High Performance Programmable Analog Sub-System.
*
* \param interrupt
* The mask of interrupts to clear. Each bit of this mask is responsible for
* clearing of the particular interrupt \ref group_hppass_interrupts.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_ISR
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_ClearInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR));

    HPPASS_MMIO_INTR(HPPASS_BASE) = CY_HPPASS_INTR & interrupt;

    /* This dummy reading is necessary here. It provides a guarantee that
    * interrupt is cleared at returning from this function.
    */
    (void)HPPASS_MMIO_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SetInterrupt
****************************************************************************//**
*
* Sets a software interrupt request for the High Performance Programmable Analog Sub-System.
*
* \param interrupt
* The mask of interrupts to set. Each bit of this mask is responsible for
* triggering of the particular interrupt \ref group_hppass_interrupts.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_SET_INT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SetInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR));

    HPPASS_MMIO_INTR_SET(HPPASS_BASE) = CY_HPPASS_INTR & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SetInterruptMask
****************************************************************************//**
*
* Configures which bits of the interrupt request register will trigger
* an interrupt event in the High Performance Programmable Analog Sub-System.
*
* \param interrupt
* The mask of interrupts allowable for activation. Each bit of this mask
* allows triggering of the particular interrupt \ref group_hppass_interrupts.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_SET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SetInterruptMask(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR));

    HPPASS_MMIO_INTR_MASK(HPPASS_BASE) = CY_HPPASS_INTR & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_GetInterruptMask
****************************************************************************//**
*
* Returns the interrupt mask. Each bit of this mask allows triggering of
* the particular interrupt in the High Performance Programmable Analog Sub-System.
*
* \return
* The mask of the interrupts allowable for activation \ref group_hppass_interrupts.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_GET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_GetInterruptMask(void)
{
    return HPPASS_MMIO_INTR_MASK(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_GetInterruptStatusMasked
****************************************************************************//**
*
* Returns the result of the bitwise AND operation between the
* corresponding interrupt request and mask bits.
* See \ref Cy_HPPASS_GetInterruptStatus and \ref Cy_HPPASS_SetInterruptMask.
*
* \return
* The masked status of combined interrupt requests \ref group_hppass_interrupts.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_ISR
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_GetInterruptStatusMasked(void)
{
    return HPPASS_MMIO_INTR_MASKED(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SetFwTriggerPulse
****************************************************************************//**
*
* Generates FW Pulse trigger if according configuration done in
* Cy_HPPASS_TriggerInConfig().
*
* \param mask
* Trigger mask.
* See \ref group_hppass_triggers_masks.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SetFwTriggerPulse(uint8_t mask)
{
    HPPASS_INFRA_FW_TR_PULSE(HPPASS_BASE) = (uint32_t)mask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SetFwTriggerLevel
****************************************************************************//**
*
* Generates FW Level trigger if according configuration done in
* Cy_HPPASS_TriggerInConfig().
*
* \param mask
* Trigger mask.
* See \ref group_hppass_triggers_masks.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SetFwTriggerLevel(uint8_t mask)
{
    HPPASS_INFRA_FW_TR_LEVEL(HPPASS_BASE) |= (uint32_t)mask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SetFwTrigger
****************************************************************************//**
*
* Generates FW trigger, Pulse or Level, depends on configuration in
* Cy_HPPASS_TriggerInConfig().
*
* \param mask
* Trigger mask.
* See \ref group_hppass_triggers_masks.
*
* \return
* Status of operation, \ref cy_en_hppass_status_t.
*
* \funcusage
* \snippet test_apps/hppass_common/main.c SNIPPET_HPPASS_SET_FW_TRIG
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_SetFwTrigger(uint8_t mask);


/*******************************************************************************
* Function Name: Cy_HPPASS_ClearFwTriggerLevel
****************************************************************************//**
*
* Clears the firmware Level trigger.
*
* \param mask
* Trigger mask.
* See \ref group_hppass_triggers_masks.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_ClearFwTriggerLevel(uint8_t mask)
{
    HPPASS_INFRA_FW_TR_LEVEL(HPPASS_BASE) &= ~(uint32_t)mask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_ClearFwTrigger
****************************************************************************//**
*
* Clears the firmware trigger.
*
* \param mask
* Trigger mask.
* See \ref group_hppass_triggers_masks.
*
* \return
* Status of operation, \ref cy_en_hppass_status_t.
*
* \funcusage
* \snippet hppass/snippet/hppass_common.c SNIPPET_HPPASS_CLR_FW_TRIG
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_ClearFwTrigger(uint8_t mask);


/*******************************************************************************
* Function Name: Cy_HPPASS_IsVddaOk
****************************************************************************//**
*
* Returns the VDDA voltage status.
*
* \return
* VDDA status: true - VDDA is at or above minimum threshold,
*              false - VDDA is below minimum threshold.
*
* \funcusage
* \snippet hppass/snippet/hppass_common.c SNIPPET_HPPASS_IS_VDDA_OK
*
*******************************************************************************/
__STATIC_INLINE bool Cy_HPPASS_IsVddaOk(void)
{
    return _FLD2BOOL(HPPASS_INFRA_VDDA_STATUS_VDDA_OK, HPPASS_INFRA_VDDA_STATUS(HPPASS_BASE));
}

/** \} group_hppass_rt_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXS40MCPASS */

#endif /** !defined(CY_HPPASS_H) */

/** \} group_hppass */

/* [] END OF FILE */
