/***************************************************************************//**
* \file cy_cryptolite_cmac.h
* \version 2.80
*
* \brief
*  This file provides constants and function prototypes
*  for the API for the CMAC method in the Cryptolite block driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2020-2023), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
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


#if !defined (CY_CRYPTOLITE_CMAC_H)
#define CY_CRYPTOLITE_CMAC_H

#include "cy_device.h"

#if defined(CY_IP_MXCRYPTOLITE) 

#include "cy_cryptolite_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CRYPTOLITE_AES_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_CMAC_C)

#include "cy_cryptolite_aes.h"

/**
* \addtogroup group_cryptolite_lld_mac_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Cmac_Init
****************************************************************************//**
*
* The function for initialization of CMAC operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param key
* The pointer to the CMAC key.
*
* \param cmacState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \param aesBuffers The buffers should be a SAHB mapped addresses.
* The pointer to the memory buffers storage.
*
* \return
* \ref cy_en_cryptolite_status_t
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Cmac_Init(CRYPTOLITE_Type *base,
                                                 uint8_t const *key,
                                                 cy_stc_cryptolite_aes_state_t *cmacState,
                                                 cy_stc_cryptolite_aes_buffers_t *aesBuffers);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Cmac_Start
****************************************************************************//**
*
* Starts CMAC calculation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \return
* \ref cy_en_cryptolite_status_t
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Cmac_Start(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *cmacState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Cmac_Update
****************************************************************************//**
*
* Performs cmac update for multi stage operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param message
* The pointer to the message whose CMAC is being computed.
*
* \param messageSize
* The size of the message whose CMAC is being computed.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \return
* \ref cy_en_cryptolite_status_t
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Cmac_Update(CRYPTOLITE_Type *base,
                                                    uint8_t const *message,
                                                    uint32_t  messageSize,
                                                    cy_stc_cryptolite_aes_state_t *cmacState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Cmac_Finish
****************************************************************************//**
*
* Completes CMAC calculation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cmac
* The pointer to the computed CMAC value.
*
* \param cmacState
* The pointer to the structure which stores the CMAC context.
*
* \return
* \ref cy_en_cryptolite_status_t
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Cmac_Finish(CRYPTOLITE_Type *base,  uint8_t* cmac, cy_stc_cryptolite_aes_state_t *cmacState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Cmac_Free
****************************************************************************//**
*
* Clears CMAC operation context.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param cmacState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Cmac_Free(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *cmacState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Cmac
****************************************************************************//**
*
* Performs CMAC(Cipher-based Message Authentication Code) operation
* on a message to produce message authentication code using AES.
**
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param message
* The pointer to a source plain text.
*
* \param messageSize
* The size of a source plain text.
*
* \param key
* The pointer to the encryption key.
*
* \param cmac
* The pointer to the calculated CMAC.
*
* \param cmacState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Cmac(CRYPTOLITE_Type *base,
                                          uint8_t  const *key,
                                          uint8_t const *message,
                                          uint32_t messageSize,
                                          uint8_t *cmac,
                                          cy_stc_cryptolite_aes_state_t *cmacState);

/** \} group_cryptolite_lld_mac_functions */

#endif /*defined(CY_CRYPTOLITE_CFG_CMAC_C) */
#endif /* CRYPTOLITE_AES_PRESENT */

#if defined(__cplusplus)
}
#endif

#endif /* defined(CY_IP_MXCRYPTOLITE) */

#endif /* #if !defined (CY_CRYPTOLITE_CMAC_H) */


/* [] END OF FILE */
