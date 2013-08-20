/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2013 ARM Limited. All rights reserved.    
*    
* $Date:        17. January 2013 
* $Revision: 	V1.4.1
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_mat_sub_q31.c    
*    
* Description:	Q31 matrix subtraction    
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
 * @ingroup groupMatrix        
 */

/**        
 * @addtogroup MatrixSub        
 * @{        
 */

/**        
 * @brief Q31 matrix subtraction.        
 * @param[in]       *pSrcA points to the first input matrix structure        
 * @param[in]       *pSrcB points to the second input matrix structure        
 * @param[out]      *pDst points to output matrix structure        
 * @return     		The function returns either        
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.        
 *        
 * <b>Scaling and Overflow Behavior:</b>        
 * \par        
 * The function uses saturating arithmetic.        
 * Results outside of the allowable Q31 range [0x80000000 0x7FFFFFFF] will be saturated.        
 */


arm_status arm_mat_sub_q31(
  const arm_matrix_instance_q31 * pSrcA,
  const arm_matrix_instance_q31 * pSrcB,
  arm_matrix_instance_q31 * pDst)
{
  q31_t *pIn1 = pSrcA->pData;                    /* input data matrix pointer A */
  q31_t *pIn2 = pSrcB->pData;                    /* input data matrix pointer B */
  q31_t *pOut = pDst->pData;                     /* output data matrix pointer */
  q31_t inA1, inB1;                              /* temporary variables */

#ifndef ARM_MATH_CM0_FAMILY

  q31_t inA2, inB2;                              /* temporary variables */
  q31_t out1, out2;                              /* temporary variables */

#endif //      #ifndef ARM_MATH_CM0_FAMILY

  uint32_t numSamples;                           /* total number of elements in the matrix  */
  uint32_t blkCnt;                               /* loop counters */
  arm_status status;                             /* status of matrix subtraction */


#ifdef ARM_MATH_MATRIX_CHECK
  /* Check for matrix mismatch condition  */
  if((pSrcA->numRows != pSrcB->numRows) ||
     (pSrcA->numCols != pSrcB->numCols) ||
     (pSrcA->numRows != pDst->numRows) || (pSrcA->numCols != pDst->numCols))
  {
    /* Set status as ARM_MATH_SIZE_MISMATCH */
    status = ARM_MATH_SIZE_MISMATCH;
  }
  else
#endif
  {
    /* Total number of samples in the input matrix */
    numSamples = (uint32_t) pSrcA->numRows * pSrcA->numCols;

#ifndef ARM_MATH_CM0_FAMILY

    /* Run the below code for Cortex-M4 and Cortex-M3 */

    /* Loop Unrolling */
    blkCnt = numSamples >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.    
     ** a second loop below computes the remaining 1 to 3 samples. */
    while(blkCnt > 0u)
    {
      /* C(m,n) = A(m,n) - B(m,n) */
      /* Subtract, saturate and then store the results in the destination buffer. */
      /* Read values from source A */
      inA1 = pIn1[0];

      /* Read values from source B */
      inB1 = pIn2[0];

      /* Read values from source A */
      inA2 = pIn1[1];

      /* Subtract and saturate */
      out1 = __QSUB(inA1, inB1);

      /* Read values from source B */
      inB2 = pIn2[1];

      /* Read values from source A */
      inA1 = pIn1[2];

      /* Subtract and saturate */
      out2 = __QSUB(inA2, inB2);

      /* Read values from source B */
      inB1 = pIn2[2];

      /* Store result in destination */
      pOut[0] = out1;
      pOut[1] = out2;

      /* Read values from source A */
      inA2 = pIn1[3];

      /* Read values from source B */
      inB2 = pIn2[3];

      /* Subtract and saturate */
      out1 = __QSUB(inA1, inB1);

      /* Subtract and saturate */
      out2 = __QSUB(inA2, inB2);

      /* Store result in destination */
      pOut[2] = out1;
      pOut[3] = out2;

      /* update pointers to process next samples */
      pIn1 += 4u;
      pIn2 += 4u;
      pOut += 4u;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* If the numSamples is not a multiple of 4, compute any remaining output samples here.        
     ** No loop unrolling is used. */
    blkCnt = numSamples % 0x4u;

#else

    /* Run the below code for Cortex-M0 */

    /* Initialize blkCnt with number of samples */
    blkCnt = numSamples;

#endif /* #ifndef ARM_MATH_CM0_FAMILY */

    while(blkCnt > 0u)
    {
      /* C(m,n) = A(m,n) - B(m,n) */
      /* Subtract, saturate and then store the results in the destination buffer. */
      inA1 = *pIn1++;
      inB1 = *pIn2++;

      inA1 = __QSUB(inA1, inB1);

      *pOut++ = inA1;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* Set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);
}

/**        
 * @} end of MatrixSub group        
 */
