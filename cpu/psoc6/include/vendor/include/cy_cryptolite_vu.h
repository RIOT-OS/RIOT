/***************************************************************************//**
* \file cy_cryptolite_vu.h
* \version 2.80
*
* \brief
*  This file provides provides constant and parameters
*  for the API of the VU in the Cryptolite driver.
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

#if !defined (CY_CRYPTOLITE_VU_H)
#define CY_CRYPTOLITE_VU_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#include "cy_cryptolite_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CRYPTOLITE_VU_PRESENT == 1)

#define VU_BITS_TO_WORDS(b)     (((b)+31U) >> 5)
#define VU_BITS_TO_BYTES(b)     ((4U)*VU_BITS_TO_WORDS(b))
#define VU_BYTES_TO_WORDS(b)    ((b) >> 2)

/* Fix: update it for other sizes */
#define VU_TEST_EQUAL_LESS_SIZE   (521u)

void Cy_Cryptolite_Vu_lsl (uint8_t* p_z,uint32_t word_size_z,uint8_t* p_a, uint32_t word_size_a,uint32_t bit_sh);
void Cy_Cryptolite_Vu_clr (uint8_t* p_z,uint32_t word_size);
bool Cy_Cryptolite_Vu_test_zero(CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *psrc, uint16_t bitsize);
bool Cy_Cryptolite_Vu_test_equal(CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *rsrc0, uint8_t *rsrc1, uint16_t bitsize);
bool Cy_Cryptolite_Vu_test_less_than(CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *rsrc0, uint8_t *rsrc1, uint16_t bitsize);
cy_en_cryptolite_status_t Cy_Cryptolite_Vu_RunInstr(CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t *p_struct);

/*Compares size bytes of src0 to src1*/
__STATIC_INLINE uint32_t Cy_Cryptolite_Vu_memcmp (void const *src0, void const *src1, uint32_t size)
{
    uint32_t i,diff;
    uint8_t *s0,*s1;

    s0 = (uint8_t*)src0;
    s1 = (uint8_t*)src1;
    diff = 0;

    for(i=0; i < size; i++)
    {
        diff |= (uint32_t)s0[i] ^ (uint32_t)s1[i];
    }

    if((bool)diff){
        return 1;
    }else{
        return 0;
    }
}


/*Copy the buffer src to dest of size bytes*/
__STATIC_INLINE void Cy_Cryptolite_Vu_memcpy (void  *dest, const void  *src, uint32_t size)
{
   uint32_t i;
   uint8_t  *dest_P = (uint8_t *)dest;
   uint8_t  *src_P = (uint8_t *)src;

   for(i=0u; i < size; i++)
   {
      dest_P[i] = src_P[i];
   }

}

/*Set the buffer dest to data of size bytes*/
__STATIC_INLINE void Cy_Cryptolite_Vu_memset (void  *dest, uint8_t data, uint32_t size)
{
   uint32_t i;
   uint8_t  *dest_P = (uint8_t *)dest;

   for(i=0u; i < size; i++)
   {
      dest_P[i] = data;
   }

}

/*Tests for input word to be even or  Odd*/
__STATIC_INLINE bool Cy_Cryptolite_Vu_test_even ( uint8_t* p_a)
{
   CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 11.3','Intentional pointer type conversion');
   uint32_t* p_a_uint32 = (uint32_t *) p_a;
   uint32_t  word  = *p_a_uint32;
   
   return ((word & 1U) == 0U);
}
/*
 Clears a bit in the specified position in a word
 bit_pos" must be within "p_a" size.
*/
__STATIC_INLINE void Cy_Cryptolite_Vu_clr_bit ( uint8_t* p_z, uint32_t bit_pos )
{
   uint32_t byte_offset = bit_pos >> 3U;
   uint32_t bit_offset  = bit_pos & 7U;

   p_z[byte_offset] = p_z[byte_offset] & ~(1U << bit_offset);
}

/*
 sets a bit in the specified position in a word
 "pos" must be within "p_a" size.
*/
__STATIC_INLINE void Cy_Cryptolite_Vu_set_bit (uint8_t* p_z, uint32_t bit_pos )
{
   uint32_t byte_offset = bit_pos >> 3U;
   uint32_t bit_offset  = bit_pos & 7U;

   p_z[byte_offset] = p_z[byte_offset] | (1U << bit_offset);
}
/*
 returns a bit from the specified position in a word
 "pos" must be within "p_a" size.
*/
__STATIC_INLINE uint32_t Cy_Cryptolite_Vu_get_bit ( uint8_t* p_a, uint32_t bit_pos )
{
   uint32_t byte_offset = bit_pos >> 3U;
   uint32_t bit_offset  = bit_pos & 7U;

   return ((uint32_t)p_a[byte_offset] >> bit_offset) & 1U;
}

/*wait till IP is idle*/
__STATIC_INLINE void Cy_Cryptolite_Vu_wait_hw (CRYPTOLITE_Type *base)
{
   while ((bool)(REG_CRYPTOLITE_STATUS(base) & CRYPTOLITE_STATUS_BUSY_Msk)){};
}


/**
* \addtogroup group_cryptolite_lld_vu_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_mul_hw
****************************************************************************//**
*
* Performs long integer multiplication operation.
* 
* \note  The pointers p_a, p_b & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the first operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.

* \param p_b
* The pointer to the second operand buffer.
*
* \param word_size_b
* The size of the p_b buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_mul_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_mul_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a,
                                            uint8_t* p_b,
                                            uint32_t word_size_b)
{
   p_struct->data0 = (uint32_t) ((uint32_t)0 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_b-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_b;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_xmul_hw
****************************************************************************//**
*
* Performs carry less long integer multiplication operation. 
* \note  The pointers p_a, p_b & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the first operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.

* \param p_b
* The pointer to the second operand buffer.
*
* \param word_size_b
* The size of the p_b buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_xmul_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_xmul_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a,
                                            uint8_t* p_b,
                                            uint32_t word_size_b)
{
   p_struct->data0 = (uint32_t) ((uint32_t)4 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_b-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_b;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}



/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_add_hw
****************************************************************************//**
*
* Performs long integer addition operation. 
* \note  The pointers p_a, p_b & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the first operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.

* \param p_b
* The pointer to the second operand buffer.
*
* \param word_size_b
* The size of the p_b buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_add_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_add_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t *p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a,
                                            uint8_t* p_b,
                                            uint32_t word_size_b)
{
   p_struct->data0 = (uint32_t) ((uint32_t)1 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_b-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_b;
   p_struct->data3 = (uint32_t) p_z;
   /*run instruction*/
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}


/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_sub_hw
****************************************************************************//**
*
* Performs long integer subtraction operation.
* \note  The pointers p_a, p_b & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the first operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.

* \param p_b
* The pointer to the second operand buffer.
*
* \param word_size_b
* The size of the p_b buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_sub_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_sub_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t *p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a,
                                            uint8_t* p_b,
                                            uint32_t word_size_b)
{
   p_struct->data0 = (uint32_t) ((uint32_t)2 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_b-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_b;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}



/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_mov_hw
****************************************************************************//**
*
* Performs long integer move operation.
* \note  The pointers p_a & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_mov_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_mov_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t *p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a)
{
   p_struct->data0 = (uint32_t) (9UL << 28U) | ((word_size_z-1U) << 16U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}




/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_lsl1_hw
****************************************************************************//**
*
* Performs left shifting of long integer by one bit.
* \note  The pointers p_a & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_lsl1_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_lsl1_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a)
{
   p_struct->data0 = (uint32_t) ((uint32_t)6 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_a-1U) << 8U)| (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data3 = (uint32_t) p_z;

   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);


}




/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_lsr1_hw
****************************************************************************//**
*
* Performs right shifting of long integer by one bit.
* \note  The pointers p_a & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_lsr1_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_lsr1_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *p_z,
                                            uint32_t word_size_z,
                                            uint8_t *p_a,
                                            uint32_t word_size_a)
{
   p_struct->data0 = (uint32_t) ((uint32_t)5 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_a-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}


/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_lsr_hw
****************************************************************************//**
*
* Performs right shifting of long integer by shift bits.
* \note  The pointers p_a & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.
*
* \param shift
* The number of bits to shift right.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_lsr_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_lsr_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a,
                                            uint32_t shift)
{
   p_struct->data0 = (uint32_t) ((uint32_t)7 << 28U) | ((word_size_z-1U) << 16U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = shift;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_xor_hw
****************************************************************************//**
*
* Performs long integer exclusive-or operation.
* \note  The pointers p_a, p_b & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the first operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.

* \param p_b
* The pointer to the second operand buffer.
*
* \param word_size_b
* The size of the p_b buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Vu_xor_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_xor_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t* p_z,
                                            uint32_t word_size_z,
                                            uint8_t* p_a,
                                            uint32_t word_size_a,
                                            uint8_t* p_b,
                                            uint32_t word_size_b)
{
   p_struct->data0 = (uint32_t) ((uint32_t)3 << 28U) | ((word_size_z-1U) << 16U) | ((word_size_b-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_b;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}



/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_cond_sub_hw
****************************************************************************//**
*
* Performs long integer conditional subtraction operation.
* \note  The pointers p_a, p_b & p_z must be 4 byte aligned and end in 4 byte boundary.
* \note  User should use the function CY_REMAP_ADDRESS_CRYPTOLITE() to get the remapped address and use it for all the VU operations.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the first operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.

* \param p_b
* The pointer to the second operand buffer.
*
* \param word_size_b
* The size of the p_b buffer in word size.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_VU_cond_sub_hw
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_cond_sub_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *p_z,
                                            uint32_t word_size_z,
                                            uint8_t *p_a,
                                            uint32_t word_size_a,
                                            uint8_t *p_b,
                                            uint32_t word_size_b)
{
   p_struct->data0 = (uint32_t) (8UL << 28U) | ((word_size_z-1U) << 16U) | ((word_size_b-1U) << 8U) | (word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_b;
   p_struct->data3 = (uint32_t) p_z;
   return Cy_Cryptolite_Vu_RunInstr(base, p_struct);
}

/** \} group_cryptolite_lld_vu_functions */


/*******************************************************************************
* Function Name: Cy_Cryptolite_Vu_lsr1_carry_hw
****************************************************************************//**
*
* Performs right shifting of long integer by shift bits with carry.
* \note  The pointers p_a & p_z must be 4 byte aligned and end in 4 byte boundary.
*
* \param base
* The pointer to a Cryptolite instance.
*
* \param p_struct
* The pointer to the cy_stc_cryptolite_descr_t.
*
* \param p_z
* The buffer pointer to the store the output.
*
* \param word_size_z
* The size of the p_z buffer in word size.
*
* \param p_a
* The pointer to the operand buffer.
*
* \param word_size_a
* The size of the p_a buffer in word size.
*
* \param carry
* To set carry value to the bit.
*
* \param bitsize
* The size of the p_z buffer in bits.
*
* \return status code. See \ref cy_en_cryptolite_status_t.
*
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Vu_lsr1_carry_hw (CRYPTOLITE_Type *base, cy_stc_cryptolite_descr_t* p_struct, uint8_t *p_z,
                                            uint32_t word_size_z,
                                            uint8_t *p_a,
                                            uint32_t word_size_a,
                                            uint32_t carry,
                                            uint32_t bitsize)
{
   cy_en_cryptolite_status_t status;
   p_struct->data0 = (uint32_t) ((uint32_t)5 << 28U) | ((word_size_z-1U) << 16U) |  ((word_size_a-1U) << 8U) |(word_size_a-1U);
   p_struct->data1 = (uint32_t) p_a;
   p_struct->data2 = (uint32_t) p_a;
   p_struct->data3 = (uint32_t) p_z;

   status = Cy_Cryptolite_Vu_RunInstr(base, p_struct);
   Cy_Cryptolite_Vu_wait_hw(base);
   if(CY_CRYPTOLITE_SUCCESS !=status)
   {
      return status;
   }
   if((bool)carry)
   {
       Cy_Cryptolite_Vu_set_bit(p_z, bitsize-1U);
   }

   return status;
}


#endif /* #if (CPUSS_CRYPTOLITE_VU == 1) */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_VU_H) */

/* [] END OF FILE */
