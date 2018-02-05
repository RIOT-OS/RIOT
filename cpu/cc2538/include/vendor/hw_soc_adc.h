/******************************************************************************
*  Filename:       hw_soc_adc.h
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

#ifndef __HW_SOC_ADC_H__
#define __HW_SOC_ADC_H__

//*****************************************************************************
//
// The following are defines for the SOC_ADC register offsets.
//
//*****************************************************************************
#define SOC_ADC_ADCCON1         0x400D7000  // This register controls the ADC. 
#define SOC_ADC_ADCCON2         0x400D7004  // This register controls the ADC. 
#define SOC_ADC_ADCCON3         0x400D7008  // This register controls the ADC. 
#define SOC_ADC_ADCL            0x400D700C  // This register contains the 
                                            // least-significant part of ADC 
                                            // conversion result. 
#define SOC_ADC_ADCH            0x400D7010  // This register contains the 
                                            // most-significant part of ADC 
                                            // conversion result. 
#define SOC_ADC_RNDL            0x400D7014  // This registers contains 
                                            // random-number-generator data; 
                                            // low byte. 
#define SOC_ADC_RNDH            0x400D7018  // This register contains 
                                            // random-number-generator data; 
                                            // high byte. 
#define SOC_ADC_CMPCTL          0x400D7024  // Analog comparator control and 
                                            // status register. 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SOC_ADC_ADCCON1 register.
//
//*****************************************************************************
#define SOC_ADC_ADCCON1_EOC     0x00000080  // End of conversion. Cleared when 
                                            // ADCH has been read. If a new 
                                            // conversion is completed before 
                                            // the previous data has been read, 
                                            // the EOC bit remains high. 0: 
                                            // Conversion not complete 1: 
                                            // Conversion completed 
#define SOC_ADC_ADCCON1_EOC_M   0x00000080
#define SOC_ADC_ADCCON1_EOC_S   7
#define SOC_ADC_ADCCON1_ST      0x00000040  // Start conversion Read as 1 
                                            // until conversion completes 0: No 
                                            // conversion in progress. 1: Start 
                                            // a conversion sequence if 
                                            // ADCCON1.STSEL = 11 and no 
                                            // sequence is running. 
#define SOC_ADC_ADCCON1_ST_M    0x00000040
#define SOC_ADC_ADCCON1_ST_S    6
#define SOC_ADC_ADCCON1_STSEL_M 0x00000030  // Start select Selects the event 
                                            // that starts a new conversion 
                                            // sequence 00: Not implemented 01: 
                                            // Full speed. Do not wait for 
                                            // triggers 10: Timer 1 channel 0 
                                            // compare event 11: ADCCON1.ST = 1 
#define SOC_ADC_ADCCON1_STSEL_S 4
#define SOC_ADC_ADCCON1_RCTRL_M 0x0000000C  // Controls the 16-bit 
                                            // random-number generator (see 
                                            // User Guide Chapter 16) When 01 
                                            // is written, the setting 
                                            // automatically returns to 00 when 
                                            // the operation completes. 00: 
                                            // Normal operation (13x unrolling) 
                                            // 01: Clock the LFSR once (13x 
                                            // unrolling) 10: Reserved 11: 
                                            // Stopped. The random-number 
                                            // generator is turned off. 
#define SOC_ADC_ADCCON1_RCTRL_S 2
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SOC_ADC_ADCCON2 register.
//
//*****************************************************************************
#define SOC_ADC_ADCCON2_SREF_M  0x000000C0  // Selects reference voltage used 
                                            // for the sequence of conversions 
                                            // 00: Internal reference 01: 
                                            // External reference on AIN7 pin 
                                            // 10: AVDD5 pin 11: External 
                                            // reference on AIN6-AIN7 
                                            // differential input 
#define SOC_ADC_ADCCON2_SREF_S  6
#define SOC_ADC_ADCCON2_SDIV_M  0x00000030  // Sets the decimation rate for 
                                            // channels included in the 
                                            // sequence of conversions. The 
                                            // decimation rate also determines 
                                            // the resolution and time required 
                                            // to complete a conversion. 00: 64 
                                            // decimation rate (7 bits ENOB 
                                            // setting) 01: 128 decimation rate 
                                            // (9 bits ENOB setting) 10: 256 
                                            // decimation rate (10 bits ENOB 
                                            // setting) 11: 512 decimation rate 
                                            // (12 bits ENOB setting) 
#define SOC_ADC_ADCCON2_SDIV_S  4
#define SOC_ADC_ADCCON2_SCH_M   0x0000000F  // Sequence channel select Selects 
                                            // the end of the sequence A 
                                            // sequence can either be from AIN0 
                                            // to AIN7 (SCH <= 7) or from 
                                            // differential input AIN0-AIN1 to 
                                            // AIN6-AIN7 (8 <= SCH <= 11). For 
                                            // other settings, only one 
                                            // conversions is performed. When 
                                            // read, these bits indicate the 
                                            // channel number on which a 
                                            // conversion is ongoing: 0000: 
                                            // AIN0 0001: AIN1 0010: AIN2 0011: 
                                            // AIN3 0100: AIN4 0101: AIN5 0110: 
                                            // AIN6 0111: AIN7 1000: AIN0-AIN1 
                                            // 1001: AIN2-AIN3 1010: AIN4-AIN5 
                                            // 1011: AIN6-AIN7 1100: GND 1101: 
                                            // Reserved 1110: Temperature 
                                            // sensor 1111: VDD/3 
#define SOC_ADC_ADCCON2_SCH_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SOC_ADC_ADCCON3 register.
//
//*****************************************************************************
#define SOC_ADC_ADCCON3_EREF_M  0x000000C0  // Selects reference voltage used 
                                            // for the extra conversion 00: 
                                            // Internal reference 01: External 
                                            // reference on AIN7 pin 10: AVDD5 
                                            // pin 11: External reference on 
                                            // AIN6-AIN7 differential input 
#define SOC_ADC_ADCCON3_EREF_S  6
#define SOC_ADC_ADCCON3_EDIV_M  0x00000030  // Sets the decimation rate used 
                                            // for the extra conversion The 
                                            // decimation rate also determines 
                                            // the resolution and the time 
                                            // required to complete the 
                                            // conversion. 00: 64 decimation 
                                            // rate (7 bits ENOB) 01: 128 
                                            // decimation rate (9 bits ENOB) 
                                            // 10: 256 decimation rate (10 bits 
                                            // ENOB) 11: 512 decimation rate 
                                            // (12 bits ENOB) 
#define SOC_ADC_ADCCON3_EDIV_S  4
#define SOC_ADC_ADCCON3_ECH_M   0x0000000F  // Single channel select. Selects 
                                            // the channel number of the single 
                                            // conversion that is triggered by 
                                            // writing to ADCCON3. 0000: AIN0 
                                            // 0001: AIN1 0010: AIN2 0011: AIN3 
                                            // 0100: AIN4 0101: AIN5 0110: AIN6 
                                            // 0111: AIN7 1000: AIN0-AIN1 1001: 
                                            // AIN2-AIN3 1010: AIN4-AIN5 1011: 
                                            // AIN6-AIN7 1100: GND 1101: 
                                            // Reserved 1110: Temperature 
                                            // sensor 1111: VDD/3 
#define SOC_ADC_ADCCON3_ECH_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the SOC_ADC_ADCL register.
//
//*****************************************************************************
#define SOC_ADC_ADCL_ADC_M      0x000000FC  // Least-significant part of ADC 
                                            // conversion result 
#define SOC_ADC_ADCL_ADC_S      2
//*****************************************************************************
//
// The following are defines for the bit fields in the SOC_ADC_ADCH register.
//
//*****************************************************************************
#define SOC_ADC_ADCH_ADC_M      0x000000FF  // Most-significant part of ADC 
                                            // conversion result 
#define SOC_ADC_ADCH_ADC_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the SOC_ADC_RNDL register.
//
//*****************************************************************************
#define SOC_ADC_RNDL_RNDL_M     0x000000FF  // Random value/seed or CRC 
                                            // result, low byte When used for 
                                            // random-number generation, 
                                            // writing to this register twice 
                                            // seeds the random-number 
                                            // generator. Writing to this 
                                            // register copies the 8 LSBs of 
                                            // the LFSR to the 8 MSBs and 
                                            // replaces the 8 LSBs with the 
                                            // data value written. The value 
                                            // returned when reading from this 
                                            // register is the 8 LSBs of the 
                                            // LFSR. When used for 
                                            // random-number generation, 
                                            // reading this register returns 
                                            // the 8 LSBs of the random number. 
                                            // When used for CRC calculations, 
                                            // reading this register returns 
                                            // the 8 LSBs of the CRC result. 
#define SOC_ADC_RNDL_RNDL_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the SOC_ADC_RNDH register.
//
//*****************************************************************************
#define SOC_ADC_RNDH_RNDH_M     0x000000FF  // Random value or CRC 
                                            // result/input data, high byte 
                                            // When written, a CRC16 
                                            // calculation is triggered, and 
                                            // the data value written is 
                                            // processed starting with the MSB. 
                                            // The value returned when reading 
                                            // from this register is the 8 MSBs 
                                            // of the LFSR. When used for 
                                            // random-number generation, 
                                            // reading this register returns 
                                            // the 8 MSBs of the random number. 
                                            // When used for CRC calculations, 
                                            // reading this register returns 
                                            // the 8 MSBs of the CRC result. 
#define SOC_ADC_RNDH_RNDH_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SOC_ADC_CMPCTL register.
//
//*****************************************************************************
#define SOC_ADC_CMPCTL_EN       0x00000002  // Comparator enable 
#define SOC_ADC_CMPCTL_EN_M     0x00000002
#define SOC_ADC_CMPCTL_EN_S     1
#define SOC_ADC_CMPCTL_OUTPUT   0x00000001  // Comparator output 
#define SOC_ADC_CMPCTL_OUTPUT_M 0x00000001
#define SOC_ADC_CMPCTL_OUTPUT_S 0


#endif // __HW_SOC_ADC_H__

