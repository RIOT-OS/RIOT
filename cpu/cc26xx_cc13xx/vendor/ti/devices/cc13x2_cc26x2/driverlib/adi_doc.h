/******************************************************************************
*  Filename:       adi_doc.h
*  Revised:        2016-03-30 13:03:59 +0200 (Wed, 30 Mar 2016)
*  Revision:       45971
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
//! \addtogroup adi_api
//! @{
//! \section sec_adi Introduction
//! \n
//!
//! \section sec_adi_api API
//!
//! The API functions can be grouped like this:
//!
//! Write:
//! - Direct (all bits):
//!   - \ref ADI8RegWrite()
//!   - \ref ADI16RegWrite()
//!   - \ref ADI32RegWrite()
//! - Set individual bits:
//!   - \ref ADI8BitsSet()
//!   - \ref ADI16BitsSet()
//!   - \ref ADI32BitsSet()
//! - Clear individual bits:
//!   - \ref ADI8BitsClear()
//!   - \ref ADI16BitsClear()
//!   - \ref ADI32BitsClear()
//! - Masked:
//!   - \ref ADI4SetValBit()
//!   - \ref ADI8SetValBit()
//!   - \ref ADI16SetValBit()
//!
//! Read:
//! - \ref ADI8RegRead()
//! - \ref ADI16RegRead()
//! - \ref ADI32RegRead()
//!
//! @}
