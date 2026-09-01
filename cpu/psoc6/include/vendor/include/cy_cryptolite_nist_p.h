/***************************************************************************//**
* \file cy_cryptolite_nist_p.h
* \version 2.80
*
* \brief
*  This file provides constant and parameters
*  for the API of the ECC PDL in the Cryptolite driver.
*
********************************************************************************
* Copyright 2020-2021 Cypress Semiconductor Corporation
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

#if !defined (CY_CRYPTOLITE_NIST_P_H)
#define CY_CRYPTOLITE_NIST_P_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"
#include "cy_cryptolite_vu.h"
#include "cy_cryptolite_ecdsa.h"

#if (CRYPTOLITE_VU_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_ECP_C)

void Cy_Cryptolite_EC_Bar_MulRed ( CRYPTOLITE_Type *base,
                                cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                uint8_t* p_z,
                                uint8_t* p_x,
                                uint32_t bit_size );
                                
void Cy_Cryptolite_EC_NistP_PointMul(CRYPTOLITE_Type *base, cy_stc_cryptolite_context_ecdsa_t *cfContext, uint8_t *p_x, uint8_t *p_y,
                                uint8_t *p_d, uint8_t *p_order, int bitsize);

void Cy_Cryptolite_EC_DivMod( CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                            uint8_t * z,
                            uint8_t * a,
                            uint8_t * b,
                            int size );

void Cy_Cryptolite_EC_SquareMod( CRYPTOLITE_Type *base,
                        cy_stc_cryptolite_context_ecdsa_t *cfContext,
                        uint8_t * z,
                        uint8_t * a,
                        int size );
void Cy_Cryptolite_EC_MulMod ( CRYPTOLITE_Type *base,
                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                            uint8_t * z,
                            uint8_t * a,
                            uint8_t * b,
                            int size );

//void Cy_Cryptolite_EC_NistP_SetRedAlg(cy_en_cryptolite_ecc_red_mul_algs_t alg);
// void Cy_Cryptolite_EC_NistP_SetMode(uint32_t bitsize);

/// @brief Modular addition in GF(p).
///
/// z = a + b % mod
///
/// Leaf function.
///
/// @param[in] z Register index for sum value.
/// @param[in] a Register index for augend a value.
/// @param[in] b Register index for addend b value.
/// @param[in] mod Register index for modulo value.
__STATIC_INLINE void Cy_Cryptolite_EC_AddMod (CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                            uint8_t *z, uint8_t *a, uint8_t *b)
{
    uint8_t *temp = cfContext->p_buf;//[VU_BITS_TO_BYTES(BIT_SIZE+1)];
    //int sign;
    uint8_t *my_P = cfContext->my_P;
    uint16_t bitsize = (uint16_t)cfContext->bitsize;
    cy_stc_cryptolite_descr_t *vu_struct0 = &cfContext->vu_desptr[0];
    cy_stc_cryptolite_descr_t *vu_struct1 = &cfContext->vu_desptr[1];

    (void)Cy_Cryptolite_Vu_add_hw (base, vu_struct1, temp,VU_BITS_TO_WORDS((uint32_t)bitsize+1U), a,VU_BITS_TO_WORDS((uint32_t)bitsize), b,VU_BITS_TO_WORDS((uint32_t)bitsize));
    (void)Cy_Cryptolite_Vu_cond_sub_hw (base, vu_struct0, z, VU_BITS_TO_WORDS((uint32_t)bitsize), temp, VU_BITS_TO_WORDS((uint32_t)bitsize+1U), my_P, VU_BITS_TO_WORDS((uint32_t)bitsize));
}

/// @brief Modular subtraction in GF(p).
///
/// z = a - b % mod
///
/// Leaf function.
///
/// @param[in] z Register index for difference value.
/// @param[in] a Register index for minuend a value.
/// @param[in] b Register index for subtrahend b value.
/// @param[in] mod Register index for modulo value.

__STATIC_INLINE void Cy_Cryptolite_EC_SubMod (CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                            uint8_t * z, uint8_t * a, uint8_t * b)
{
    int sign;
    //int flag = 0;
    cy_stc_cryptolite_descr_t *vu_struct0 = &cfContext->vu_desptr[0];
    cy_stc_cryptolite_descr_t *vu_struct1 = &cfContext->vu_desptr[1];
    uint8_t *my_P = cfContext->my_P;
    uint16_t bitsize = (uint16_t)cfContext->bitsize;
    uint8_t *temp_z = cfContext->p_buf;//[VU_BITS_TO_BYTES(BIT_SIZE+1)];

    (void)Cy_Cryptolite_Vu_sub_hw (base, vu_struct0, temp_z, VU_BITS_TO_WORDS((uint32_t)bitsize+1U), a, VU_BITS_TO_WORDS((uint32_t)bitsize), b, VU_BITS_TO_WORDS((uint32_t)bitsize));
    (void)Cy_Cryptolite_Vu_mov_hw (base, vu_struct1, z, VU_BITS_TO_WORDS((uint32_t)bitsize), temp_z,VU_BITS_TO_WORDS((uint32_t)bitsize));

    Cy_Cryptolite_Vu_wait_hw(base);
    sign = (int)Cy_Cryptolite_Vu_get_bit(temp_z, bitsize);

    if ((bool)sign)
    {
        (void)Cy_Cryptolite_Vu_add_hw (base, vu_struct0, z, VU_BITS_TO_WORDS((uint32_t)bitsize), z,VU_BITS_TO_WORDS((uint32_t)bitsize), my_P,VU_BITS_TO_WORDS((uint32_t)bitsize));
    }
}

/// @brief Modular halving in GF(p).
///
/// z = a / 2 % mod
///
/// Leaf function.
///
/// @param[in] z Register index for result value.
/// @param[in] a Register index for value to be halved.
/// @param[in] mod Register index for modulo value.
__STATIC_INLINE void Cy_Cryptolite_EC_HalfMod (CRYPTOLITE_Type *base,
                                            cy_stc_cryptolite_context_ecdsa_t *cfContext,
                                            uint8_t * z, uint8_t * a)
{
    bool odd, sign;
    uint8_t *temp_a = cfContext->p_buf;//[VU_BITS_TO_BYTES(BIT_SIZE+1)];
    cy_stc_cryptolite_descr_t *vu_struct0 = &cfContext->vu_desptr[0];
    cy_stc_cryptolite_descr_t *vu_struct1 = &cfContext->vu_desptr[1];
    uint8_t *my_P = cfContext->my_P;
    uint16_t bitsize = (uint16_t)cfContext->bitsize;

    Cy_Cryptolite_Vu_wait_hw(base);
    odd =  !Cy_Cryptolite_Vu_test_even(a);
    sign = 0;

    if (odd)
    {
        (void)Cy_Cryptolite_Vu_add_hw(base, vu_struct0, temp_a, VU_BITS_TO_WORDS((uint32_t)bitsize+1U), a, VU_BITS_TO_WORDS((uint32_t)bitsize), my_P, VU_BITS_TO_WORDS((uint32_t)bitsize));
        (void)Cy_Cryptolite_Vu_add_hw(base, vu_struct1, a, VU_BITS_TO_WORDS((uint32_t)bitsize), a, VU_BITS_TO_WORDS((uint32_t)bitsize), my_P, VU_BITS_TO_WORDS((uint32_t)bitsize));
        sign = (bool)Cy_Cryptolite_Vu_get_bit(temp_a, bitsize);
    }
    (void)Cy_Cryptolite_Vu_lsr1_carry_hw(base, vu_struct0, z, VU_BITS_TO_WORDS((uint32_t)bitsize), a, VU_BITS_TO_WORDS((uint32_t)bitsize), (uint32_t)sign, (uint32_t)bitsize);
}


#endif /* #if (CY_CRYPTOLITE_CFG_ECP_C == 1) */
#endif /* #if (CPUSS_CRYPTOLITE_VU == 1) */
#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */
#endif /* #if !defined (CY_CRYPTOLITE_NIST_P_H) */

/* [] END OF FILE */
