/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2013 ARM Limited. All rights reserved.    
*    
* $Date:        17. January 2013
* $Revision: 	V1.4.1
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_add_q31.c    
*    
* Description:	Q31 vector addition.    
*    
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*  
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the 
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.  
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**    
 * @ingroup groupMath    
 */

/**    
 * @addtogroup BasicAdd    
 * @{    
 */


/**    
 * @brief Q31 vector addition.    
 * @param[in]       *pSrcA points to the first input vector    
 * @param[in]       *pSrcB points to the second input vector    
 * @param[out]      *pDst points to the output vector    
 * @param[in]       blockSize number of samples in each vector    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * The function uses saturating arithmetic.    
 * Results outside of the allowable Q31 range[0x80000000 0x7FFFFFFF] will be saturated.    
 */

void arm_add_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0_FAMILY

/* Run the below code for Cortex-M4 and Cortex-M3 */
  q31_t inA1, inA2, inA3, inA4;
  q31_t inB1, inB2, inB3, inB4;

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.    
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */
    inA1 = *pSrcA++;
    inA2 = *pSrcA++;
    inB1 = *pSrcB++;
    inB2 = *pSrcB++;

    inA3 = *pSrcA++;
    inA4 = *pSrcA++;
    inB3 = *pSrcB++;
    inB4 = *pSrcB++;

    *pDst++ = __QADD(inA1, inB1);
    *pDst++ = __QADD(inA2, inB2);
    *pDst++ = __QADD(inA3, inB3);
    *pDst++ = __QADD(inA4, inB4);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.    
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */
    *pDst++ = __QADD(*pSrcA++, *pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */



  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */
    *pDst++ = (q31_t) clip_q63_to_q31((q63_t) * pSrcA++ + *pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }

#endif /* #ifndef ARM_MATH_CM0_FAMILY */

}

/**    
 * @} end of BasicAdd group    
 */
