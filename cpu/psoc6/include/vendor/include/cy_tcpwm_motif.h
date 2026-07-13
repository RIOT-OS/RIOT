/***************************************************************************//**
* \file cy_tcpwm_motif.h
* \version 1.90
*
* The header file of the TCPWM  MOTIF driver.
*
********************************************************************************
* \copyright
* Copyright 2023-2025 Cypress Semiconductor Corporation (an Infineon company) or
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


#if !defined(CY_TCPWM_MOTIF_H)
#define CY_TCPWM_MOTIF_H

#include "cy_tcpwm.h"

#ifdef CY_IP_MXTCPWM

#if defined (CY_IP_MXS40TCPWM) || defined (CY_DOXYGEN)

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_tcpwm_motif
* Driver API for Motion Interface.
*
* The functions and other declarations used in this part of the driver are in cy_tcpwm_motif.h.
* You can also include cy_pdl.h to get access to all functions and declarations in the PDL.
*
* The MOTIF unit is a flexible and powerful component for motor control systems that use Rotary Encoders, Hall Sensors as feedback loop.
* The several configuration schemes of the module, target a very large universe of motor control application requirements.
* This enables the build of simple and complex control feedback loops, for industrial and automotive motor applications, targeting high performance motion and position monitoring.
*
* Each MOTIF module can operate in four different modes,
* * Quadrature Decoder
* * Hall Sensor
* * Stand-alone Multi-channel Mode
* * Quadrature mode with stand-alone Multi-channel.

*
* The MOTIF module has the following features:
* * Interface for position measurement.
* * Interface for motor revolution measurement.
* * Interface for velocity measurement.
* * Interrupt sources for phase error, motor revolution, direction change and error on phase decoding
* * Simple build-in mode for brush less DC motor control.
* * Shadow register for the multi-channel pattern.
* * Complete synchronization with the PWM signals and the multi-channel pattern update
* * Interrupt sources for Correct Hall Event detection, Wrong Hall Event Detection
* * Previous and Expected Pattern monitoring to detect less torque situations
* * Input and output Look-up table for automatic modulation control
* * Simple usage with Hall Sensor Mode
* * Stand-alone Multi-Channel mode
* * Shadow register for the multi-channel pattern
* * Output Look-up table for automatic modulation control
* * Second Compare Match (CC_MATCH1) event is only available in TCPWM Version 2.
*
* \section group_tcpwm_motif_configuration Configuration Considerations
*
* The QuadDec configuration can be divided to number of sequential
* steps listed below:
* * \ref group_tcpwm_motif_config
* * \ref group_tcpwm_motif_clock
* * \ref group_tcpwm_motif_enable
* * \ref group_tcpwm_motif_start
*
* \subsection group_tcpwm_motif_config Configure MOTIF Module
* To configure MOTIF module in HALL Sensor mode, Quadrature mode, Multi Channel Pattern mode provide the configuration parameters in the
* \ref cy_stc_tcpwm_motif_hall_sensor_config_t structure for Hall Sensor Mode.
* \ref cy_stc_tcpwm_motif_mcp_config_t structure for Multi Channel Pattern Mode.
* \ref cy_stc_tcpwm_motif_quaddec_config_t structure for Quadrature Mode.
* The Configuration structure can be modified through software, but if the configurator in ModusToolbox is used
* then the configuration structure will be updated with the users input. To initialize
* the driver, call \ref Cy_TCPWM_MOTIF_Hall_Sensor_Init \ref Cy_TCPWM_MOTIF_MCP_Init \ref Cy_TCPWM_MOTIF_Quaddec_Init function providing a pointer to the populated
* \ref cy_stc_tcpwm_motif_hall_sensor_config_t \ref cy_stc_tcpwm_motif_mcp_config_t \ref cy_stc_tcpwm_motif_quaddec_config_t structures respectively.
*
*
* \subsection group_tcpwm_motif_clock Assign Clock Divider
* The clock source must be connected to proper working.
* Any of the peripheral clock dividers could be used. Use the
* \ref group_sysclk driver API to do that.
*
* \subsection group_tcpwm_motif_enable Enable Motion interface module
* MOTIF block has to be enabled before starting
*
*
* \subsection group_tcpwm_motif_start Start MOTIF block
* MOTIF has to be started
*
* \{
*/

/**
* \defgroup group_tcpwm_macros_motif                Macros
* \defgroup group_tcpwm_functions_motif             Functions
* \defgroup group_tcpwm_functions_motif_hall        HALL Sensor specific functions
* \defgroup group_tcpwm_functions_motif_mcp         Multi-Channel specific functions
* \defgroup group_tcpwm_functions_motif_quaddec     Quadrature decoder specific functions
* \defgroup group_tcpwm_data_structures_motif Data Structures
* \} */

/**
* \addtogroup group_tcpwm_macros_motif
* \{
*/
#define CY_MOTIF_CORRECT_HALL_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_CHES_Msk                    /**< Correct Hall Event Status Interrupt  */
#define CY_MOTIF_WRONG_HALL_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_WHES_Msk                      /**< Wrong Hall Event Status Interrupt  */
#define CY_MOTIF_HALL_INPUT_UPDATE_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_HIES_Msk               /**< Hall Inputs Update Event Status Interrupt  */
#define CY_MOTIF_MCP_SHADOW_TRANSFER_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_MSTS_Msk             /**< Multi-Channel pattern shadow transfer status  */
#define CY_MOTIF_QUADRATURE_INDEX_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_INDXS_Msk               /**< Quadrature Index Status Interrupt  */
#define CY_MOTIF_QUADRATURE_PHASE_ERROR_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_ERRS_Msk          /**< Quadrature Phase Error Status Interrupt  */
#define CY_MOTIF_QUADRATURE_CLOCK_ERROR_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_CNTS_Msk          /**< Quadrature CLK error Interrupt  */
#define CY_MOTIF_QUADRATURE_DIRECTION_CHANGE_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_DIRS_Msk     /**< Quadrature Direction Change Interrupt  */
#define CY_MOTIF_QUADRATURE_PERIOD_CLOCK_ERROR_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_PCLKS_Msk  /**< QQuadrature Period Clk Status Interrupt  */
#define CY_MOTIF_CURRENT_PATTERN_MATCH_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_CPES_Msk           /**< Equality between Sampled Hall input values and Current Pattern value detected.  */
#define CY_MOTIF_PREVIOUS_PATTERN_MATCH_EVENT TCPWM_MOTIF_GRP_MOTIF_PFLG_PPES_Msk          /**< Equality between Sampled Hall input values and Previous Pattern value detected.  */

/** Interrupt events in the MOTIF Module */
#define CY_MOTIF_ALL_INTERRUPTS (CY_MOTIF_CORRECT_HALL_EVENT                  |\
                                 CY_MOTIF_WRONG_HALL_EVENT                    |\
                                 CY_MOTIF_HALL_INPUT_UPDATE_EVENT             |\
                                 CY_MOTIF_MCP_SHADOW_TRANSFER_EVENT           |\
                                 CY_MOTIF_QUADRATURE_INDEX_EVENT              |\
                                 CY_MOTIF_QUADRATURE_PHASE_ERROR_EVENT        |\
                                 CY_MOTIF_QUADRATURE_CLOCK_ERROR_EVENT        |\
                                 CY_MOTIF_QUADRATURE_DIRECTION_CHANGE_EVENT   |\
                                 CY_MOTIF_QUADRATURE_PERIOD_CLOCK_ERROR_EVENT |\
                                 CY_MOTIF_CURRENT_PATTERN_MATCH_EVENT         |\
                                 CY_MOTIF_PREVIOUS_PATTERN_MATCH_EVENT)

/** Interrupt events in the MOTIF Module HALL Sensor mode */
#define CY_MOTIF_HALL_INTERRUPTS (CY_MOTIF_CORRECT_HALL_EVENT                  |\
                                  CY_MOTIF_WRONG_HALL_EVENT                    |\
                                  CY_MOTIF_HALL_INPUT_UPDATE_EVENT             |\
                                  CY_MOTIF_MCP_SHADOW_TRANSFER_EVENT           |\
                                  CY_MOTIF_CURRENT_PATTERN_MATCH_EVENT         |\
                                  CY_MOTIF_PREVIOUS_PATTERN_MATCH_EVENT)

/** Interrupt events in the MOTIF Module Multi Channel Pattern Mode */
#define CY_MOTIF_MCP_INTERRUPTS (CY_MOTIF_MCP_SHADOW_TRANSFER_EVENT           |\
                                 CY_MOTIF_CURRENT_PATTERN_MATCH_EVENT         |\
                                 CY_MOTIF_PREVIOUS_PATTERN_MATCH_EVENT)

/** Interrupt events in the MOTIF Module Quadrature Mode */
#define CY_MOTIF_QUDRATURE_INTERRUPTS (CY_MOTIF_QUADRATURE_INDEX_EVENT              |\
                                       CY_MOTIF_QUADRATURE_PHASE_ERROR_EVENT        |\
                                       CY_MOTIF_QUADRATURE_CLOCK_ERROR_EVENT        |\
                                       CY_MOTIF_QUADRATURE_DIRECTION_CHANGE_EVENT   |\
                                       CY_MOTIF_QUADRATURE_PERIOD_CLOCK_ERROR_EVENT)
/** \} group_tcpwm_macros_motif */

/**
* \addtogroup group_tcpwm_data_structures_motif
* \{
*/
/** Enumeration values for Motif mode Configuration */
typedef enum
{
    CY_MOTIF_HALL_SENSOR_MODE               = 0UL,   /**< MOTIF mode is set to HALL Sensor.                      */
    CY_MOTIF_QUAD_DECODER_MODE              = 1UL,   /**< MOTIF mode is set to Quadrature decoder.               */
    CY_MOTIF_STANDALONE_MULTICHANNEL_MODE   = 2UL,   /**< MOTIF mode is set to stand alone multi channel.        */
    CY_MOTIF_SAMC_QD_MODE                   = 3UL,   /**< MOTIF mode is set to stand alone multi channel  and quadrature decoder */
} cy_en_motif_mode_t;

// /** Enumeration values for quadrature mode Configuration */
typedef enum
{
    CY_MOTIF_PD_QUAD_MODE                   = 0UL,   /**< Position decoder in quadrature decoder mode. */
    CY_MOTIF_PD_DIRECTION_COUNT_MODE        = 1UL,   /**< Position decoder in direction count mode.     */
} cy_en_position_decoder_mode_t;

/** Enumeration values for Multi Channel Pattern Update Configuration */
typedef enum
{
    CY_MULTI_CHANNEL_PATTERN_UPDATE_BYHW    = 0UL,    /**< Multichannel pattern updated by Hardware             */
    CY_MULTI_CHANNEL_PATTERN_UPDATE_BYSW    = 1UL,    /**< Multichannel pattern updated by software. multi channel patter will be updated only
                                                           when has  set MCMS register MNPS filed            */
} cy_en_mcp_mode_t;

/** Enumeration values for Motif HALL event Configuration */
typedef enum
{
    CY_WHE_OR_MODULE_PIN                    = 0UL,    /**< OR between Wrong hall event and the module pin is the source for fault or correct event */
    CY_WRONG_HALL_EVENT                     = 1UL,    /**< Wrong hall event is the source for fault or correct event */
    CY_MODULE_PIN                           = 2UL,    /**< the module pin is the source for fault or correct event   */
} cy_en_hall_input_event_selection_t;

/** Enumeration values for Motif Sampling Trigger Configuration */
typedef enum
{
    CY_SAMPLING_TRIGGER_RISING_EDGE         = 0UL,    /**< Rising edge is used of the selected MOTIFx.HSD signal to trigger a sample of the Hall inputs. */
    CY_SAMPLING_TRIGGER_FALLING_EDGE        = 1UL,    /**< Falling edge is used of the selected MOTIFx.HSD signal to trigger a sample of the Hall inputs. */
} cy_en_trigger_edge_selection_t;



/** Enumeration values for Motif Low Pass Filter Configuration */
typedef enum
{
    CY_LOWPASS_FILTER_DISABLED    = 0UL,        /**< Low pass filter is disabled. */
    CY_LOWPASS_OF_1CLOCK_CYCLE    = 1UL,        /**< Low pass of one clock cycle */
    CY_LOWPASS_OF_2CLOCK_CYCLE    = 2UL,        /**< Low pass of two clock cycle */
    CY_LOWPASS_OF_4CLOCK_CYCLE    = 3UL,        /**< Low pass of four clock cycle */
    CY_LOWPASS_OF_8CLOCK_CYCLE    = 4UL,        /**< Low pass of eight clock cycle */
    CY_LOWPASS_OF_16CLOCK_CYCLE   = 5UL,        /**< Low pass of sixteen clock cycle */
    CY_LOWPASS_OF_32CLOCK_CYCLE   = 6UL,        /**< Low pass of thirty two clock cycle */
    CY_LOWPASS_OF_64CLOCK_CYCLE   = 7UL,        /**< Low pass of sixty four clock cycle */
    CY_LOWPASS_OF_128CLOCK_CYCLE  = 8UL,        /**< Low pass of one twenty eight clock cycle */
    CY_LOWPASS_OF_256CLOCK_CYCLE  = 9UL,        /**< Low pass of two fifty six clock cycle */
    CY_LOWPASS_OF_512CLOCK_CYCLE  = 10UL,       /**< Low pass of five twelve clock cycle */
    CY_LOWPASS_OF_1024CLOCK_CYCLE = 11UL,       /**< Low pass of one thousand twenty four clock cycle */
} cy_en_low_pass_filter_config_t;

/** Motif Suspend Configuration */
typedef enum
{
    CY_MOTIF_SUSPEND_REQUEST_IGNORED = 0UL,      /**< Motif suspend request is ignored. motif module continues to run as configured. */
    CY_MOTIF_MODULE_STOP             = 1UL,      /**< Motif module is stopped immediately */
    CY_MOTIF_SUSPEND_NEXT_INDEX      = 2UL,      /**< Motif suspend in the next index for quad mode and quad with stand alone mode. other modes stop immediately */
    CY_MOTIF_SUSPEND_NEXT_PHASE      = 3UL,      /**< Motif suspend in the next phase for quad mode and quad with stand alone mode. other modes stop immediately */
    CY_MOTIF_MODULE_STOP_RESET       = 4UL,      /**< Motif module is stopped immediately. Multi channel pattern is to the reset value */
    CY_MOTIF_SUSPEND_SYNCH           = 5UL,      /**< Suspend with sync of pwm signal. Multi channel pattern is to the reset value. quad mode stop immediately */
} cy_en_motif_suspend_config_t;

/** Hall look up table data */
typedef struct cy_stc_tcpwm_motif_lut
{
    uint8_t             hlut_data_pos0;    /**< Hall look up table data position 0 */
    uint8_t             hlut_data_pos1;    /**< Hall look up table data position 1 */
    uint8_t             hlut_data_pos2;    /**< Hall look up table data position 2 */
    uint8_t             hlut_data_pos3;    /**< Hall look up table data position 3 */
    uint8_t             hlut_data_pos4;    /**< Hall look up table data position 4 */
    uint8_t             hlut_data_pos5;    /**< Hall look up table data position 5 */
}cy_stc_tcpwm_motif_hlut_t;

/** HALL Sensor output event node selection */
typedef enum
{
    CY_IGNORE_HALL_SENSOR_EVENT      = 0UL,   /**< Ignore hall sensor event */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT0 = 1UL,   /**< Forward hall sensor event to out0 */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT1 = 2UL,   /**< Forward hall sensor event to out1 */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT2 = 3UL,   /**< Forward hall sensor event to out2 */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT3 = 4UL,   /**< Forward hall sensor event to out3 */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT4 = 5UL,   /**< Forward hall sensor event to out4 */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT5 = 6UL,   /**< Forward hall sensor event to out5 */
    CY_FWD_HALL_SENSOR_EVENT_TO_OUT6 = 7UL,   /**< Forward hall sensor event to out6 */
} cy_en_hall_sensor_event_output_node_t;


/** Multi channel pattern output event node selection */
typedef enum
{
    CY_IGNORE_MULTI_CHANNEL_EVENT      = 0UL,   /**< Ignore multi channel event */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT0 = 1UL,   /**< Forward multi channel event to out0 */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT1 = 2UL,   /**< Forward multi channel event to out1 */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT2 = 3UL,   /**< Forward multi channel event to out2 */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT3 = 4UL,   /**< Forward multi channel event to out3 */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT4 = 5UL,   /**< Forward multi channel event to out4 */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT5 = 6UL,   /**< Forward multi channel event to out5 */
    CY_FWD_MULTI_CHANNEL_EVENT_TO_OUT6 = 7UL,   /**< Forward multi channel event to out6 */
} cy_en_multi_channel_event_output_node_t;



/** Quaddec index marker generation */
typedef enum
{
    CY_QUADDEC_NO_INDEX_MARKER_GENERATION          = 0UL,   /**< No index marker generation. */
    CY_QUADDEC_FIRST_INDEX_OCCURRENCE_GENERATION   = 1UL,   /**< First Index occurrence */
    CY_QUADDEC_ALL_INDEX_OCCURRENCE_GENERATION     = 2UL,   /**< First Index occurrence */
} cy_en_quaddec_index_occurrence_t;

/** Quaddec Phase edge mode. */
typedef enum
{
    CY_TCPWM_DETECT_ON_BOTH_EDGE          = 0UL,   /**< Clock decoded on both rising edge and falling edge */
    CY_TCPWM_DETECT_ON_RISING_EDGE        = 1UL,   /**< Clock decoded on rising edge. */
    CY_TCPWM_DETECT_ON_FALLING_EDGE       = 2UL,   /**< Clock decoded on falling edge */
    CY_TCPWM_IGNORE_EDGE                  = 3UL,   /**< Clock is not decoded. */
} cy_en_tcpwm_edge_config_t;

/** Quaddec clock and index selector */
typedef enum
{
    CY_QUADDEC_DISABLED          = 0UL,   /**< Clock signal is not used in quaddec mode or direction count mode. */
    CY_QUADDEC_POSI0             = 1UL,   /**< Clock signal POSI0 is used in quaddec mode or direction count mode. */
    CY_QUADDEC_POSI1             = 2UL,   /**< Clock signal POSI1 is used in quaddec mode or direction count mode. */
    CY_QUADDEC_POSI2             = 3UL,   /**< Clock signal POSI2 is used in quaddec mode or direction count mode. */
} cy_en_quaddec_signal_selector_t;


/** Quaddec event output node selector */
typedef enum
{
    CY_IGNORE_QUADDEC_EVENT      = 0UL,   /**< Ignore quaddec event */
    CY_FWD_QUADDEC_EVENT_TO_OUT0 = 1UL,   /**< Forward quaddec event to out0 */
    CY_FWD_QUADDEC_EVENT_TO_OUT1 = 2UL,   /**< Forward quaddec event to out1 */
    CY_FWD_QUADDEC_EVENT_TO_OUT2 = 3UL,   /**< Forward quaddec event to out2 */
    CY_FWD_QUADDEC_EVENT_TO_OUT3 = 4UL,   /**< Forward quaddec event to out3 */
    CY_FWD_QUADDEC_EVENT_TO_OUT4 = 5UL,   /**< Forward quaddec event to out4 */
    CY_FWD_QUADDEC_EVENT_TO_OUT5 = 6UL,   /**< Forward quaddec event to out5 */
    CY_FWD_QUADDEC_EVENT_TO_OUT6 = 7UL,   /**< Forward quaddec event to out6 */
} cy_en_quaddec_event_output_node_t;


/** Low pass filter numbers */
typedef enum
{
    CY_LOW_PASS_FILTER_0    = 0UL,        /**< Low pass filter 0  */
    CY_LOW_PASS_FILTER_1    = 1UL,        /**< Low pass filter 1  */
    CY_LOW_PASS_FILTER_2    = 2UL,        /**< Low pass filter 2  */
} cy_en_low_pass_filter_number_t;

/** Multi Channel Look Up table configuration */
typedef struct cy_stc_tcpwm_motif_mlut
{
    uint32_t             mlut_data_pos0;    /**< Multi channel look up table data position 0 */
    uint32_t             mlut_data_pos1;    /**< Multi channel look up table data position 1 */
    uint32_t             mlut_data_pos2;    /**< Multi channel look up table data position 2 */
    uint32_t             mlut_data_pos3;    /**< Multi channel look up table data position 3 */
    uint32_t             mlut_data_pos4;    /**< Multi channel look up table data position 4 */
    uint32_t             mlut_data_pos5;    /**< Multi channel look up table data position 5 */
}cy_stc_tcpwm_motif_mlut_t;

/** User to make sure that all events are not routed to the same node */
typedef struct cy_stc_hall_sensor_output_config
{
    cy_en_hall_sensor_event_output_node_t edge_detect_event_config;         /**< Forward edge detection event to  */
    cy_en_hall_sensor_event_output_node_t correct_hall_event_config;        /**< Forward edge detection event to  */
    cy_en_hall_sensor_event_output_node_t idle_wrong_hall_event_config;     /**< Forward edge detection event to  */
    cy_en_hall_sensor_event_output_node_t start_event_config;               /**< Forward edge detection event to  */
    cy_en_hall_sensor_event_output_node_t pattern_mismatch_event_config;    /**< Forward edge detection event to  */
    cy_en_hall_sensor_event_output_node_t curr_pattern_match_event_config;  /**< Forward edge detection event to  */
    cy_en_hall_sensor_event_output_node_t prev_pattern_match_event_config;  /**< Forward edge detection event to  */
}cy_stc_hall_output_config_t;

/** MOTIF Multi channel output configuration structure */
typedef struct cy_stc_multi_channel_output_config
{
    cy_en_multi_channel_event_output_node_t pattern_update_event_config; /**< Forward pattern update event  */
    cy_en_multi_channel_event_output_node_t update_sync_event_config;    /**< Forward update sync event */
    cy_en_multi_channel_event_output_node_t start_event_config;          /**< Forward start event */
}cy_stc_multi_channel_output_config_t;

/** MOTIF configuration structure */
typedef struct cy_stc_tcpwm_motif_hall_sensor_config
{
    cy_en_mcp_mode_t        mcp_mode;                                /**< Multi channel pattern update by Hardware or software */
    cy_en_hall_input_event_selection_t    fault_event_selection;     /**< Event selection for fault HALL Sensor event */
    cy_en_hall_input_event_selection_t    correct_event_selection;   /**< Event selection for correct HALL Sensor event */
    cy_en_trigger_edge_selection_t  edge_for_sampling_trigger;       /**< edge selection for sampling trigger */
    cy_en_trigger_edge_selection_t  edge_for_mset_trigger;           /**< edge selection for Multi-Channel pattern update mset trigger */
    cy_en_trigger_edge_selection_t  edge_for_msync_trigger;          /**< edge selection for Multi-Channel pattern update msync trigger */
    bool external_wrong_hall_event_enable;                           /**< External wrong hall event emulation signal enable or disable */
    bool external_wrong_hall_event_active_level;                     /**< External wrong hall event emulation signal is active high or active low. */
    bool              sync_bypass;                                   /**< Bypasses synchronization signal.
                                                                          0: Update of the Multi-Channel pattern is only done after the sync signal is detected,
                                                                          1: Update of the Multi-Channel pattern is done immediately */
    bool              delay_bypass;                                  /**< will bypass the delay between a Correct Hall Event and the enable of the Multi-Channel Pattern update.
                                                                          0: Multi-Channel Pattern update is only done after the external SET signal is detected.
                                                                          1:  Multi-Channel Pattern update is done immediately after a Correct Hall Event is detected. */
    bool              fault_clear;                                   /**< enable/disable a clear of the Multi-Channel pattern, whenever a Hall Fault Event is detected.
                                                                          0: Multi-Channel pattern clear with Hall Fault Event disabled.
                                                                          1: Multi-Channel pattern clear with Hall Fault Event enabled. */
    bool              debounce_bypass;                                /**< Setting this true, will disable the usage of the debounce delay pins.
                                                                          This means that after the low pass filter, the hall inputs are immediately sampled and compared against the expected and current patterns. */
    bool              pp_compare;                                     /**< Setting this bitfield to 1, enables the compare logic between the sampled hall input values and the previous hall pattern.
                                                                          This will be used together with the comparison of the current and expected pattern to generate a wrong hall event. */
    cy_en_low_pass_filter_config_t low_pass_filter_config;           /**< low pass filter config */
    bool              lut_enable;                                    /**< Setting this bitfield to 1, will enable the usage of the Hall Look-up table.*/
    cy_stc_tcpwm_motif_hlut_t *hlut_config;                          /**< Hall Sensor LUT configuration. Applicable when LUT mode is enabled. */
    cy_stc_tcpwm_motif_mlut_t *mlut_config;                          /**< Multi channel pattern LUT configuration. Applicable when LUT mode is enabled. */
    cy_stc_hall_output_config_t  *output_config;              /**< Configuration for output events of hall sensor */

}cy_stc_tcpwm_motif_hall_sensor_config_t;

/** MOTIF Quadrature mode output configuration structure */
typedef struct cy_stc_quaddec_output_config
{
    cy_en_quaddec_event_output_node_t clock_event_config;           /**< Forward clock event  */
    cy_en_quaddec_event_output_node_t direction_event_config;       /**< Forward direction event */
    cy_en_quaddec_event_output_node_t period_clock_event_config;    /**< Forward period clock event */
    cy_en_quaddec_event_output_node_t clear_capture_event_config;   /**< Forward clear capture event  */
    cy_en_quaddec_event_output_node_t index_event_config;           /**< Forward index event */
    cy_en_quaddec_event_output_node_t start_event_config;           /**< Forward start event */
}cy_stc_quaddec_output_config_t;


/** Quadrature decoder configuration structure */
typedef struct cy_stc_tcpwm_motif_quaddec_config
{
    cy_en_position_decoder_mode_t   position_decoder_mode;           /**< Sets the position decoder mode to either quadrature mode or Direction count mode. */
    bool              phase_A_level_selector;                        /**< Sets Phase A level Selector. 0:Active high, 1: Active Low */
    bool              phase_B_level_selector;                        /**< Sets Phase B level Selector. 0:Active high, 1: Active Low */
    bool              phase_signal_swap;                             /**< 0: Sets Phase A leading signal for clockwise rotation. 1: Sets Phase B leading signal for clockwise rotation. */
    bool              index_signal_level_selector;                   /**< Sets Index signal level Selector. 0:Active high, 1: Active Low */
    cy_en_quaddec_index_occurrence_t    progindex_occurrence;        /**< Sets PROGIndex output event occurrence configuration. */
    cy_en_tcpwm_edge_config_t           phae_A_edge_mode;            /**< Sets edge mode for Phase A */
    cy_en_tcpwm_edge_config_t           phae_B_edge_mode;            /**< Sets edge mode for Phase B */
    cy_en_quaddec_signal_selector_t     clock_signal_for_phase_A;    /**< Sets input signal for Phase A */
    cy_en_quaddec_signal_selector_t     clock_signal_for_phase_B;    /**< Sets input signal for Phase B */
    cy_en_quaddec_signal_selector_t     clock_signal_for_index;      /**< Sets input signal for index */
    cy_en_low_pass_filter_config_t      low_pass_filter_config;      /**< low pass filter config */
    cy_stc_quaddec_output_config_t      *output_config;              /**< Configuration for output events of quaddec */
}cy_stc_tcpwm_motif_quaddec_config_t;


/** Multi Channel mode configuration structure */
typedef struct cy_stc_tcpwm_motif_mcp_config
{
    bool              sync_bypass;    /**< Bypasses synchronization signal.
                                           0: Update of the Multi-Channel pattern is only done after the sync signal is detected,
                                           1: Update of the Multi-Channel pattern is done immediately */
    cy_en_mcp_mode_t        mcp_mode;                          /**< Multi channel pattern update by Hardware or software */
    cy_en_trigger_edge_selection_t  edge_for_mset_trigger;     /**< edge selection for mset trigger */
    cy_en_trigger_edge_selection_t  edge_for_msync_trigger;    /**< edge selection for msync trigger */
    bool              lut_enable;                              /**< Setting this bitfield to 1, will enable the usage of the Hall Look-up table.*/
    cy_stc_tcpwm_motif_mlut_t *mlut_config;                    /**< Multi channel pattern LUT configuration. Applicable when LUT mode is enabled. */
    cy_stc_multi_channel_output_config_t *output_config;       /**< Configuration for output events of mcp */

}cy_stc_tcpwm_motif_mcp_config_t;


/** Multi Channel mode and quaddec configuration structure */
typedef struct cy_stc_tcpwm_motif_mcp_quaddec_config
{
    cy_stc_tcpwm_motif_mcp_config_t       *mcp_config;       /**< multi channel pattern config */
    cy_stc_tcpwm_motif_quaddec_config_t   *quaddec_config;   /**< Quad decoding config */
}cy_stc_tcpwm_motif_mcp_quaddec_config_t;

/** \} group_tcpwm_data_structures_motif */


/**
* \addtogroup group_tcpwm_functions_motif
* \{
*/

__STATIC_INLINE void Cy_TCPWM_MOTIF_Enable(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Disable(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Start(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Stop(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_DeInit(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Suspend(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_motif_suspend_config_t suspend_config);
__STATIC_INLINE uint32_t Cy_TCPWM_MOTIF_GetInterruptStatus(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_ClearInterrupt(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mask);
__STATIC_INLINE void Cy_TCPWM_MOTIF_SetInterrupt(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mask);
__STATIC_INLINE void Cy_TCPWM_MOTIF_SetInterruptMask(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mask);
__STATIC_INLINE uint32_t Cy_TCPWM_MOTIF_GetInterruptMask(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE uint8_t Cy_TCPWM_MOTIF_Get_LUT_Status(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Set_LUT_Mode(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool lut_enable);


/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Get_Lowpass_Filter_Status
***************************************************************************//**
*
* Returns the status of the low pass filter.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param lowpass_filter low pass filter. \ref cy_en_low_pass_filter_number_t
*
* \return The status of the low pass filter
*
******************************************************************************/
uint16_t Cy_TCPWM_MOTIF_Get_Lowpass_Filter_Status(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_low_pass_filter_number_t lowpass_filter);

/** \} group_tcpwm_functions_motif */


/**
* \addtogroup group_tcpwm_functions_motif_hall
* \{
*/

__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_HLUT(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_hlut_t *hlut_config);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Configure_Debounce_Bypass(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool bypass_sd);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_Hall_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_HALL_Previous_Pattern_Compare(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool pp_compare);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Set_HALL_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint8_t current_pattern, uint8_t expected_pattern, uint8_t previous_pattern);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Hall_Input_Sample_Trigger(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_HALL_Output_Config(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_hall_output_config_t *output_config);
__STATIC_INLINE uint32_t Cy_TCPWM_MOTIF_HALL_Get_Output_Modulation_Value(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_HALL_Sample_Trigger_EdgeSel(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_trigger_edge_selection_t edge_select);

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Hall_Sensor_Init
***************************************************************************//**
*
* MOTIF module initialization in Hall sensor Mode
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param config Hall sensor configuration \ref cy_stc_tcpwm_motif_hall_sensor_config_t
*
* \return Returns the status of the operation.
*
*******************************************************************************/
cy_en_tcpwm_status_t Cy_TCPWM_MOTIF_Hall_Sensor_Init(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_hall_sensor_config_t const *config);

/** \} group_tcpwm_functions_motif_hall */


/**
* \addtogroup group_tcpwm_functions_motif_mcp
* \{
*/

__STATIC_INLINE void Cy_TCPWM_MOTIF_Set_MCP_Value(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mcp_value);
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_Value_When_Fault_Hall(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mcp_value);
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_Bypass_Delay(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool bypass_delay);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Clear_Multi_Channel_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Clear_Updation_Of_Multi_Channel_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern_Immediately(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE bool Cy_TCPWM_MOTIF_Get_Multi_Channel_Pattern_Update_Status(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_SW_Update_Enable(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool sw_update_enable);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Multi_Channel_Output_Config(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_multi_channel_output_config_t *output_config);
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_Bypass_Synchronization(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool bypass_sync);

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Update_MLUT
***************************************************************************//**
*
* Update look up table for multi channel pattern MLUT mode
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mlut_config  Multi Channel Pattern Look UP Table values. \ref cy_stc_tcpwm_motif_mlut_t
*
* \note Update look up table for multi channel pattern MLUT
*   User writes HLUT and MLUT when MOTIF is stopped.
*   Writing into both LUT while MOTIF is started might lead to incorrect behavior.
*   The LUT is loaded with forward or reverse pattern depending on motor desired direction.
*   The pattern is stored in LUT independently from motor position.
*   The hardware is responsible to find out the current motor position and to load the right hall and modulation pattern.
*   HLUT and MLUT share the same hardware pointer for data extraction, therefore user needs to take care of keeping them aligned with respect to LUT address.
*   A wrong programming might lead to incorrect behavior.
******************************************************************************/
void Cy_TCPWM_MOTIF_Update_MLUT(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_mlut_t *mlut_config);

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_MCP_Init
***************************************************************************//**
*
* MOTIF module initialization in Multi Channel Pattern Mode
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mcp_config Multi Channel Pattern configuration \ref cy_stc_tcpwm_motif_mcp_config_t
*
* \return Returns the status of the operation.
*
******************************************************************************/
cy_en_tcpwm_status_t Cy_TCPWM_MOTIF_MCP_Init(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_mcp_config_t const *mcp_config);

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_MCP_Quaddec_Init
***************************************************************************//**
*
* MOTIF module initialization in Quadrature Decoder Mode and Multi Channel Pattern mode.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mcp_quad_config MCP and Quad configuration \ref cy_stc_tcpwm_motif_mcp_quaddec_config_t
*
* \return Returns the status of the operation.
*
*
******************************************************************************/
cy_en_tcpwm_status_t Cy_TCPWM_MOTIF_MCP_Quaddec_Init(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_mcp_quaddec_config_t const *mcp_quad_config);

/** \} group_tcpwm_functions_motif_mcp */


/**
* \addtogroup group_tcpwm_functions_motif_quaddec
* \{
*/

__STATIC_INLINE void Cy_TCPWM_MOTIF_Configure_Postion_Decoder_Mode(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_position_decoder_mode_t mode);
__STATIC_INLINE bool Cy_TCPWM_MOTIF_Get_Quaddec_Rotation_Direction(TCPWM_MOTIF_GRP_MOTIF_Type *base);
__STATIC_INLINE void Cy_TCPWM_MOTIF_Quaddec_Output_Config(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_quaddec_output_config_t *output_config);

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Quaddec_Init
***************************************************************************//**
*
* MOTIF module initialization in Quadrature Decoder Mode
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param config Quadrature decoder configuration \ref cy_stc_tcpwm_motif_quaddec_config_t
*
* \return Returns the status of the operation.
*
******************************************************************************/
cy_en_tcpwm_status_t Cy_TCPWM_MOTIF_Quaddec_Init(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_quaddec_config_t const *config);


/** \} group_tcpwm_functions_motif_quaddec */


/**
* \addtogroup group_tcpwm_functions_motif
* \{
*/


/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_DeInit
***************************************************************************//**
*
* De initializes MOTIF block.
*
* param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_DeInit(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_PCONF(base), TCPWM_MOTIF_GRP_MOTIF_PCONF_ENABLE, 0U);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Suspend
***************************************************************************//**
*
* Configures the entering in suspend for the MOTIF module.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param suspend_config \ref cy_en_motif_suspend_config_t
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Suspend(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_motif_suspend_config_t suspend_config)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_PSUS(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_PSUS_SUS, suspend_config));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Set_LUT_Mode
***************************************************************************//**
*
* Enables/disables LUT mode.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param lut_enable LUT mode enable disable
*
* \note HALL Sensor works in LUT mode enabled and disabled. When LUT mode is enabled user provides look up table values
*   and they will be compared with the actual output values. When motor changes direction, MOTIF is stopped and
*   user need to populate the LUT again.
* \note This function is used to enable disable LUT mode. Sam function for both HALL Sensor mode and Multi channel pattern mode
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Set_LUT_Mode(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool lut_enable)
{
    CY_ASSERT_L1(NULL != base);
    /* Each MOTIF module contains two LUT's(Look Up Tables). Hall LUT (HLUT, also called "input LUT") and Modulation LUT (MLUT, also called "output LUT").
       HLUT contains Hall input pattern which gets loaded to HALP registers.
       MLUT contains output PWM modulation pattern which gets loaded to MCM.MCMP register.
       HLUT is 3x6 bits. MLUT is 32x6 bits
     */
    CY_REG32_CLR_SET(TCPWM_MOTIF_CLUT(base), TCPWM_MOTIF_GRP_MOTIF_CLUT_LUTEN, lut_enable);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Get_LUT_Status
***************************************************************************//**
*
* Returns a value that points to the corresponding input Hall LUT values set
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \return value that points to the corresponding input Hall LUT values set
*
* \note If LUT mode is not enabled this function returns always 0.
*  In Lut mode the Valid values that this function return are 0,1,2,3,4,5. Each value points to the corresponding input Hall LUT values set.
*  Invalid values are 6,7
******************************************************************************/
__STATIC_INLINE uint8_t Cy_TCPWM_MOTIF_Get_LUT_Status(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    return (uint8_t)(_FLD2VAL(TCPWM_MOTIF_GRP_MOTIF_SLUT_PTR, TCPWM_MOTIF_SLUT(base)));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_GetInterruptStatus
***************************************************************************//**
*
* Reports the status of the interrupts
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \return returns the Status of the interrupts
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_TCPWM_MOTIF_GetInterruptStatus(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    return TCPWM_MOTIF_PFLG(base);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_ClearInterrupt
***************************************************************************//**
*
* Clears the triggered interrupts
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mask interrupts to be cleared.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_ClearInterrupt(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mask)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_RPFLG(base) = mask;
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_SetInterrupt
***************************************************************************//**
*
* Software option to set the required interrupts. Interrupt handler will be called.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mask interrupts to be set.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_SetInterrupt(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mask)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_SPFLG(base) = mask;
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_SetInterruptMask
***************************************************************************//**
*
* Sets one or more MOTIF module interrupt factor bits (sets the INTR_MASK register).
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mask interrupt Interrupt bit mask
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_SetInterruptMask(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mask)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_PFLGE(base) = mask;
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_GetInterruptMask
***************************************************************************//**
*
* Reports the interrupt mask set
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \return returns the interrupt mask set
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_TCPWM_MOTIF_GetInterruptMask(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);
    return TCPWM_MOTIF_PFLGE(base);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Enable
***************************************************************************//**
*
* Enables the MOTIF module. Modulation output from motif is started with it's default values
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Enable(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_PCONF(base) |= (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_PCONF_ENABLE, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Disable
***************************************************************************//**
*
* Disables the MOTIF Module. Modulation output from motif is reset to it's default value
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Disable(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_PCONF(base), TCPWM_MOTIF_GRP_MOTIF_PCONF_ENABLE, 0U);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Start
***************************************************************************//**
*
* Modulation output from motif is started with existing values
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Start(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_PRUNS(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_PRUNS_SRB, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Stop
***************************************************************************//**
*
* Modulation output from motif retains the existing values and MOTIF functionality Stops.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Stop(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);
    /* Output events gets reset to default value,
     *Interrupt output get reset to default value and modulation output keep the existing value
     */
    TCPWM_MOTIF_PRUNS(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_PRUNS_SRB, 0U));
}

/** \} group_tcpwm_functions_motif */


/**
* \addtogroup group_tcpwm_functions_motif_hall
* \{
*/

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Configure_Debounce_Bypass
***************************************************************************//**
*
* Enable/Disable HALL Sensor debounce stage bypass
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param bypass_sd Passing true will disable HALL Sensor sample delay. hall inputs are immediately sample after the low pass filter with out delay.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Configure_Debounce_Bypass(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool bypass_sd)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_HMEC(base), TCPWM_MOTIF_GRP_MOTIF_HMEC_HDBP, bypass_sd);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Update_Hall_Pattern
***************************************************************************//**
*
* Update Hall pattern immediately.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \note The update of the fields HALP.HCP, HALP.HEP and HALP.HPP can come from software by calling this function.
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_Hall_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCMS(base) |= (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCMS_STHR, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_HALL_Previous_Pattern_Compare
***************************************************************************//**
*
* Enable/Disable HALL Previous Pattern compare
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param pp_compare Previous Pattern Compare enable or disable
*
* \note Passing true will enables the compare logic between the sampled hall input values and the previous hall pattern.
*   HALP.HPP will hold the previous pattern.
*   This will be used together with the comparison of the current and expected pattern to generate a wrong hall event.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_HALL_Previous_Pattern_Compare(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool pp_compare)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_HMEC(base), TCPWM_MOTIF_GRP_MOTIF_HMEC_HPPE, pp_compare);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Set_HALL_Pattern
***************************************************************************//**
*
* Sets the Hall patterns: Current, Expected and Previous patterns
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param current_pattern Current Pattern
*
* \param expected_pattern Expected Pattern
*
* \param previous_pattern Previous Pattern
*
* \note When the sampling trigger is sensed, the Hall inputs are stored and compared against the Current Pattern, HALP.HCP,
*   the Expected Hall Pattern, HALP.HEP and the Previous Pattern HALP.HPP (the Previous Pattern comparison can be disabled)
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Set_HALL_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint8_t current_pattern, uint8_t expected_pattern, uint8_t previous_pattern)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_HALPS(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HCPS0, current_pattern) |
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HEPS1, expected_pattern)|
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HPPS2, previous_pattern));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Hall_Input_Sample_Trigger
***************************************************************************//**
*
* The application software can issue/request a sample of the Hall Inputs
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Hall_Input_Sample_Trigger(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_HIST(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HIST_HIS, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Update_HLUT
***************************************************************************//**
*
* Update look up table contents for Hall Sensor in LUT mode.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param hlut_config HALL Look Up Table Configuration \ref cy_stc_tcpwm_motif_hlut_t
*
* \note Update look up table contents for Hall Sensor in LUT mode.
*   User writes HLUT and MLUT when MOTIF is stopped.
*   Writing into both LUT while MOTIF is started might lead to incorrect behavior.
*   The LUT is loaded with forward or reverse pattern depending on motor desired direction.
*   The pattern is stored in LUT independently from motor position.
*   The hardware is responsible to find out the current motor position and to load the right hall and modulation pattern.
*   HLUT and MLUT share the same hardware pointer for data extraction, therefore user needs to take care of keeping them aligned with respect to LUT address.
*   A wrong programming might lead to incorrect behavior.
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_HLUT(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_tcpwm_motif_hlut_t *hlut_config)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != hlut_config);

    TCPWM_MOTIF_HALPS(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HCPS0, hlut_config->hlut_data_pos0) |
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HEPS1, hlut_config->hlut_data_pos1) |
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HPPS2, hlut_config->hlut_data_pos2) |
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HLUT3, hlut_config->hlut_data_pos3) |
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HLUT4, hlut_config->hlut_data_pos4) |
                               _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HALPS_HLUT5, hlut_config->hlut_data_pos5));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_HALL_Output_Config
***************************************************************************//**
*
* Configures the output pins for each of the hall event.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param output_config HALL Output Configuration \ref cy_stc_hall_output_config_t
*
* \note User to make sure that no two event s are routed to same node.
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_HALL_Output_Config(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_hall_output_config_t *output_config)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_HOSC(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_EDN,  output_config->edge_detect_event_config)        |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_CHN,  output_config->correct_hall_event_config)       |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_IDN,  output_config->idle_wrong_hall_event_config)    |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_HSTN, output_config->start_event_config)              |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_SPN,  output_config->pattern_mismatch_event_config)   |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_CPN,  output_config->curr_pattern_match_event_config) |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_HOSC_PPN,  output_config->prev_pattern_match_event_config));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_HALL_Get_Output_Modulation_Value
***************************************************************************//**
*
* Returns the output modulation value.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_TCPWM_MOTIF_HALL_Get_Output_Modulation_Value(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    return (uint32_t) (_FLD2VAL(TCPWM_MOTIF_GRP_MOTIF_MCM_MCMP, TCPWM_MOTIF_MCM(base)));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_HALL_Sample_Trigger_EdgeSel
***************************************************************************//**
*
* Select the active edge to trigger a sample of the Hall inputs.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param edge_select Edge to trigger a sample of the Hall inputs \ref cy_en_trigger_edge_selection_t.
*
* \note Edge selector for the sampling trigger
*       This field selects which edge is used of the selected MOTIFx.HSD signal to trigger a sample of the Hall inputs.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_HALL_Sample_Trigger_EdgeSel(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_trigger_edge_selection_t edge_select)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_PCONF(base), TCPWM_MOTIF_GRP_MOTIF_PCONF_SPES, edge_select);
}

/** \} group_tcpwm_functions_motif_hall */

/**
* \addtogroup group_tcpwm_functions_motif_mcp
* \{
*/

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Set_MCP_Value
***************************************************************************//**
*
* Sets Multi channel pattern value.
* Every time a multi channel pattern transfer is triggered, this value is transferred.
* This is applicable when multi channel look up table mode is disabled.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mcp_value Multi Channel Pattern Value
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Set_MCP_Value (TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mcp_value)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCSM(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCSM_MCMPS, mcp_value));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_MCP_Value_When_Fault_Hall
***************************************************************************//**
*
* Sets MCP value when there is a fault HALL event.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mcp_value Multi channel pattern value to be configured when there is a fault HALL event.
*
* \note It is possible also to use the Hall Fault Event to clear/load the Multi-Channel Pattern with a pre-configured value - MCPF.MCFV.
*  This mechanism can be used whenever a wrong operation is detected and a specific safe pattern needs to be applied to the PWM timers that are controlling the output switches.
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_Value_When_Fault_Hall(TCPWM_MOTIF_GRP_MOTIF_Type *base, uint32_t mcp_value)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCPF(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCPF_MCFV, mcp_value));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_MCP_Bypass_Delay
***************************************************************************//**
*
* Enable/Disable multi channel pattern delay bypass
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param bypass_delay Bypass Delay.
*
* \note After the Correct Hall Event is detected, a delay can be generated between this detection and the update of the Multi-Channel pattern.
*   The delay for the update of the Multi-Channel pattern can be controlled directly by a TCPWM counter.
*   The trigger that indicates that the delay is finished, is mapped to input MOTIFx.MSET (input for MCP trigger)
*   This delay can be by passed by passing true to this function
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_Bypass_Delay(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool bypass_delay)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_MCMEC(base), TCPWM_MOTIF_GRP_MOTIF_MCMEC_MEBP, bypass_delay);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern
***************************************************************************//**
*
* Update multi channel pattern.
* The update is not done immediately due to the fact that the trigger that synchronizes the update with the PWM is still needed
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \note Calling this function enables the Multi-Channel pattern update.
*  The update is not done immediately due to the fact that the trigger that synchronizes the update with the PWM is still needed.
 * The input MOTIFx.MSYNC is used for synchronization.
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCMS(base) |= (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCMS_MNPS, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Clear_Multi_Channel_Pattern
***************************************************************************//**
*
* Clears the Multi-Channel Pattern value with the content of the MCP Fault register
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Clear_Multi_Channel_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCMC(base) |= (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCMC_MPC, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Clear_Updation_Of_Multi_Channel_Pattern
***************************************************************************//**
*
* Clears updating of multi channel pattern. Sets that Multi channel pattern is not ready to be updated.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Clear_Updation_Of_Multi_Channel_Pattern(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCMC(base) |= (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCMC_MNPC, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern_Immediately
***************************************************************************//**
*
* Update multi channel pattern immediately.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \note Calling this function enables the Multi-Channel pattern update immediately.
*   The update is done immediately with out waiting for the trigger that synchronizes the update with the PWM.
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern_Immediately(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    TCPWM_MOTIF_MCMS(base) |= (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MCMS_STMR, 1U));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Get_Multi_Channel_Pattern_Update_Status
***************************************************************************//**
*
* returns the Multi Channel Pattern update status.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \return indicates if the Multi-Channel pattern is ready to be updated or not. 1: MCP is ready and 0: MCP is not ready.
*
******************************************************************************/
__STATIC_INLINE bool Cy_TCPWM_MOTIF_Get_Multi_Channel_Pattern_Update_Status(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    return (bool) (_FLD2VAL(TCPWM_MOTIF_GRP_MOTIF_MCMF_MSS, TCPWM_MOTIF_MCMF(base)));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_MCP_SW_Update_Enable
***************************************************************************//**
*
* Selects the source that enables an update of the Multi-Channel pattern.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param sw_update_enable When set to true, the Multi-Channel pattern can only be updated after the SW has called the function Cy_TCPWM_MOTIF_Update_Multi_Channel_Pattern.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_SW_Update_Enable(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool sw_update_enable)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_PCONF(base), TCPWM_MOTIF_GRP_MOTIF_PCONF_MCUE, sw_update_enable);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Multi_Channel_Output_Config
***************************************************************************//**
*
* Configures the output nodes for each of the Multi Channel Pattern output event
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param output_config Output configuration. \ref cy_stc_multi_channel_output_config_t
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Multi_Channel_Output_Config(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_multi_channel_output_config_t *output_config)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != output_config);

    TCPWM_MOTIF_MOSC(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MOSC_PUN,  output_config->pattern_update_event_config)  |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MOSC_SHN,  output_config->update_sync_event_config)     |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_MOSC_MSTN, output_config->start_event_config));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_MCP_Bypass_Synchronization
***************************************************************************//**
*
* Synchronization of the trigger with the PWM is bypassed if set
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param bypass_sync Synchronization bypass configuration.
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_MCP_Bypass_Synchronization(TCPWM_MOTIF_GRP_MOTIF_Type *base, bool bypass_sync)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_MCMEC(base), TCPWM_MOTIF_GRP_MOTIF_MCMEC_MSBP, bypass_sync);
}

/** \} group_tcpwm_functions_motif_mcp */

/**
* \addtogroup group_tcpwm_functions_motif_quaddec
* \{
*/

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Configure_Postion_Decoder_Mode
***************************************************************************//**
*
* Configures Position Decoder to either Quad mode or direction count mode based on the selection.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param mode Mode to be set for the position decoder. \ref cy_en_position_decoder_mode_t
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Configure_Postion_Decoder_Mode(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_en_position_decoder_mode_t mode)
{
    CY_ASSERT_L1(NULL != base);

    CY_REG32_CLR_SET(TCPWM_MOTIF_PCONF(base), TCPWM_MOTIF_GRP_MOTIF_PCONF_QDCM, mode);
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Get_Quaddec_Rotation_Direction
***************************************************************************//**
*
* returns the quaddec rotation direction.
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \return returns the quaddec rotation direction. 0: Counter clockwise rotation. 1: Clockwise Rotation
*
******************************************************************************/
__STATIC_INLINE bool Cy_TCPWM_MOTIF_Get_Quaddec_Rotation_Direction(TCPWM_MOTIF_GRP_MOTIF_Type *base)
{
    CY_ASSERT_L1(NULL != base);

    return (bool)(_FLD2VAL(TCPWM_MOTIF_GRP_MOTIF_QDC_DVAL, TCPWM_MOTIF_QDC(base)));
}

/******************************************************************************
* Function Name: Cy_TCPWM_MOTIF_Quaddec_Output_Config
***************************************************************************//**
*
* Configures the output nodes for each of the quaddec output event
*
* \param base The pointer to the TCPWM_MOTIF instance address.
*
* \param output_config Output configuration. \ref cy_stc_quaddec_output_config_t
*
******************************************************************************/
__STATIC_INLINE void Cy_TCPWM_MOTIF_Quaddec_Output_Config(TCPWM_MOTIF_GRP_MOTIF_Type *base, cy_stc_quaddec_output_config_t *output_config)
{
    CY_ASSERT_L1(NULL != base);
    CY_ASSERT_L1(NULL != output_config);

    TCPWM_MOTIF_QOSC(base) = (_VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_QOSC_QCN,  output_config->clock_event_config)         |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_QOSC_DON,  output_config->direction_event_config)     |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_QOSC_PCN,  output_config->period_clock_event_config)  |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_QOSC_CCN,  output_config->clear_capture_event_config) |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_QOSC_IXN,  output_config->index_event_config)         |
                              _VAL2FLD(TCPWM_MOTIF_GRP_MOTIF_QOSC_QSTN, output_config->start_event_config));

}

/** \} group_tcpwm_functions_motif_quaddec */


#if defined(__cplusplus)
}
#endif
#endif /*defined (CY_IP_MXS40TCPWM) || defined (CY_DOXYGEN) */
#endif /* CY_IP_MXTCPWM */

#endif /* CY_TCPWM_MOTIF_H */

