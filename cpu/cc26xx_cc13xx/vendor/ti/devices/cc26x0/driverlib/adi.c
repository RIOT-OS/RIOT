/******************************************************************************
*  Filename:       adi.c
*  Revised:        2016-11-17 16:39:28 +0100 (Thu, 17 Nov 2016)
*  Revision:       47706
*
*  Description:    Driver for the ADI interface
*
*  Copyright (c) 2015 - 2017, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include "../inc/hw_types.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_aux_smph.h"
#include "adi.h"
#include "cpu.h"


//*****************************************************************************
//
// SafeHapiVoid() and SafeHapiAuxAdiSelect()
// Common wrapper functions for the Hapi functions needing workaround for the
// "bus arbitration" issue.
//
//*****************************************************************************
void SafeHapiVoid( FPTR_VOID_VOID_T fPtr )
{
    bool bIrqEnabled = ( ! CPUcpsid() );
    while ( ! HWREG( AUX_SMPH_BASE + AUX_SMPH_O_SMPH0 ));
    fPtr();
    HWREG( AUX_SMPH_BASE + AUX_SMPH_O_SMPH0 ) = 1;
    if ( bIrqEnabled ) {
        CPUcpsie();
    }
}

void SafeHapiAuxAdiSelect( FPTR_VOID_UINT8_T fPtr, uint8_t ut8Signal )
{
    bool bIrqEnabled = ( ! CPUcpsid() );
    while ( ! HWREG( AUX_SMPH_BASE + AUX_SMPH_O_SMPH0 ));
    fPtr( ut8Signal );
    HWREG( AUX_SMPH_BASE + AUX_SMPH_O_SMPH0 ) = 1;
    if ( bIrqEnabled ) {
        CPUcpsie();
    }
}
