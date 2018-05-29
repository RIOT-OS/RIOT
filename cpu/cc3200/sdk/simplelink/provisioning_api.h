/*
 * provisioning_api.h -
 *  This file holds provisioning library external APIs.
 *  The library manages the provisioning sequence for Simple Link device
 *  (cc3100/cc3200). This serves for configuring the device when it has no
 *  access to WiFi network as a STA.
 *  The library is responsible for selecting the provisioning method and
 *  reporting the configuration result to the configuring application.
 *  If the library isn't started, device can still be configured but user shall
 *  not be informed with configuration result.
 *
 *  NOTES:
 *   (1)  Provisioning necessity is defined by user application. The library
 *        does not test profiles existence nor WiFi connection.
 *        If autonomous provisioning activation is enabled, the library shall
 *        monitor the WiFi connection and start provisioning when there's no
 *        connection for a pre-defined timeout. This mode is off course power
 *        consuming and should be used only if there's no physical access to the
 *        device. Otherwise, provisioning should be triggered by user request.
 *   (2)  It's assumed the WiFi device is initialized when the library started.
 *
 * Created on: April 6, 2015
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef PROVISIONING_API_H_
#define PROVISIONING_API_H_

#include "simplelink.h"


/* define the event handlers the prov library want to be notified on */
#define sl_Provisioning_NOTIFY_WLAN_EVENT           TRUE
#define sl_Provisioning_NOTIFY_NETAPP_EVENT         TRUE
#define sl_Provisioning_NOTIFY_HTTP_SERVER_EVENT    TRUE


#define SL_EXT_PROVLIB_VERSION          "1.0.10.5"
#define SL_EXT_PROVLIB_PRODUCT_VERSION  "R1.0"

// Wait Forever
#define SL_EXT_PROV_WAIT_FOREVER		(-1)

// Provisioning State
#define SL_EXT_PROV_SUCCESS				(0)
#define SL_EXT_PROV_ERROR_DEVICE_ERROR	(-1)
#define SL_EXT_PROV_ERROR_NOT_NEEDED	(-2)
#define SL_EXT_PROV_TIMEOUT				(-3)

// Provisioning configuration
typedef struct
{
    _u8 IsBlocking;        // unused

    _u8 AutoStartEnabled;  // 1 = enabled: if STA fails to connect to configured
                            // profiles within timeout, autonomously switch to provisioning.
                            // If library is active and WiFi connection exists
                            // the connection is monitored and in case of long
                            // disconnection provisioning is activated autonomously

    _i8 Timeout10Secs;     // Inactivity maximal time (defined in units of 10 seconds)
                            // -1 defines wait forever

    _u8 ModeAfterFailure;  // Device mode to be set if devices fails to connect
                            // to configured AP

    _u8 ModeAfterTimeout;  // Device mode to be set if device isn't configured
                            // within a predefined time
} slExtLibProvCfg_t;

/*****************************************************************************/
/* Provisioning module APIs                                                  */
/*****************************************************************************/

//*****************************************************************************
//! \brief  Host waiting callback function for provisioning. This timer will
//!         only listen to the event and does not manipulate it. The event
//!         should be set to FALSE before giving to this function. Once the
//!         timer is up or the event becomes TRUE, function returns.
//!         If timeout is set to WAIT_FOREVER, this function will return if,
//!         and only if, event becomes TRUE.
//!
//! \param  event - The event this function listens to. Either TRUE or FALSE.
//! \param  timeout - Timeout, in mSec
//! \return 0 if function finishes because of the event. -1 if timeout.
//*****************************************************************************
extern _i8 sl_extlib_ProvEventTimeoutHdl(_u8* event, _i32 timeout);

//*****************************************************************************
//! \brief  Host waiting callback function for other purposes. This timer does
//!         not listen to any events.
//! \param  timeout - Timeout, in mSec
//! \return none
//*****************************************************************************
extern void sl_extlib_ProvWaitHdl(_i32 timeout);

//*****************************************************************************
//! \brief Starts Provisioning Process with configuration from user application
//!        - if SL is in STA mode and fully connected to a WiFi network,
//!          provisioning is not needed and function returns immediately.
//!
//! \param  current device mode
//! \param  pointer to provisioning configuration
//! \return	SL_PROV_SUCCESS
//!	        SL_PROV_ERROR_DEVICE_ERROR
//!			SL_PROV_ERROR_NOT_NEEDED
//!			SL_PROV_TIMEOUT
//*****************************************************************************
_i32 sl_extlib_ProvisioningStart(_u8 aCurrMode, slExtLibProvCfg_t *apCfg);

#endif /* PROVISIONING_API_H_ */
