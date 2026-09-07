/***************************************************************************//**
* \file cy_hppass_ac.h
* \version 1.30.1
*
* Header file for the Autonomous Controller (AC) subsystem of
* the High Performance Programmable Analog Sub-System (HPPASS).
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
* \addtogroup group_hppass_ac
* \{
* This driver provides API functions to configure, manage and interact with the Autonomous Controller (AC)
* subsystem within High Performance Programmable Analog Sub-System (HPPASS).
*
* The autonomous controller is a programmable finite-state machine (FSM) sequencer that works from a 16-entry state
* transition table, to transition appropriately through various pre-defined states, based on timer, counter, and/or
* events.
* The AC provides hardware control without the need for CPU or external intervention. <br>
* The internal structure and external HW interface of the AC is shown at the picture below.
*
* \image html hppass_ac_structure.png width=30%
*
* Autonomous controller can perform the following functions:
* - Enable/disable SAR ADC and/or CSG, provide a BLOCK_READY status to the firmware
* - Process input hardware/firmware triggers to execute programmed actions
* - Trigger any of the internal or connected blocks
* - Implement hardware delays/counters
* - Perform periodic actions (e.g., triggering) in a timed loop
* - Receive conditions from the connected blocks and acting on those events
* - Generate an AC-based interrupt to the CPU
* - Digital output to as many as five GPIOs
*
* The AC has a standard interface to each sub-block of HPPASS, the interface consists of: Enable, Trigger, and
* Conditions signals groups.
*
* Refer to the Technical Reference Manual for detailed information.
*
* To configure the AC, the driver uses a configuration structure of type \ref cy_stc_hppass_ac_t that must be predefined.
* In turn, this structure holds the non-NULL pointer to the State Transition Table (STT) array of type \ref cy_stc_hppass_ac_stt_t.
* The configuration structure also holds a specialized startup configuration array of type \ref cy_stc_hppass_startup_t.<br>
* This array defines the parameters of the AC HW startup.
*
* \section group_hppass_ac_stt State Transition Table
* The AC can implement a single threaded programming sequence using a simple format.
* The AC program is stored in a RAM table called a STT, which consists of 16 States (or programming instructions) of type cy_stc_hppass_ac_stt_t.
* These programming instructions can form one or more programs, called by FW, like subroutines.
*
* \note At least one state (e.g. 'Stop') of the state transition table must be predefined in the STT.
*
* \note The CSG and/or SAR should be enabled in the first state of the STT to ensure proper working of the AC.
*
* Here is an example of a simple program coded by STT instructions.
* \snippet hppass_ac_toggle_gpio/main.c SNIPPET_HPPASS_AC_STT_CONFIG
*
* \section group_hppass_ac_startup Startup
* Startup is initiated by enabling the AC with loaded AC startup sequence instructions (cy_stc_hppass_ac_t::startup).
* The startup block have dedicated logic to time the HPPASS startup and will generate BLOCK_READY condition once
* HPPASS startup is done. The application should wait for the BLOCK_READY condition before using any HPPASS features.
* The BLOCK_READY condition can be checked by \ref Cy_HPPASS_AC_IsBlockReady function.
*
* \note Only usage of the startup parameters recommended by the vendor guarantees the reliable work of the autonomous
* controller.<br>
* Please use the <b>Device Configurator</b> tool to make configurations or refer to the <b>Technical Reference Manual</b>
* chapter <b>27.2.1.1</b> for detailed information.
*
* \section group_hppass_ac_code_snippets Code Snippets
*
* \subsection group_hppass_ac_gpio_toggle Snippet 1: GPIO Toggling by Autonomous Controller
*
* An example is shown below, describes a simple case of usage Autonomous
* Controller for toggling GPIO Out:
*
* - Configure GPIO pin P7_0 and set HSIOM mode as P7_0_PASS_MCPASS_DOUT0:
* \snippet hppass_ac_toggle_gpio/main.c SNIPPET_HPPASS_AC_GPIO_CONFIG
*
* - Configure \ref cy_stc_hppass_ac_stt_t structure array with appropriate
* configurations for each state of Autonomous Controller:
* \snippet hppass_ac_toggle_gpio/main.c SNIPPET_HPPASS_AC_STT_CONFIG
*
* - Set HPPASS and AC state transition table configuration, used by
* \ref Cy_HPPASS_Init API:
* \snippet hppass_ac_toggle_gpio/main.c SNIPPET_HPPASS_CONFIG
*
* - Initialize and start Autonomous Controller:
* \snippet hppass_ac_toggle_gpio/main.c SNIPPET_HPPASS_AC_INIT
* \snippet hppass_ac_toggle_gpio/main.c SNIPPET_HPPASS_AC_START
*
*
* \subsection group_hppass_input_trigger Snippet 2: The AC state triggering by FW and HW input triggers
*
* The below example is a simple case of using HPPASS input
* trigger events to trigger the Autonomous Controller state. The code snippet shows
* two approaches: FW trigger and HW trigger configurations.
*
* - Configure the \ref cy_stc_hppass_ac_stt_t structure array, where the
* Autonomous Controller in state #0 waits for a trigger to change that state:
* \snippet hppass_trigger/main.c SNIPPET_HPPASS_TRIGGER_AC_STT_CONFIG
*
* <b>Initialization of the hardware modules for the AC with input triggers:</b>
* \snippet hppass_trigger/main.c SNIPPET_HPPASS_INIT_SECTION
*
* <b>Triggering HPPASS Autonomous Controller state by FW trigger:</b>
* \snippet hppass_trigger/main.c SNIPPET_HPPASS_TRIGGER_FW_TRIGGER_TEST
*
* <b>Triggering HPPASS Autonomous Controller state by HW trigger:</b>
* \snippet hppass_trigger/main.c SNIPPET_HPPASS_TRIGGER_HW_TRIGGER_TEST
*
* \defgroup group_hppass_ac_macros Macros
* \{
*   \defgroup group_hppass_ac_dout GPIO Out Masks
*   \defgroup group_hppass_ac_seq_grp SAR Sequencer Groups mask.
* \}
* \defgroup group_hppass_ac_functions Functions
* \defgroup group_hppass_ac_data_structures Data Structures
* \defgroup group_hppass_ac_enums Enumerated Types
* \} */

#if !defined(CY_HPPASS_AC_H)
#define CY_HPPASS_AC_H

#include "cy_device.h"

#ifdef CY_IP_MXS40MCPASS

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** \addtogroup group_hppass_macros
* \{
*/

/** Driver major version */
#define CY_HPPASS_DRV_VERSION_MAJOR 1

/** Driver minor version */
#define CY_HPPASS_DRV_VERSION_MINOR 30

/** The driver identifier for the High Performance Programmable Analog Sub-System */
#define CY_HPPASS_ID                CY_PDL_DRV_ID(0x77UL)

/** The number of startup configurations */
#define CY_HPPASS_STARTUP_CFG_NUM   (4U)

/** The AC Timer Table size */
#define CY_HPPASS_AC_STT_SIZE       (16U)

/** The number of CSG slices */
#define CY_HPPASS_CSG_NUM           (5UL)

/** The number of the sampler muxes */
#define CY_HPPASS_SAR_MUX_NUM       (4U)

/** \} group_hppass_macros */

/** \cond Internal macros */
/* Mask validation macros */
#define CY_HPPASS_MASK_VALID(value, mask)  (0UL == ((value) & ~(mask)))

/* BOOL to UINT conversion macros */
#define CY_HPPASS_BOOL_TO_UINT(value)  ((value) ? 1UL : 0UL)
/** \endcond */


/**
* \addtogroup group_hppass_ac_macros
* \{
*/

#define CY_HPPASS_TC_NUM            (2U) /**< The number of Timer/Counters */

/** \} group_hppass_ac_macros */

/**
* \addtogroup group_hppass_ac_dout
* \{
*/
#define CY_HPPASS_GPIO_OUT_0        (0x01U) /**< The GPIO out0 mask. */
#define CY_HPPASS_GPIO_OUT_1        (0x02U) /**< The GPIO out1 mask. */
#define CY_HPPASS_GPIO_OUT_2        (0x04U) /**< The GPIO out2 mask. */
#define CY_HPPASS_GPIO_OUT_3        (0x08U) /**< The GPIO out3 mask. */
#define CY_HPPASS_GPIO_OUT_4        (0x10U) /**< The GPIO out4 mask. */
/** \} group_hppass_ac_dout */

/**
* \addtogroup group_hppass_ac_seq_grp
* \{
*/
#define CY_HPPASS_SEQ_GRP_0_TRIG    (0x01U) /**< The SAR Sequencer Group 0 trigger mask. */
#define CY_HPPASS_SEQ_GRP_1_TRIG    (0x02U) /**< The SAR Sequencer Group 1 trigger mask. */
#define CY_HPPASS_SEQ_GRP_2_TRIG    (0x04U) /**< The SAR Sequencer Group 2 trigger mask. */
#define CY_HPPASS_SEQ_GRP_3_TRIG    (0x08U) /**< The SAR Sequencer Group 3 trigger mask. */
#define CY_HPPASS_SEQ_GRP_4_TRIG    (0x10U) /**< The SAR Sequencer Group 4 trigger mask. */
#define CY_HPPASS_SEQ_GRP_5_TRIG    (0x20U) /**< The SAR Sequencer Group 5 trigger mask. */
#define CY_HPPASS_SEQ_GRP_6_TRIG    (0x40U) /**< The SAR Sequencer Group 6 trigger mask. */
#define CY_HPPASS_SEQ_GRP_7_TRIG    (0x80U) /**< The SAR Sequencer Group 7 trigger mask. */
/** \} group_hppass_ac_seq_grp */



/** \cond Macros for conditions used by CY_ASSERT calls */
#define CY_HPPASS_DIVSTCLK_VALID(divStClk)          ((256U >= (divStClk)) && (0U < (divStClk)))
#define CY_HPPASS_ACGPIOOUTEN_VALID(acGpioOutEn)    (0U == ((acGpioOutEn) & ((uint8_t) ~(CY_HPPASS_GPIO_OUT_0 | \
                                                                                         CY_HPPASS_GPIO_OUT_1 | \
                                                                                         CY_HPPASS_GPIO_OUT_2 | \
                                                                                         CY_HPPASS_GPIO_OUT_3 | \
                                                                                         CY_HPPASS_GPIO_OUT_4))))

#define CY_HPPASS_GPIOOUT_VALID(gpioOutMsk)         (0U == ((gpioOutMsk) & ((uint8_t) ~(CY_HPPASS_GPIO_OUT_0 | \
                                                                                        CY_HPPASS_GPIO_OUT_1 | \
                                                                                        CY_HPPASS_GPIO_OUT_2 | \
                                                                                        CY_HPPASS_GPIO_OUT_3 | \
                                                                                        CY_HPPASS_GPIO_OUT_4))))

#define CY_HPPASS_BRANCHSTATE_VALID(branchState)    (15U >= (branchState))
#define CY_HPPASS_AC_COUNT_VALID(count)             ((4096U >= (count)) && (0U < (count)))

#define CY_HPPASS_AC_CONDITION_VALID(condition)     ((CY_HPPASS_CONDITION_FALSE               == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRUE                == (condition)) || \
                                                     (CY_HPPASS_CONDITION_BLOCK_READY         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CNT_DONE            == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_0_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_1_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_2_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_3_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_4_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_5_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_6_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_GROUP_7_DONE    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_0         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_1         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_2         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_3         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_4         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_5         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_6         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_LIMIT_7         == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_BUSY            == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_FIR_0_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_FIR_1_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_QUEUE_HI_EMPTY  == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_QUEUE_LO_EMPTY  == (condition)) || \
                                                     (CY_HPPASS_CONDITION_SAR_QUEUES_EMPTY    == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_0              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_1              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_2              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_3              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_4              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_5              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_6              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_TRIG_7              == (condition)) || \
                                                     (CY_HPPASS_CONDITION_FIFO_0_LEVEL        == (condition)) || \
                                                     (CY_HPPASS_CONDITION_FIFO_1_LEVEL        == (condition)) || \
                                                     (CY_HPPASS_CONDITION_FIFO_2_LEVEL        == (condition)) || \
                                                     (CY_HPPASS_CONDITION_FIFO_3_LEVEL        == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_0_DAC_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_1_DAC_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_2_DAC_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_3_DAC_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_4_DAC_DONE      == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_0_COMP          == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_1_COMP          == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_2_COMP          == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_3_COMP          == (condition)) || \
                                                     (CY_HPPASS_CONDITION_CSG_4_COMP          == (condition)))

#define CY_HPPASS_AC_ACTION_VALID(action)           ((CY_HPPASS_ACTION_STOP            == (action)) || \
                                                     (CY_HPPASS_ACTION_NEXT            == (action)) || \
                                                     (CY_HPPASS_ACTION_WAIT_FOR        == (action)) || \
                                                     (CY_HPPASS_ACTION_BRANCH_IF_TRUE  == (action)) || \
                                                     (CY_HPPASS_ACTION_BRANCH_IF_FALSE == (action)))

#define CY_HPPASS_MUXSELECT_VALID(select)           (3U >= (select))
/** \endcond */

/***************************************
*       Enumerated Types
***************************************/

/** \addtogroup group_hppass_enums
* \{
*/

/** Return status for functions of the driver for High Performance Programmable Analog Sub-System */
typedef enum
{
    CY_HPPASS_SUCCESS           = CY_RSLT_SUCCESS,
    CY_HPPASS_BAD_PARAM         = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x000UL), /**< Input parameter is out of range or
                                                                                                                  *   input pointer is NULL
                                                                                                                  *   and initialization could not be completed
                                                                                                                  */
    CY_HPPASS_AC_BAD_PARAM      = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x001UL), /**< AC subsystem error */
    CY_HPPASS_AC_INVALID_STATE  = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x101UL), /**< AC invalid state */
    CY_HPPASS_CSG_BAD_PARAM     = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x002UL), /**< CSG subsystem error */
    CY_HPPASS_CSG_INVALID_STATE = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x102UL), /**< CSG invalid state */
    CY_HPPASS_SAR_BAD_PARAM     = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x004UL), /**< SAR subsystem error */
    CY_HPPASS_SAR_INVALID_STATE = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x104UL), /**< SAR invalid state */
    CY_HPPASS_TRIM_ERROR        = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x010UL), /**< Trimming error */
    CY_HPPASS_VDDA_FAIL         = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x020UL), /**< VDDA is out of range */
    CY_HPPASS_TIMEOUT           = CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_GET_MODULE(CY_HPPASS_ID), 0x040UL)  /**< Startup Timeout */
} cy_en_hppass_status_t;

/** \} group_hppass_enums */

/**
* \addtogroup group_hppass_ac_enums
* \{
*/


/**
* The Condition field in the State Transition Table,
* see \ref cy_stc_hppass_ac_stt_t.
*/
typedef enum
{
    CY_HPPASS_CONDITION_FALSE              = 0U,  /**< Hardcoded FALSE value. */
    CY_HPPASS_CONDITION_TRUE               = 1U,  /**< Hardcoded TRUE value. */
    CY_HPPASS_CONDITION_BLOCK_READY        = 2U,  /**< Enabled SAR, CSG block(s) are ready for use.
                                                   *   When this option is used in conjunction with
                                                   *   ACTION = CY_HPPASS_ACTION_WAIT_FOR, AC will stay
                                                   *   in current state, until enabled block(s) are ready for use,
                                                   *   and then go to the next state.
                                                   */
    CY_HPPASS_CONDITION_CNT_DONE           = 3U,  /**< Runout for an interval timer if the ACTION is WAIT_FOR or
                                                   *   runout for a loop counter, if the ACTION is BRANCH IF.
                                                   */
    CY_HPPASS_CONDITION_SAR_GROUP_0_DONE   = 4U,  /**< SAR Sequencer Group 0 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_1_DONE   = 5U,  /**< SAR Sequencer Group 1 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_2_DONE   = 6U,  /**< SAR Sequencer Group 2 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_3_DONE   = 7U,  /**< SAR Sequencer Group 3 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_4_DONE   = 8U,  /**< SAR Sequencer Group 4 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_5_DONE   = 9U,  /**< SAR Sequencer Group 5 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_6_DONE   = 10U, /**< SAR Sequencer Group 6 DONE. */
    CY_HPPASS_CONDITION_SAR_GROUP_7_DONE   = 11U, /**< SAR Sequencer Group 7 DONE. */
    CY_HPPASS_CONDITION_SAR_LIMIT_0        = 12U, /**< SAR Limit Crossing Detector 0. */
    CY_HPPASS_CONDITION_SAR_LIMIT_1        = 13U, /**< SAR Limit Crossing Detector 1. */
    CY_HPPASS_CONDITION_SAR_LIMIT_2        = 14U, /**< SAR Limit Crossing Detector 2. */
    CY_HPPASS_CONDITION_SAR_LIMIT_3        = 15U, /**< SAR Limit Crossing Detector 3. */
    CY_HPPASS_CONDITION_SAR_LIMIT_4        = 16U, /**< SAR Limit Crossing Detector 4. */
    CY_HPPASS_CONDITION_SAR_LIMIT_5        = 17U, /**< SAR Limit Crossing Detector 5. */
    CY_HPPASS_CONDITION_SAR_LIMIT_6        = 18U, /**< SAR Limit Crossing Detector 6. */
    CY_HPPASS_CONDITION_SAR_LIMIT_7        = 19U, /**< SAR Limit Crossing Detector 7. */
    CY_HPPASS_CONDITION_SAR_BUSY           = 20U, /**< SAR Busy status. */
    CY_HPPASS_CONDITION_SAR_FIR_0_DONE     = 21U, /**< SAR FIR0 result done. */
    CY_HPPASS_CONDITION_SAR_FIR_1_DONE     = 22U, /**< SAR FIR1 result done. */
    CY_HPPASS_CONDITION_SAR_QUEUE_HI_EMPTY = 23U, /**< SAR Sequencer Hi Priority Queue is empty. */
    CY_HPPASS_CONDITION_SAR_QUEUE_LO_EMPTY = 24U, /**< SAR Sequencer Low Priority Queue is empty. */
    CY_HPPASS_CONDITION_SAR_QUEUES_EMPTY   = 25U, /**< SAR Sequencer both Queues are empty. */
    CY_HPPASS_CONDITION_TRIG_0             = 32U, /**< HW/FW Trigger 0. */
    CY_HPPASS_CONDITION_TRIG_1             = 33U, /**< HW/FW Trigger 1. */
    CY_HPPASS_CONDITION_TRIG_2             = 34U, /**< HW/FW Trigger 2. */
    CY_HPPASS_CONDITION_TRIG_3             = 35U, /**< HW/FW Trigger 3. */
    CY_HPPASS_CONDITION_TRIG_4             = 36U, /**< HW/FW Trigger 4. */
    CY_HPPASS_CONDITION_TRIG_5             = 37U, /**< HW/FW Trigger 5. */
    CY_HPPASS_CONDITION_TRIG_6             = 38U, /**< HW/FW Trigger 6. */
    CY_HPPASS_CONDITION_TRIG_7             = 39U, /**< HW/FW Trigger 7. */
    CY_HPPASS_CONDITION_FIFO_0_LEVEL       = 42U, /**< FIFO Level 0 trigger. */
    CY_HPPASS_CONDITION_FIFO_1_LEVEL       = 43U, /**< FIFO Level 1 trigger. */
    CY_HPPASS_CONDITION_FIFO_2_LEVEL       = 44U, /**< FIFO Level 2 trigger. */
    CY_HPPASS_CONDITION_FIFO_3_LEVEL       = 45U, /**< FIFO Level 3 trigger. */
    CY_HPPASS_CONDITION_CSG_0_DAC_DONE     = 48U, /**< CSG0 DAC Slope/LUT waveform Done. */
    CY_HPPASS_CONDITION_CSG_1_DAC_DONE     = 49U, /**< CSG1 Slope/LUT waveform Done. */
    CY_HPPASS_CONDITION_CSG_2_DAC_DONE     = 50U, /**< CSG2 Slope/LUT waveform Done. */
    CY_HPPASS_CONDITION_CSG_3_DAC_DONE     = 51U, /**< CSG3 Slope/LUT waveform Done. */
    CY_HPPASS_CONDITION_CSG_4_DAC_DONE     = 52U, /**< CSG4 Slope/LUT waveform Done. */
    CY_HPPASS_CONDITION_CSG_0_COMP         = 56U, /**< CSG0 Comparator Trip. */
    CY_HPPASS_CONDITION_CSG_1_COMP         = 57U, /**< CSG1 Comparator Trip. */
    CY_HPPASS_CONDITION_CSG_2_COMP         = 58U, /**< CSG2 Comparator Trip. */
    CY_HPPASS_CONDITION_CSG_3_COMP         = 59U, /**< CSG3 Comparator Trip. */
    CY_HPPASS_CONDITION_CSG_4_COMP         = 60U, /**< CSG4 Comparator Trip. */

} cy_en_hppass_condition_t;

/**
* The Action field in the State Transition Table,
* see \ref cy_stc_hppass_ac_stt_t.
*/
typedef enum
{
    CY_HPPASS_ACTION_STOP             = 0U,  /**< Stop the Autonomous Controller. */
    CY_HPPASS_ACTION_NEXT             = 1U,  /**< Proceed to the next state in the State Transition Table (default state). */
    CY_HPPASS_ACTION_WAIT_FOR         = 2U,  /**< Wait (indefinitely) for the associated CONDITION to be TRUE
                                              *   before moving to the next state, see \ref cy_en_hppass_condition_t.
                                              */
    CY_HPPASS_ACTION_BRANCH_IF_TRUE   = 3U,  /**< Check if CONDITION has occurred.
                                              *   If it has, take the indicated branch (\ref cy_stc_hppass_ac_stt_t),
                                              *   if not, proceed to the next state.
                                              */
    CY_HPPASS_ACTION_BRANCH_IF_FALSE  = 4U,  /**< Check if CONDITION has occurred.
                                              *   If it has proceed to the next state,
                                              *   if not, take the indicated branch.
                                              */
} cy_en_hppass_action_t;


/** \} group_hppass_ac_enums */

/***************************************
*       Configuration Structures
***************************************/

/**
* \addtogroup group_hppass_ac_data_structures
* \{
*/

/** Startup configuration structure */
typedef struct
{
    uint8_t count;  /**< Count value in HPPASS clock cycles. The range is 0 to 255. Actual value is count + 1 */
    bool sar;       /**< Enable SAR startup */
    bool csgChan;   /**< Enable CSG Channel startup */
    bool csgSlice;  /**< Enable CSG Slice startup */
    bool csgReady;  /**< Enable CSG Auto Zero/Comparator gate startup */
} cy_stc_hppass_startup_t;


/** The Autonomous Controller itself state */
typedef struct
{
    bool running;       /**< The current status (running/stopped) of the Autonomous Controller. */
    uint8_t stateIdx;   /**< The current state of the Autonomous Controller in the State Transition Table. */
} cy_stc_hppass_state_ac_t;


/** The Timer/Counter state of the Autonomous Controller */
typedef struct
{
    uint16_t count;     /**< The current Timer/Counter value. */
    uint8_t stateIdx;   /**< The current State Transition Table state tied to this Timer/Counter. */
    bool busy;          /**< The current Timer/Counter busy/idle status. */
} cy_stc_hppass_state_tc_t;


/** The Autonomous Controller state, used in the \ref Cy_HPPASS_AC_GetState */
typedef struct
{
    cy_stc_hppass_state_ac_t ac;                    /**< The current Autonomous Controller state. */
    cy_stc_hppass_state_tc_t tc[CY_HPPASS_TC_NUM];  /**< The current Timer/Counter states. */
} cy_stc_hppass_state_t;


/**
* Analog Routing MUX control.
*/
typedef struct
{
    bool unlock;        /**< Muxed Sampler unlock to change the MUX setting by \ref cy_stc_hppass_stt_mux_t::chanIdx */
    uint8_t chanIdx;    /**< Muxed Sampler channel selection. Valid Range: 0..3.
                         *   Effective if the \ref cy_stc_hppass_stt_mux_t::unlock is true. Otherwise ignored.
                         */
} cy_stc_hppass_stt_mux_t;


/**
* Configuration structure for one row of the State Transition Table,
* array of these structures defines whole State Transition Table.
* The State Transition Table may have maximum 16 rows.
*/
typedef struct
{
    /* Autonomous Controller section of the State Transition Table state */
    cy_en_hppass_condition_t condition; /**< Condition field. */
    cy_en_hppass_action_t action;       /**< Action field. */
    uint8_t branchStateIdx;             /**< Index of the next state, used if
                                         *   cy_stc_hppass_ac_stt_t::action is
                                         *   \ref CY_HPPASS_ACTION_BRANCH_IF_TRUE or
                                         *   \ref CY_HPPASS_ACTION_BRANCH_IF_FALSE.
                                         *   The range is 0 to 15.
                                         */
    bool interrupt; /**< Send Output Pulse Trigger or generate CPU Interrupt if
                                         *   configured. If cy_stc_hppass_ac_stt_t::action is
                                         *   \ref CY_HPPASS_ACTION_WAIT_FOR, the trigger or
                                         *   interrupt is generated after the condition occurs.
                                         */
    uint16_t count; /**< Field to set the timer/counter value for an interval
                                         *   or loop counts. If cy_stc_hppass_ac_stt_t::action is
                                         *   \ref CY_HPPASS_ACTION_WAIT_FOR, the field is an
                                         *   interval timer, if \ref CY_HPPASS_ACTION_BRANCH_IF_TRUE
                                         *   or \ref CY_HPPASS_ACTION_BRANCH_IF_FALSE - the field is
                                         *   a loop counter. The range is 1 to 4096.
                                         */
    /* GPIO out */
    bool gpioOutUnlock;                 /**< GPIO Out Unlock:
                                         *   false, Lock: GPIO Out does NOT get updated
                                         *   true, Unlock: GPIO Out does get updated
                                         */
    uint8_t gpioOutMsk;                 /**< GPIO out generation, use \ref group_hppass_ac_dout masks */

    /* CSG section of the State Transition Table state */
    bool csgUnlock[CY_HPPASS_CSG_NUM];  /**< CSG Unlock:
                                         *   false, Lock: cy_stc_hppass_ac_stt_t::csgEnable is
                                         *   ignored
                                         *   true, Unlock: CSG state is set per
                                         *   cy_stc_hppass_ac_stt_t::csgEnable
                                         */
    bool csgEnable[CY_HPPASS_CSG_NUM];  /**< CSG Slice Enable.
                                         *   Must be used with the cy_stc_hppass_ac_stt_t::csgUnlock
                                         *   to enable or disable selected CSG Slice.
                                         *   Normally used in conjunction with
                                         *   cy_stc_hppass_ac_stt_t::action ==
                                         *   \ref CY_HPPASS_ACTION_WAIT_FOR and
                                         *   cy_stc_hppass_ac_stt_t::condition ==
                                         *   \ref CY_HPPASS_CONDITION_BLOCK_READY
                                         */
    bool csgDacTrig[CY_HPPASS_CSG_NUM]; /**< CSG Slice Trigger.
                                         *   When the CSG Slice is enabled, this field can be
                                         *   used as CSG DAC Start or Update trigger
                                         *   when cy_stc_hppass_dac_t::start or
                                         *   cy_stc_hppass_dac_t::update is set to
                                         *   \ref CY_HPPASS_DAC_START_AC and
                                         *   \ref CY_HPPASS_DAC_UPDATE_AC correspondingly.
                                         */
    /* SAR section of the State Transition Table state */
    bool sarUnlock;                     /**< SAR Unlock:
                                         *   false, Lock: cy_stc_hppass_ac_stt_t::sarEnable
                                         *   is ignored,
                                         *   true, Unlock: SAR state is set per
                                         *   cy_stc_hppass_ac_stt_t::sarEnable
                                         */
    bool sarEnable;                     /**< SAR Enable.
                                         *   Must be used with the cy_stc_hppass_ac_stt_t::sarUnlock
                                         *   to enable or disable selected CSG Slice.
                                         *   Normally used in conjunction with
                                         *   cy_stc_hppass_ac_stt_t::action ==
                                         *   \ref CY_HPPASS_ACTION_WAIT_FOR and
                                         *   cy_stc_hppass_ac_stt_t::condition ==
                                         *   \ref CY_HPPASS_CONDITION_BLOCK_READY
                                         */
    uint8_t sarGrpMsk;                  /**< The Sequencer Groups mask, use
                                         *  \ref group_hppass_ac_seq_grp.
                                         *   When the SAR is enabled, this field can be
                                         *   used to trigger one ore more SAR Group entries
                                         *   when cy_stc_hppass_sar_grp_t::trig is set to
                                         *   \ref CY_HPPASS_SAR_TRIG_AC.
                                         */
    cy_stc_hppass_stt_mux_t sarMux[CY_HPPASS_SAR_MUX_NUM];  /**< Array of the Muxed Sampler control settings.
                                         *   Effective only if the correspondent
                                         *   cy_stc_hppass_sar_t::muxMode ==
                                         *   \ref CY_HPPASS_SAR_MUX_AC, otherwise ignored.
                                         */
} cy_stc_hppass_ac_stt_t;


/** Configuration structure to set up the Autonomous Controller */
typedef struct
{
    uint8_t sttEntriesNum;              /**< Number of states in State Transition Table */
    const cy_stc_hppass_ac_stt_t * stt; /**< The non-NULL pointer to the array of State
                                         *   Transition Table \ref cy_stc_hppass_ac_stt_t
                                         *   items, the size is specified by the
                                         *   cy_stc_hppass_ac_t::sttEntriesNum.
                                         */
    uint8_t gpioOutEnMsk;               /**< The Autonomous Controller GPIO output enable mask */
    uint16_t startupClkDiv;             /**< Startup Clock Divider. The range is 1 to 256. */
    cy_stc_hppass_startup_t startup[CY_HPPASS_STARTUP_CFG_NUM]; /**< Startup configuration */
} cy_stc_hppass_ac_t;

/** \} group_hppass_ac_data_structures */


/***************************************
*        Function Prototypes
***************************************/

/** \addtogroup group_hppass_ac_functions
*   \{ */


/*******************************************************************************
* Function Name: Cy_HPPASS_AC_UpdateStateTransitionTable
****************************************************************************//**
*
* Used for adding or replacing state into State Transition Table - useful for multiple
* applications accessing the High Performance Programmable Analog Sub-System.
*
* \param numEntries
* Number of entries of the State Transition Table for replace.
* Valid range: 1...\ref CY_HPPASS_AC_STT_SIZE.
*
* \param stateTransitionTable
* Pointer to array containing new set of configuration data for State Transition Table,
* which must be replaced or added. The pointer cannot be NULL.
*
* \param startState
* Defines state in the State Transition Table from which update of the content begins.
* Valid range: 0...\ref CY_HPPASS_AC_STT_SIZE - 1.
*
* \return
* Combined value of all the subsystem statuses, \ref cy_en_hppass_status_t.
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_AC_UpdateStateTransitionTable(uint8_t numEntries, const cy_stc_hppass_ac_stt_t * stateTransitionTable, uint8_t startState);


/*******************************************************************************
* Function Name: Cy_HPPASS_AC_LoadStateTransitionTable
****************************************************************************//**
*
* Configuring the State Transition Table of the the Autonomous Controller
* according to the provided settings.
*
* \param numEntries
* Number of entries in the State Transition Table.
* Valid range: 1...\ref CY_HPPASS_AC_STT_SIZE.
*
* \param stateTransitionTable
* Pointer to array containing configuration data for entire State Transition Table,
* \ref cy_stc_hppass_ac_stt_t. The pointer cannot be NULL.
*
* \return
* Combined value of all the subsystem statuses, \ref cy_en_hppass_status_t.
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_AC_LoadStateTransitionTable(uint8_t numEntries, const cy_stc_hppass_ac_stt_t * stateTransitionTable)
{
    return Cy_HPPASS_AC_UpdateStateTransitionTable(numEntries, stateTransitionTable, 0U);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_AC_IsRunning
****************************************************************************//**
*
* Returns the running/stopped status of the Autonomous Controller.
*
* \return
* Running status: true - running, false - stopped.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_HPPASS_AC_IsRunning(void)
{
    return _FLD2BOOL(HPPASS_ACTRLR_STATUS_STATUS, HPPASS_AC_STATUS(HPPASS_BASE));
}


/*******************************************************************************
* Function Name: Cy_HPPASS_AC_Start
****************************************************************************//**
*
* Starts the Autonomous Controller from the specified STT state.
*
* \note The AC, in case of emergency hanging up,
* can be forcibly stopped by the \ref Cy_HPPASS_DeInit only.
*
* \param startState
* Starting state index in the State Transition Table.

* \param timeoutUs
* The startup timeout in microseconds.
* When non-zero - the function waits for the BLOCK_READY status and check the timeout.
* When zero - the function just triggers AC start command and returns immediately.
*
* \return
* Status of execution:
* - CY_HPPASS_SUCCESS - the AC is started successfully.
* - CY_HPPASS_AC_INVALID_STATE - the AC is already running.
* - CY_HPPASS_AC_BAD_PARAM - input parameter is invalid.
* - CY_HPPASS_VDDA_FAIL - VDDA is out of range.
* - CY_HPPASS_TIMEOUT - the timeout timer has expired before BLOCK_READY condition.
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_AC_Start(uint8_t startState, uint16_t timeoutUs);


/*******************************************************************************
* Function Name: Cy_HPPASS_AC_GetState
****************************************************************************//**
*
* Provides status of operation for the Autonomous Controller.
*
* \param state
* Pointer to structure for receiving state regarding operation conditions
* of the Autonomous Controller, \ref cy_stc_hppass_state_t.
* The pointer cannot be NULL.
*
* \return
* Status of execution, \ref cy_en_hppass_status_t.
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_AC_GetState(cy_stc_hppass_state_t * state);


/*******************************************************************************
* Function Name: Cy_HPPASS_AC_IsBlockReady
****************************************************************************//**
*
* Returns the AC block ready status.
*
* \return
* Running status: true - ready for use, false - not ready.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_HPPASS_AC_IsBlockReady(void)
{
    return _FLD2BOOL(HPPASS_ACTRLR_BLOCK_STATUS_READY, HPPASS_AC_BLOCK_STATUS(HPPASS_BASE));
}


/** \} group_hppass_ac_functions */

/** \cond
* Internal function - not to be used in the user code.
* The function suppress the Autonomous Controller state (stopped/running) check
* in case when it is called from the cy_hppass_*.c files
* to do not repeat this checks repeatedly in 'for' cycles.
*/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_AC_CondRunCheck(void)
{
    #if !defined CY_HPPASS_SUPPRESS_AC_RUN_CHECK
        cy_en_hppass_status_t result = CY_HPPASS_SUCCESS;

        if (Cy_HPPASS_AC_IsRunning())
        {
            result = CY_HPPASS_AC_INVALID_STATE;
        }

        return result;
    #else
        return CY_HPPASS_SUCCESS;
    #endif
}
/** \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXS40MCPASS */

#endif /** !defined(CY_HPPASS_AC_H) */

/* [] END OF FILE */
