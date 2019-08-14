/******************************************************************************
*  Filename:       hw_flash_ctrl.h
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

#ifndef __HW_FLASH_CTRL_H__
#define __HW_FLASH_CTRL_H__

//*****************************************************************************
//
// The following are defines for the FLASH_CTRL register offsets.
//
//*****************************************************************************
#define FLASH_CTRL_FCTL         0x400D3008  // Flash control This register 
                                            // provides control and monitoring 
                                            // functions for the flash module. 
#define FLASH_CTRL_FADDR        0x400D300C  // Flash address The register sets 
                                            // the address to be written in 
                                            // flash memory. See the bitfield 
                                            // descriptions for formatting 
                                            // information. 
#define FLASH_CTRL_FWDATA       0x400D3010  // Flash data This register 
                                            // contains the 32-bits of data to 
                                            // be written to the flash location 
                                            // selected in FADDR. 
#define FLASH_CTRL_DIECFG0      0x400D3014  // These settings are a function 
                                            // of the FLASH information page 
                                            // bit settings, which are 
                                            // programmed during production 
                                            // test, and are subject for 
                                            // specific configuration for 
                                            // multiple device flavors of 
                                            // cc2538. 
#define FLASH_CTRL_DIECFG1      0x400D3018  // These settings are a function 
                                            // of the FLASH information page 
                                            // bit settings, which are 
                                            // programmed during production 
                                            // test, and are subject for 
                                            // specific configuration for 
                                            // multiple device flavors of 
                                            // cc2538. 
#define FLASH_CTRL_DIECFG2      0x400D301C  // These settings are a function 
                                            // of the FLASH information page 
                                            // bit settings, which are 
                                            // programmed during production 
                                            // test, and are subject for 
                                            // specific configuration for 
                                            // multiple device flavors of 
                                            // cc2538. The DIE_*_REVISION 
                                            // registers are an exeception to 
                                            // this, as they are hardwired and 
                                            // are not part of the FLASH 
                                            // information page. 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// FLASH_CTRL_FCTL register.
//
//*****************************************************************************
#define FLASH_CTRL_FCTL_UPPER_PAGE_ACCESS \
                                0x00000200  // Lock bit for lock bit page 0: 
                                            // Neither write nor erase not 
                                            // allowed 1: Both write and erase 
                                            // allowed 

#define FLASH_CTRL_FCTL_UPPER_PAGE_ACCESS_M \
                                0x00000200
#define FLASH_CTRL_FCTL_UPPER_PAGE_ACCESS_S 9
#define FLASH_CTRL_FCTL_SEL_INFO_PAGE \
                                0x00000100  // Flash erase or write operation 
                                            // on APB bus must assert this when 
                                            // accessing the information page 

#define FLASH_CTRL_FCTL_SEL_INFO_PAGE_M \
                                0x00000100
#define FLASH_CTRL_FCTL_SEL_INFO_PAGE_S 8
#define FLASH_CTRL_FCTL_BUSY    0x00000080  // Set when the WRITE or ERASE bit 
                                            // is set; that is, when the flash 
                                            // controller is busy 
#define FLASH_CTRL_FCTL_BUSY_M  0x00000080
#define FLASH_CTRL_FCTL_BUSY_S  7
#define FLASH_CTRL_FCTL_FULL    0x00000040  // Write buffer full The CPU can 
                                            // write to FWDATA when this bit is 
                                            // 0 and WRITE is 1. This bit is 
                                            // cleared when BUSY is cleared. 
#define FLASH_CTRL_FCTL_FULL_M  0x00000040
#define FLASH_CTRL_FCTL_FULL_S  6
#define FLASH_CTRL_FCTL_ABORT   0x00000020  // Abort status This bit is set to 
                                            // 1 when a write sequence or page 
                                            // erase is aborted. An operation 
                                            // is aborted when the accessed 
                                            // page is locked. Cleared when a 
                                            // write or page erase is started. 
                                            // If a write operation times out 
                                            // (because the FWDATA register is 
                                            // not written fast enough), the 
                                            // ABORT bit is not set even if the 
                                            // page is locked. If a page erase 
                                            // and a write operation are 
                                            // started simultaneously, the 
                                            // ABORT bit reflects the status of 
                                            // the last write operation. For 
                                            // example, if the page is locked 
                                            // and the write times out, the 
                                            // ABORT bit is not set because 
                                            // only the write operation times 
                                            // out. 
#define FLASH_CTRL_FCTL_ABORT_M 0x00000020
#define FLASH_CTRL_FCTL_ABORT_S 5
#define FLASH_CTRL_FCTL_CM_M    0x0000000C  // Cache Mode Disabling the cache 
                                            // increases the power consumption 
                                            // and reduces performance. 
                                            // Prefetching improves performance 
                                            // at the expense of a potential 
                                            // increase in power consumption. 
                                            // Real-time mode provides 
                                            // predictable flash read access 
                                            // time, the execution time is 
                                            // equal to cache disabled mode, 
                                            // but the power consumption is 
                                            // lower. 00: Cache disabled 01: 
                                            // Cache enabled 10: Cache enabled, 
                                            // with prefetch 11: Real-time mode 
                                            // Note: The read value always 
                                            // represents the current cache 
                                            // mode. Writing a new cache mode 
                                            // starts a cache mode change 
                                            // request that does not take 
                                            // effect until the controller is 
                                            // ready. Writes to this register 
                                            // are ignored if there is a 
                                            // current cache change request in 
                                            // progress. 
#define FLASH_CTRL_FCTL_CM_S    2
#define FLASH_CTRL_FCTL_WRITE   0x00000002  // Write bit Start a write 
                                            // sequence by setting this bit to 
                                            // 1. Cleared by hardware when the 
                                            // operation completes. Writes to 
                                            // this bit are ignored when 
                                            // FCTL.BUSY is 1. If FCTL.ERASE is 
                                            // set simultaneously with this 
                                            // bit, the erase operation is 
                                            // started first, then the write is 
                                            // started. 
#define FLASH_CTRL_FCTL_WRITE_M 0x00000002
#define FLASH_CTRL_FCTL_WRITE_S 1
#define FLASH_CTRL_FCTL_ERASE   0x00000001  // Erase bit Start an erase 
                                            // operation by setting this bit to 
                                            // 1. Cleared by hardware when the 
                                            // operation completes. Writes to 
                                            // this bit are ignored when 
                                            // FCTL.BUSY is 1. If FCTL.WRITE is 
                                            // set simultaneously with this 
                                            // bit, the erase operation is 
                                            // started first, then the write is 
                                            // started. 
#define FLASH_CTRL_FCTL_ERASE_M 0x00000001
#define FLASH_CTRL_FCTL_ERASE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// FLASH_CTRL_FADDR register.
//
//*****************************************************************************
#define FLASH_CTRL_FADDR_FADDR_M \
                                0x0001FFFF  // Bit number [16:9] selects one 
                                            // of 256 pages for page erase. Bit 
                                            // number [8:7] selects one of the 
                                            // 4 row in a given page Bit number 
                                            // [6:1] selects one of the 64-bit 
                                            // wide locations in a give row. 
                                            // Bit number [0] will select 
                                            // upper/lower 32-bits in a given 
                                            // 64-bit location - 64Kbytes --> 
                                            // Bits [16:14] will always be 0. - 
                                            // 128Kbytes --> Bits [16:15] will 
                                            // always be 0. - 256Kbytes --> Bit 
                                            // [16] will always be 0. - 
                                            // 384/512Kbytes --> All bits 
                                            // written and valid. Writes to 
                                            // this register will be ignored 
                                            // when any of FCTL.WRITE and 
                                            // FCTL.ERASE is set. FADDR should 
                                            // be written with byte addressable 
                                            // location of the Flash to be 
                                            // programmed. Read back value 
                                            // always reflects a 32-bit aligned 
                                            // address. When the register is 
                                            // read back, the value that was 
                                            // written to FADDR gets right 
                                            // shift by 2 to indicate 32-bit 
                                            // aligned address. In other words 
                                            // lower 2 bits are discarded while 
                                            // reading back the register. Out 
                                            // of range address results in roll 
                                            // over. There is no status signal 
                                            // generated by flash controller to 
                                            // indicate this. Firmware is 
                                            // responsible to managing the 
                                            // addresses correctly. 

#define FLASH_CTRL_FADDR_FADDR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// FLASH_CTRL_FWDATA register.
//
//*****************************************************************************
#define FLASH_CTRL_FWDATA_FWDATA_M \
                                0xFFFFFFFF  // 32-bit flash write data Writes 
                                            // to this register are accepted 
                                            // only during a flash write 
                                            // sequence; that is, writes to 
                                            // this register after having 
                                            // written 1 to the FCTL.WRITE bit. 
                                            // New 32-bit data is written only 
                                            // if FCTL.FULL = 0. 

#define FLASH_CTRL_FWDATA_FWDATA_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// FLASH_CTRL_DIECFG0 register.
//
//*****************************************************************************
#define FLASH_CTRL_DIECFG0_CHIPID_M \
                                0xFFFF0000  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit 
                                            // field is equal to the field with 
                                            // the same name in the information 
                                            // page. 

#define FLASH_CTRL_DIECFG0_CHIPID_S 16
#define FLASH_CTRL_DIECFG0_CLK_SEL_GATE_EN_N \
                                0x00000400  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit is 
                                            // equal to the field with the same 
                                            // name in the information page. 

#define FLASH_CTRL_DIECFG0_CLK_SEL_GATE_EN_N_M \
                                0x00000400
#define FLASH_CTRL_DIECFG0_CLK_SEL_GATE_EN_N_S 10
#define FLASH_CTRL_DIECFG0_SRAM_SIZE_M \
                                0x00000380  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit 
                                            // field is equal to the field with 
                                            // the same name in the information 
                                            // page. 

#define FLASH_CTRL_DIECFG0_SRAM_SIZE_S 7
#define FLASH_CTRL_DIECFG0_FLASH_SIZE_M \
                                0x00000070  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit 
                                            // field is equal to the field with 
                                            // the same name in the information 
                                            // page. 

#define FLASH_CTRL_DIECFG0_FLASH_SIZE_S 4
#define FLASH_CTRL_DIECFG0_USB_ENABLE \
                                0x00000008  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit is 
                                            // equal to the field with the same 
                                            // name in the information page. 

#define FLASH_CTRL_DIECFG0_USB_ENABLE_M \
                                0x00000008
#define FLASH_CTRL_DIECFG0_USB_ENABLE_S 3
#define FLASH_CTRL_DIECFG0_MASS_ERASE_ENABLE \
                                0x00000004  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit is 
                                            // equal to the field with the same 
                                            // name in the information page. 

#define FLASH_CTRL_DIECFG0_MASS_ERASE_ENABLE_M \
                                0x00000004
#define FLASH_CTRL_DIECFG0_MASS_ERASE_ENABLE_S 2
#define FLASH_CTRL_DIECFG0_LOCK_FWT_N \
                                0x00000002  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit is 
                                            // equal to the field with the same 
                                            // name in the information page. 

#define FLASH_CTRL_DIECFG0_LOCK_FWT_N_M \
                                0x00000002
#define FLASH_CTRL_DIECFG0_LOCK_FWT_N_S 1
#define FLASH_CTRL_DIECFG0_LOCK_IP_N \
                                0x00000001  // Register copy of configuration 
                                            // bits Three clock cycles after 
                                            // reset is released, this bit is 
                                            // equal to the field with the same 
                                            // name in the information page. 

#define FLASH_CTRL_DIECFG0_LOCK_IP_N_M \
                                0x00000001
#define FLASH_CTRL_DIECFG0_LOCK_IP_N_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// FLASH_CTRL_DIECFG1 register.
//
//*****************************************************************************
#define FLASH_CTRL_DIECFG1_I2C_EN \
                                0x01000000  // 1: I2C is enabled. 0: I2C is 
                                            // permanently disabled. 

#define FLASH_CTRL_DIECFG1_I2C_EN_M \
                                0x01000000
#define FLASH_CTRL_DIECFG1_I2C_EN_S 24
#define FLASH_CTRL_DIECFG1_UART1_EN \
                                0x00020000  // 1: UART1 is enabled. 0: UART1 
                                            // is permanently disabled. 

#define FLASH_CTRL_DIECFG1_UART1_EN_M \
                                0x00020000
#define FLASH_CTRL_DIECFG1_UART1_EN_S 17
#define FLASH_CTRL_DIECFG1_UART0_EN \
                                0x00010000  // 1: UART0 is enabled. 0: UART0 
                                            // is permanently disabled. 

#define FLASH_CTRL_DIECFG1_UART0_EN_M \
                                0x00010000
#define FLASH_CTRL_DIECFG1_UART0_EN_S 16
#define FLASH_CTRL_DIECFG1_SSI1_EN \
                                0x00000200  // 1: SSI1 is enabled. 0: SSI1 is 
                                            // permanently disabled. 

#define FLASH_CTRL_DIECFG1_SSI1_EN_M \
                                0x00000200
#define FLASH_CTRL_DIECFG1_SSI1_EN_S 9
#define FLASH_CTRL_DIECFG1_SSI0_EN \
                                0x00000100  // 1: SSI0 is enabled. 0: SSI0 is 
                                            // permanently disabled. 

#define FLASH_CTRL_DIECFG1_SSI0_EN_M \
                                0x00000100
#define FLASH_CTRL_DIECFG1_SSI0_EN_S 8
#define FLASH_CTRL_DIECFG1_GPTM3_EN \
                                0x00000008  // 1: GPTM3 is enabled. 0: GPTM3 
                                            // is permanently disabled. 

#define FLASH_CTRL_DIECFG1_GPTM3_EN_M \
                                0x00000008
#define FLASH_CTRL_DIECFG1_GPTM3_EN_S 3
#define FLASH_CTRL_DIECFG1_GPTM2_EN \
                                0x00000004  // 1: GPTM2 is enabled. 0: GPTM2 
                                            // is permanently disabled. 

#define FLASH_CTRL_DIECFG1_GPTM2_EN_M \
                                0x00000004
#define FLASH_CTRL_DIECFG1_GPTM2_EN_S 2
#define FLASH_CTRL_DIECFG1_GPTM1_EN \
                                0x00000002  // 1: GPTM1 is enabled. 0: GPTM1 
                                            // is permanently disabled. 

#define FLASH_CTRL_DIECFG1_GPTM1_EN_M \
                                0x00000002
#define FLASH_CTRL_DIECFG1_GPTM1_EN_S 1
#define FLASH_CTRL_DIECFG1_GPTM0_EN \
                                0x00000001  // 1: GPTM0 is enabled. 0: GPTM0 
                                            // is permanently disabled. 

#define FLASH_CTRL_DIECFG1_GPTM0_EN_M \
                                0x00000001
#define FLASH_CTRL_DIECFG1_GPTM0_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// FLASH_CTRL_DIECFG2 register.
//
//*****************************************************************************
#define FLASH_CTRL_DIECFG2_DIE_MAJOR_REVISION_M \
                                0x0000F000  // Indicates the major revision 
                                            // (all layer change) number for 
                                            // the cc2538 0x0 - PG1.0 0x2 - 
                                            // PG2.0 

#define FLASH_CTRL_DIECFG2_DIE_MAJOR_REVISION_S 12
#define FLASH_CTRL_DIECFG2_DIE_MINOR_REVISION_M \
                                0x00000F00  // Indicates the minor revision 
                                            // (metla layer only) number for 
                                            // the cc2538 0x0 - PG1.0 or PG2.0 

#define FLASH_CTRL_DIECFG2_DIE_MINOR_REVISION_S 8
#define FLASH_CTRL_DIECFG2_RF_CORE_EN \
                                0x00000004  // 1: RF_CORE is enabled. 0: 
                                            // RF_CORE is permanently disabled. 

#define FLASH_CTRL_DIECFG2_RF_CORE_EN_M \
                                0x00000004
#define FLASH_CTRL_DIECFG2_RF_CORE_EN_S 2
#define FLASH_CTRL_DIECFG2_AES_EN \
                                0x00000002  // 1: AES is enabled. 0: AES is 
                                            // permanently disabled. 

#define FLASH_CTRL_DIECFG2_AES_EN_M \
                                0x00000002
#define FLASH_CTRL_DIECFG2_AES_EN_S 1
#define FLASH_CTRL_DIECFG2_PKA_EN \
                                0x00000001  // 1: PKA is enabled. 0: PKA is 
                                            // permanently disabled. 

#define FLASH_CTRL_DIECFG2_PKA_EN_M \
                                0x00000001
#define FLASH_CTRL_DIECFG2_PKA_EN_S 0


#endif // __HW_FLASH_CTRL_H__

