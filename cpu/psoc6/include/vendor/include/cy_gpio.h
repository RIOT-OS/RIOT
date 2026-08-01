/***************************************************************************//**
* \file cy_gpio.h
* \version 1.140
*
* Provides an API declaration of the GPIO driver
*
********************************************************************************
* \copyright
* Copyright (c) (2016-2024), Cypress Semiconductor Corporation (an Infineon company) or
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
* \addtogroup group_gpio
* \{
* The GPIO driver provides an API to configure and access device Input/Output pins.
*
* The functions and other declarations used in this driver are in cy_gpio.h.
* You can include cy_pdl.h to get access to all functions
* and declarations in the PDL.
*
* IO pins include all general purpose types such as GPIO, SIO, HSIO, AUXIO, and
* their variants.
*
* Initialization can be performed either at the port level or by configuring the
* individual pins. For efficient use of code space, port
* configuration should be used in the field. Refer to the product device header files
* for the list of supported ports and pins.
*
* A port is represented by GPIO_PRT_Type and a pin is represented by a number
* 0 to 7.
*
* For PSoC 64 devices the the un-intended protected pins (due to constrain on PPU configuration)
* are modified using PRA driver. But the GPIO diver does not modify the intended protected pins .
*
* - Single pin configuration is performed by using \ref Cy_GPIO_Pin_FastInit
*   (provide specific values) or \ref Cy_GPIO_Pin_Init (provide a filled
*   cy_stc_gpio_pin_config_t structure).
* - An entire port can be configured using \ref Cy_GPIO_Port_Init. Provide a filled
*   cy_stc_gpio_prt_config_t structure. The values in the structure are
*   bitfields representing the desired value for each pin in the port.
* - Pin configuration and management is based on the port address and pin number.
*   \ref Cy_GPIO_PortToAddr function can optionally be used to calculate the port
*   address from the port number at run-time.
* - Each I/O is individually configurable to one of eight drive modes represented
*   by drivemode of cy_stc_gpio_pin_config_t structure.
*
* Once the pin/port initialization is complete, each pin can be accessed by
* specifying the port (GPIO_PRT_Type) and the pin (0-7) in the provided API
* functions.
*
* \section group_gpio_configuration Configuration Considerations
*
* 1. Pin multiplexing is controlled through the High-Speed IO Matrix (HSIOM) selection.
*    This allows the pin to connect to signal sources/sinks throughout the device,
*    as defined by the pin HSIOM selection options (en_hsiom_sel_t).
* 2. All pins are initialized to High-Z drive mode with HSIOM connected to CPU (SW
*    control digital pin only) at Power-On-Reset(POR).
* 3. Some API functions perform read-modify-write operations on shared port
*    registers. These functions are not thread safe and care must be taken when
*    called by the application.
* 4. Digital input buffer provides a high-impedance buffer for the external
*    digital input. The input buffer is connected to the HSIOM for routing to
*    the CPU port registers and selected peripheral. Enabling the input
*    buffer provides possibility to read the pin state via the CPU.
*    If pin is connected to an analog signal, the input buffer should be
*    disabled to avoid crowbar currents. For more information refer to device
*    TRM and the device datasheet.
*
* Multiple pins on a port can be updated using direct port register writes with an
* appropriate port mask. An example is shown below, highlighting the different ways of
* configuring Port 1 pins using:
*
* - Initialize a Pin using cy_stc_gpio_pin_config_t structure
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Pin_Init
*
* - Initialize entire port using cy_stc_gpio_prt_config_t structure
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Port_Init
*
* - Port output data register
* - Port output data set register
* - Port output data clear register
*
* \snippet gpio/snippet/main.c Cy_GPIO_Snippet
*
* \section group_gpio_more_information More Information
*
* Refer to the technical reference manual (TRM) and the device datasheet.
*
* \section group_gpio_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.140</td>
*     <td>Updated APIs \ref Cy_GPIO_Port_Init, \ref Cy_GPIO_WritePort.</td>
*     <td>Defect fix.</td>
*   </tr>
*   <tr>
*     <td>1.130</td>
*     <td>Added new APIs to support new devices (particularly Traveo II Cluster) with Drive Trim features: \ref Cy_GPIO_SetDriveSelTrim, \ref Cy_GPIO_GetDriveSelTrim.</td>
*     <td>New functionality.</td>
*   </tr>
*   <tr>
*     <td rowspan="2">1.120</td>
*     <td>Added new APIs: \ref Cy_GPIO_WritePort.</td>
*     <td>New functionality.</td>
*   </tr>
*   <tr>
*     <td>Reworked API structure to support new devices.</td>
*     <td>Code enhancement.</td>
*   </tr>
*   <tr>
*     <td>1.110</td>
*     <td>Added support for SMIF GPIO PORT1 and PORT2 and reverted to GPIO inline functions.</td>
*     <td>Code enhancement and support for new GPIO port for SMIF.</td>
*   </tr>
*   <tr>
*     <td>1.100</td>
*     <td>Added support for TRAVEO&trade; II Body Entry devices.<br>
*       Updated pre-processor checks to check for GPIO auto-leveling capability rather than rely on IOSS version.</td>
*     <td>Code enhancement and support for new devices.</td>
*   </tr>
*   <tr>
*     <td>1.90</td>
*     <td>Updated APIs \ref Cy_GPIO_Port_Init, \ref Cy_GPIO_Port_Deinit, \ref Cy_GPIO_GetDrivemode.</td>
*     <td>Fixed coverity defects.</td>
*   </tr>
*   <tr>
*     <td rowspan="2">1.80</td>
*     <td>Updated \ref Cy_GPIO_SetDrivemode and \ref Cy_GPIO_GetDrivemode APIs.</td>
*     <td>Updated drive mode configuration for CAT1D devices.</td>
*   </tr>
*   <tr>
*     <td>Fixed MISRA 2012 violations.</td>
*     <td>MISRA 2012 compliance.</td>
*   </tr>
*   <tr>
*     <td>1.70</td>
*     <td>Updated driver to support the CAT1D family of devices.</td>
*     <td>Added new family of devices.</td>
*   </tr>
*   <tr>
*     <td rowspan="2">1.60</td>
*     <td>Added \ref Cy_GPIO_SetVtripAuto and \ref Cy_GPIO_GetVtripAuto APIs for
*         configuring    GPIO input buffer voltage for automotive compatible or not,
*         for CAT1C devices.</td>
*     <td>New   APIs support for CAT1C devices.</td>
*   </tr>
*   <tr>
*     <td>Fixed unused variables error.</td>
*     <td>Minor error fixes.</td>
*   </tr>
*   <tr>
*     <td>1.50.1</td>
*     <td>Updated doxygen for External clock source to HF0.</td>
*     <td>Documentation enhancement.</td>
*   </tr>
*   <tr>
*     <td>1.50</td>
*     <td>Modified \ref Cy_GPIO_Pin_Init, \ref Cy_GPIO_Pin_FastInit, and
*         \ref Cy_GPIO_SetDrivemode APIs to catch wrong drive modes.</td>
*     <td>Defect fix.</td>
*   </tr>
*   <tr>
*     <td rowspan="2">1.40</td>
*     <td>Changes in Support of the new family of devices</td>
*     <td>Added new family of devices</td>
*   </tr>
*   <tr>
*     <td>Changes in support of Secure pins used for External clocks on Secure devices</td>
*     <td>Added support for accessing External clocks protected pins</td>
*   </tr>
*   <tr>
*     <td>1.30</td>
*     <td>Fixed/documented MISRA 2012 violations.</td>
*     <td>MISRA 2012 compliance.</td>
*   </tr>
*   <tr>
*     <td>1.20.1</td>
*     <td>Minor documentation updates.</td>
*     <td>Documentation enhancement.</td>
*   </tr>
*   <tr>
*     <td rowspan="3">1.20</td>
*     <td>Flattened the organization of the driver source code into the single source directory and the single include directory.</td>
*     <td>Driver library directory-structure simplification.</td>
*   </tr>
*   <tr>
*     <td>Added the functions for configuring the AMux bus splitter switch cells:
*     - \ref Cy_GPIO_SetAmuxSplit
*     - \ref Cy_GPIO_GetAmuxSplit
*     </td>
*     <td>Added a new functionality related to AMux bus.</td>
*   </tr>
*   <tr>
*     <td>Added register access layer. Use register access macros instead
*         of direct register access using dereferenced pointers.</td>
*     <td>Makes register access device-independent, so that the PDL does
*         not need to be recompiled for each supported part number.</td>
*   </tr>
*   <tr>
*     <td>1.10.1</td>
*     <td>Updated description for the functions: \ref Cy_GPIO_GetInterruptStatus,
*         \ref Cy_GPIO_GetInterruptMask, \ref Cy_GPIO_GetInterruptStatusMasked.
*
*         Minor documentation edits.
*     </td>
*     <td>Documentation update and clarification</td>
*   </tr>
*   <tr>
*     <td>1.10</td>
*     <td>Added input parameter validation to the API functions</td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_gpio_macros Macros
* \defgroup group_gpio_functions Functions
* \{
*   \defgroup group_gpio_functions_init       Initialization Functions
*   \defgroup group_gpio_functions_gpio       GPIO Functions
*   \defgroup group_gpio_functions_sio        SIO Functions
*   \defgroup group_gpio_functions_interrupt  Port Interrupt Functions
* \}
* \defgroup group_gpio_data_structures Data Structures
* \defgroup group_gpio_enums Enumerated Types
*/

#if !defined(CY_GPIO_H)
#define CY_GPIO_H

#include "cy_device.h"

#if defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS40IOSS) || defined (CY_IP_MXS22IOSS)

#include <stddef.h>
#include "cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** \addtogroup group_gpio_macros
* \{
*/

/** Driver major version */
#define CY_GPIO_DRV_VERSION_MAJOR       1

/** Driver minor version */
#define CY_GPIO_DRV_VERSION_MINOR      140

/** GPIO driver ID */
#define CY_GPIO_ID CY_PDL_DRV_ID(0x16U)

/** \} group_gpio_macros */


/***************************************
*       Enumerations
***************************************/
/**
* \addtogroup group_gpio_enums
* \{
*/

/** GPIO Driver error codes */
typedef enum
{
    CY_GPIO_SUCCESS   = 0x00U,                                    /**< Returned successful */
    CY_GPIO_BAD_PARAM = CY_GPIO_ID | CY_PDL_STATUS_ERROR | 0x01U, /**< Bad parameter was passed */
} cy_en_gpio_status_t;

/** AMux switch open/close config */
typedef enum
{
    CY_GPIO_AMUX_OPENALL,   /**< Open ground switch. Open right switch. Open left switch */
    CY_GPIO_AMUX_L,         /**< Open ground switch. Open right switch. Close left switch */
    CY_GPIO_AMUX_R,         /**< Open ground switch. Close right switch. Open left switch */
    CY_GPIO_AMUX_LR,        /**< Open ground switch. Close right switch. Close left switch */
    CY_GPIO_AMUX_G,         /**< Close ground switch. Open right switch. Open left switch */
    CY_GPIO_AMUX_GL,        /**< Close ground switch. Open right switch. Close left switch */
    CY_GPIO_AMUX_GR,        /**< Close ground switch. Close right switch. Open left switch */
    CY_GPIO_AMUX_GLR,       /**< Close ground switch. Close right switch. Close left switch */
}cy_en_gpio_amuxconnect_t;

/**
* AMux Bus selection
*/
typedef enum
{
    CY_GPIO_AMUXBUSA,       /**< AMuxBus A */
    CY_GPIO_AMUXBUSB        /**< AMuxBus B */
}cy_en_gpio_amuxselect_t;

/** \} group_gpio_enums */


/***************************************
*       Configuration Structures
***************************************/

/**
* \addtogroup group_gpio_data_structures
* \{
*/

/** This structure is used to initialize a port of GPIO pins */
typedef struct
{
    uint32_t out;            /**< Initial output data for the IO pins in the port */
    uint32_t intrMask;       /**< Interrupt enable mask for the port interrupt */
    uint32_t intrCfg;        /**< Port pin interrupt edge detection configuration */
    uint32_t cfg;            /**< Port pin drive modes and input buffer enable configuration */
    uint32_t cfgIn;          /**< Port pin input buffer configuration */
    uint32_t cfgOut;         /**< Port pin output buffer configuration */
    uint32_t cfgSIO;         /**< Port SIO pins configuration */
    uint32_t sel0Active;     /**< HSIOM selection for port pins 0,1,2,3 */
    uint32_t sel1Active;     /**< HSIOM selection for port pins 4,5,6,7 */
#if defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS22IOSS)
    /**
    * \note
    * This parameter is available for the CAT1B and CAT1D devices.
    **/
    uint32_t cfgSlew;        /**< Port slew rate configuration */
    /**
    * \note
    * This parameter is available for the CAT1B and CAT1D devices.
    **/
    uint32_t cfgDriveSel0;   /**< Drive strength configuration for pins 0,1,2,3 */
    /**
    * \note
    * This parameter is available for the CAT1B and CAT1D devices.
    **/
    uint32_t cfgDriveSel1;   /**< Drive strength configuration for pins 4,5,6,7 */
    /**
    * \note
    * This parameter is available for the CAT1B and CAT1D devices.
    **/
    uint32_t nonSecMask;     /**< HSIOM non secure mask for port pins 0-7 */
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */
#if defined (CY_IP_MXS22IOSS)
    /**
    * \note
    * This parameter is available for the CAT1D devices.
    **/
    uint32_t cfgRes;         /**< Pull-up resistor configuration for port pins */
    /**
    * \note
    * This parameter is available for the CAT1D devices.
    **/
    uint32_t cfgOut3;        /**< Port pin extra drive mode \ref CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ */
#endif /* CY_IP_MXS22IOSS */
} cy_stc_gpio_prt_config_t;

/** This structure is used to initialize a single GPIO pin */
typedef struct
{
    uint32_t outVal;         /**< Pin output state */
    uint32_t driveMode;      /**< Drive mode */
    en_hsiom_sel_t hsiom;    /**< HSIOM selection */
    uint32_t intEdge;        /**< Interrupt Edge type */
    uint32_t intMask;        /**< Interrupt enable mask */
    uint32_t vtrip;          /**< Input buffer voltage trip type */
    uint32_t slewRate;       /**< Output buffer slew rate */
    uint32_t driveSel;       /**< Drive strength */
    uint32_t vregEn;         /**< SIO pair output buffer mode */
    uint32_t ibufMode;       /**< SIO pair input buffer mode */
    uint32_t vtripSel;       /**< SIO pair input buffer trip point */
    uint32_t vrefSel;        /**< SIO pair reference voltage for input buffer trip point */
    uint32_t vohSel;         /**< SIO pair regulated voltage output level */
#if defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS22IOSS)
    /**
    * \note
    * This parameter is available for the CAT1B and CAT1D devices.
    **/
    uint32_t nonSec;         /**< Secure attribute for each Pin of a port */
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */
#if defined (CY_IP_MXS22IOSS)
    /**
    * \note
    * This parameter is available for the CAT1D devices.
    **/
    uint32_t pullUpRes; /**< Pull-up resistor configuration for each pin of a port */
#endif /* CY_IP_MXS22IOSS */
} cy_stc_gpio_pin_config_t;

/** \} group_gpio_data_structures */

/***************************************
*        Constants
***************************************/

/** \cond INTERNAL */

/* General Constants */
#define CY_GPIO_PRT_HALF                       (4UL)      /**< Half-way point of a GPIO port */
#define CY_GPIO_PRT_DEINIT                     (0UL)      /**< De-init value for port registers */
#if (defined (CY_IP_MXS40SIOSS) && ((IOSS_HSIOM_HSIOM_SEC_PORT_NR != 0) || (CPUSS_CM33_0_SECEXT_PRESENT != 0))) || defined (CY_IP_MXS22IOSS)
#define CY_HSIOM_NONSEC_DEINIT                 (0xFFUL)   /**< De-init value for port non sec register */
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */

/* GPIO Masks */
#define CY_GPIO_HSIOM_SECURE_ACCESS            (0UL)      /**< HSIOM Secure access value */
#define CY_GPIO_HSIOM_MASK                     (0x1FUL)   /**< HSIOM selection mask */
#define CY_GPIO_OUT_MASK                       (0x01UL)   /**< Single pin mask for OUT register */
#define CY_GPIO_IN_MASK                        (0x01UL)   /**< Single pin mask for IN register */
#define CY_GPIO_CFG_DM_MASK                    (0x0FUL)   /**< Single pin mask for drive mode in CFG register */
#define CY_GPIO_CFG_DM_WIDTH_MASK              (0xFUL)    /**< Single pin mask for drive mode width in CFG/CFG_OUT3 register */
#define CY_GPIO_CFG_DM_NO_INBUF_MASK           (0x07UL)   /**< Single pin mask for drive mode ( without input buffer ) in CFG register */
#define CY_GPIO_CFG_IN_VTRIP_SEL_0_MASK        (0x01UL)   /**< Single pin mask for VTRIP selection in CFG IN register */
#if (defined (CY_IP_MXS40IOSS) && ( \
        (CY_IP_MXS40IOSS_VERSION == 3U) || \
        (CY_IP_MXS40IOSS_VERSION == 2U && (defined (SRSS_HT_VARIANT) && (SRSS_HT_VARIANT == 1U)))) || \
    defined (CY_IP_MXS40SIOSS_VERSION))

    #define GPIO_AUTOLVL_AVAIL                 (1U)       /**< Specifies that the AUTOLVL registers are available on this device>**/
    #define CY_GPIO_CFG_IN_VTRIP_SEL_1_MASK    (0x01UL)   /**< Single pin mask for VTRIP selection in CFG IN AUTOLVL register */
#endif /* CY_IP_MXS40IOSS_VERSION */
#define CY_GPIO_CFG_IN_VTRIP_SEL_MASK          CY_GPIO_CFG_IN_VTRIP_SEL_0_MASK   /**< Single pin mask for VTRIP selection in CFG IN register */
#define CY_GPIO_INTR_STATUS_MASK               (0x01UL)   /**< Single pin mask for interrupt status in INTR register */
#define CY_GPIO_INTR_EN_MASK                   (0x01UL)   /**< Single pin mask for interrupt status in INTR register */
#define CY_GPIO_INTR_MASKED_MASK               (0x01UL)   /**< Single pin mask for masked interrupt status in INTR_MASKED register */
#define CY_GPIO_INTR_SET_MASK                  (0x01UL)   /**< Single pin mask for setting the interrupt in INTR_MASK register */
#define CY_GPIO_INTR_EDGE_MASK                 (0x03UL)   /**< Single pin mask for interrupt edge type in INTR_EDGE register */
#define CY_GPIO_INTR_FLT_EDGE_MASK             (0x07UL)   /**< Single pin mask for setting filtered interrupt */
#if defined (CY_IP_MXS40IOSS)
#define CY_GPIO_CFG_OUT_SLOW_MASK              (0x01UL)   /**< Single pin mask for slew rate in CFG OUT register */
#define CY_GPIO_CFG_OUT_DRIVE_SEL_MASK         (0x03UL)   /**< Single pin mask for drive strength in CFG OUT register */
#else
#define CY_GPIO_HSIOM_SEC_MASK                 (0x01UL)   /**< Single pin mask for NONSECURE_MASK register */
#define CY_GPIO_CFG_SLEW_EXT_MASK              (0x07UL)   /**< Single pin mask for slew rate in CFG SLEW EXT register */
#define CY_GPIO_CFG_DRIVE_SEL_EXT_MASK         (0x1FUL)   /**< Single pin mask for drive strength in CFG DRIVE EXT register */
#endif /* CY_IP_MXS40IOSS */
#if defined (CY_IP_MXS22IOSS)
#define CY_GPIO_CFG_RES_PULLUP_MODE_MASK       (0x0FUL)   /**< Single pin mask for Pull-up mode in CFG_RES register */
#endif /* CY_IP_MXS22IOSS */

/* SIO Masks */
#define CY_GPIO_VREG_EN_MASK                   (0x01UL)   /**< Single SIO pin mask for voltage regulation enable */
#define CY_GPIO_IBUF_MASK                      (0x01UL)   /**< Single SIO pin mask for input buffer */
#define CY_GPIO_IBUF_SHIFT                     (0x01UL)   /**< Single SIO pin shift for input buffer */
#define CY_GPIO_VTRIP_SEL_MASK                 (0x01UL)   /**< Single SIO pin mask for the input buffer trip point */
#define CY_GPIO_VTRIP_SEL_SHIFT                (0x02UL)   /**< Single SIO pin shift for the input buffer trip point */
#define CY_GPIO_VREF_SEL_MASK                  (0x03UL)   /**< Single SIO pin mask for voltage reference */
#define CY_GPIO_VREF_SEL_SHIFT                 (0x03UL)   /**< Single SIO pin shift for voltage reference */
#define CY_GPIO_VOH_SEL_MASK                   (0x07UL)   /**< Single SIO pin mask for VOH */
#define CY_GPIO_VOH_SEL_SHIFT                  (0x05UL)   /**< Single SIO pin shift for VOH */

/* Special mask for SIO pin pair setting */
#define CY_GPIO_SIO_ODD_PIN_MASK               (0x00FEUL) /**< SIO pin pair selection mask */
#define CY_GPIO_SIO_PIN_MASK                   (0x00FFUL) /**< SIO pin pair mask */

/* Offsets */
#define CY_GPIO_HSIOM_OFFSET                   (3UL)      /**< Offset for HSIOM */
#define CY_GPIO_DRIVE_MODE_OFFSET              (2UL)      /**< Offset for Drive mode */
#define CY_GPIO_INBUF_OFFSET                   (3UL)      /**< Offset for input buffer */
#define CY_GPIO_CFG_OUT_DRIVE_OFFSET           (16UL)     /**< Offset for drive strength */
#define CY_GPIO_INTR_CFG_OFFSET                (1UL)      /**< Offset for interrupt config */
#define CY_GPIO_INTR_FILT_OFFSET               (18UL)     /**< Offset for filtered interrupt config */
#define CY_GPIO_CFG_SIO_OFFSET                 (2UL)      /**< Offset for SIO config */
#if defined (CY_IP_MXS40IOSS_VERSION) && (CY_IP_MXS40IOSS_VERSION >= 4)
#define CY_GPIO_DRIVE_TRIM_OFFSET              (3UL)      /**< Offset for drive strength trim value */
#endif
#if defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS22IOSS)
#define CY_GPIO_CFG_SLEW_EXT_OFFSET            (2UL)      /**< Offset for CFG SLEW EXT */
#define CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET       (3UL)      /**< Offset for CFG SLEW EXT */
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */
#if defined (CY_IP_MXS22IOSS)
#define CY_GPIO_CFG_RES_PULLUP_MODE_OFFSET     (2UL)      /**< Offset for Pull-up mode */
#endif /* CY_IP_MXS22IOSS */

/* Parameter validation constants */
#define CY_GPIO_PINS_MAX                       (8UL)      /**< Number of pins in the port */
#define CY_GPIO_PRT_PINS_MASK                  (0x0000000FFUL)
#define CY_GPIO_PRT_INTR_CFG_EDGE_SEL_MASK     (GPIO_PRT_INTR_CFG_EDGE0_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE1_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE2_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE3_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE4_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE5_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE6_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_EDGE7_SEL_Msk)
#define CY_GPIO_PRT_INTR_CFG_RANGE_MASK        (CY_GPIO_PRT_INTR_CFG_EDGE_SEL_MASK | \
                                                GPIO_PRT_INTR_CFG_FLT_EDGE_SEL_Msk | \
                                                GPIO_PRT_INTR_CFG_FLT_SEL_Msk)
#define CY_GPIO_PRT_INT_MASK_MASK              (0x0000001FFUL)
#define CY_GPIO_PRT_SEL_ACTIVE_MASK            (0x1FFFFFFFUL)
#if defined (CY_IP_MXS40IOSS_VERSION) && (CY_IP_MXS40IOSS_VERSION >= 4)
#define CY_GPIO_CFG_OUT2_DRIVE_SEL_TRIM_MASK   (0x07UL)   /**< Single pin mask for drive select trim in CFG OUT2 register */
#endif
#if defined (CY_IP_MXS22IOSS)
#define CY_GPIO_PRT_CFG_RES_MASK               (0x77777777UL)
#endif /* CY_IP_MXS22IOSS */

#define GPIO_MAX_SPLIT_CELL_SEGMENTS           (9U)
#if defined (CY_IP_MXS22IOSS)
/* CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ is 0x80UL, but CFG_OUT3 register uses 0x8UL. */
/* To correctly configure drive mode value in CFG_OUT3 register, CY_GPIO_EXT_DM_SHIFT is used to shift CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ. */
#define CY_GPIO_EXT_DM_SHIFT                   (0x4UL)
#endif /* CY_IP_MXS22IOSS */

/* Parameter validation macros */
#define CY_GPIO_IS_PIN_VALID(pinNum)           (CY_GPIO_PINS_MAX > (pinNum))
#define CY_GPIO_IS_FILTER_PIN_VALID(pinNum)    (CY_GPIO_PINS_MAX >= (pinNum))
#define CY_GPIO_IS_VALUE_VALID(outVal)         (1UL >= (outVal))
#define CY_GPIO_IS_PORT_VALUE_VALID(outVal)    (0U == ((outVal) & (uint32_t)~CY_GPIO_PRT_PINS_MASK))

#if defined(CY_IP_MXS22IOSS)
#define CY_GPIO_IS_DM_VALID(driveMode)         (((0U == ((driveMode) & (uint32_t)~CY_GPIO_CFG_DM_MASK)) ||\
                                               ((driveMode) == CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ)) && \
                                               ((driveMode) != CY_GPIO_DM_INVALID_IN_OFF) && \
                                               ((driveMode) != CY_GPIO_DM_INVALID))
#else
#define CY_GPIO_IS_DM_VALID(driveMode)         ((0U == ((driveMode) & (uint32_t)~CY_GPIO_CFG_DM_MASK)) && \
                                               ((driveMode) != CY_GPIO_DM_INVALID_IN_OFF) && \
                                               ((driveMode) != CY_GPIO_DM_INVALID))
#endif /* CY_IP_MXS22IOSS */

#define CY_GPIO_IS_HSIOM_VALID(hsiom)          (0U == ((hsiom) & (uint32_t)~CY_GPIO_HSIOM_MASK))

#define CY_GPIO_IS_INT_EDGE_VALID(intEdge)     ((CY_GPIO_INTR_DISABLE  == (intEdge)) || \
                                                (CY_GPIO_INTR_RISING   == (intEdge)) || \
                                                (CY_GPIO_INTR_FALLING  == (intEdge)) || \
                                                (CY_GPIO_INTR_BOTH     == (intEdge)))

#define CY_GPIO_IS_VREF_SEL_VALID(vrefSel)     ((CY_SIO_VREF_PINREF    == (vrefSel)) || \
                                                (CY_SIO_VREF_1_2V      == (vrefSel)) || \
                                                (CY_SIO_VREF_AMUX_A    == (vrefSel)) || \
                                                (CY_SIO_VREF_AMUX_B    == (vrefSel)))

#define CY_GPIO_IS_VOH_SEL_VALID(vrefSel)      ((CY_SIO_VOH_1_00       == (vrefSel)) || \
                                                (CY_SIO_VOH_1_25       == (vrefSel)) || \
                                                (CY_SIO_VOH_1_49       == (vrefSel)) || \
                                                (CY_SIO_VOH_1_67       == (vrefSel)) || \
                                                (CY_SIO_VOH_2_08       == (vrefSel)) || \
                                                (CY_SIO_VOH_2_50       == (vrefSel)) || \
                                                (CY_SIO_VOH_2_78       == (vrefSel)) || \
                                                (CY_SIO_VOH_4_16       == (vrefSel)))

#if defined (CY_IP_MXS22IOSS)
#define CY_GPIO_IS_PULLUP_RES_VALID(PullUp)    ((CY_GPIO_PULLUP_RES_DISABLE   == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_570       == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_720       == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_840       == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_1100      == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_1200      == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_1800      == (PullUp)) || \
                                                (CY_GPIO_PULLUP_RES_2800      == (PullUp)))

#define CY_GPIO_PRT_IS_PULLUP_RES_VALID(PullUp)    (0U == ((PullUp) & (uint32_t)~CY_GPIO_PRT_CFG_RES_MASK))
#endif /* CY_IP_MXS22IOSS */

#define CY_GPIO_IS_PIN_BIT_VALID(pinBit)       (0U == ((pinBit) & (uint32_t)~CY_GPIO_PRT_PINS_MASK))
#define CY_GPIO_IS_INTR_CFG_VALID(intrCfg)     (0U == ((intrCfg) & (uint32_t)~CY_GPIO_PRT_INTR_CFG_RANGE_MASK))
#define CY_GPIO_IS_INTR_MASK_VALID(intrMask)   (0U == ((intrMask) & (uint32_t)~CY_GPIO_PRT_INT_MASK_MASK))
#define CY_GPIO_IS_SEL_ACT_VALID(selActive)    (0U == ((selActive) & (uint32_t)~CY_GPIO_PRT_SEL_ACTIVE_MASK))

#define CY_GPIO_IS_AMUX_SPLIT_VALID(switchCtrl)       (((uint32_t) (switchCtrl)) < GPIO_MAX_SPLIT_CELL_SEGMENTS)

#define CY_GPIO_IS_AMUX_CONNECT_VALID(amuxConnect)    ((CY_GPIO_AMUX_OPENALL == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_L       == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_R       == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_LR      == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_G       == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_GL      == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_GR      == (amuxConnect)) || \
                                                       (CY_GPIO_AMUX_GLR     == (amuxConnect)))

#define CY_GPIO_IS_AMUX_SELECT_VALID(amuxBus)       ((CY_GPIO_AMUXBUSA       == (amuxBus)) || \
                                                     (CY_GPIO_AMUXBUSB       == (amuxBus)))

#if defined (CY_IP_MXS40IOSS)
#define CY_GPIO_IS_DRIVE_SEL_VALID(driveSel)   ((CY_GPIO_DRIVE_FULL    == (driveSel)) || \
                                                (CY_GPIO_DRIVE_1_2     == (driveSel)) || \
                                                (CY_GPIO_DRIVE_1_4     == (driveSel)) || \
                                                (CY_GPIO_DRIVE_1_8     == (driveSel)))
#else
#define CY_GPIO_IS_HSIOM_SEC_VALID(secValue)   (0U == ((secValue) & (uint32_t)~CY_GPIO_HSIOM_SEC_MASK))
#define CY_GPIO_IS_SLEW_RATE_VALID(slewRate)   (0U == ((slewRate) & (uint32_t)~CY_GPIO_CFG_SLEW_EXT_MASK))
#define CY_GPIO_IS_DRIVE_SEL_VALID(driveSel)   (0U == ((driveSel) & (uint32_t)~CY_GPIO_CFG_DRIVE_SEL_EXT_MASK))
#endif /* CY_IP_MXS40IOSS */

#if defined (CY_IP_MXSMIF)

#if (CY_IP_MXSMIF_VERSION == 5)
    #define SMIF_INST0_PRT0 SMIF0_CORE0_SMIF_GPIO_SMIF_PRT0
    #define SMIF_INST0_PRT1 SMIF0_CORE0_SMIF_GPIO_SMIF_PRT1
    #define SMIF_INST0_PRT2 SMIF0_CORE0_SMIF_GPIO_SMIF_PRT2
    #define SMIF_INST1_PRT0 SMIF0_CORE1_SMIF_GPIO_SMIF_PRT0
    #define SMIF_INST1_PRT1 SMIF0_CORE1_SMIF_GPIO_SMIF_PRT1
    #define SMIF_INST1_PRT2 SMIF0_CORE1_SMIF_GPIO_SMIF_PRT2
    #define SMIF_INST_OFFSET SMIF_CORE_SECTION_SIZE
#elif (CY_IP_MXSMIF_VERSION == 6)
    #define SMIF_INST0_PRT0 SMIF0_CORE_SMIF_GPIO_SMIF_PRT0
    #define SMIF_INST0_PRT1 SMIF0_CORE_SMIF_GPIO_SMIF_PRT1
    #define SMIF_INST0_PRT2 SMIF0_CORE_SMIF_GPIO_SMIF_PRT2
    #define SMIF_INST1_PRT0 SMIF1_CORE_SMIF_GPIO_SMIF_PRT0
    #define SMIF_INST1_PRT1 SMIF1_CORE_SMIF_GPIO_SMIF_PRT1
    #define SMIF_INST1_PRT2 SMIF1_CORE_SMIF_GPIO_SMIF_PRT2
    #define SMIF_INST_OFFSET SMIF_SECTION_SIZE
#endif

#define CY_GPIO_IS_SMIF_GPIO(baseAddress)       (((void *)(baseAddress) == (void *)SMIF_INST0_PRT0) || \
                                                 ((void *)(baseAddress) == (void *)SMIF_INST0_PRT1)  || \
                                                 ((void *)(baseAddress) == (void *)SMIF_INST0_PRT2) || \
                                                 ((void *)(baseAddress) == (void *)SMIF_INST1_PRT0)  || \
                                                 ((void *)(baseAddress) == (void *)SMIF_INST1_PRT1) || \
                                                 ((void *)(baseAddress) == (void *)SMIF_INST1_PRT2))
#endif

/** \endcond */


/***************************************
*        Function Constants
***************************************/

/**
* \addtogroup group_gpio_macros
* \{
*/

/**
* \defgroup group_gpio_driveModes Pin drive mode
* \{
* Constants to be used for setting the drive mode of the pin. There are eight
* primary drive modes.
* Below diagrams are simplified output driver diagrams of the pin view for the
* CPU register and UDB/DSI based digital peripherals on each of the eight drive
* modes.
* \image html gpio_cpu_dm_block_diagram.png
*
* Below is a simplified output driver diagram that shows the pin view for
* fixed-function-based peripherals for each of the eight drive modes.
* \image html gpio_periio_dm_block_diagram.png
*
* - High-Impedance:
*   This is the standard high-impedance (HI-Z) state recommended for analog and
*   digital inputs. For digital signals, the input buffer is enabled; for analog
*   signals, the input buffer is typically disabled to reduce crowbar current
*   and leakage in low-power designs. To achieve the lowest device current, unused
*   GPIOs must be configured to the high-impedance drive mode with input buffer
*   disabled. Highimpedance drive mode with input buffer disabled is also the
*   default pin reset state.
*
* - Resistive Pull-Up or Resistive Pull-Down:
*   Resistive modes provide a series resistance in one of the data states and
*   strong drive in the other. Pins can be used for either digital input or
*   digital output in these modes. If resistive pull-up is required, a '1' must be
*   written to that pin's Data Register bit. If resistive pull-down is required,
*   a '0' must be written to that pin's Data Register. Interfacing mechanical
*   switches is a common application of these drive modes. The resistive modes are
*   also used to interface PSoC with open drain drive lines. Resistive pull-up is
*   used when the input is open drain low and resistive pull-down is used when the
*   input is open drain high.
*
* - Open Drain Drives High and Open Drain Drives Low:
*   Open drain modes provide high impedance in one of the data states and strong
*   drive in the other. Pins are useful as digital inputs or outputs in these
*   modes. Therefore, these modes are widely used in bidirectional digital
*   communication. Open drain drive high mode is used when the signal is
*   externally pulled down and open drain drive low is used when the signal is
*   externally pulled high. A common application for the open drain drives low
*   mode is driving I2C bus signal lines.
*
* - Strong Drive:
*   The strong drive mode is the standard digital output mode for pins; it
*   provides a strong CMOS output drive in both high and low states. Strong drive
*   mode pins should not be used as inputs under normal circumstances. This mode
*   is often used for digital output signals or to drive external devices.
*
* - Resistive Pull-Up and Resistive Pull-Down:
*   In the resistive pull-up and pull-down mode, the GPIO will have a series
*   resistance in both logic 1 and logic 0 output states. The high data state is
*   pulled up while the low data state is pulled down. This mode is useful when
*   the pin is driven by other signals that may cause shorts.
*/
#define CY_GPIO_DM_ANALOG                      (0x00UL)    /**< Analog High-Z. Input buffer off */
#define CY_GPIO_DM_INVALID_IN_OFF              (0x01UL)    /**< Invalid mode. It should not be used */
#define CY_GPIO_DM_PULLUP_IN_OFF               (0x02UL)    /**< Resistive Pull-Up. Input buffer off */
#define CY_GPIO_DM_PULLDOWN_IN_OFF             (0x03UL)    /**< Resistive Pull-Down. Input buffer off */
#define CY_GPIO_DM_OD_DRIVESLOW_IN_OFF         (0x04UL)    /**< Open Drain, Drives Low. Input buffer off */
#define CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF        (0x05UL)    /**< Open Drain, Drives High. Input buffer off */
#define CY_GPIO_DM_STRONG_IN_OFF               (0x06UL)    /**< Strong Drive. Input buffer off */
#define CY_GPIO_DM_PULLUP_DOWN_IN_OFF          (0x07UL)    /**< Resistive Pull-Up/Down. Input buffer off */
#define CY_GPIO_DM_HIGHZ                       (0x08UL)    /**< Digital High-Z. Input buffer on */
#define CY_GPIO_DM_INVALID                     (0x09UL)    /**< Invalid mode. It should not be used */
#define CY_GPIO_DM_PULLUP                      (0x0AUL)    /**< Resistive Pull-Up. Input buffer on */
#define CY_GPIO_DM_PULLDOWN                    (0x0BUL)    /**< Resistive Pull-Down. Input buffer on */
#define CY_GPIO_DM_OD_DRIVESLOW                (0x0CUL)    /**< Open Drain, Drives Low. Input buffer on */
#define CY_GPIO_DM_OD_DRIVESHIGH               (0x0DUL)    /**< Open Drain, Drives High. Input buffer on */
#define CY_GPIO_DM_STRONG                      (0x0EUL)    /**< Strong Drive. Input buffer on */
#define CY_GPIO_DM_PULLUP_DOWN                 (0x0FUL)    /**< Resistive Pull-Up/Down. Input buffer on */
#if defined (CY_IP_MXS22IOSS)
    /**
    * \note
    * This drive mode is available for the CAT1D devices.
    **/
#define CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ (0x80UL)    /**< Strong Drive/Resistive Pull-Up/High-Z based on data_out_en and data_out signals. It is applicable for I3C */
#endif /* CY_IP_MXS22IOSS */
/** \} */

/**
* \defgroup group_gpio_vtrip Voltage trip mode
* \{
* Constants to be used for setting the voltage trip type on the pin.
*/
#define CY_GPIO_VTRIP_CMOS                     (0x00UL) /**< Input buffer compatible with CMOS and I2C interfaces */
#define CY_GPIO_VTRIP_TTL                      (0x01UL) /**< Input buffer compatible with TTL and MediaLB interfaces */
/** \} */

#if (defined(CY_IP_MXS40IOSS) && (CY_IP_MXS40IOSS_VERSION == 3U)) || defined (CY_DOXYGEN)
/**
* \defgroup group_gpio_vtrip_auto Voltage trip automotive compatible mode
* \{
* Constants to be used for setting the input buffer automotive compatible mode (trip points and hysteresis) for GPIO upper bit.
*
* \note
* These macros are available for CAT1B devices.
*
*/
#define CY_GPIO_VTRIP_DIS_AUTO                 (0x00UL) /**< Input buffer is not compatible with automotive and it is compatible with CMOS/TTL interfaces */
#define CY_GPIO_VTRIP_SEL_AUTO                 (0x01UL) /**< Input buffer is compatible with automotive */
/** \} */
#endif/* CY_IP_MXS40IOSS_VERSION */

/**
* \defgroup group_gpio_slewRate Slew Rate Mode
* \{
* Constants to be used for setting the slew rate of the pin.
*/
#define CY_GPIO_SLEW_FAST                      (0x00UL) /**< Fast slew rate */
#define CY_GPIO_SLEW_SLOW                      (0x01UL) /**< Slow slew rate */
/** \} */

/**
* \defgroup group_gpio_driveStrength Pin drive strength
* \{
* Constants to be used for setting the drive strength of the pin.
*/
#if defined (CY_IP_MXS22IOSS)
#define CY_GPIO_DRIVE_SEL_7     (0x07UL) /**< 7 drive selection: 8mA/16mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_6     (0x06UL) /**< 6 drive selection: 7mA/14mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_5     (0x05UL) /**< 5 drive selection: 6mA/12mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_4     (0x04UL) /**< 4 drive selection: 5mA/10mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_3     (0x03UL) /**< 3 drive selection: 4mA/8mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_2     (0x02UL) /**< 2 drive selection: 3mA/6mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_1     (0x01UL) /**< 1 drive selection: 2mA/4mA drive current (normal/high speed IO)*/
#define CY_GPIO_DRIVE_SEL_0     (0x00UL) /**< 0 drive selection: 1mA/2mA drive current (normal/high speed IO)*/
/* Backward compatible macros */
#define CY_GPIO_DRIVE_FULL      (0x07UL) /**< Full drive strength: Max drive current */
#define CY_GPIO_DRIVE_1_2       (0x03UL) /**< 1/2 drive strength: 1/2 drive current */
#define CY_GPIO_DRIVE_1_4       (0x01UL) /**< 1/4 drive strength: 1/4 drive current */
#define CY_GPIO_DRIVE_1_8       (0x00UL) /**< 1/8 drive strength: 1/8 drive current */
#else
#define CY_GPIO_DRIVE_FULL                     (0x00UL) /**< Full drive strength: Max drive current */
#define CY_GPIO_DRIVE_1_2                      (0x01UL) /**< 1/2 drive strength: 1/2 drive current */
#define CY_GPIO_DRIVE_1_4                      (0x02UL) /**< 1/4 drive strength: 1/4 drive current */
#define CY_GPIO_DRIVE_1_8                      (0x03UL) /**< 1/8 drive strength: 1/8 drive current */
#endif
/** \} */

/**
* \defgroup group_gpio_driveStrength_trim Pin drive strength trim
* \{
* Constants to be used for setting the drive strength trim value of the pin.
*/
#if defined (CY_IP_MXS40IOSS_VERSION) && (CY_IP_MXS40IOSS_VERSION >= 4) || defined (CY_DOXYGEN)
  #define CY_GPIO_DRIVE_STRENGTH_DEFAULT         (0x00UL)  /**< \brief Default drive strength: 50 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_120OHM          (0x01UL)  /**< \brief 120 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_90OHM           (0x02UL)  /**< \brief 90 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_60OHM           (0x03UL)  /**< \brief 60 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_50OHM           (0x04UL)  /**< \brief 50 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_30OHM           (0x05UL)  /**< \brief 30 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_20OHM           (0x06UL)  /**< \brief 20 ohm */
  #define CY_GPIO_DRIVE_STRENGTH_15OHM           (0x07UL)  /**< \brief 15 ohm */
#endif
/** \} */

/**
* \defgroup group_gpio_interruptTrigger Interrupt trigger type
* \{
* Constants to be used for setting the interrupt trigger type on the pin.
*/
#define CY_GPIO_INTR_DISABLE                   (0x00UL) /**< Disable the pin interrupt generation */
#define CY_GPIO_INTR_RISING                    (0x01UL) /**< Rising-Edge interrupt */
#define CY_GPIO_INTR_FALLING                   (0x02UL) /**< Falling-Edge interrupt */
#define CY_GPIO_INTR_BOTH                      (0x03UL) /**< Both-Edge interrupt */
/** \} */

/**
* \defgroup group_gpio_sioVreg SIO output buffer mode
* \{
* Constants to be used for setting the SIO output buffer mode on the pin.
*/
#define CY_SIO_VREG_UNREGULATED                (0x00UL) /**< Unregulated output buffer */
#define CY_SIO_VREG_REGULATED                  (0x01UL) /**< Regulated output buffer */
/** \} */

/**
* \defgroup group_gpio_sioIbuf SIO input buffer mode
* \{
* Constants to be used for setting the SIO input buffer mode on the pin.
*/
#define CY_SIO_IBUF_SINGLEENDED                (0x00UL) /**< Single ended input buffer */
#define CY_SIO_IBUF_DIFFERENTIAL               (0x01UL) /**< Differential input buffer */
/** \} */

/**
* \defgroup group_gpio_sioVtrip SIO input buffer trip-point
* \{
* Constants to be used for setting the SIO input buffer trip-point of the pin.
*/
#define CY_SIO_VTRIP_CMOS                      (0x00UL) /**< CMOS input buffer (single-ended) */
#define CY_SIO_VTRIP_TTL                       (0x01UL) /**< TTL input buffer (single-ended) */
#define CY_SIO_VTRIP_0_5VDDIO_0_5VOH           (0x00UL) /**< 0.5xVddio or 0.5xVoh (differential) */
#define CY_SIO_VTRIP_0_4VDDIO_1_0VREF          (0x01UL) /**< 0.4xVddio or 0.4xVoh (differential) */
/** \} */

/**
* \defgroup group_gpio_sioVref SIO reference voltage for input buffer trip-point
* \{
* Constants to be used for setting the reference voltage of SIO input buffer trip-point.
*/
#define CY_SIO_VREF_PINREF                     (0x00UL) /**< Vref from analog pin */
#define CY_SIO_VREF_1_2V                       (0x01UL) /**< Vref from internal 1.2V reference */
#define CY_SIO_VREF_AMUX_A                     (0x02UL) /**< Vref from AMUXBUS_A */
#define CY_SIO_VREF_AMUX_B                     (0x03UL) /**< Vref from AMUXBUS_B */
/** \} */

/**
* \defgroup group_gpio_sioVoh Regulated output voltage level (Voh) and input buffer trip-point of an SIO pair
* \{
* Constants to be used for setting the Voh and input buffer trip-point of an SIO pair
*/
#define CY_SIO_VOH_1_00                        (0x00UL) /**< Voh = 1 x Reference */
#define CY_SIO_VOH_1_25                        (0x01UL) /**< Voh = 1.25 x Reference */
#define CY_SIO_VOH_1_49                        (0x02UL) /**< Voh = 1.49 x Reference */
#define CY_SIO_VOH_1_67                        (0x03UL) /**< Voh = 1.67 x Reference */
#define CY_SIO_VOH_2_08                        (0x04UL) /**< Voh = 2.08 x Reference */
#define CY_SIO_VOH_2_50                        (0x05UL) /**< Voh = 2.50 x Reference */
#define CY_SIO_VOH_2_78                        (0x06UL) /**< Voh = 2.78 x Reference */
#define CY_SIO_VOH_4_16                        (0x07UL) /**< Voh = 4.16 x Reference */
/** \} */

#if defined (CY_IP_MXS22IOSS)
/**
* \defgroup group_gpio_PullUpMode Pull-up mode
* \{
* Constants to be used for setting the Pull-up mode on the pin.
*
* \note
* This parameter is available for the CAT1D devices.
*
*/
#define CY_GPIO_PULLUP_RES_DISABLE             (0x00UL) /**< Disable additional Pull-ups */
#define CY_GPIO_PULLUP_RES_570                 (0x07UL) /**< Pull-up 570 ohms */
#define CY_GPIO_PULLUP_RES_720                 (0x06UL) /**< Pull-up 720 ohms */
#define CY_GPIO_PULLUP_RES_840                 (0x05UL) /**< Pull-up 840 ohms */
#define CY_GPIO_PULLUP_RES_1100                (0x03UL) /**< Pull-up 1100 ohms */
#define CY_GPIO_PULLUP_RES_1200                (0x04UL) /**< Pull-up 1200 ohms */
#define CY_GPIO_PULLUP_RES_1800                (0x02UL) /**< Pull-up 1800 ohms */
#define CY_GPIO_PULLUP_RES_2800                (0x01UL) /**< Pull-up 2800 ohms */
/** \} */
#endif /* CY_IP_MXS22IOSS */
/** \} group_gpio_macros */

/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_gpio_functions
* \{
*/

/**
* \addtogroup group_gpio_functions_init
* \{
*/

cy_en_gpio_status_t Cy_GPIO_Pin_Init(GPIO_PRT_Type* base, uint32_t pinNum, const cy_stc_gpio_pin_config_t *config);
cy_en_gpio_status_t Cy_GPIO_Port_Init(GPIO_PRT_Type* base, const cy_stc_gpio_prt_config_t *config);
void Cy_GPIO_Pin_FastInit(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t driveMode, uint32_t outVal, en_hsiom_sel_t hsiom);
void Cy_GPIO_Port_Deinit(GPIO_PRT_Type* base);

#if (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)
void Cy_GPIO_SetHSIOM(GPIO_PRT_Type* base, uint32_t pinNum, en_hsiom_sel_t value);
en_hsiom_sel_t Cy_GPIO_GetHSIOM(GPIO_PRT_Type* base, uint32_t pinNum);
#else
__STATIC_INLINE void Cy_GPIO_SetHSIOM(GPIO_PRT_Type* base, uint32_t pinNum, en_hsiom_sel_t value);
__STATIC_INLINE en_hsiom_sel_t Cy_GPIO_GetHSIOM(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE) */

__STATIC_INLINE GPIO_PRT_Type* Cy_GPIO_PortToAddr(uint32_t portNum);
#if (defined (CY_IP_MXS40SIOSS) && ((IOSS_HSIOM_HSIOM_SEC_PORT_NR != 0) || (CPUSS_CM33_0_SECEXT_PRESENT != 0))) || defined (CY_IP_MXS22IOSS)
void Cy_GPIO_Pin_SecFastInit(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t driveMode, uint32_t outVal, en_hsiom_sel_t hsiom);
__STATIC_INLINE void Cy_GPIO_SetHSIOM_SecPin(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetHSIOM_SecPin(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* CY_IP_MXS40SIOSS, IOSS_HSIOM_HSIOM_SEC_PORT_NR, CPUSS_CM33_0_SECEXT_PRESENT, CY_IP_MXS22IOSS */

/** \} group_gpio_functions_init */

/**
* \addtogroup group_gpio_functions_gpio
* \{
*/

void Cy_GPIO_SetAmuxSplit(cy_en_amux_split_t switchCtrl, cy_en_gpio_amuxconnect_t amuxConnect, cy_en_gpio_amuxselect_t amuxBus);
cy_en_gpio_amuxconnect_t Cy_GPIO_GetAmuxSplit(cy_en_amux_split_t switchCtrl, cy_en_gpio_amuxselect_t amuxBus);

#if (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)
uint32_t Cy_GPIO_Read(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_Write(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_ReadOut(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_Set(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_Clr(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_Inv(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetDrivemode(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetDrivemode(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetVtrip(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetVtrip(GPIO_PRT_Type* base, uint32_t pinNum);
#else
__STATIC_INLINE uint32_t Cy_GPIO_Read(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_Write(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_ReadOut(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_Set(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_Clr(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_FORCEINLINE void Cy_GPIO_Inv(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetDrivemode(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetDrivemode(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetVtrip(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetVtrip(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE) */

#if defined(GPIO_AUTOLVL_AVAIL) || defined(CY_DOXYGEN)
void Cy_GPIO_SetVtripAuto(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetVtripAuto(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* CY_IP_MXS40IOSS_VERSION */

#if (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)
void Cy_GPIO_SetSlewRate(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetSlewRate(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetDriveSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetDriveSel(GPIO_PRT_Type* base, uint32_t pinNum);
#else
__STATIC_INLINE void Cy_GPIO_SetSlewRate(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetSlewRate(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetDriveSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetDriveSel(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE) */

#if defined (CY_IP_MXS40IOSS_VERSION) && (CY_IP_MXS40IOSS_VERSION >= 4UL) || defined (CY_DOXYGEN)
void Cy_GPIO_SetDriveSelTrim(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetDriveSelTrim(GPIO_PRT_Type* base, uint32_t pinNum);
#endif

#if defined (CY_IP_MXS22IOSS)
void Cy_GPIO_SetPullupResistance(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetPullupResistance(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* CY_IP_MXS22IOSS */

/** \} group_gpio_functions_gpio */

/**
* \addtogroup group_gpio_functions_sio
* \{
*/

#if (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)
void Cy_GPIO_SetVregEn(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetVregEn(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetIbufMode(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetIbufMode(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetVtripSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetVtripSel(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetVrefSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetVrefSel(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetVohSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetVohSel(GPIO_PRT_Type* base, uint32_t pinNum);
#else
__STATIC_INLINE void Cy_GPIO_SetVregEn(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetVregEn(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetIbufMode(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetIbufMode(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetVtripSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetVtripSel(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetVrefSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetVrefSel(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetVohSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetVohSel(GPIO_PRT_Type* base, uint32_t pinNum);
#endif /* (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE) */

/** \} group_gpio_functions_sio */

/**
* \addtogroup group_gpio_functions_interrupt
* \{
*/
#if (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)
uint32_t Cy_GPIO_GetInterruptStatus(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_ClearInterrupt(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetInterruptMask(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetInterruptMask(GPIO_PRT_Type* base, uint32_t pinNum);
uint32_t Cy_GPIO_GetInterruptStatusMasked(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetSwInterrupt(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetInterruptEdge(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
uint32_t Cy_GPIO_GetInterruptEdge(GPIO_PRT_Type* base, uint32_t pinNum);
void Cy_GPIO_SetFilter(GPIO_PRT_Type* base, uint32_t value);
uint32_t Cy_GPIO_GetFilter(GPIO_PRT_Type* base);
#else
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptStatus(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_ClearInterrupt(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetInterruptMask(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptMask(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptStatusMasked(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetSwInterrupt(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetInterruptEdge(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptEdge(GPIO_PRT_Type* base, uint32_t pinNum);
__STATIC_INLINE void Cy_GPIO_SetFilter(GPIO_PRT_Type* base, uint32_t value);
__STATIC_INLINE uint32_t Cy_GPIO_GetFilter(GPIO_PRT_Type* base);
#endif /* (CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE) */

__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause0(void);
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause1(void);
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause2(void);
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause3(void);
#if (defined (CY_IP_MXS40SIOSS) && (CPUSS_CM33_0_SECEXT_PRESENT != 0)) || defined (CY_IP_MXS22IOSS)
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause0(void);
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause1(void);
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause2(void);
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause3(void);
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */

/** \} group_gpio_functions_interrupt */

/** \cond INTERNAL */
#if defined (CY_IP_MXS40SIOSS) || (defined (CY_IP_MXS40IOSS) && (CY_IP_MXS40IOSS_VERSION > 2)) || defined (CY_IP_MXS22IOSS)
#define HSIOM_PRT_V1_Type HSIOM_PRT_Type
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */
/** \endcond */

/**
* \addtogroup group_gpio_functions_init
* \{
*/

#if (defined (CY_IP_MXS40SIOSS) && ((IOSS_HSIOM_HSIOM_SEC_PORT_NR != 0) || (CPUSS_CM33_0_SECEXT_PRESENT != 0))) || defined (CY_IP_MXS22IOSS)
/*******************************************************************************
* Function Name: Cy_GPIO_SetHSIOM_SecPin
****************************************************************************//**
*
* Configures the pin as secure or non-secure.
*
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* Secure HSIOM non-secure mask
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
* This function should be called from the right protection context to access
* HSIOM secure port (HSIOM_SECURE_PRT_Type).
*
* \note
* This API is available for the CAT1B and CAT1D devices. It should be accessed
* from secure domain only.
*
* \funcusage
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetHSIOM_SecPin(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
/* Restrict access to secure API. It should be accessed from secure domain only */
#if defined (CY_PDL_TZ_ENABLED)
    uint32_t tempReg;
    uint32_t portNum;
    HSIOM_SECURE_PRT_Type* portAddrSecHSIOM;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_HSIOM_VALID(value));

    portNum = ((uint32_t)(base) - CY_GPIO_BASE) / GPIO_PRT_SECTION_SIZE;
    portAddrSecHSIOM = (HSIOM_SECURE_PRT_Type*)(CY_HSIOM_SECURE_BASE + (HSIOM_SECURE_PRT_SECTION_SIZE * portNum));

#if defined (CY_IP_MXSMIF) && ((CY_IP_MXSMIF_VERSION == 5) || (CY_IP_MXSMIF_VERSION == 6))
    if (CY_GPIO_IS_SMIF_GPIO(base))
    {
        uint32_t smif_core_offset = ((void *)base >= (void *)SMIF_INST1_PRT0) ? SMIF_INST_OFFSET : 0U;
        portNum = ((uint32_t)(base) - CY_SMIF_GPIO_BASE - smif_core_offset) / SMIF_CORE_SMIF_GPIO_SMIF_PRT_SECTION_SIZE;
        portAddrSecHSIOM = (HSIOM_SECURE_PRT_Type*)(CY_SMIF_SECURE_HSIOM_BASE + (SMIF_CORE_SMIF_HSIOM_SMIF_SECURE_PRT_SECTION_SIZE * portNum) + smif_core_offset);
    }
#endif
    tempReg= HSIOM_SEC_PRT_NONSEC_MASK(portAddrSecHSIOM) & ~(CY_GPIO_HSIOM_SEC_MASK << pinNum);
    HSIOM_SEC_PRT_NONSEC_MASK(portAddrSecHSIOM) = tempReg | ((value & CY_GPIO_HSIOM_SEC_MASK) << pinNum);
#else
    /* Suppress a compiler warning about unused variables */
    (void)base;
    (void)pinNum;
    (void)value;
#endif /* CY_PDL_TZ_ENABLED */
}

/*******************************************************************************
* Function Name: Cy_GPIO_GetHSIOM_SecPin
****************************************************************************//**
*
* Returns the current status of secure Pin.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* HSIOM input selection. In case of accessing this API from secure domain, it
* will return 0 ( secure ) or 1 ( non-secure ). Whereas, in case of accessing
* this API from non-secure domain, it will return \ref CY_GPIO_BAD_PARAM.
*
* \note
* This API is available for the CAT1B and CAT1D devices. It should be accessed
* from secure domain only.
*
* \funcusage
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetHSIOM_SecPin(GPIO_PRT_Type* base, uint32_t pinNum)
{
/* Restrict access to secure API. It should be accessed from secure domain only */
#if defined (CY_PDL_TZ_ENABLED)
    uint32_t portNum;
    HSIOM_SECURE_PRT_Type* portAddrSecHSIOM;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    portNum = ((uint32_t)(base) - CY_GPIO_BASE) / GPIO_PRT_SECTION_SIZE;
    portAddrSecHSIOM = (HSIOM_SECURE_PRT_Type*)(CY_HSIOM_SECURE_BASE + (HSIOM_SECURE_PRT_SECTION_SIZE * portNum));

#if defined (CY_IP_MXSMIF) && ((CY_IP_MXSMIF_VERSION == 5) || (CY_IP_MXSMIF_VERSION == 6))
    if (CY_GPIO_IS_SMIF_GPIO(base))
    {
        uint32_t smif_core_offset = ((void *)base >= (void *)SMIF_INST1_PRT0) ? SMIF_INST_OFFSET : 0U;
        portNum = ((uint32_t)(base) - CY_SMIF_GPIO_BASE - smif_core_offset) / SMIF_CORE_SMIF_GPIO_SMIF_PRT_SECTION_SIZE;
        portAddrSecHSIOM = (HSIOM_SECURE_PRT_Type*)(CY_SMIF_SECURE_HSIOM_BASE + (SMIF_CORE_SMIF_HSIOM_SMIF_SECURE_PRT_SECTION_SIZE * portNum) + smif_core_offset);
    }
#endif

    return (uint32_t)((HSIOM_SEC_PRT_NONSEC_MASK(portAddrSecHSIOM) >> pinNum) & CY_GPIO_HSIOM_SEC_MASK);
#else
    /* Suppress a compiler warning about unused variables */
    (void)base;
    (void)pinNum;

    return (uint32_t)CY_GPIO_BAD_PARAM;
#endif /* CY_PDL_TZ_ENABLED */
}
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */

#if (!((CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)))
/*******************************************************************************
* Function Name: Cy_GPIO_SetHSIOM
****************************************************************************//**
*
* Configures the HSIOM connection to the pin.
*
* Connects the specified High-Speed Input Output Multiplexer (HSIOM) selection
* to the pin.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* HSIOM input selection
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetHSIOM
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetHSIOM(GPIO_PRT_Type* base, uint32_t pinNum, en_hsiom_sel_t value)
{
    uint32_t portNum;
    uint32_t tempReg;
    HSIOM_PRT_V1_Type* portAddrHSIOM;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_HSIOM_VALID(value));

    portNum = ((uint32_t)(base) - CY_GPIO_BASE) / GPIO_PRT_SECTION_SIZE;
    portAddrHSIOM = (HSIOM_PRT_V1_Type*)(CY_HSIOM_BASE + (HSIOM_PRT_SECTION_SIZE * portNum));

#if defined (CY_IP_MXSMIF) && ((CY_IP_MXSMIF_VERSION == 5) || (CY_IP_MXSMIF_VERSION == 6))
    if (CY_GPIO_IS_SMIF_GPIO(base))
    {
        uint32_t smif_core_offset = ((void *)base >= (void *)SMIF_INST1_PRT0) ? SMIF_INST_OFFSET : 0U;
        portNum = ((uint32_t)(base) - CY_SMIF_GPIO_BASE - smif_core_offset) / SMIF_CORE_SMIF_GPIO_SMIF_PRT_SECTION_SIZE;
        portAddrHSIOM = (HSIOM_PRT_V1_Type*)(CY_SMIF_HSIOM_BASE + (SMIF_CORE_SMIF_HSIOM_SMIF_PRT_SECTION_SIZE * portNum) + smif_core_offset);
    }
#endif

    if(pinNum < CY_GPIO_PRT_HALF)
    {
        tempReg = HSIOM_PRT_PORT_SEL0(portAddrHSIOM) & ~(CY_GPIO_HSIOM_MASK << (pinNum << CY_GPIO_HSIOM_OFFSET));
        HSIOM_PRT_PORT_SEL0(portAddrHSIOM) = tempReg | (((uint32_t)value & CY_GPIO_HSIOM_MASK) << (pinNum << CY_GPIO_HSIOM_OFFSET));
    }
    else
    {
        pinNum -= CY_GPIO_PRT_HALF;
        tempReg = HSIOM_PRT_PORT_SEL1(portAddrHSIOM) & ~(CY_GPIO_HSIOM_MASK << (pinNum << CY_GPIO_HSIOM_OFFSET));
        HSIOM_PRT_PORT_SEL1(portAddrHSIOM) = tempReg | (((uint32_t)value & CY_GPIO_HSIOM_MASK) << (pinNum << CY_GPIO_HSIOM_OFFSET));
    }
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetHSIOM
****************************************************************************//**
*
* Returns the current HSIOM multiplexer connection to the pin.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* HSIOM input selection
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetHSIOM
*
*******************************************************************************/
__STATIC_INLINE en_hsiom_sel_t Cy_GPIO_GetHSIOM(GPIO_PRT_Type* base, uint32_t pinNum)
{
    uint32_t returnValue;
    uint32_t portNum;
    HSIOM_PRT_V1_Type* portAddrHSIOM;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    portNum = ((uint32_t)(base) - CY_GPIO_BASE) / GPIO_PRT_SECTION_SIZE;
    portAddrHSIOM = (HSIOM_PRT_V1_Type*)(CY_HSIOM_BASE + (HSIOM_PRT_SECTION_SIZE * portNum));

#if defined (CY_IP_MXSMIF) && ((CY_IP_MXSMIF_VERSION == 5) || (CY_IP_MXSMIF_VERSION == 6))
    if (CY_GPIO_IS_SMIF_GPIO(base))
    {
        uint32_t smif_core_offset = ((void *)base >= (void *)SMIF_INST1_PRT0) ? SMIF_INST_OFFSET : 0U;
        portNum = ((uint32_t)(base) - CY_SMIF_GPIO_BASE - smif_core_offset) / SMIF_CORE_SMIF_GPIO_SMIF_PRT_SECTION_SIZE;
        portAddrHSIOM = (HSIOM_PRT_V1_Type*)(CY_SMIF_HSIOM_BASE + (SMIF_CORE_SMIF_HSIOM_SMIF_PRT_SECTION_SIZE * portNum) + smif_core_offset);
    }
#endif

    if(pinNum < CY_GPIO_PRT_HALF)
    {
        returnValue = (HSIOM_PRT_PORT_SEL0(portAddrHSIOM) >> (pinNum << CY_GPIO_HSIOM_OFFSET)) & CY_GPIO_HSIOM_MASK;
    }
    else
    {
        pinNum -= CY_GPIO_PRT_HALF;
        returnValue = (HSIOM_PRT_PORT_SEL1(portAddrHSIOM) >> (pinNum << CY_GPIO_HSIOM_OFFSET)) & CY_GPIO_HSIOM_MASK;
    }

    return (en_hsiom_sel_t)returnValue;
}
#endif /* (!(CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)) */

/*******************************************************************************
* Function Name: Cy_GPIO_PortToAddr
****************************************************************************//**
*
* Retrieves the port address based on the given port number.
*
* This is a helper function to calculate the port base address when given a port
* number. It is to be used when pin access needs to be calculated at runtime.
*
* \param portNum
* Port number
*
* \return
* Base address of the port register structure
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_PortToAddr
*
*******************************************************************************/
__STATIC_INLINE GPIO_PRT_Type* Cy_GPIO_PortToAddr(uint32_t portNum)
{
    GPIO_PRT_Type* portBase;

    if(portNum < (uint32_t)IOSS_GPIO_GPIO_PORT_NR)
    {
        portBase = (GPIO_PRT_Type *)(CY_GPIO_BASE + (GPIO_PRT_SECTION_SIZE * portNum));
    }
    else
    {
        /* Error: Return default base address */
        portBase = (GPIO_PRT_Type *)(CY_GPIO_BASE);
    }

    return (portBase);
}

/** \} group_gpio_functions_init */


/**
* \addtogroup group_gpio_functions_gpio
* \{
*/

#if (!((CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)))
/*******************************************************************************
* Function Name: Cy_GPIO_Read
****************************************************************************//**
*
* Reads the current logic level on the input buffer of the pin.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \return
* Logic level present on the pin
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Read
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_Read(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));

    return (GPIO_PRT_IN(base) >> (pinNum)) & CY_GPIO_IN_MASK;
}


/*******************************************************************************
* Function Name: Cy_GPIO_Write
****************************************************************************//**
*
* Write a logic 0 or logic 1 state to the output driver.
*
* This function should be used only for software driven pins. It does not have
* any effect on peripheral driven pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* Logic level to drive out on the pin
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Write
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_Write(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

    /* Thread-safe: Directly access the pin registers instead of base->OUT */
    if(0UL == value)
    {
        GPIO_PRT_OUT_CLR(base) = CY_GPIO_OUT_MASK << pinNum;
    }
    else
    {
        GPIO_PRT_OUT_SET(base) = CY_GPIO_OUT_MASK << pinNum;
    }
}


/*******************************************************************************
* Function Name: Cy_GPIO_WritePort
****************************************************************************//**
*
* Write value to the whole GPIO port.
*
* This function should be used only for software driven pins. It does not have
* any effect on peripheral driven pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param value
* Logic level to drive out on the each pin of the selected port
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_WritePort
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_WritePort(GPIO_PRT_Type* base, uint32_t value)
{
    CY_ASSERT_L2(CY_GPIO_IS_PORT_VALUE_VALID(value));
    GPIO_PRT_OUT(base) = value;
}


/*******************************************************************************
* Function Name: Cy_GPIO_ReadOut
****************************************************************************//**
*
* Reads the current logic level on the pin output driver.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* Logic level on the pin output driver
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_ReadOut
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_ReadOut(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_OUT(base) >> pinNum) & CY_GPIO_OUT_MASK;
}


/*******************************************************************************
* Function Name: Cy_GPIO_Set
****************************************************************************//**
*
* Set a pin output to logic state high.
*
* This function should be used only for software driven pins. It does not have
* any effect on peripheral driven pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Set
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_Set(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    GPIO_PRT_OUT_SET(base) = CY_GPIO_OUT_MASK << pinNum;

}


/*******************************************************************************
* Function Name: Cy_GPIO_Clr
****************************************************************************//**
*
* Set a pin output to logic state Low.
*
* This function should be used only for software driven pins. It does not have
* any effect on peripheral driven pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Clr
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_Clr(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    GPIO_PRT_OUT_CLR(base) = CY_GPIO_OUT_MASK << pinNum;
}


/*******************************************************************************
* Function Name: Cy_GPIO_Inv
****************************************************************************//**
*
* Set a pin output logic state to the inverse of the current output
* logic state.
*
* This function should be used only for software driven pins. It does not have
* any effect on peripheral driven pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_Inv
*
*******************************************************************************/
__STATIC_FORCEINLINE void Cy_GPIO_Inv(GPIO_PRT_Type* base, uint32_t pinNum)
{
#if defined ( __ICCARM__ )
    /* Suppress error on IAR compiler when caller function is in the RAM,
     * but assert function is in the Flash.
     * This is a workaround and should be removed.
     */
    CY_PRAGMA(diag_suppress = Ta023)
#endif

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

#if defined ( __ICCARM__ )
    /* Change the severity level back to the default
     * This is a workaround and should be removed.
     */
    CY_PRAGMA(diag_default = Ta023)
#endif

    GPIO_PRT_OUT_INV(base) = CY_GPIO_OUT_MASK << pinNum;
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetDrivemode
****************************************************************************//**
*
* Configures the pin output buffer drive mode and input buffer enable.
*
* The output buffer drive mode and input buffer enable are combined into a single
* parameter. The drive mode controls the behavior of the pin in general.
* Enabling the input buffer allows the digital pin state to be read but also
* contributes to extra current consumption.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* Pin drive mode. Options are detailed in \ref group_gpio_driveModes macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetDrivemode
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetDrivemode(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    uint32_t tempReg;
    uint32_t pinLoc;
    uint32_t prtCfg;
#if defined (CY_IP_MXS22IOSS)
    uint32_t prtCfg3;
    uint32_t tempRegCfg3;
#endif /* CY_IP_MXS22IOSS */

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_DM_VALID(value));

    pinLoc = pinNum << CY_GPIO_DRIVE_MODE_OFFSET;

    tempReg = (GPIO_PRT_CFG(base) & ~(CY_GPIO_CFG_DM_MASK << pinLoc));

#if defined (CY_IP_MXS22IOSS)
    tempRegCfg3 = (GPIO_PRT_CFG_OUT3(base) & ~(CY_GPIO_CFG_DM_MASK << pinLoc));
    if(CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ == value)
    {
        prtCfg = tempReg & (~(CY_GPIO_CFG_DM_WIDTH_MASK << pinLoc));
        prtCfg |= (GPIO_PRT_CFG_IN_EN0_Msk << pinLoc);
        prtCfg3 = tempRegCfg3 | ((value >> CY_GPIO_EXT_DM_SHIFT) << pinLoc);
    }
    else
    {
        prtCfg = tempReg | ((value & CY_GPIO_CFG_DM_MASK) << pinLoc);
        prtCfg3 = tempRegCfg3 & (~(CY_GPIO_CFG_DM_WIDTH_MASK << pinLoc));
    }
#else
    prtCfg = tempReg | ((value & CY_GPIO_CFG_DM_MASK) << pinLoc);
#endif /* CY_IP_MXS22IOSS */

    GPIO_PRT_CFG(base) = prtCfg;
#if defined (CY_IP_MXS22IOSS)
    GPIO_PRT_CFG_OUT3(base) = prtCfg3;
#endif /* CY_IP_MXS22IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetDrivemode
****************************************************************************//**
*
* Returns the pin output buffer drive mode and input buffer enable state.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* Pin drive mode. Options are detailed in \ref group_gpio_driveModes macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetDrivemode
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetDrivemode(GPIO_PRT_Type* base, uint32_t pinNum)
{
    uint32_t tempReg, ret;
#if defined (CY_IP_MXS22IOSS)
    uint32_t tempRegCfg3;
#endif /* CY_IP_MXS22IOSS */

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    tempReg = GPIO_PRT_CFG(base);

#if defined (CY_IP_MXS22IOSS)
    /* Check drive mode value in CFG_OUT register to check if CFG_OUT3 is enabled */
    if((((tempReg >> (pinNum << CY_GPIO_DRIVE_MODE_OFFSET)) & CY_GPIO_CFG_DM_MASK) & CY_GPIO_CFG_DM_NO_INBUF_MASK) == 0U)
    {
        tempRegCfg3 = GPIO_PRT_CFG_OUT3(base);
        /* Check driver mode value in CFG_OUT3 register to check if it is set to CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ */
        if(((tempRegCfg3 >> (pinNum << CY_GPIO_DRIVE_MODE_OFFSET)) & CY_GPIO_CFG_DM_WIDTH_MASK) == (CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ >> CY_GPIO_EXT_DM_SHIFT))
        {
            ret = CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ;
        }
        else
        {
            ret = (tempReg >> (pinNum << CY_GPIO_DRIVE_MODE_OFFSET)) & CY_GPIO_CFG_DM_MASK;
        }
    }
    else
    {
        ret = (tempReg >> (pinNum << CY_GPIO_DRIVE_MODE_OFFSET)) & CY_GPIO_CFG_DM_MASK;
    }
#else
    ret = (tempReg >> (pinNum << CY_GPIO_DRIVE_MODE_OFFSET)) & CY_GPIO_CFG_DM_MASK;
#endif /* CY_IP_MXS22IOSS */

    return ret;
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetVtrip
****************************************************************************//**
*
* Configures the GPIO pin input buffer voltage threshold mode.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* Pin voltage threshold mode. Options are detailed in \ref group_gpio_vtrip macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVtrip
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetVtrip(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    uint32_t tempReg;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

    tempReg = GPIO_PRT_CFG_IN(base) & ~(CY_GPIO_CFG_IN_VTRIP_SEL_0_MASK << pinNum);
    GPIO_PRT_CFG_IN(base) = tempReg | ((value & CY_GPIO_CFG_IN_VTRIP_SEL_0_MASK) << pinNum);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetVtrip
****************************************************************************//**
*
* Returns the pin input buffer voltage threshold mode.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* Pin voltage threshold mode. Options are detailed in \ref group_gpio_vtrip macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVtrip
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetVtrip(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_CFG_IN(base) >> pinNum) & CY_GPIO_CFG_IN_VTRIP_SEL_0_MASK;
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetSlewRate
****************************************************************************//**
*
* Configures the pin output buffer slew rate.
* GPIO pins have fast and slow output slew rate options for the strong drivers
* configured using this API. By default the port works in fast slew mode.
* Slower slew rate results in reduced EMI and crosstalk and are recommended for
* low-frequency signals or signals without strict timing constraints.
*
* \note
* This function has no effect for the GPIO ports, where the slew rate
* configuration is not available. Refer to device datasheet for details.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* Pin slew rate. Options are detailed in \ref group_gpio_slewRate macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetSlewRate
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetSlewRate(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    uint32_t tempReg;
#if defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS22IOSS)
    uint32_t pinLoc;
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

#if defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS22IOSS)
    pinLoc = pinNum << CY_GPIO_CFG_SLEW_EXT_OFFSET;
    tempReg = (GPIO_PRT_SLEW_EXT(base) & ~(CY_GPIO_CFG_SLEW_EXT_MASK << pinLoc));
    GPIO_PRT_SLEW_EXT(base) = tempReg | ((value & CY_GPIO_CFG_SLEW_EXT_MASK) << pinLoc);
#else
    tempReg = GPIO_PRT_CFG_OUT(base) & ~(CY_GPIO_CFG_OUT_SLOW_MASK << pinNum);
    GPIO_PRT_CFG_OUT(base) = tempReg | ((value & CY_GPIO_CFG_OUT_SLOW_MASK) << pinNum);
#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS22IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetSlewRate
****************************************************************************//**
*
* Returns the pin output buffer slew rate.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* Pin slew rate. Options are detailed in \ref group_gpio_slewRate macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetSlewRate
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetSlewRate(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

#if defined (CY_IP_MXS40IOSS)
    return (GPIO_PRT_CFG_OUT(base) >> pinNum) & CY_GPIO_CFG_OUT_SLOW_MASK;
#else
    return (GPIO_PRT_SLEW_EXT(base) >> (pinNum << CY_GPIO_CFG_SLEW_EXT_OFFSET)) & CY_GPIO_CFG_SLEW_EXT_MASK;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetDriveSel
****************************************************************************//**
*
* Configures the pin output buffer drive strength.
* The drive strength field determines the active portion of the output drivers
* used and can affect the slew rate of output signals. Drive strength options
* are full drive strength (default), one-half strength, one-quarter strength,
* and oneeighth strength. Drive strength must be set to full drive strength when
* the slow slew rate bit (SLOW) is set.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
*
* \param value
* Pin drive strength. Options are detailed in \ref group_gpio_driveStrength macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetDriveSel
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetDriveSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    uint32_t tempReg;
#if defined (CY_IP_MXS40IOSS)
    uint32_t pinLoc;
#endif /* CY_IP_MXS40IOSS */

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_DRIVE_SEL_VALID(value));

#if defined (CY_IP_MXS40IOSS)
    pinLoc = (uint32_t)(pinNum << 1u) + CY_GPIO_CFG_OUT_DRIVE_OFFSET;
    tempReg = GPIO_PRT_CFG_OUT(base) & ~(CY_GPIO_CFG_OUT_DRIVE_SEL_MASK << pinLoc);
    GPIO_PRT_CFG_OUT(base)  = tempReg | ((value & CY_GPIO_CFG_OUT_DRIVE_SEL_MASK) << pinLoc);
#else
    if(pinNum < CY_GPIO_PRT_HALF)
    {
        tempReg = GPIO_PRT_DRIVE_EXT0(base) & ~(CY_GPIO_CFG_DRIVE_SEL_EXT_MASK << (pinNum << CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET));
        GPIO_PRT_DRIVE_EXT0(base) = tempReg | ((value & CY_GPIO_CFG_DRIVE_SEL_EXT_MASK) << (pinNum << CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET));
    }
    else
    {
        pinNum -= CY_GPIO_PRT_HALF;
        tempReg = GPIO_PRT_DRIVE_EXT1(base) & ~(CY_GPIO_CFG_DRIVE_SEL_EXT_MASK << (pinNum << CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET));
        GPIO_PRT_DRIVE_EXT1(base) = tempReg | ((value & CY_GPIO_CFG_DRIVE_SEL_EXT_MASK) << (pinNum << CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET));
    }
#endif /* CY_IP_MXS40SIOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetDriveSel
****************************************************************************//**
*
* Returns the pin output buffer drive strength.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* Pin drive strength. Options are detailed in \ref group_gpio_driveStrength macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetDriveSel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetDriveSel(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

#if defined (CY_IP_MXS40IOSS)
    return ((GPIO_PRT_CFG_OUT(base) >> ((uint32_t)(pinNum << 1u) + CY_GPIO_CFG_OUT_DRIVE_OFFSET))
            & CY_GPIO_CFG_OUT_DRIVE_SEL_MASK);
#else
    uint32_t returnValue;
    if(pinNum < CY_GPIO_PRT_HALF)
    {
        returnValue = (GPIO_PRT_DRIVE_EXT0(base) >> (pinNum << CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET)) & CY_GPIO_CFG_DRIVE_SEL_EXT_MASK;
    }
    else
    {
        pinNum -= CY_GPIO_PRT_HALF;
        returnValue = (GPIO_PRT_DRIVE_EXT1(base) >> (pinNum << CY_GPIO_CFG_DRIVE_SEL_EXT_OFFSET)) & CY_GPIO_CFG_DRIVE_SEL_EXT_MASK;
    }
    return returnValue;
#endif /* CY_IP_MXS40IOSS */
}
#endif /* (!(defined(CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE))) */


/** \} group_gpio_functions_gpio */

/**
* \addtogroup group_gpio_functions_sio
* \{
*/
#if (!((CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)))
/*******************************************************************************
* Function Name: Cy_GPIO_SetVregEn
****************************************************************************//**
*
* Configures the SIO pin pair output buffer regulation mode.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* SIO pair output buffer regulator mode. Options are detailed in \ref group_gpio_sioVreg macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVregEn
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetVregEn(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
#if defined (CY_IP_MXS40IOSS)
    uint32_t tempReg;
    uint32_t pinLoc;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

    pinLoc = (pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET;
    tempReg = GPIO_PRT_CFG_SIO(base) & ~(CY_GPIO_VREG_EN_MASK << pinLoc);
    GPIO_PRT_CFG_SIO(base)  = tempReg | ((value & CY_GPIO_VREG_EN_MASK) << pinLoc);
#else
    CY_ASSERT_L2(1);
    (void) base;
    (void) pinNum;
    (void) value;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetVregEn
****************************************************************************//**
*
* Returns the SIO pin pair output buffer regulation mode.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* SIO pair output buffer regulator mode. Options are detailed in \ref group_gpio_sioVreg macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVregEn
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetVregEn(GPIO_PRT_Type* base, uint32_t pinNum)
{
#if defined (CY_IP_MXS40IOSS)

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_CFG_SIO(base) >> ((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET)) & CY_GPIO_VREG_EN_MASK;
#else
    CY_ASSERT_L2(1);

    (void) base;
    (void) pinNum;

    return CYRET_INVALID_STATE;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetIbufMode
****************************************************************************//**
*
* Configures the SIO pin pair input buffer mode.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* SIO pair input buffer mode. Options are detailed in \ref group_gpio_sioIbuf macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetIbufMode
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetIbufMode(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
#if defined (CY_IP_MXS40IOSS)
    uint32_t tempReg;
    uint32_t pinLoc;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

    pinLoc = ((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_IBUF_SHIFT;
    tempReg = (GPIO_PRT_CFG_SIO(base) & ~(CY_GPIO_IBUF_MASK << pinLoc));
    GPIO_PRT_CFG_SIO(base) = tempReg | ((value & CY_GPIO_IBUF_MASK) << pinLoc);
#else
    CY_ASSERT_L2(1);
    (void) base;
    (void) pinNum;
    (void) value;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetIbufMode
****************************************************************************//**
*
* Returns the SIO pin pair input buffer mode.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* SIO pair input buffer mode. Options are detailed in \ref group_gpio_sioIbuf macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetIbufMode
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetIbufMode(GPIO_PRT_Type* base, uint32_t pinNum)
{
#if defined (CY_IP_MXS40IOSS)
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_CFG_SIO(base) >> (((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_IBUF_SHIFT)) & CY_GPIO_IBUF_MASK;
#else
    CY_ASSERT_L2(1);

    (void) base;
    (void) pinNum;

    return CYRET_INVALID_STATE;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetVtripSel
****************************************************************************//**
*
* Configures the SIO pin pair input buffer trip point.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* SIO pair input buffer trip point. Options are detailed in \ref group_gpio_sioVtrip macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVtripSel
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetVtripSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
#if defined (CY_IP_MXS40IOSS)
    uint32_t tempReg;
    uint32_t pinLoc;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

    pinLoc = ((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_VTRIP_SEL_SHIFT;
    tempReg = (GPIO_PRT_CFG_SIO(base) & ~(CY_GPIO_VTRIP_SEL_MASK << pinLoc));
    GPIO_PRT_CFG_SIO(base) = tempReg | ((value & CY_GPIO_VTRIP_SEL_MASK) << pinLoc);
#else
        (void) base;
        (void) pinNum;
        (void) value;

        CY_ASSERT_L2(1);
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetVtripSel
****************************************************************************//**
*
* Returns the SIO pin pair input buffer trip point.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* SIO pair input buffer trip point. Options are detailed in \ref group_gpio_sioVtrip macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVtripSel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetVtripSel(GPIO_PRT_Type* base, uint32_t pinNum)
{
#if defined (CY_IP_MXS40IOSS)
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_CFG_SIO(base) >> (((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_VTRIP_SEL_SHIFT)) & CY_GPIO_VTRIP_SEL_MASK;
#else
    CY_ASSERT_L2(1);

    (void) base;
    (void) pinNum;

    return CYRET_INVALID_STATE;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetVrefSel
****************************************************************************//**
*
* Configures the SIO reference voltage for the input buffer trip point.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* SIO pair reference voltage. Options are detailed in \ref group_gpio_sioVref macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVrefSel
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetVrefSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
#if defined (CY_IP_MXS40IOSS)
    uint32_t tempReg;
    uint32_t pinLoc;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VREF_SEL_VALID(value));

    pinLoc = ((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_VREF_SEL_SHIFT;
    tempReg = (GPIO_PRT_CFG_SIO(base) & ~(CY_GPIO_VREF_SEL_MASK << pinLoc));
    GPIO_PRT_CFG_SIO(base) = tempReg | ((value & CY_GPIO_VREF_SEL_MASK) << pinLoc);
#else
    (void) base;
    (void) pinNum;
    (void) value;

    CY_ASSERT_L2(1);
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetVrefSel
****************************************************************************//**
*
* Returns the SIO reference voltage for the input buffer trip point.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* SIO pair reference voltage. Options are detailed in \ref group_gpio_sioVref macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVrefSel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetVrefSel(GPIO_PRT_Type* base, uint32_t pinNum)
{
#if defined (CY_IP_MXS40IOSS)
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_CFG_SIO(base) >> (((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_VREF_SEL_SHIFT)) & CY_GPIO_VREF_SEL_MASK;
#else
    CY_ASSERT_L2(1);

    (void) base;
    (void) pinNum;

    return CYRET_INVALID_STATE;
#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetVohSel
****************************************************************************//**
*
* Configures the regulated output reference multiplier for the SIO pin pair.
*
* The regulated output reference controls both the output level of digital output
* pin and the input trip point of digital input pin in the SIO pair.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \param value
* SIO pair reference voltage. Options are detailed in \ref group_gpio_sioVoh macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVohSel
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetVohSel(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
#if defined (CY_IP_MXS40IOSS)
    uint32_t tempReg;
    uint32_t pinLoc;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VOH_SEL_VALID(value));

    pinLoc = ((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_VOH_SEL_SHIFT;
    tempReg = (GPIO_PRT_CFG_SIO(base) & ~(CY_GPIO_VOH_SEL_MASK << pinLoc));
    GPIO_PRT_CFG_SIO(base) = tempReg | ((value & CY_GPIO_VOH_SEL_MASK) << pinLoc);

 #else
    CY_ASSERT_L2(1);
    (void) base;
    (void) pinNum;
    (void) value;

#endif /* CY_IP_MXS40IOSS */
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetVohSel
****************************************************************************//**
*
* Returns the regulated output reference multiplier for the SIO pin pair.
*
* Note that this function has no effect on non-SIO pins.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
*
* \return
* SIO pair reference voltage. Options are detailed in \ref group_gpio_sioVoh macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetVohSel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetVohSel(GPIO_PRT_Type* base, uint32_t pinNum)
{
#if defined (CY_IP_MXS40IOSS)
    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(pinNum));

    return (GPIO_PRT_CFG_SIO(base) >> (((pinNum & CY_GPIO_SIO_ODD_PIN_MASK) << CY_GPIO_CFG_SIO_OFFSET) + CY_GPIO_VOH_SEL_SHIFT)) & CY_GPIO_VOH_SEL_MASK;
#else
    CY_ASSERT_L2(1);
    (void) base;
    (void) pinNum;

    return CYRET_INVALID_STATE;
#endif /* CY_IP_MXS40IOSS */
}
#endif /* (!((CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE))) */


/** \} group_gpio_functions_sio */



/**
* \addtogroup group_gpio_functions_interrupt
* \{
*/

#if (!((CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE)))
/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptStatus
****************************************************************************//**
*
* Returns the current unmasked interrupt state of the pin.
*
* The core processor's NVIC is triggered by the masked interrupt bits. This
* function allows reading the unmasked interrupt state. Whether the bit
* positions actually trigger the interrupt are defined by the interrupt mask bits.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
* Bit position 8 is the routed pin through the port glitch filter.
*
* \return
* 0 = Pin interrupt condition not detected
* 1 = Pin interrupt condition detected
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_GetInterruptStatus
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptStatus(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));
    return (GPIO_PRT_INTR(base) >> pinNum) & CY_GPIO_INTR_STATUS_MASK;
}


/*******************************************************************************
* Function Name: Cy_GPIO_ClearInterrupt
****************************************************************************//**
*
* Clears the triggered pin interrupt.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register
* Bit position 8 is the routed pin through the port glitch filter.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_ClearInterrupt
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_ClearInterrupt(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));

    /* Any INTR MMIO registers AHB clearing must be preceded with an AHB read access */
    (void)GPIO_PRT_INTR(base);

    GPIO_PRT_INTR(base) =  CY_GPIO_INTR_STATUS_MASK << pinNum;

    /* This read ensures that the initial write has been flushed out to the hardware */
    (void)GPIO_PRT_INTR(base);
}


/*******************************************************************************
* Function Name: Cy_GPIO_SetInterruptMask
****************************************************************************//**
*
* Configures the pin interrupt to be forwarded to the CPU NVIC.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \param value
* 0 = Pin interrupt not forwarded to CPU interrupt controller
* 1 = Pin interrupt masked and forwarded to CPU interrupt controller
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetInterruptMask
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetInterruptMask(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    uint32_t tempReg;

    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_VALUE_VALID(value));

    tempReg= GPIO_PRT_INTR_MASK(base) & ~(CY_GPIO_INTR_EN_MASK << pinNum);

    GPIO_PRT_INTR_MASK(base) = tempReg | ((value & CY_GPIO_INTR_EN_MASK) << pinNum);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptMask
****************************************************************************//**
*
* Returns the state of the pin interrupt mask.
*
* This mask is used to determine whether the pin is configured to be forwarded
* to the CPU NVIC.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \return
* 0 = Pin interrupt not forwarded to CPU interrupt controller
* 1 = Pin interrupt masked and forwarded to CPU interrupt controller
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetInterruptMask
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptMask(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));

    return (GPIO_PRT_INTR_MASK(base) >> pinNum) & CY_GPIO_INTR_EN_MASK;
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptStatusMasked
****************************************************************************//**
*
* Return the pin's current interrupt state after being masked.
*
* The core processor's NVIC is triggered by the masked interrupt bits. This
* function allows reading this masked interrupt state. Note that the bits that
* are not masked will not be forwarded to the NVIC.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \return
* 0 = Pin interrupt not detected or not forwarded to CPU interrupt controller
* 1 = Pin interrupt detected and forwarded to CPU interrupt controller
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_GetInterruptStatusMasked
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptStatusMasked(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));

    return (GPIO_PRT_INTR_MASKED(base) >> pinNum) & CY_GPIO_INTR_MASKED_MASK;

}


/*******************************************************************************
* Function Name: Cy_GPIO_SetSwInterrupt
****************************************************************************//**
*
* Force a pin interrupt to trigger.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetSwInterrupt
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetSwInterrupt(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));

    GPIO_PRT_INTR_SET(base) = CY_GPIO_INTR_SET_MASK << pinNum;

}


/*******************************************************************************
* Function Name: Cy_GPIO_SetInterruptEdge
****************************************************************************//**
*
* Configures the type of edge that will trigger a pin interrupt.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \param value
* Pin interrupt mode. Options are detailed in \ref group_gpio_interruptTrigger macros
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetInterruptEdge
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetInterruptEdge(GPIO_PRT_Type* base, uint32_t pinNum, uint32_t value)
{
    uint32_t tempReg;
    uint32_t pinLoc;

    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));
    CY_ASSERT_L2(CY_GPIO_IS_INT_EDGE_VALID(value));

    pinLoc = pinNum << CY_GPIO_INTR_CFG_OFFSET;
    tempReg = GPIO_PRT_INTR_CFG(base) & ~(CY_GPIO_INTR_EDGE_MASK << pinLoc);

    GPIO_PRT_INTR_CFG(base) = tempReg | ((value & CY_GPIO_INTR_EDGE_MASK) << pinLoc);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptEdge
****************************************************************************//**
*
* Returns the current pin interrupt edge type.
*
* \param base
* Pointer to the pin's port register base address
*
* \param pinNum
* Position of the pin bit-field within the port register.
* Bit position 8 is the routed pin through the port glitch filter.
*
* \return
* Pin interrupt mode. Options are detailed in \ref group_gpio_interruptTrigger macros
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetInterruptEdge
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptEdge(GPIO_PRT_Type* base, uint32_t pinNum)
{
    CY_ASSERT_L2(CY_GPIO_IS_FILTER_PIN_VALID(pinNum));

    return (GPIO_PRT_INTR_CFG(base) >> (pinNum << CY_GPIO_INTR_CFG_OFFSET)) & CY_GPIO_INTR_EDGE_MASK;

}


/*******************************************************************************
* Function Name: Cy_GPIO_SetFilter
****************************************************************************//**
*
* Configures which pin on the port connects to the port-specific glitch filter.
*
* Each port contains a single 50ns glitch filter. Any of the pins on the port
* can be routed to this filter such that the input signal is filtered before
* reaching the edge-detect interrupt circuitry. The state of the filtered pin
* can also be read by calling the Cy_GPIO_Read() function.
*
* \param base
* Pointer to the pin's port register base address
*
* \param value
* The number of the port pin to route to the port filter (0...7)
*
* \note
* This function modifies a port register in a read-modify-write operation. It is
* not thread safe as the resource is shared among multiple pins on a port.
*
* \note
* The filtered pin does not have an associated HSIOM connection. Therefore
* it cannot be routed directly to other peripherals in hardware.
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetFilter
*
*******************************************************************************/
__STATIC_INLINE void Cy_GPIO_SetFilter(GPIO_PRT_Type* base, uint32_t value)
{
    uint32_t tempReg;

    CY_ASSERT_L2(CY_GPIO_IS_PIN_VALID(value));

    tempReg = GPIO_PRT_INTR_CFG(base) & ~(CY_GPIO_INTR_FLT_EDGE_MASK << CY_GPIO_INTR_FILT_OFFSET);
    GPIO_PRT_INTR_CFG(base) = tempReg | ((value & CY_GPIO_INTR_FLT_EDGE_MASK) << CY_GPIO_INTR_FILT_OFFSET);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetFilter
****************************************************************************//**
*
* Returns which pin is currently configured to connect to the port-specific
* glitch filter.
*
* Each port contains a single 50ns glitch filter. Any of the pins on the port
* can be routed to this filter such that the input signal is filtered before
* reaching the edge-detect interrupt circuitry. The state of the filtered pin
* can also be read by calling the Cy_GPIO_Read() function.
*
* \param base
* Pointer to the pin's port register base address
*
* \return
* The number of the port pin routed to the port filter (0...7)
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_SetFilter
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetFilter(GPIO_PRT_Type* base)
{
    return ((GPIO_PRT_INTR_CFG(base) >> CY_GPIO_INTR_FILT_OFFSET) & CY_GPIO_INTR_FLT_EDGE_MASK);
}
#endif /* (!((CY_CPU_CORTEX_M4) && defined(CY_DEVICE_SECURE))) */

/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptCause0
****************************************************************************//**
*
* Returns the interrupt status for ports 0 to 31.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \funcusage
* \snippet gpio/snippet/main.c snippet_Cy_GPIO_GetInterruptCause0
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause0(void)
{
    return (GPIO_INTR_CAUSE0);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptCause1
****************************************************************************//**
*
* Returns the interrupt status for ports 32 to 63.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \funcusage
* Refer to the Cy_GPIO_GetInterruptCause0() example.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause1(void)
{
    return (GPIO_INTR_CAUSE1);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptCause2
****************************************************************************//**
*
* Returns the interrupt status for ports 64 to 95.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
* \funcusage
* Refer to the Cy_GPIO_GetInterruptCause0() example.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause2(void)
{
    return (GPIO_INTR_CAUSE2);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetInterruptCause3
****************************************************************************//**
*
* Returns the interrupt status for ports 96 to 127.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \funcusage
* Refer to the Cy_GPIO_GetInterruptCause0() example.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetInterruptCause3(void)
{
    return (GPIO_INTR_CAUSE3);
}

#if (defined (CY_IP_MXS40SIOSS) && (CPUSS_CM33_0_SECEXT_PRESENT != 0)) || defined (CY_IP_MXS22IOSS)
/*******************************************************************************
* Function Name: Cy_GPIO_GetSecureInterruptCause0
****************************************************************************//**
*
* Returns the interrupt status for ports 0 to 31.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \note
* This API is available for the CAT1B and CAT1D devices.
*
* \funcusage
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause0(void)
{
    return (GPIO_SEC_INTR_CAUSE0);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetSecureInterruptCause1
****************************************************************************//**
*
* Returns the interrupt status for ports 32 to 63.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \note
* This API is available for the CAT1B and CAT1D devices.
*
* \funcusage
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause1(void)
{
    return (GPIO_SEC_INTR_CAUSE1);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetSecureInterruptCause2
****************************************************************************//**
*
* Returns the interrupt status for ports 64 to 95.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \note
* This API is available for the CAT1B and CAT1D devices.
*
* \funcusage
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause2(void)
{
    return (GPIO_SEC_INTR_CAUSE2);
}


/*******************************************************************************
* Function Name: Cy_GPIO_GetSecureInterruptCause3
****************************************************************************//**
*
* Returns the interrupt status for ports 96 to 127.
*
* \return
* 0 = Interrupt not detected on port
* 1 = Interrupt detected on port
*
* \note
* This API is available for the CAT1B and CAT1D devices.
*
* \funcusage
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_GPIO_GetSecureInterruptCause3(void)
{
    return (GPIO_SEC_INTR_CAUSE3);
}
#endif /* CY_IP_MXS40SIOSS, CPUSS_CM33_0_SECEXT_PRESENT, CY_IP_MXS22IOSS */

/** \} group_gpio_functions_interrupt */

/** \} group_gpio_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXS40SIOSS, CY_IP_MXS40IOSS, CY_IP_MXS22IOSS */

#endif /* CY_GPIO_H */

/** \} group_gpio */

/* [] END OF FILE */
