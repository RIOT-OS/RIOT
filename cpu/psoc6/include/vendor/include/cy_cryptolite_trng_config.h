/***************************************************************************//**
* \file cy_cryptolite_trng_config.h
* \version 2.80
*
* \brief
*  This file provides  constants and parameters
*  for the Cryptolite TRNG driver.
*
********************************************************************************
* Copyright 2016-2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/


#if !defined (CY_CRYPTOLITE_TRNG_CONFIG_H)
#define CY_CRYPTOLITE_TRNG_CONFIG_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"


#if (CRYPTOLITE_TRNG_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_TRNG_C)

/** TRNG Configuration default values */
/**
* Specifies the clock divider that is used to sample oscillator data.
* "0": sample clock is "clk_sys".
* "1": sample clock is "clk_sys"/2.
* ...
* "255": sample clock is "clk_sys"/256.
*/
#define CY_CRYPTOLITE_DEF_TRNG_SAMPLE_CLOCK_DIV          (1UL)

/**
* Specifies the clock divider used to produce reduced bits.
* "0": 1 reduced bit is produced for each sample.
* "1": 1 reduced bit is produced for each 2 samples.
* ...
* "255": 1 reduced bit is produced for each 256 samples.
*
* The reduced bits are considered random bits and shifted into TR_RESULT0.DATA32.
*/
#define CY_CRYPTOLITE_DEF_TRNG_RED_CLOCK_DIV                  (3UL)

/**
* Specifies an initialization delay: a number of removed/dropped samples before
* reduced bits are generated. This field should be programmed in the range [1, 255].
* After starting the oscillators, at least the first 2 samples should be
* removed/dropped to clear the state of internal synchronizers. In addition,
* it is advised to drop at least the second 2 samples from the oscillators
* (to circumvent the semi-predictable oscillator start-up behavior). This results
* in the default field value of "3". the field encoding is as follows:
* "0": 1 sample is dropped.
* "1": 2 samples are dropped.
* ...
* "255": 256 samples are dropped.
*
* The TR_INITIALIZED interrupt cause is set to '1', when the initialization delay is passed.
*/
#define CY_CRYPTOLITE_DEF_TRNG_INIT_DELAY                        (255UL)

/**
* Specifies if the "von Neumann corrector" is disabled or enabled:
* '0': disabled.
* '1': enabled.
* The "von Neumann corrector" post-processes the reduced bits to remove a '0' or '1' bias.
* The corrector operates on reduced bit pairs ("oldest bit, newest bit"):
* "00": no bit is produced.
* "01": '0' bit is produced (oldest bit).
* "10": '1' bit is produced (oldest bit).
* "11": no bit is produced.
* NOTE: The corrector produces bits at a random pace and at a frequency that
* is 1/4 of the reduced bit frequency (reduced bits are processed in pairs,
* and half of the pairs do NOT produce a bit).
*/
#define CY_CRYPTOLITE_DEF_TRNG_VON_NEUMANN_CORR            (1UL)

/**
* Specifies if the TRNG adaptive proportion test is enabled
* Is test fails HW sets INTR.TR_AP_DETECT to '1' */
#define CY_CRYPTOLITE_DEF_TRNG_AP_ENABLE          (1UL)

/**
* Specifies if the TRNG repetition count test is enabled
* Is test fails HW sets INTR.TR_RC_DETECT to '1' */
#define CY_CRYPTOLITE_DEF_TRNG_RC_ENABLE          (1UL)

/**
* Specifies if the TRNG functionality is stopped on an adaptive proportion test
* detection (when HW sets INTR.TR_AP_DETECT to '1'):
* '0': Functionality is stopped (TR_CTL1 fields are set to '0' by HW).
* '1': Functionality is NOT stopped.
*/
#define CY_CRYPTOLITE_DEF_TRNG_STOP_ON_AP_DETECT          (1UL)

/**
* Specifies if the TRNG functionality is stopped on a repetition count test detection
* (when HW sets INTR.TR_RC_DETECT to '1'):
* '0': Functionality is stopped (TR_CTL1 fields are set to '0' by HW).
* '1': Functionality is NOT stopped.
*/
#define CY_CRYPTOLITE_DEF_TRNG_STOP_ON_RC_DETECT          (1UL)

/**
* The polynomial for programmable Galois ring oscillator. The polynomial is represented
* WITHOUT the high order bit (this bit is always assumed '1'). The polynomial should be aligned
* so that the more significant bits (bit 30 and down) contain the polynomial and the less
* significant bits (bit 0 and up) contain padding '0's.
*/
#define CY_CRYPTOLITE_DEF_TRNG_GARO                                    (0xb2d1ab70U)

/**
* The polynomial for the programmable Fibonacci ring oscillator. The polynomial is represented
* WITHOUT the high order bit (this bit is always assumed '1'). The polynomial should
* be aligned so that the more significant bits (bit 30 and down) contain the polynomial
* and the less significant bits (bit 0 and up) contain padding '0's.
*/
#define CY_CRYPTOLITE_DEF_TRNG_FIRO                                    (0xe6b8c3b3UL)

/**
* Selection of the bit stream:
* "0": DAS bit stream.
* "1": RED bit stream.
* "2": TR bit stream.
* "3": Undefined.
*/
#define CY_CRYPTOLITE_DEF_TRNG_BITSTREAM_SEL                  (CY_CRYPTOLITE_TRMON_BS_TR)

/**
* Cut-off count (legal range is [1, 255]):
* "0": Illegal.
* "1": 1 repetition.
* ...
* "255": 255 repetitions.
*/
#define CY_CRYPTOLITE_DEF_TRNG_CUTOFF_COUNT8                  (255UL)

/**
* Cut-off count (legal range is [1, 65535]).
* "0": Illegal.
* "1": 1 occurrence.
* ...
* "65535": 65535 occurrences.
*/
#define CY_CRYPTOLITE_DEF_TRNG_CUTOFF_COUNT16                (255UL)

/**
* The window size (minus 1) :
* "0": 1 bit.
* ...
* "65535": 65536 bits.
*/

#define CY_CRYPTOLITE_DEF_TRNG_WINDOW_SIZE           (255UL)


#if defined(__cplusplus)
}
#endif

#endif /* CRYPTOLITE_TRNG_PRESENT */
#endif /* CY_CRYPTOLITE_CFG_TRNG_C */
#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_TRNG_CONFIG_H) */


/* [] END OF FILE */
