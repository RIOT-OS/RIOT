/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2013 ARM Limited. All rights reserved.    
*    
* $Date:        17. January 2013
* $Revision: 	V1.4.1
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_cmplx_mult_real_q31.c    
*    
* Description:	Q31 complex by real multiplication    
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
 * @ingroup groupCmplxMath    
 */

/**    
 * @addtogroup CmplxByRealMult    
 * @{    
 */


/**    
 * @brief  Q31 complex-by-real multiplication    
 * @param[in]  *pSrcCmplx points to the complex input vector    
 * @param[in]  *pSrcReal points to the real input vector    
 * @param[out]  *pCmplxDst points to the complex output vector    
 * @param[in]  numSamples number of samples in each vector    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * The function uses saturating arithmetic.    
 * Results outside of the allowable Q31 range[0x80000000 0x7FFFFFFF] will be saturated.    
 */

void arm_cmplx_mult_real_q31(
  q31_t * pSrcCmplx,
  q31_t * pSrcReal,
  q31_t * pCmplxDst,
  uint32_t numSamples)
{
  q31_t inA1;                                    /* Temporary variable to store input value */

#ifndef ARM_MATH_CM0_FAMILY

  /* Run the below code for Cortex-M4 and Cortex-M3 */
  uint32_t blkCnt;                               /* loop counters */
  q31_t inA2, inA3, inA4;                        /* Temporary variables to hold input data */
  q31_t inB1, inB2;                              /* Temporary variabels to hold input data */
  q31_t out1, out2, out3, out4;                  /* Temporary variables to hold output data */

  /* loop Unrolling */
  blkCnt = numSamples >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.    
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C[2 * i] = A[2 * i] * B[i].            */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i].        */
    /* read real input from complex input buffer */
    inA1 = *pSrcCmplx++;
    inA2 = *pSrcCmplx++;
    /* read input from real input bufer */
    inB1 = *pSrcReal++;
    inB2 = *pSrcReal++;
    /* read imaginary input from complex input buffer */
    inA3 = *pSrcCmplx++;
    inA4 = *pSrcCmplx++;

    /* multiply complex input with real input */
    out1 = ((q63_t) inA1 * inB1) >> 32;
    out2 = ((q63_t) inA2 * inB1) >> 32;
    out3 = ((q63_t) inA3 * inB2) >> 32;
    out4 = ((q63_t) inA4 * inB2) >> 32;

    /* sature the result */
    out1 = __SSAT(out1, 31);
    out2 = __SSAT(out2, 31);
    out3 = __SSAT(out3, 31);
    out4 = __SSAT(out4, 31);

    /* get result in 1.31 format */
    out1 = out1 << 1;
    out2 = out2 << 1;
    out3 = out3 << 1;
    out4 = out4 << 1;

    /* store the result to destination buffer */
    *pCmplxDst++ = out1;
    *pCmplxDst++ = out2;
    *pCmplxDst++ = out3;
    *pCmplxDst++ = out4;

    /* read real input from complex input buffer */
    inA1 = *pSrcCmplx++;
    inA2 = *pSrcCmplx++;
    /* read input from real input bufer */
    inB1 = *pSrcReal++;
    inB2 = *pSrcReal++;
    /* read imaginary input from complex input buffer */
    inA3 = *pSrcCmplx++;
    inA4 = *pSrcCmplx++;

    /* multiply complex input with real input */
    out1 = ((q63_t) inA1 * inB1) >> 32;
    out2 = ((q63_t) inA2 * inB1) >> 32;
    out3 = ((q63_t) inA3 * inB2) >> 32;
    out4 = ((q63_t) inA4 * inB2) >> 32;

    /* sature the result */
    out1 = __SSAT(out1, 31);
    out2 = __SSAT(out2, 31);
    out3 = __SSAT(out3, 31);
    out4 = __SSAT(out4, 31);

    /* get result in 1.31 format */
    out1 = out1 << 1;
    out2 = out2 << 1;
    out3 = out3 << 1;
    out4 = out4 << 1;

    /* store the result to destination buffer */
    *pCmplxDst++ = out1;
    *pCmplxDst++ = out2;
    *pCmplxDst++ = out3;
    *pCmplxDst++ = out4;

    /* Decrement the numSamples loop counter */
    blkCnt--;
  }

  /* If the numSamples is not a multiple of 4, compute any remaining output samples here.    
   ** No loop unrolling is used. */
  blkCnt = numSamples % 0x4u;

  while(blkCnt > 0u)
  {
    /* C[2 * i] = A[2 * i] * B[i].            */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i].        */
    /* read real input from complex input buffer */
    inA1 = *pSrcCmplx++;
    inA2 = *pSrcCmplx++;
    /* read input from real input bufer */
    inB1 = *pSrcReal++;

    /* multiply complex input with real input */
    out1 = ((q63_t) inA1 * inB1) >> 32;
    out2 = ((q63_t) inA2 * inB1) >> 32;

    /* sature the result */
    out1 = __SSAT(out1, 31);
    out2 = __SSAT(out2, 31);

    /* get result in 1.31 format */
    out1 = out1 << 1;
    out2 = out2 << 1;

    /* store the result to destination buffer */
    *pCmplxDst++ = out1;
    *pCmplxDst++ = out2;

    /* Decrement the numSamples loop counter */
    blkCnt--;
  }

#else

  /* Run the below code for Cortex-M0 */

  while(numSamples > 0u)
  {
    /* realOut = realA * realB.            */
    /* imagReal = imagA * realB.               */
    inA1 = *pSrcReal++;
    /* store the result in the destination buffer. */
    *pCmplxDst++ =
      (q31_t) clip_q63_to_q31(((q63_t) * pSrcCmplx++ * inA1) >> 31);
    *pCmplxDst++ =
      (q31_t) clip_q63_to_q31(((q63_t) * pSrcCmplx++ * inA1) >> 31);

    /* Decrement the numSamples loop counter */
    numSamples--;
  }

#endif /* #ifndef ARM_MATH_CM0_FAMILY */

}

/**    
 * @} end of CmplxByRealMult group    
 */
