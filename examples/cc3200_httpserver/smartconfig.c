//*****************************************************************************
// smartconfig.c - APIs for SmartConfig
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//simplelink include
#include "simplelink.h"

//common interface include
#include "common.h"

#ifdef ORIGINAL_EXAMPLE

#ifndef NOTERM
#include "uart_if.h"
#endif

#endif

#define WLAN_DEL_ALL_PROFILES        0xff

//*****************************************************************************
//
//!   \brief Connecting to a WLAN Accesspoint using SmartConfig provisioning
//!
//!    This function enables SmartConfig provisioning for adding a new
//!    connection profile to CC3200. Since we have set the connection policy
//!    to Auto, once SmartConfig is complete,CC3200 will connect
//!    automatically to the new connection profile added by smartConfig.
//!
//!   \param[in]               None
//!
//!   \return  0 - Success
//!            -1 - Failure
//!
//!   \warning           If the WLAN connection fails or we don't acquire an
//!                      IP address,We will be stuck in this function forever.
//!
//*****************************************************************************
long SmartConfigConnect() {
    unsigned char policyVal;

    long lRetVal;

    //
    // Clear all profiles
    // This is of course not a must, it is used in this example to make sure
    // we will connect to the new profile added by SmartConfig
    //
    lRetVal = sl_WlanProfileDel(WLAN_DEL_ALL_PROFILES);
    ASSERT_ON_ERROR(lRetVal);

    //set AUTO policy
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 0), &policyVal,
            1 /*PolicyValLen*/);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Start SmartConfig
    // This example uses the unsecured SmartConfig method
    //
    lRetVal = sl_WlanSmartConfigStart(0, /*groupIdBitmask*/
    SMART_CONFIG_CIPHER_NONE, /*cipher*/
    0, /*publicKeyLen*/
    0, /*group1KeyLen*/
    0, /*group2KeyLen */
    NULL, /*publicKey */
    NULL, /*group1Key */
    NULL); /*group2Key*/
    ASSERT_ON_ERROR(lRetVal);

    return SUCCESS;
}

//*****************************************************************************
//
//!
//!    \brief Stop SmartConfig provisioning
//!
//!    This function Stops SmartConfig provisioning
//!
//!    \param[in]                   None
//!
//! \return  0 - Success
//!            -1 - Failure
//!
//!   \note
//!
//*****************************************************************************
long SmartConfigStop() {
    long lRetVal;
    lRetVal = sl_WlanSmartConfigStop();
    ASSERT_ON_ERROR(lRetVal);
    return SUCCESS;
}
