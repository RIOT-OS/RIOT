/******************************************************************************
*  Filename:       hw_cctest.h
*  Revised:        $Date: 2013-04-30 17:13:44 +0200 (Tue, 30 Apr 2013) $
*  Revision:       $Revision: 9943 $
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
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
*    documentation and/or other materials provided with the distribution.
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
*
******************************************************************************/

#ifndef __HW_CCTEST_H__
#define __HW_CCTEST_H__

//*****************************************************************************
//
// The following are defines for the CCTEST register offsets.
//
//*****************************************************************************
#define CCTEST_IO               0x44010000  // Output strength control 
#define CCTEST_OBSSEL0          0x44010014  // Select output signal on 
                                            // observation output 0 
#define CCTEST_OBSSEL1          0x44010018  // Select output signal on 
                                            // observation output 1 
#define CCTEST_OBSSEL2          0x4401001C  // Select output signal on 
                                            // observation output 2 
#define CCTEST_OBSSEL3          0x44010020  // Select output signal on 
                                            // observation output 3 
#define CCTEST_OBSSEL4          0x44010024  // Select output signal on 
                                            // observation output 4 
#define CCTEST_OBSSEL5          0x44010028  // Select output signal on 
                                            // observation output 5 
#define CCTEST_OBSSEL6          0x4401002C  // Select output signal on 
                                            // observation output 6 
#define CCTEST_OBSSEL7          0x44010030  // Select output signal on 
                                            // observation output 7 
#define CCTEST_TR0              0x44010034  // Test register 0 
#define CCTEST_USBCTRL          0x44010050  // USB PHY stand-by control 


//*****************************************************************************
//
// The following are defines for the bit fields in the CCTEST_IO register.
//
//*****************************************************************************
#define CCTEST_IO_SC            0x00000001  // I/O strength control bit Common 
                                            // to all digital output pads 
                                            // Should be set when unregulated 
                                            // voltage is below approximately 
                                            // 2.6 V. 
#define CCTEST_IO_SC_M          0x00000001
#define CCTEST_IO_SC_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL0 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL0_EN       0x00000080  // Observation output 0 enable 
                                            // control for PC0 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC0. 
#define CCTEST_OBSSEL0_EN_M     0x00000080
#define CCTEST_OBSSEL0_EN_S     7
#define CCTEST_OBSSEL0_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 0: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL0_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL1 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL1_EN       0x00000080  // Observation output 1 enable 
                                            // control for PC1 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC1. 
#define CCTEST_OBSSEL1_EN_M     0x00000080
#define CCTEST_OBSSEL1_EN_S     7
#define CCTEST_OBSSEL1_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 1: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL1_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL2 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL2_EN       0x00000080  // Observation output 2 enable 
                                            // control for PC2 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC2. 
#define CCTEST_OBSSEL2_EN_M     0x00000080
#define CCTEST_OBSSEL2_EN_S     7
#define CCTEST_OBSSEL2_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 2: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL2_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL3 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL3_EN       0x00000080  // Observation output 3 enable 
                                            // control for PC3 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC3. 
#define CCTEST_OBSSEL3_EN_M     0x00000080
#define CCTEST_OBSSEL3_EN_S     7
#define CCTEST_OBSSEL3_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 3: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL3_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL4 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL4_EN       0x00000080  // Observation output 4 enable 
                                            // control for PC4 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC4. 
#define CCTEST_OBSSEL4_EN_M     0x00000080
#define CCTEST_OBSSEL4_EN_S     7
#define CCTEST_OBSSEL4_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 4: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL4_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL5 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL5_EN       0x00000080  // Observation output 5 enable 
                                            // control for PC5 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC5. 
#define CCTEST_OBSSEL5_EN_M     0x00000080
#define CCTEST_OBSSEL5_EN_S     7
#define CCTEST_OBSSEL5_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 5: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL5_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL6 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL6_EN       0x00000080  // Observation output 6 enable 
                                            // control for PC6 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC6. 
#define CCTEST_OBSSEL6_EN_M     0x00000080
#define CCTEST_OBSSEL6_EN_S     7
#define CCTEST_OBSSEL6_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 6: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL6_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_OBSSEL7 register.
//
//*****************************************************************************
#define CCTEST_OBSSEL7_EN       0x00000080  // Observation output 7 enable 
                                            // control for PC7 0: Observation 
                                            // output disabled 1: Observation 
                                            // output enabled Note: If enabled, 
                                            // this overwrites the standard 
                                            // GPIO behavior of PC7. 
#define CCTEST_OBSSEL7_EN_M     0x00000080
#define CCTEST_OBSSEL7_EN_S     7
#define CCTEST_OBSSEL7_SEL_M    0x0000007F  // n - obs_sigs[n] output on 
                                            // output 7: 0: rfc_obs_sig0 1: 
                                            // rfc_obs_sig1 2: rfc_obs_sig2 
                                            // Others: Reserved 
#define CCTEST_OBSSEL7_SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the CCTEST_TR0 register.
//
//*****************************************************************************
#define CCTEST_TR0_ADCTM        0x00000002  // Set to 1 to connect the 
                                            // temperature sensor to the 
                                            // SOC_ADC. See also 
                                            // RFCORE_XREG_ATEST register 
                                            // description to enable the 
                                            // temperature sensor. 
#define CCTEST_TR0_ADCTM_M      0x00000002
#define CCTEST_TR0_ADCTM_S      1
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// CCTEST_USBCTRL register.
//
//*****************************************************************************
#define CCTEST_USBCTRL_USB_STB  0x00000001  // USB PHY stand-by override bit 
                                            // When this bit is cleared to 0 
                                            // (default state) the USB module 
                                            // cannot change the stand-by mode 
                                            // of the PHY (USB pads) and the 
                                            // PHY is forced out of stand-by 
                                            // mode. This bit must be 1 as well 
                                            // as the stand-by control from the 
                                            // USB controller, before the mode 
                                            // of the PHY is stand-by. 
#define CCTEST_USBCTRL_USB_STB_M \
                                0x00000001
#define CCTEST_USBCTRL_USB_STB_S 0


#endif // __HW_CCTEST_H__

