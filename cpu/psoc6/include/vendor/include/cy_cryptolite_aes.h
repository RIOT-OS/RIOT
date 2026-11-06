/***************************************************************************//**
* \file cy_cryptolite_aes.h
* \version 2.80
*
* \brief
*  This file provides common constants and parameters
*  for the Cryptolite AES driver.
*
********************************************************************************
* Copyright 2022 Cypress Semiconductor Corporation
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

#if !defined (CY_CRYPTOLITE_AES_H)
#define CY_CRYPTOLITE_AES_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"

#if (CRYPTOLITE_AES_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_AES_C)


/** Defines the Cryptolite AES block size (in bytes) */
/** Only 128 bit key supported **/
#define CY_CRYPTOLITE_AES_BLOCK_SIZE          (16u)

/** Defines the Cryptolite AES_128 key maximum size (in bytes) */
#define CY_CRYPTOLITE_AES_128_KEY_SIZE        (16u)

/** Defines the Cryptolite AES key maximum size (in bytes) */
#define CY_CRYPTOLITE_AES_MAX_KEY_SIZE        (16u)

/** Defines the Cryptolite AES_128 key maximum size (in four-byte words) */
#define CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32    (uint32_t)(CY_CRYPTOLITE_AES_128_KEY_SIZE / 4UL)

/** Defines size of the AES block, in four-byte words */
#define CY_CRYPTOLITE_AES_BLOCK_SIZE_U32      (uint32_t)(CY_CRYPTOLITE_AES_BLOCK_SIZE / 4UL)

#define CY_CRYPTO_AES_CTR_CNT_POS          (0x02U)

/**
* \addtogroup group_cryptolite_data_structures
* \{
*/
/* The structure to define used memory buffers */
typedef struct
{
    /** \cond INTERNAL */
    CY_ALIGN(4) uint32_t key[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    
    CY_ALIGN(4) uint32_t src[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    CY_ALIGN(4) uint32_t dst[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];

    #if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC) || defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CFB) || defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR)
    CY_ALIGN(4) uint32_t block0[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    #endif

    #if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CFB) || defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR)
    CY_ALIGN(4) uint32_t block1[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    #endif

    #if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR)
    CY_ALIGN(4) uint32_t block2[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    CY_ALIGN(4) uint32_t block3[CY_CRYPTOLITE_AES_MAX_KEY_SIZE_U32];
    #endif

    CY_ALIGN(4) uint8_t unProcessedData[CY_CRYPTOLITE_AES_BLOCK_SIZE];
    CY_ALIGN(4) uint8_t iv[CY_CRYPTOLITE_AES_BLOCK_SIZE];

    #if defined (CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC)
    CY_ALIGN(4) uint8_t temp[CY_CRYPTOLITE_AES_BLOCK_SIZE];
    #endif

    /*Assigned dummy byte for the 4 byte alignment*/
    CY_ALIGN(4) uint32_t dummy[1u]; 
    /** \endcond */
} cy_stc_cryptolite_aes_buffers_t;




/* The structure to define used memory buffers */
typedef struct
{
    /** \cond INTERNAL */
    cy_stc_cryptolite_aes_buffers_t aesCbcMac_buffer;
    cy_stc_cryptolite_aes_buffers_t aesCtr_buffer;
    CY_ALIGN(4) uint8_t temp_buffer[CY_CRYPTOLITE_AES_BLOCK_SIZE];
    CY_ALIGN(4) uint8_t ctr[CY_CRYPTOLITE_AES_BLOCK_SIZE];
    CY_ALIGN(4) uint8_t y[CY_CRYPTOLITE_AES_BLOCK_SIZE];

    /** \endcond */
} cy_stc_cryptolite_aes_ccm_buffers_t;

/** \cond INTERNAL */
/** The cryptolite AES128 IP descriptor structure.
* All fields for the structure are internal. Firmware never reads or
* writes these values.
*/
typedef struct 
{
   uint32_t key;
   uint32_t plainText;
   uint32_t cipherText;
} cy_stc_cryptolite_aes_descr_t;
/** \endcond */

/** The structure for storing the AES context.
* All fields for the context structure are internal. Firmware never reads or
* writes these values. Firmware allocates the structure and provides the
* address of the structure to the driver in the function calls. Firmware must
* ensure that the defined instance of this structure remains in scope
* while the drive is in use.
*/
typedef struct
{
    /** \cond INTERNAL */
    /** AES state data */
        /** Pointer to AES work buffers */
    cy_stc_cryptolite_aes_buffers_t *buffers;
    /** Operation data descriptors */
    cy_stc_cryptolite_aes_descr_t message_encrypt_struct;
    /** AES unprocessed message block*/
    uint32_t unProcessedBytes;
    /** AES ivSize*/
    uint16_t ivSize;
    /** Is MAC*/
    bool isMac;
    /** AES mode*/
    cy_en_cryptolite_dir_mode_t dirMode;
    /** \endcond */
} cy_stc_cryptolite_aes_state_t;

typedef struct
{
    /** \cond INTERNAL */
    /** AES state data */
    cy_en_cryptolite_dir_mode_t dirMode;
    /** Pointer to AES work buffers */
    cy_stc_cryptolite_aes_state_t aesCbcMacState;
    /** Operation data descriptors */
    cy_stc_cryptolite_aes_state_t aesCtrState;
    /** pointer to temp buffer */
    uint8_t *temp;
    /** pointer to ctr buffer */
    uint8_t *ctr;
    /** pointer to y buffer */
    uint8_t *y;        
    /** Length field Size*/
    uint32_t L;
    /** Total text size*/
    uint32_t textLength;
    /** Total AAD size*/
    uint32_t aadLength;
    /** Total tag size*/
    uint8_t tagLength;
    /** AAD size processed*/
    uint32_t aadLengthProcessed;
    /** AAD processed flag*/
    bool isAadProcessed;
    /** IV set flag*/
    bool isIvSet;
    /** Length Set flag*/
    bool isLengthSet;
    /** \endcond */
} cy_stc_cryptolite_aes_ccm_state_t;

/** \} group_cryptolite_data_structures */


/**
* \addtogroup group_cryptolite_lld_aes_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_ProcessBlock
****************************************************************************//**
*
* Performs the AES block cipher.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* the pointer to the cy_stc_cryptolite_aes_state_t structure that stores all
* internal variables for Cryptolite driver.
*
* \param dstBlock
* The pointer to the cipher text.
*
* \param srcBlock
* The pointer to the plain text. Must be 4-Byte aligned!
*
* \return
* \ref cy_en_cryptolite_status_t
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_ProcessBlock(CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_aes_state_t *aesState,
                            uint32_t *dstBlock,
                            uint32_t const *srcBlock);



/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Init
****************************************************************************//**
*
* Sets AES key and the buffers.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param key
* The pointer to the encryption/decryption key. The key must align in 4 byte boundary.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \param aesBuffers
* The pointer to the memory buffers storage.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Aes_Ecb
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Init(CRYPTOLITE_Type *base,
                                                 uint8_t const *key,
                                                 cy_stc_cryptolite_aes_state_t *aesState,
                                                 cy_stc_cryptolite_aes_buffers_t *aesBuffers);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Free
****************************************************************************//**
*
* Clears AES operation context.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Aes_Ecb
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Free(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ecb
****************************************************************************//**
*
* Performs an AES operation on one block.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Aes_Ecb
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ecb(CRYPTOLITE_Type *base,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ecb_Setup
****************************************************************************//**
*
* Performs an AES ECB setup operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
******************************************************************************/

cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ecb_Setup(CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ecb_Update
****************************************************************************//**
*
* Performs an AES ECB Multistage update operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param dstLength
* The size of the calculated cipher block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ecb_Update(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            uint32_t *dstLength,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ecb_Finish
****************************************************************************//**
*
* Performs an AES ECB finish operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ecb_Finish(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *aesState);


#if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC)
/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cbc
****************************************************************************//**
*
* Performs AES operation on a plain text with Cipher Block Chaining (CBC).
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of the source plain text.
*
* \param ivPtr
* The pointer to the initial vector.
*
* \param dst
* The pointer to a destination cipher text.
*
* \param src
* The pointer to a source plain text.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Aes_Cbc
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cbc(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *ivPtr,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cbc_Setup
****************************************************************************//**
*
* Performs an AES CBC setup operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cbc_Setup(CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cbc_Set_IV
****************************************************************************//**
*
* Function to set AES CBC IV.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param iv
* The pointer to the IV.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cbc_Set_IV(CRYPTOLITE_Type *base,
                                            uint8_t const * iv,
                                            cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cbc_Update
****************************************************************************//**
*
* Performs an AES CBC Multistage update operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param dstLength
* The size of the calculated dst block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cbc_Update(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            uint32_t *dstLength,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cbc_Finish
****************************************************************************//**
*
* Performs an AES CBC finish operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cbc_Finish(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *aesState);



#if defined(CY_CRYPTOLITE_CFG_CBC_MAC_C)

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_CbcMac_Setup
****************************************************************************//**
*
* Performs an AES CBC MAC setup operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_CbcMac_Setup(CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_CbcMac_Update
****************************************************************************//**
*
* Performs an AES CBC MAC Multistage update operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of the source block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_CbcMac_Update(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint8_t const *src,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_CbcMac_Finish
****************************************************************************//**
*
* Performs an AES CBC MAC finish operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param mac
* The pointer to store the calculated mac.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_CbcMac_Finish(CRYPTOLITE_Type *base, uint8_t * mac, cy_stc_cryptolite_aes_state_t *aesState);                                                                                 
#endif /*CY_CRYPTOLITE_CFG_CBC_MAC_C*/

#endif /*CY_CRYPTOLITE_CFG_CIPHER_MODE_CBC*/



#if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CFB)
/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cfb
********************************************************************************//**
*
* Performs AES operation on a plain text with the Cipher Feedback Block method (CFB).
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param dirMode
* \ref cy_en_cryptolite_dir_mode_t
*
* \param srcSize
* The size of the source plain text.
*
* \param ivPtr
* The pointer to the initial vector.
*
* \param dst
* The pointer to a destination cipher text.
*
* \param src
* The pointer to a source plain text.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user must
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Aes_Cfb
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cfb(CRYPTOLITE_Type *base,
                                             cy_en_cryptolite_dir_mode_t dirMode,
                                             uint32_t srcSize,
                                             uint8_t *ivPtr,
                                             uint8_t *dst,
                                             uint8_t const *src,
                                             cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cfb_Setup
****************************************************************************//**
*
* Performs an AES CFB setup operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param dirMode
* Can be \ref CY_CRYPTOLITE_ENCRYPT or \ref CY_CRYPTO_DECRYPT
* (\ref cy_en_cryptolite_dir_mode_t).
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                             
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cfb_Setup(CRYPTOLITE_Type *base,
                                            cy_en_cryptolite_dir_mode_t dirMode,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cfb_Set_IV
****************************************************************************//**
*
* Sets IV for AES CFB mode.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param iv
* The pointer to iv.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cfb_Set_IV(CRYPTOLITE_Type *base,
                                            uint8_t const * iv,
                                            cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cfb_Update
****************************************************************************//**
*
* Performs an AES CFB Multistage update operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cfb_Update(CRYPTOLITE_Type *base,
                                             uint32_t srcSize,
                                             uint8_t *dst,
                                             uint8_t const *src,
                                             cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Cfb_Finish
****************************************************************************//**
*
* Performs an AES CFB finish operation.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                             
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Cfb_Finish(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *aesState);

#endif /*CY_CRYPTOLITE_CFG_CIPHER_MODE_CFB*/


#if defined(CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR)
/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ctr
********************************************************************************//**
*
* Performs an AES operation on a plain text using the counter method (CTR).
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of a source plain text.
*
* \param srcOffset
* The size of an offset within the current block stream for resuming within the
* current cipher stream.
*
* \param ivPtr
* The 128-bit initial vector that contains a 64-bit nonce and 64-bit counter.
*
* \param dst
* The pointer to a destination cipher text.
*
* \param src
* The pointer to a source plain text. Must be 4-Byte aligned.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user must
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Aes_Ctr
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ctr(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint32_t *srcOffset,
                                            uint8_t  *ivPtr,
                                            uint8_t  *dst,
                                            uint8_t  const *src,
                                            cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ctr_Setup
****************************************************************************//**
*
* Performs an AES CTR setup operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_crypto_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ctr_Setup(CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ctr_Set_IV
****************************************************************************//**
*
* Sets IV for the AES CTR operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param iv
* The pointer to iv.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/                                            

cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ctr_Set_IV(CRYPTOLITE_Type *base,
                                            const uint8_t *iv,
                                            cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ctr_Update
****************************************************************************//**
*
* Performs an AES CTR Multistage update operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of the source block.
*
* \param dst
* The pointer to a destination cipher block.
*
* \param src
* The pointer to a source block.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ctr_Update(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint8_t *dst,
                                            uint8_t const *src,
                                            cy_stc_cryptolite_aes_state_t *aesState);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ctr_Finish
****************************************************************************//**
*
* Performs an AES CTR Finish operation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ctr_Finish(CRYPTOLITE_Type *base, cy_stc_cryptolite_aes_state_t *aesState);

/*******************************************************************************
* Function Name: Cy_Cryptolite_Aes_Ctr_Zeropad
********************************************************************************//**
*
* Performs an AES operation on a plain text using the counter method (CTR).
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param srcSize
* The size of a source plain text.
*
* \param ivPtr
* The 128-bit initial vector that contains a 64-bit nonce and 64-bit counter.
*
* \param dst
* The pointer to a destination cipher text.
*
* \param src
* The pointer to a source plain text. Must be 4-Byte aligned.
*
* \param aesState
* The pointer to the AES state structure allocated by the user. The user must
* must not modify anything in this structure.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Aes_Ctr_Zeropad(CRYPTOLITE_Type *base,
                                            uint32_t srcSize,
                                            uint8_t  *ivPtr,
                                            uint8_t  *dst,
                                            uint8_t  const *src,
                                            cy_stc_cryptolite_aes_state_t *aesState);


#endif /*CY_CRYPTOLITE_CFG_CIPHER_MODE_CTR*/

#endif /* #if (CY_CRYPTOLITE_CFG_AES_C)*/
#endif /* #if CRYPTOLITE_AES_PRESENT*/

/** \} group_cryptolite_lld_aes_functions */
#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_AES_H) */

/* [] END OF FILE */
