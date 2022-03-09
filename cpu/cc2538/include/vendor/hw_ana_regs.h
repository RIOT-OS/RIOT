/******************************************************************************
*  Filename:       hw_ana_regs.h
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

#ifndef __HW_ANA_REGS_H__
#define __HW_ANA_REGS_H__

//*****************************************************************************
//
// The following are defines for the ANA_REGS register offsets.
//
//*****************************************************************************
#define ANA_REGS_O_IVCTRL       0x00000004  // Analog control register 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// ANA_REGS_O_IVCTRL register.
//
//*****************************************************************************
#define ANA_REGS_IVCTRL_DAC_CURR_CTRL_M \
                                0x00000030  // Controls bias current to DAC 
                                            // 00: 100% IVREF, 0% IREF bias 01: 
                                            // 60% IVREF, 40% IREF bias 10: 40% 
                                            // IVREF, 60% IREF bias 11: 0% 
                                            // IVREF, 100% IREF bias 

#define ANA_REGS_IVCTRL_DAC_CURR_CTRL_S 4
#define ANA_REGS_IVCTRL_LODIV_BIAS_CTRL \
                                0x00000008  // Controls bias current to LODIV 
                                            // 1: PTAT bias 0: IVREF bias 

#define ANA_REGS_IVCTRL_LODIV_BIAS_CTRL_M \
                                0x00000008
#define ANA_REGS_IVCTRL_LODIV_BIAS_CTRL_S 3
#define ANA_REGS_IVCTRL_TXMIX_DC_CTRL \
                                0x00000004  // Controls DC bias in TXMIX 

#define ANA_REGS_IVCTRL_TXMIX_DC_CTRL_M \
                                0x00000004
#define ANA_REGS_IVCTRL_TXMIX_DC_CTRL_S 2
#define ANA_REGS_IVCTRL_PA_BIAS_CTRL_M \
                                0x00000003  // Controls bias current to PA 00: 
                                            // IREF bias 01: IREF and IVREF 
                                            // bias (CC2530 mode) 10: PTAT bias 
                                            // 11: Increased PTAT slope bias 

#define ANA_REGS_IVCTRL_PA_BIAS_CTRL_S 0


#endif // __HW_ANA_REGS_H__

