//*****************************************************************************
//
// lm4fs1gh5bb.h - LM4FS1GH5BB Register Definitions
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#ifndef __LM4FS1GH5BB_H__
#define __LM4FS1GH5BB_H__

//*****************************************************************************
//
// Watchdog Timer registers (WATCHDOG0)
//
//*****************************************************************************
#define WATCHDOG0_LOAD_R        (*((volatile unsigned long *)0x40000000))
#define WATCHDOG0_VALUE_R       (*((volatile unsigned long *)0x40000004))
#define WATCHDOG0_CTL_R         (*((volatile unsigned long *)0x40000008))
#define WATCHDOG0_ICR_R         (*((volatile unsigned long *)0x4000000C))
#define WATCHDOG0_RIS_R         (*((volatile unsigned long *)0x40000010))
#define WATCHDOG0_MIS_R         (*((volatile unsigned long *)0x40000014))
#define WATCHDOG0_TEST_R        (*((volatile unsigned long *)0x40000418))
#define WATCHDOG0_LOCK_R        (*((volatile unsigned long *)0x40000C00))

//*****************************************************************************
//
// Watchdog Timer registers (WATCHDOG1)
//
//*****************************************************************************
#define WATCHDOG1_LOAD_R        (*((volatile unsigned long *)0x40001000))
#define WATCHDOG1_VALUE_R       (*((volatile unsigned long *)0x40001004))
#define WATCHDOG1_CTL_R         (*((volatile unsigned long *)0x40001008))
#define WATCHDOG1_ICR_R         (*((volatile unsigned long *)0x4000100C))
#define WATCHDOG1_RIS_R         (*((volatile unsigned long *)0x40001010))
#define WATCHDOG1_MIS_R         (*((volatile unsigned long *)0x40001014))
#define WATCHDOG1_TEST_R        (*((volatile unsigned long *)0x40001418))
#define WATCHDOG1_LOCK_R        (*((volatile unsigned long *)0x40001C00))

//*****************************************************************************
//
// GPIO registers (PORTA)
//
//*****************************************************************************
#define GPIO_PORTA_DATA_BITS_R  ((volatile unsigned long *)0x40004000)
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_IS_R         (*((volatile unsigned long *)0x40004404))
#define GPIO_PORTA_IBE_R        (*((volatile unsigned long *)0x40004408))
#define GPIO_PORTA_IEV_R        (*((volatile unsigned long *)0x4000440C))
#define GPIO_PORTA_IM_R         (*((volatile unsigned long *)0x40004410))
#define GPIO_PORTA_RIS_R        (*((volatile unsigned long *)0x40004414))
#define GPIO_PORTA_MIS_R        (*((volatile unsigned long *)0x40004418))
#define GPIO_PORTA_ICR_R        (*((volatile unsigned long *)0x4000441C))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DR2R_R       (*((volatile unsigned long *)0x40004500))
#define GPIO_PORTA_DR4R_R       (*((volatile unsigned long *)0x40004504))
#define GPIO_PORTA_DR8R_R       (*((volatile unsigned long *)0x40004508))
#define GPIO_PORTA_ODR_R        (*((volatile unsigned long *)0x4000450C))
#define GPIO_PORTA_PUR_R        (*((volatile unsigned long *)0x40004510))
#define GPIO_PORTA_PDR_R        (*((volatile unsigned long *)0x40004514))
#define GPIO_PORTA_SLR_R        (*((volatile unsigned long *)0x40004518))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40004520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_ADCCTL_R     (*((volatile unsigned long *)0x40004530))
#define GPIO_PORTA_DMACTL_R     (*((volatile unsigned long *)0x40004534))
#define GPIO_PORTA_SI_R         (*((volatile unsigned long *)0x40004538))

//*****************************************************************************
//
// GPIO registers (PORTB)
//
//*****************************************************************************
#define GPIO_PORTB_DATA_BITS_R  ((volatile unsigned long *)0x40005000)
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_IS_R         (*((volatile unsigned long *)0x40005404))
#define GPIO_PORTB_IBE_R        (*((volatile unsigned long *)0x40005408))
#define GPIO_PORTB_IEV_R        (*((volatile unsigned long *)0x4000540C))
#define GPIO_PORTB_IM_R         (*((volatile unsigned long *)0x40005410))
#define GPIO_PORTB_RIS_R        (*((volatile unsigned long *)0x40005414))
#define GPIO_PORTB_MIS_R        (*((volatile unsigned long *)0x40005418))
#define GPIO_PORTB_ICR_R        (*((volatile unsigned long *)0x4000541C))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DR2R_R       (*((volatile unsigned long *)0x40005500))
#define GPIO_PORTB_DR4R_R       (*((volatile unsigned long *)0x40005504))
#define GPIO_PORTB_DR8R_R       (*((volatile unsigned long *)0x40005508))
#define GPIO_PORTB_ODR_R        (*((volatile unsigned long *)0x4000550C))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_PDR_R        (*((volatile unsigned long *)0x40005514))
#define GPIO_PORTB_SLR_R        (*((volatile unsigned long *)0x40005518))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTB_ADCCTL_R     (*((volatile unsigned long *)0x40005530))
#define GPIO_PORTB_DMACTL_R     (*((volatile unsigned long *)0x40005534))
#define GPIO_PORTB_SI_R         (*((volatile unsigned long *)0x40005538))

//*****************************************************************************
//
// GPIO registers (PORTC)
//
//*****************************************************************************
#define GPIO_PORTC_DATA_BITS_R  ((volatile unsigned long *)0x40006000)
#define GPIO_PORTC_DATA_R       (*((volatile unsigned long *)0x400063FC))
#define GPIO_PORTC_DIR_R        (*((volatile unsigned long *)0x40006400))
#define GPIO_PORTC_IS_R         (*((volatile unsigned long *)0x40006404))
#define GPIO_PORTC_IBE_R        (*((volatile unsigned long *)0x40006408))
#define GPIO_PORTC_IEV_R        (*((volatile unsigned long *)0x4000640C))
#define GPIO_PORTC_IM_R         (*((volatile unsigned long *)0x40006410))
#define GPIO_PORTC_RIS_R        (*((volatile unsigned long *)0x40006414))
#define GPIO_PORTC_MIS_R        (*((volatile unsigned long *)0x40006418))
#define GPIO_PORTC_ICR_R        (*((volatile unsigned long *)0x4000641C))
#define GPIO_PORTC_AFSEL_R      (*((volatile unsigned long *)0x40006420))
#define GPIO_PORTC_DR2R_R       (*((volatile unsigned long *)0x40006500))
#define GPIO_PORTC_DR4R_R       (*((volatile unsigned long *)0x40006504))
#define GPIO_PORTC_DR8R_R       (*((volatile unsigned long *)0x40006508))
#define GPIO_PORTC_ODR_R        (*((volatile unsigned long *)0x4000650C))
#define GPIO_PORTC_PUR_R        (*((volatile unsigned long *)0x40006510))
#define GPIO_PORTC_PDR_R        (*((volatile unsigned long *)0x40006514))
#define GPIO_PORTC_SLR_R        (*((volatile unsigned long *)0x40006518))
#define GPIO_PORTC_DEN_R        (*((volatile unsigned long *)0x4000651C))
#define GPIO_PORTC_LOCK_R       (*((volatile unsigned long *)0x40006520))
#define GPIO_PORTC_CR_R         (*((volatile unsigned long *)0x40006524))
#define GPIO_PORTC_AMSEL_R      (*((volatile unsigned long *)0x40006528))
#define GPIO_PORTC_PCTL_R       (*((volatile unsigned long *)0x4000652C))
#define GPIO_PORTC_ADCCTL_R     (*((volatile unsigned long *)0x40006530))
#define GPIO_PORTC_DMACTL_R     (*((volatile unsigned long *)0x40006534))
#define GPIO_PORTC_SI_R         (*((volatile unsigned long *)0x40006538))

//*****************************************************************************
//
// GPIO registers (PORTD)
//
//*****************************************************************************
#define GPIO_PORTD_DATA_BITS_R  ((volatile unsigned long *)0x40007000)
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_IS_R         (*((volatile unsigned long *)0x40007404))
#define GPIO_PORTD_IBE_R        (*((volatile unsigned long *)0x40007408))
#define GPIO_PORTD_IEV_R        (*((volatile unsigned long *)0x4000740C))
#define GPIO_PORTD_IM_R         (*((volatile unsigned long *)0x40007410))
#define GPIO_PORTD_RIS_R        (*((volatile unsigned long *)0x40007414))
#define GPIO_PORTD_MIS_R        (*((volatile unsigned long *)0x40007418))
#define GPIO_PORTD_ICR_R        (*((volatile unsigned long *)0x4000741C))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DR2R_R       (*((volatile unsigned long *)0x40007500))
#define GPIO_PORTD_DR4R_R       (*((volatile unsigned long *)0x40007504))
#define GPIO_PORTD_DR8R_R       (*((volatile unsigned long *)0x40007508))
#define GPIO_PORTD_ODR_R        (*((volatile unsigned long *)0x4000750C))
#define GPIO_PORTD_PUR_R        (*((volatile unsigned long *)0x40007510))
#define GPIO_PORTD_PDR_R        (*((volatile unsigned long *)0x40007514))
#define GPIO_PORTD_SLR_R        (*((volatile unsigned long *)0x40007518))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R       (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))
#define GPIO_PORTD_ADCCTL_R     (*((volatile unsigned long *)0x40007530))
#define GPIO_PORTD_DMACTL_R     (*((volatile unsigned long *)0x40007534))
#define GPIO_PORTD_SI_R         (*((volatile unsigned long *)0x40007538))

//*****************************************************************************
//
// SSI registers (SSI0)
//
//*****************************************************************************
#define SSI0_CR0_R              (*((volatile unsigned long *)0x40008000))
#define SSI0_CR1_R              (*((volatile unsigned long *)0x40008004))
#define SSI0_DR_R               (*((volatile unsigned long *)0x40008008))
#define SSI0_SR_R               (*((volatile unsigned long *)0x4000800C))
#define SSI0_CPSR_R             (*((volatile unsigned long *)0x40008010))
#define SSI0_IM_R               (*((volatile unsigned long *)0x40008014))
#define SSI0_RIS_R              (*((volatile unsigned long *)0x40008018))
#define SSI0_MIS_R              (*((volatile unsigned long *)0x4000801C))
#define SSI0_ICR_R              (*((volatile unsigned long *)0x40008020))
#define SSI0_DMACTL_R           (*((volatile unsigned long *)0x40008024))
#define SSI0_CC_R               (*((volatile unsigned long *)0x40008FC8))

//*****************************************************************************
//
// SSI registers (SSI1)
//
//*****************************************************************************
#define SSI1_CR0_R              (*((volatile unsigned long *)0x40009000))
#define SSI1_CR1_R              (*((volatile unsigned long *)0x40009004))
#define SSI1_DR_R               (*((volatile unsigned long *)0x40009008))
#define SSI1_SR_R               (*((volatile unsigned long *)0x4000900C))
#define SSI1_CPSR_R             (*((volatile unsigned long *)0x40009010))
#define SSI1_IM_R               (*((volatile unsigned long *)0x40009014))
#define SSI1_RIS_R              (*((volatile unsigned long *)0x40009018))
#define SSI1_MIS_R              (*((volatile unsigned long *)0x4000901C))
#define SSI1_ICR_R              (*((volatile unsigned long *)0x40009020))
#define SSI1_DMACTL_R           (*((volatile unsigned long *)0x40009024))
#define SSI1_CC_R               (*((volatile unsigned long *)0x40009FC8))

//*****************************************************************************
//
// SSI registers (SSI2)
//
//*****************************************************************************
#define SSI2_CR0_R              (*((volatile unsigned long *)0x4000A000))
#define SSI2_CR1_R              (*((volatile unsigned long *)0x4000A004))
#define SSI2_DR_R               (*((volatile unsigned long *)0x4000A008))
#define SSI2_SR_R               (*((volatile unsigned long *)0x4000A00C))
#define SSI2_CPSR_R             (*((volatile unsigned long *)0x4000A010))
#define SSI2_IM_R               (*((volatile unsigned long *)0x4000A014))
#define SSI2_RIS_R              (*((volatile unsigned long *)0x4000A018))
#define SSI2_MIS_R              (*((volatile unsigned long *)0x4000A01C))
#define SSI2_ICR_R              (*((volatile unsigned long *)0x4000A020))
#define SSI2_DMACTL_R           (*((volatile unsigned long *)0x4000A024))
#define SSI2_CC_R               (*((volatile unsigned long *)0x4000AFC8))

//*****************************************************************************
//
// SSI registers (SSI3)
//
//*****************************************************************************
#define SSI3_CR0_R              (*((volatile unsigned long *)0x4000B000))
#define SSI3_CR1_R              (*((volatile unsigned long *)0x4000B004))
#define SSI3_DR_R               (*((volatile unsigned long *)0x4000B008))
#define SSI3_SR_R               (*((volatile unsigned long *)0x4000B00C))
#define SSI3_CPSR_R             (*((volatile unsigned long *)0x4000B010))
#define SSI3_IM_R               (*((volatile unsigned long *)0x4000B014))
#define SSI3_RIS_R              (*((volatile unsigned long *)0x4000B018))
#define SSI3_MIS_R              (*((volatile unsigned long *)0x4000B01C))
#define SSI3_ICR_R              (*((volatile unsigned long *)0x4000B020))
#define SSI3_DMACTL_R           (*((volatile unsigned long *)0x4000B024))
#define SSI3_CC_R               (*((volatile unsigned long *)0x4000BFC8))

//*****************************************************************************
//
// UART registers (UART0)
//
//*****************************************************************************
#define UART0_DR_R              (*((volatile unsigned long *)0x4000C000))
#define UART0_RSR_R             (*((volatile unsigned long *)0x4000C004))
#define UART0_ECR_R             (*((volatile unsigned long *)0x4000C004))
#define UART0_FR_R              (*((volatile unsigned long *)0x4000C018))
#define UART0_ILPR_R            (*((volatile unsigned long *)0x4000C020))
#define UART0_IBRD_R            (*((volatile unsigned long *)0x4000C024))
#define UART0_FBRD_R            (*((volatile unsigned long *)0x4000C028))
#define UART0_LCRH_R            (*((volatile unsigned long *)0x4000C02C))
#define UART0_CTL_R             (*((volatile unsigned long *)0x4000C030))
#define UART0_IFLS_R            (*((volatile unsigned long *)0x4000C034))
#define UART0_IM_R              (*((volatile unsigned long *)0x4000C038))
#define UART0_RIS_R             (*((volatile unsigned long *)0x4000C03C))
#define UART0_MIS_R             (*((volatile unsigned long *)0x4000C040))
#define UART0_ICR_R             (*((volatile unsigned long *)0x4000C044))
#define UART0_DMACTL_R          (*((volatile unsigned long *)0x4000C048))
#define UART0_LCTL_R            (*((volatile unsigned long *)0x4000C090))
#define UART0_LSS_R             (*((volatile unsigned long *)0x4000C094))
#define UART0_LTIM_R            (*((volatile unsigned long *)0x4000C098))
#define UART0_9BITADDR_R        (*((volatile unsigned long *)0x4000C0A4))
#define UART0_9BITAMASK_R       (*((volatile unsigned long *)0x4000C0A8))
#define UART0_PP_R              (*((volatile unsigned long *)0x4000CFC0))
#define UART0_CC_R              (*((volatile unsigned long *)0x4000CFC8))

//*****************************************************************************
//
// UART registers (UART1)
//
//*****************************************************************************
#define UART1_DR_R              (*((volatile unsigned long *)0x4000D000))
#define UART1_RSR_R             (*((volatile unsigned long *)0x4000D004))
#define UART1_ECR_R             (*((volatile unsigned long *)0x4000D004))
#define UART1_FR_R              (*((volatile unsigned long *)0x4000D018))
#define UART1_ILPR_R            (*((volatile unsigned long *)0x4000D020))
#define UART1_IBRD_R            (*((volatile unsigned long *)0x4000D024))
#define UART1_FBRD_R            (*((volatile unsigned long *)0x4000D028))
#define UART1_LCRH_R            (*((volatile unsigned long *)0x4000D02C))
#define UART1_CTL_R             (*((volatile unsigned long *)0x4000D030))
#define UART1_IFLS_R            (*((volatile unsigned long *)0x4000D034))
#define UART1_IM_R              (*((volatile unsigned long *)0x4000D038))
#define UART1_RIS_R             (*((volatile unsigned long *)0x4000D03C))
#define UART1_MIS_R             (*((volatile unsigned long *)0x4000D040))
#define UART1_ICR_R             (*((volatile unsigned long *)0x4000D044))
#define UART1_DMACTL_R          (*((volatile unsigned long *)0x4000D048))
#define UART1_LCTL_R            (*((volatile unsigned long *)0x4000D090))
#define UART1_LSS_R             (*((volatile unsigned long *)0x4000D094))
#define UART1_LTIM_R            (*((volatile unsigned long *)0x4000D098))
#define UART1_9BITADDR_R        (*((volatile unsigned long *)0x4000D0A4))
#define UART1_9BITAMASK_R       (*((volatile unsigned long *)0x4000D0A8))
#define UART1_PP_R              (*((volatile unsigned long *)0x4000DFC0))
#define UART1_CC_R              (*((volatile unsigned long *)0x4000DFC8))

//*****************************************************************************
//
// UART registers (UART2)
//
//*****************************************************************************
#define UART2_DR_R              (*((volatile unsigned long *)0x4000E000))
#define UART2_RSR_R             (*((volatile unsigned long *)0x4000E004))
#define UART2_ECR_R             (*((volatile unsigned long *)0x4000E004))
#define UART2_FR_R              (*((volatile unsigned long *)0x4000E018))
#define UART2_ILPR_R            (*((volatile unsigned long *)0x4000E020))
#define UART2_IBRD_R            (*((volatile unsigned long *)0x4000E024))
#define UART2_FBRD_R            (*((volatile unsigned long *)0x4000E028))
#define UART2_LCRH_R            (*((volatile unsigned long *)0x4000E02C))
#define UART2_CTL_R             (*((volatile unsigned long *)0x4000E030))
#define UART2_IFLS_R            (*((volatile unsigned long *)0x4000E034))
#define UART2_IM_R              (*((volatile unsigned long *)0x4000E038))
#define UART2_RIS_R             (*((volatile unsigned long *)0x4000E03C))
#define UART2_MIS_R             (*((volatile unsigned long *)0x4000E040))
#define UART2_ICR_R             (*((volatile unsigned long *)0x4000E044))
#define UART2_DMACTL_R          (*((volatile unsigned long *)0x4000E048))
#define UART2_LCTL_R            (*((volatile unsigned long *)0x4000E090))
#define UART2_LSS_R             (*((volatile unsigned long *)0x4000E094))
#define UART2_LTIM_R            (*((volatile unsigned long *)0x4000E098))
#define UART2_9BITADDR_R        (*((volatile unsigned long *)0x4000E0A4))
#define UART2_9BITAMASK_R       (*((volatile unsigned long *)0x4000E0A8))
#define UART2_PP_R              (*((volatile unsigned long *)0x4000EFC0))
#define UART2_CC_R              (*((volatile unsigned long *)0x4000EFC8))

//*****************************************************************************
//
// UART registers (UART3)
//
//*****************************************************************************
#define UART3_DR_R              (*((volatile unsigned long *)0x4000F000))
#define UART3_RSR_R             (*((volatile unsigned long *)0x4000F004))
#define UART3_ECR_R             (*((volatile unsigned long *)0x4000F004))
#define UART3_FR_R              (*((volatile unsigned long *)0x4000F018))
#define UART3_ILPR_R            (*((volatile unsigned long *)0x4000F020))
#define UART3_IBRD_R            (*((volatile unsigned long *)0x4000F024))
#define UART3_FBRD_R            (*((volatile unsigned long *)0x4000F028))
#define UART3_LCRH_R            (*((volatile unsigned long *)0x4000F02C))
#define UART3_CTL_R             (*((volatile unsigned long *)0x4000F030))
#define UART3_IFLS_R            (*((volatile unsigned long *)0x4000F034))
#define UART3_IM_R              (*((volatile unsigned long *)0x4000F038))
#define UART3_RIS_R             (*((volatile unsigned long *)0x4000F03C))
#define UART3_MIS_R             (*((volatile unsigned long *)0x4000F040))
#define UART3_ICR_R             (*((volatile unsigned long *)0x4000F044))
#define UART3_DMACTL_R          (*((volatile unsigned long *)0x4000F048))
#define UART3_LCTL_R            (*((volatile unsigned long *)0x4000F090))
#define UART3_LSS_R             (*((volatile unsigned long *)0x4000F094))
#define UART3_LTIM_R            (*((volatile unsigned long *)0x4000F098))
#define UART3_9BITADDR_R        (*((volatile unsigned long *)0x4000F0A4))
#define UART3_9BITAMASK_R       (*((volatile unsigned long *)0x4000F0A8))
#define UART3_PP_R              (*((volatile unsigned long *)0x4000FFC0))
#define UART3_CC_R              (*((volatile unsigned long *)0x4000FFC8))

//*****************************************************************************
//
// UART registers (UART4)
//
//*****************************************************************************
#define UART4_DR_R              (*((volatile unsigned long *)0x40010000))
#define UART4_RSR_R             (*((volatile unsigned long *)0x40010004))
#define UART4_ECR_R             (*((volatile unsigned long *)0x40010004))
#define UART4_FR_R              (*((volatile unsigned long *)0x40010018))
#define UART4_ILPR_R            (*((volatile unsigned long *)0x40010020))
#define UART4_IBRD_R            (*((volatile unsigned long *)0x40010024))
#define UART4_FBRD_R            (*((volatile unsigned long *)0x40010028))
#define UART4_LCRH_R            (*((volatile unsigned long *)0x4001002C))
#define UART4_CTL_R             (*((volatile unsigned long *)0x40010030))
#define UART4_IFLS_R            (*((volatile unsigned long *)0x40010034))
#define UART4_IM_R              (*((volatile unsigned long *)0x40010038))
#define UART4_RIS_R             (*((volatile unsigned long *)0x4001003C))
#define UART4_MIS_R             (*((volatile unsigned long *)0x40010040))
#define UART4_ICR_R             (*((volatile unsigned long *)0x40010044))
#define UART4_DMACTL_R          (*((volatile unsigned long *)0x40010048))
#define UART4_LCTL_R            (*((volatile unsigned long *)0x40010090))
#define UART4_LSS_R             (*((volatile unsigned long *)0x40010094))
#define UART4_LTIM_R            (*((volatile unsigned long *)0x40010098))
#define UART4_9BITADDR_R        (*((volatile unsigned long *)0x400100A4))
#define UART4_9BITAMASK_R       (*((volatile unsigned long *)0x400100A8))
#define UART4_PP_R              (*((volatile unsigned long *)0x40010FC0))
#define UART4_CC_R              (*((volatile unsigned long *)0x40010FC8))

//*****************************************************************************
//
// UART registers (UART5)
//
//*****************************************************************************
#define UART5_DR_R              (*((volatile unsigned long *)0x40011000))
#define UART5_RSR_R             (*((volatile unsigned long *)0x40011004))
#define UART5_ECR_R             (*((volatile unsigned long *)0x40011004))
#define UART5_FR_R              (*((volatile unsigned long *)0x40011018))
#define UART5_ILPR_R            (*((volatile unsigned long *)0x40011020))
#define UART5_IBRD_R            (*((volatile unsigned long *)0x40011024))
#define UART5_FBRD_R            (*((volatile unsigned long *)0x40011028))
#define UART5_LCRH_R            (*((volatile unsigned long *)0x4001102C))
#define UART5_CTL_R             (*((volatile unsigned long *)0x40011030))
#define UART5_IFLS_R            (*((volatile unsigned long *)0x40011034))
#define UART5_IM_R              (*((volatile unsigned long *)0x40011038))
#define UART5_RIS_R             (*((volatile unsigned long *)0x4001103C))
#define UART5_MIS_R             (*((volatile unsigned long *)0x40011040))
#define UART5_ICR_R             (*((volatile unsigned long *)0x40011044))
#define UART5_DMACTL_R          (*((volatile unsigned long *)0x40011048))
#define UART5_LCTL_R            (*((volatile unsigned long *)0x40011090))
#define UART5_LSS_R             (*((volatile unsigned long *)0x40011094))
#define UART5_LTIM_R            (*((volatile unsigned long *)0x40011098))
#define UART5_9BITADDR_R        (*((volatile unsigned long *)0x400110A4))
#define UART5_9BITAMASK_R       (*((volatile unsigned long *)0x400110A8))
#define UART5_PP_R              (*((volatile unsigned long *)0x40011FC0))
#define UART5_CC_R              (*((volatile unsigned long *)0x40011FC8))

//*****************************************************************************
//
// UART registers (UART6)
//
//*****************************************************************************
#define UART6_DR_R              (*((volatile unsigned long *)0x40012000))
#define UART6_RSR_R             (*((volatile unsigned long *)0x40012004))
#define UART6_ECR_R             (*((volatile unsigned long *)0x40012004))
#define UART6_FR_R              (*((volatile unsigned long *)0x40012018))
#define UART6_ILPR_R            (*((volatile unsigned long *)0x40012020))
#define UART6_IBRD_R            (*((volatile unsigned long *)0x40012024))
#define UART6_FBRD_R            (*((volatile unsigned long *)0x40012028))
#define UART6_LCRH_R            (*((volatile unsigned long *)0x4001202C))
#define UART6_CTL_R             (*((volatile unsigned long *)0x40012030))
#define UART6_IFLS_R            (*((volatile unsigned long *)0x40012034))
#define UART6_IM_R              (*((volatile unsigned long *)0x40012038))
#define UART6_RIS_R             (*((volatile unsigned long *)0x4001203C))
#define UART6_MIS_R             (*((volatile unsigned long *)0x40012040))
#define UART6_ICR_R             (*((volatile unsigned long *)0x40012044))
#define UART6_DMACTL_R          (*((volatile unsigned long *)0x40012048))
#define UART6_LCTL_R            (*((volatile unsigned long *)0x40012090))
#define UART6_LSS_R             (*((volatile unsigned long *)0x40012094))
#define UART6_LTIM_R            (*((volatile unsigned long *)0x40012098))
#define UART6_9BITADDR_R        (*((volatile unsigned long *)0x400120A4))
#define UART6_9BITAMASK_R       (*((volatile unsigned long *)0x400120A8))
#define UART6_PP_R              (*((volatile unsigned long *)0x40012FC0))
#define UART6_CC_R              (*((volatile unsigned long *)0x40012FC8))

//*****************************************************************************
//
// UART registers (UART7)
//
//*****************************************************************************
#define UART7_DR_R              (*((volatile unsigned long *)0x40013000))
#define UART7_RSR_R             (*((volatile unsigned long *)0x40013004))
#define UART7_ECR_R             (*((volatile unsigned long *)0x40013004))
#define UART7_FR_R              (*((volatile unsigned long *)0x40013018))
#define UART7_ILPR_R            (*((volatile unsigned long *)0x40013020))
#define UART7_IBRD_R            (*((volatile unsigned long *)0x40013024))
#define UART7_FBRD_R            (*((volatile unsigned long *)0x40013028))
#define UART7_LCRH_R            (*((volatile unsigned long *)0x4001302C))
#define UART7_CTL_R             (*((volatile unsigned long *)0x40013030))
#define UART7_IFLS_R            (*((volatile unsigned long *)0x40013034))
#define UART7_IM_R              (*((volatile unsigned long *)0x40013038))
#define UART7_RIS_R             (*((volatile unsigned long *)0x4001303C))
#define UART7_MIS_R             (*((volatile unsigned long *)0x40013040))
#define UART7_ICR_R             (*((volatile unsigned long *)0x40013044))
#define UART7_DMACTL_R          (*((volatile unsigned long *)0x40013048))
#define UART7_LCTL_R            (*((volatile unsigned long *)0x40013090))
#define UART7_LSS_R             (*((volatile unsigned long *)0x40013094))
#define UART7_LTIM_R            (*((volatile unsigned long *)0x40013098))
#define UART7_9BITADDR_R        (*((volatile unsigned long *)0x400130A4))
#define UART7_9BITAMASK_R       (*((volatile unsigned long *)0x400130A8))
#define UART7_PP_R              (*((volatile unsigned long *)0x40013FC0))
#define UART7_CC_R              (*((volatile unsigned long *)0x40013FC8))

//*****************************************************************************
//
// I2C registers (I2C0 MASTER)
//
//*****************************************************************************
#define I2C0_MASTER_MSA_R       (*((volatile unsigned long *)0x40020000))
#define I2C0_MASTER_MCS_R       (*((volatile unsigned long *)0x40020004))
#define I2C0_MASTER_MDR_R       (*((volatile unsigned long *)0x40020008))
#define I2C0_MASTER_MTPR_R      (*((volatile unsigned long *)0x4002000C))
#define I2C0_MASTER_MIMR_R      (*((volatile unsigned long *)0x40020010))
#define I2C0_MASTER_MRIS_R      (*((volatile unsigned long *)0x40020014))
#define I2C0_MASTER_MMIS_R      (*((volatile unsigned long *)0x40020018))
#define I2C0_MASTER_MICR_R      (*((volatile unsigned long *)0x4002001C))
#define I2C0_MASTER_MCR_R       (*((volatile unsigned long *)0x40020020))
#define I2C0_MASTER_MCLKOCNT_R  (*((volatile unsigned long *)0x40020024))
#define I2C0_MASTER_MBMON_R     (*((volatile unsigned long *)0x4002002C))

//*****************************************************************************
//
// I2C registers (I2C0 SLAVE)
//
//*****************************************************************************
#define I2C0_SLAVE_SOAR_R       (*((volatile unsigned long *)0x40020800))
#define I2C0_SLAVE_SCSR_R       (*((volatile unsigned long *)0x40020804))
#define I2C0_SLAVE_SDR_R        (*((volatile unsigned long *)0x40020808))
#define I2C0_SLAVE_SIMR_R       (*((volatile unsigned long *)0x4002080C))
#define I2C0_SLAVE_SRIS_R       (*((volatile unsigned long *)0x40020810))
#define I2C0_SLAVE_SMIS_R       (*((volatile unsigned long *)0x40020814))
#define I2C0_SLAVE_SICR_R       (*((volatile unsigned long *)0x40020818))
#define I2C0_SLAVE_SOAR2_R      (*((volatile unsigned long *)0x4002081C))
#define I2C0_SLAVE_SACKCTL_R    (*((volatile unsigned long *)0x40020820))

//*****************************************************************************
//
// I2C registers (I2C1 MASTER)
//
//*****************************************************************************
#define I2C1_MASTER_MSA_R       (*((volatile unsigned long *)0x40021000))
#define I2C1_MASTER_MCS_R       (*((volatile unsigned long *)0x40021004))
#define I2C1_MASTER_MDR_R       (*((volatile unsigned long *)0x40021008))
#define I2C1_MASTER_MTPR_R      (*((volatile unsigned long *)0x4002100C))
#define I2C1_MASTER_MIMR_R      (*((volatile unsigned long *)0x40021010))
#define I2C1_MASTER_MRIS_R      (*((volatile unsigned long *)0x40021014))
#define I2C1_MASTER_MMIS_R      (*((volatile unsigned long *)0x40021018))
#define I2C1_MASTER_MICR_R      (*((volatile unsigned long *)0x4002101C))
#define I2C1_MASTER_MCR_R       (*((volatile unsigned long *)0x40021020))
#define I2C1_MASTER_MCLKOCNT_R  (*((volatile unsigned long *)0x40021024))
#define I2C1_MASTER_MBMON_R     (*((volatile unsigned long *)0x4002102C))

//*****************************************************************************
//
// I2C registers (I2C1 SLAVE)
//
//*****************************************************************************
#define I2C1_SLAVE_SOAR_R       (*((volatile unsigned long *)0x40021800))
#define I2C1_SLAVE_SCSR_R       (*((volatile unsigned long *)0x40021804))
#define I2C1_SLAVE_SDR_R        (*((volatile unsigned long *)0x40021808))
#define I2C1_SLAVE_SIMR_R       (*((volatile unsigned long *)0x4002180C))
#define I2C1_SLAVE_SRIS_R       (*((volatile unsigned long *)0x40021810))
#define I2C1_SLAVE_SMIS_R       (*((volatile unsigned long *)0x40021814))
#define I2C1_SLAVE_SICR_R       (*((volatile unsigned long *)0x40021818))
#define I2C1_SLAVE_SOAR2_R      (*((volatile unsigned long *)0x4002181C))
#define I2C1_SLAVE_SACKCTL_R    (*((volatile unsigned long *)0x40021820))

//*****************************************************************************
//
// I2C registers (I2C2 MASTER)
//
//*****************************************************************************
#define I2C2_MASTER_MSA_R       (*((volatile unsigned long *)0x40022000))
#define I2C2_MASTER_MCS_R       (*((volatile unsigned long *)0x40022004))
#define I2C2_MASTER_MDR_R       (*((volatile unsigned long *)0x40022008))
#define I2C2_MASTER_MTPR_R      (*((volatile unsigned long *)0x4002200C))
#define I2C2_MASTER_MIMR_R      (*((volatile unsigned long *)0x40022010))
#define I2C2_MASTER_MRIS_R      (*((volatile unsigned long *)0x40022014))
#define I2C2_MASTER_MMIS_R      (*((volatile unsigned long *)0x40022018))
#define I2C2_MASTER_MICR_R      (*((volatile unsigned long *)0x4002201C))
#define I2C2_MASTER_MCR_R       (*((volatile unsigned long *)0x40022020))
#define I2C2_MASTER_MCLKOCNT_R  (*((volatile unsigned long *)0x40022024))
#define I2C2_MASTER_MBMON_R     (*((volatile unsigned long *)0x4002202C))

//*****************************************************************************
//
// I2C registers (I2C2 SLAVE)
//
//*****************************************************************************
#define I2C2_SLAVE_SOAR_R       (*((volatile unsigned long *)0x40022800))
#define I2C2_SLAVE_SCSR_R       (*((volatile unsigned long *)0x40022804))
#define I2C2_SLAVE_SDR_R        (*((volatile unsigned long *)0x40022808))
#define I2C2_SLAVE_SIMR_R       (*((volatile unsigned long *)0x4002280C))
#define I2C2_SLAVE_SRIS_R       (*((volatile unsigned long *)0x40022810))
#define I2C2_SLAVE_SMIS_R       (*((volatile unsigned long *)0x40022814))
#define I2C2_SLAVE_SICR_R       (*((volatile unsigned long *)0x40022818))
#define I2C2_SLAVE_SOAR2_R      (*((volatile unsigned long *)0x4002281C))
#define I2C2_SLAVE_SACKCTL_R    (*((volatile unsigned long *)0x40022820))

//*****************************************************************************
//
// I2C registers (I2C3 MASTER)
//
//*****************************************************************************
#define I2C3_MASTER_MSA_R       (*((volatile unsigned long *)0x40023000))
#define I2C3_MASTER_MCS_R       (*((volatile unsigned long *)0x40023004))
#define I2C3_MASTER_MDR_R       (*((volatile unsigned long *)0x40023008))
#define I2C3_MASTER_MTPR_R      (*((volatile unsigned long *)0x4002300C))
#define I2C3_MASTER_MIMR_R      (*((volatile unsigned long *)0x40023010))
#define I2C3_MASTER_MRIS_R      (*((volatile unsigned long *)0x40023014))
#define I2C3_MASTER_MMIS_R      (*((volatile unsigned long *)0x40023018))
#define I2C3_MASTER_MICR_R      (*((volatile unsigned long *)0x4002301C))
#define I2C3_MASTER_MCR_R       (*((volatile unsigned long *)0x40023020))
#define I2C3_MASTER_MCLKOCNT_R  (*((volatile unsigned long *)0x40023024))
#define I2C3_MASTER_MBMON_R     (*((volatile unsigned long *)0x4002302C))

//*****************************************************************************
//
// I2C registers (I2C3 SLAVE)
//
//*****************************************************************************
#define I2C3_SLAVE_SOAR_R       (*((volatile unsigned long *)0x40023800))
#define I2C3_SLAVE_SCSR_R       (*((volatile unsigned long *)0x40023804))
#define I2C3_SLAVE_SDR_R        (*((volatile unsigned long *)0x40023808))
#define I2C3_SLAVE_SIMR_R       (*((volatile unsigned long *)0x4002380C))
#define I2C3_SLAVE_SRIS_R       (*((volatile unsigned long *)0x40023810))
#define I2C3_SLAVE_SMIS_R       (*((volatile unsigned long *)0x40023814))
#define I2C3_SLAVE_SICR_R       (*((volatile unsigned long *)0x40023818))
#define I2C3_SLAVE_SOAR2_R      (*((volatile unsigned long *)0x4002381C))
#define I2C3_SLAVE_SACKCTL_R    (*((volatile unsigned long *)0x40023820))

//*****************************************************************************
//
// GPIO registers (PORTE)
//
//*****************************************************************************
#define GPIO_PORTE_DATA_BITS_R  ((volatile unsigned long *)0x40024000)
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_IS_R         (*((volatile unsigned long *)0x40024404))
#define GPIO_PORTE_IBE_R        (*((volatile unsigned long *)0x40024408))
#define GPIO_PORTE_IEV_R        (*((volatile unsigned long *)0x4002440C))
#define GPIO_PORTE_IM_R         (*((volatile unsigned long *)0x40024410))
#define GPIO_PORTE_RIS_R        (*((volatile unsigned long *)0x40024414))
#define GPIO_PORTE_MIS_R        (*((volatile unsigned long *)0x40024418))
#define GPIO_PORTE_ICR_R        (*((volatile unsigned long *)0x4002441C))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DR2R_R       (*((volatile unsigned long *)0x40024500))
#define GPIO_PORTE_DR4R_R       (*((volatile unsigned long *)0x40024504))
#define GPIO_PORTE_DR8R_R       (*((volatile unsigned long *)0x40024508))
#define GPIO_PORTE_ODR_R        (*((volatile unsigned long *)0x4002450C))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTE_SLR_R        (*((volatile unsigned long *)0x40024518))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define GPIO_PORTE_ADCCTL_R     (*((volatile unsigned long *)0x40024530))
#define GPIO_PORTE_DMACTL_R     (*((volatile unsigned long *)0x40024534))
#define GPIO_PORTE_SI_R         (*((volatile unsigned long *)0x40024538))

//*****************************************************************************
//
// GPIO registers (PORTF)
//
//*****************************************************************************
#define GPIO_PORTF_DATA_BITS_R  ((volatile unsigned long *)0x40025000)
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_RIS_R        (*((volatile unsigned long *)0x40025414))
#define GPIO_PORTF_MIS_R        (*((volatile unsigned long *)0x40025418))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_DR2R_R       (*((volatile unsigned long *)0x40025500))
#define GPIO_PORTF_DR4R_R       (*((volatile unsigned long *)0x40025504))
#define GPIO_PORTF_DR8R_R       (*((volatile unsigned long *)0x40025508))
#define GPIO_PORTF_ODR_R        (*((volatile unsigned long *)0x4002550C))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_PDR_R        (*((volatile unsigned long *)0x40025514))
#define GPIO_PORTF_SLR_R        (*((volatile unsigned long *)0x40025518))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_ADCCTL_R     (*((volatile unsigned long *)0x40025530))
#define GPIO_PORTF_DMACTL_R     (*((volatile unsigned long *)0x40025534))
#define GPIO_PORTF_SI_R         (*((volatile unsigned long *)0x40025538))

//*****************************************************************************
//
// GPIO registers (PORTG)
//
//*****************************************************************************
#define GPIO_PORTG_DATA_BITS_R  ((volatile unsigned long *)0x40026000)
#define GPIO_PORTG_DATA_R       (*((volatile unsigned long *)0x400263FC))
#define GPIO_PORTG_DIR_R        (*((volatile unsigned long *)0x40026400))
#define GPIO_PORTG_IS_R         (*((volatile unsigned long *)0x40026404))
#define GPIO_PORTG_IBE_R        (*((volatile unsigned long *)0x40026408))
#define GPIO_PORTG_IEV_R        (*((volatile unsigned long *)0x4002640C))
#define GPIO_PORTG_IM_R         (*((volatile unsigned long *)0x40026410))
#define GPIO_PORTG_RIS_R        (*((volatile unsigned long *)0x40026414))
#define GPIO_PORTG_MIS_R        (*((volatile unsigned long *)0x40026418))
#define GPIO_PORTG_ICR_R        (*((volatile unsigned long *)0x4002641C))
#define GPIO_PORTG_AFSEL_R      (*((volatile unsigned long *)0x40026420))
#define GPIO_PORTG_DR2R_R       (*((volatile unsigned long *)0x40026500))
#define GPIO_PORTG_DR4R_R       (*((volatile unsigned long *)0x40026504))
#define GPIO_PORTG_DR8R_R       (*((volatile unsigned long *)0x40026508))
#define GPIO_PORTG_ODR_R        (*((volatile unsigned long *)0x4002650C))
#define GPIO_PORTG_PUR_R        (*((volatile unsigned long *)0x40026510))
#define GPIO_PORTG_PDR_R        (*((volatile unsigned long *)0x40026514))
#define GPIO_PORTG_SLR_R        (*((volatile unsigned long *)0x40026518))
#define GPIO_PORTG_DEN_R        (*((volatile unsigned long *)0x4002651C))
#define GPIO_PORTG_LOCK_R       (*((volatile unsigned long *)0x40026520))
#define GPIO_PORTG_CR_R         (*((volatile unsigned long *)0x40026524))
#define GPIO_PORTG_AMSEL_R      (*((volatile unsigned long *)0x40026528))
#define GPIO_PORTG_PCTL_R       (*((volatile unsigned long *)0x4002652C))
#define GPIO_PORTG_ADCCTL_R     (*((volatile unsigned long *)0x40026530))
#define GPIO_PORTG_DMACTL_R     (*((volatile unsigned long *)0x40026534))
#define GPIO_PORTG_SI_R         (*((volatile unsigned long *)0x40026538))

//*****************************************************************************
//
// GPIO registers (PORTH)
//
//*****************************************************************************
#define GPIO_PORTH_DATA_BITS_R  ((volatile unsigned long *)0x40027000)
#define GPIO_PORTH_DATA_R       (*((volatile unsigned long *)0x400273FC))
#define GPIO_PORTH_DIR_R        (*((volatile unsigned long *)0x40027400))
#define GPIO_PORTH_IS_R         (*((volatile unsigned long *)0x40027404))
#define GPIO_PORTH_IBE_R        (*((volatile unsigned long *)0x40027408))
#define GPIO_PORTH_IEV_R        (*((volatile unsigned long *)0x4002740C))
#define GPIO_PORTH_IM_R         (*((volatile unsigned long *)0x40027410))
#define GPIO_PORTH_RIS_R        (*((volatile unsigned long *)0x40027414))
#define GPIO_PORTH_MIS_R        (*((volatile unsigned long *)0x40027418))
#define GPIO_PORTH_ICR_R        (*((volatile unsigned long *)0x4002741C))
#define GPIO_PORTH_AFSEL_R      (*((volatile unsigned long *)0x40027420))
#define GPIO_PORTH_DR2R_R       (*((volatile unsigned long *)0x40027500))
#define GPIO_PORTH_DR4R_R       (*((volatile unsigned long *)0x40027504))
#define GPIO_PORTH_DR8R_R       (*((volatile unsigned long *)0x40027508))
#define GPIO_PORTH_ODR_R        (*((volatile unsigned long *)0x4002750C))
#define GPIO_PORTH_PUR_R        (*((volatile unsigned long *)0x40027510))
#define GPIO_PORTH_PDR_R        (*((volatile unsigned long *)0x40027514))
#define GPIO_PORTH_SLR_R        (*((volatile unsigned long *)0x40027518))
#define GPIO_PORTH_DEN_R        (*((volatile unsigned long *)0x4002751C))
#define GPIO_PORTH_LOCK_R       (*((volatile unsigned long *)0x40027520))
#define GPIO_PORTH_CR_R         (*((volatile unsigned long *)0x40027524))
#define GPIO_PORTH_AMSEL_R      (*((volatile unsigned long *)0x40027528))
#define GPIO_PORTH_PCTL_R       (*((volatile unsigned long *)0x4002752C))
#define GPIO_PORTH_ADCCTL_R     (*((volatile unsigned long *)0x40027530))
#define GPIO_PORTH_DMACTL_R     (*((volatile unsigned long *)0x40027534))
#define GPIO_PORTH_SI_R         (*((volatile unsigned long *)0x40027538))

//*****************************************************************************
//
// Timer registers (TIMER0)
//
//*****************************************************************************
#define TIMER0_CFG_R            (*((volatile unsigned long *)0x40030000))
#define TIMER0_TAMR_R           (*((volatile unsigned long *)0x40030004))
#define TIMER0_TBMR_R           (*((volatile unsigned long *)0x40030008))
#define TIMER0_CTL_R            (*((volatile unsigned long *)0x4003000C))
#define TIMER0_SYNC_R           (*((volatile unsigned long *)0x40030010))
#define TIMER0_IMR_R            (*((volatile unsigned long *)0x40030018))
#define TIMER0_RIS_R            (*((volatile unsigned long *)0x4003001C))
#define TIMER0_MIS_R            (*((volatile unsigned long *)0x40030020))
#define TIMER0_ICR_R            (*((volatile unsigned long *)0x40030024))
#define TIMER0_TAILR_R          (*((volatile unsigned long *)0x40030028))
#define TIMER0_TBILR_R          (*((volatile unsigned long *)0x4003002C))
#define TIMER0_TAMATCHR_R       (*((volatile unsigned long *)0x40030030))
#define TIMER0_TBMATCHR_R       (*((volatile unsigned long *)0x40030034))
#define TIMER0_TAPR_R           (*((volatile unsigned long *)0x40030038))
#define TIMER0_TBPR_R           (*((volatile unsigned long *)0x4003003C))
#define TIMER0_TAPMR_R          (*((volatile unsigned long *)0x40030040))
#define TIMER0_TBPMR_R          (*((volatile unsigned long *)0x40030044))
#define TIMER0_TAR_R            (*((volatile unsigned long *)0x40030048))
#define TIMER0_TBR_R            (*((volatile unsigned long *)0x4003004C))
#define TIMER0_TAV_R            (*((volatile unsigned long *)0x40030050))
#define TIMER0_TBV_R            (*((volatile unsigned long *)0x40030054))
#define TIMER0_RTCPD_R          (*((volatile unsigned long *)0x40030058))
#define TIMER0_TAPS_R           (*((volatile unsigned long *)0x4003005C))
#define TIMER0_TBPS_R           (*((volatile unsigned long *)0x40030060))
#define TIMER0_TAPV_R           (*((volatile unsigned long *)0x40030064))
#define TIMER0_TBPV_R           (*((volatile unsigned long *)0x40030068))
#define TIMER0_PP_R             (*((volatile unsigned long *)0x40030FC0))

//*****************************************************************************
//
// Timer registers (TIMER1)
//
//*****************************************************************************
#define TIMER1_CFG_R            (*((volatile unsigned long *)0x40031000))
#define TIMER1_TAMR_R           (*((volatile unsigned long *)0x40031004))
#define TIMER1_TBMR_R           (*((volatile unsigned long *)0x40031008))
#define TIMER1_CTL_R            (*((volatile unsigned long *)0x4003100C))
#define TIMER1_SYNC_R           (*((volatile unsigned long *)0x40031010))
#define TIMER1_IMR_R            (*((volatile unsigned long *)0x40031018))
#define TIMER1_RIS_R            (*((volatile unsigned long *)0x4003101C))
#define TIMER1_MIS_R            (*((volatile unsigned long *)0x40031020))
#define TIMER1_ICR_R            (*((volatile unsigned long *)0x40031024))
#define TIMER1_TAILR_R          (*((volatile unsigned long *)0x40031028))
#define TIMER1_TBILR_R          (*((volatile unsigned long *)0x4003102C))
#define TIMER1_TAMATCHR_R       (*((volatile unsigned long *)0x40031030))
#define TIMER1_TBMATCHR_R       (*((volatile unsigned long *)0x40031034))
#define TIMER1_TAPR_R           (*((volatile unsigned long *)0x40031038))
#define TIMER1_TBPR_R           (*((volatile unsigned long *)0x4003103C))
#define TIMER1_TAPMR_R          (*((volatile unsigned long *)0x40031040))
#define TIMER1_TBPMR_R          (*((volatile unsigned long *)0x40031044))
#define TIMER1_TAR_R            (*((volatile unsigned long *)0x40031048))
#define TIMER1_TBR_R            (*((volatile unsigned long *)0x4003104C))
#define TIMER1_TAV_R            (*((volatile unsigned long *)0x40031050))
#define TIMER1_TBV_R            (*((volatile unsigned long *)0x40031054))
#define TIMER1_RTCPD_R          (*((volatile unsigned long *)0x40031058))
#define TIMER1_TAPS_R           (*((volatile unsigned long *)0x4003105C))
#define TIMER1_TBPS_R           (*((volatile unsigned long *)0x40031060))
#define TIMER1_TAPV_R           (*((volatile unsigned long *)0x40031064))
#define TIMER1_TBPV_R           (*((volatile unsigned long *)0x40031068))
#define TIMER1_PP_R             (*((volatile unsigned long *)0x40031FC0))

//*****************************************************************************
//
// Timer registers (TIMER2)
//
//*****************************************************************************
#define TIMER2_CFG_R            (*((volatile unsigned long *)0x40032000))
#define TIMER2_TAMR_R           (*((volatile unsigned long *)0x40032004))
#define TIMER2_TBMR_R           (*((volatile unsigned long *)0x40032008))
#define TIMER2_CTL_R            (*((volatile unsigned long *)0x4003200C))
#define TIMER2_SYNC_R           (*((volatile unsigned long *)0x40032010))
#define TIMER2_IMR_R            (*((volatile unsigned long *)0x40032018))
#define TIMER2_RIS_R            (*((volatile unsigned long *)0x4003201C))
#define TIMER2_MIS_R            (*((volatile unsigned long *)0x40032020))
#define TIMER2_ICR_R            (*((volatile unsigned long *)0x40032024))
#define TIMER2_TAILR_R          (*((volatile unsigned long *)0x40032028))
#define TIMER2_TBILR_R          (*((volatile unsigned long *)0x4003202C))
#define TIMER2_TAMATCHR_R       (*((volatile unsigned long *)0x40032030))
#define TIMER2_TBMATCHR_R       (*((volatile unsigned long *)0x40032034))
#define TIMER2_TAPR_R           (*((volatile unsigned long *)0x40032038))
#define TIMER2_TBPR_R           (*((volatile unsigned long *)0x4003203C))
#define TIMER2_TAPMR_R          (*((volatile unsigned long *)0x40032040))
#define TIMER2_TBPMR_R          (*((volatile unsigned long *)0x40032044))
#define TIMER2_TAR_R            (*((volatile unsigned long *)0x40032048))
#define TIMER2_TBR_R            (*((volatile unsigned long *)0x4003204C))
#define TIMER2_TAV_R            (*((volatile unsigned long *)0x40032050))
#define TIMER2_TBV_R            (*((volatile unsigned long *)0x40032054))
#define TIMER2_RTCPD_R          (*((volatile unsigned long *)0x40032058))
#define TIMER2_TAPS_R           (*((volatile unsigned long *)0x4003205C))
#define TIMER2_TBPS_R           (*((volatile unsigned long *)0x40032060))
#define TIMER2_TAPV_R           (*((volatile unsigned long *)0x40032064))
#define TIMER2_TBPV_R           (*((volatile unsigned long *)0x40032068))
#define TIMER2_PP_R             (*((volatile unsigned long *)0x40032FC0))

//*****************************************************************************
//
// Timer registers (TIMER3)
//
//*****************************************************************************
#define TIMER3_CFG_R            (*((volatile unsigned long *)0x40033000))
#define TIMER3_TAMR_R           (*((volatile unsigned long *)0x40033004))
#define TIMER3_TBMR_R           (*((volatile unsigned long *)0x40033008))
#define TIMER3_CTL_R            (*((volatile unsigned long *)0x4003300C))
#define TIMER3_SYNC_R           (*((volatile unsigned long *)0x40033010))
#define TIMER3_IMR_R            (*((volatile unsigned long *)0x40033018))
#define TIMER3_RIS_R            (*((volatile unsigned long *)0x4003301C))
#define TIMER3_MIS_R            (*((volatile unsigned long *)0x40033020))
#define TIMER3_ICR_R            (*((volatile unsigned long *)0x40033024))
#define TIMER3_TAILR_R          (*((volatile unsigned long *)0x40033028))
#define TIMER3_TBILR_R          (*((volatile unsigned long *)0x4003302C))
#define TIMER3_TAMATCHR_R       (*((volatile unsigned long *)0x40033030))
#define TIMER3_TBMATCHR_R       (*((volatile unsigned long *)0x40033034))
#define TIMER3_TAPR_R           (*((volatile unsigned long *)0x40033038))
#define TIMER3_TBPR_R           (*((volatile unsigned long *)0x4003303C))
#define TIMER3_TAPMR_R          (*((volatile unsigned long *)0x40033040))
#define TIMER3_TBPMR_R          (*((volatile unsigned long *)0x40033044))
#define TIMER3_TAR_R            (*((volatile unsigned long *)0x40033048))
#define TIMER3_TBR_R            (*((volatile unsigned long *)0x4003304C))
#define TIMER3_TAV_R            (*((volatile unsigned long *)0x40033050))
#define TIMER3_TBV_R            (*((volatile unsigned long *)0x40033054))
#define TIMER3_RTCPD_R          (*((volatile unsigned long *)0x40033058))
#define TIMER3_TAPS_R           (*((volatile unsigned long *)0x4003305C))
#define TIMER3_TBPS_R           (*((volatile unsigned long *)0x40033060))
#define TIMER3_TAPV_R           (*((volatile unsigned long *)0x40033064))
#define TIMER3_TBPV_R           (*((volatile unsigned long *)0x40033068))
#define TIMER3_PP_R             (*((volatile unsigned long *)0x40033FC0))

//*****************************************************************************
//
// Timer registers (TIMER4)
//
//*****************************************************************************
#define TIMER4_CFG_R            (*((volatile unsigned long *)0x40034000))
#define TIMER4_TAMR_R           (*((volatile unsigned long *)0x40034004))
#define TIMER4_TBMR_R           (*((volatile unsigned long *)0x40034008))
#define TIMER4_CTL_R            (*((volatile unsigned long *)0x4003400C))
#define TIMER4_SYNC_R           (*((volatile unsigned long *)0x40034010))
#define TIMER4_IMR_R            (*((volatile unsigned long *)0x40034018))
#define TIMER4_RIS_R            (*((volatile unsigned long *)0x4003401C))
#define TIMER4_MIS_R            (*((volatile unsigned long *)0x40034020))
#define TIMER4_ICR_R            (*((volatile unsigned long *)0x40034024))
#define TIMER4_TAILR_R          (*((volatile unsigned long *)0x40034028))
#define TIMER4_TBILR_R          (*((volatile unsigned long *)0x4003402C))
#define TIMER4_TAMATCHR_R       (*((volatile unsigned long *)0x40034030))
#define TIMER4_TBMATCHR_R       (*((volatile unsigned long *)0x40034034))
#define TIMER4_TAPR_R           (*((volatile unsigned long *)0x40034038))
#define TIMER4_TBPR_R           (*((volatile unsigned long *)0x4003403C))
#define TIMER4_TAPMR_R          (*((volatile unsigned long *)0x40034040))
#define TIMER4_TBPMR_R          (*((volatile unsigned long *)0x40034044))
#define TIMER4_TAR_R            (*((volatile unsigned long *)0x40034048))
#define TIMER4_TBR_R            (*((volatile unsigned long *)0x4003404C))
#define TIMER4_TAV_R            (*((volatile unsigned long *)0x40034050))
#define TIMER4_TBV_R            (*((volatile unsigned long *)0x40034054))
#define TIMER4_RTCPD_R          (*((volatile unsigned long *)0x40034058))
#define TIMER4_TAPS_R           (*((volatile unsigned long *)0x4003405C))
#define TIMER4_TBPS_R           (*((volatile unsigned long *)0x40034060))
#define TIMER4_TAPV_R           (*((volatile unsigned long *)0x40034064))
#define TIMER4_TBPV_R           (*((volatile unsigned long *)0x40034068))
#define TIMER4_PP_R             (*((volatile unsigned long *)0x40034FC0))

//*****************************************************************************
//
// Timer registers (TIMER5)
//
//*****************************************************************************
#define TIMER5_CFG_R            (*((volatile unsigned long *)0x40035000))
#define TIMER5_TAMR_R           (*((volatile unsigned long *)0x40035004))
#define TIMER5_TBMR_R           (*((volatile unsigned long *)0x40035008))
#define TIMER5_CTL_R            (*((volatile unsigned long *)0x4003500C))
#define TIMER5_SYNC_R           (*((volatile unsigned long *)0x40035010))
#define TIMER5_IMR_R            (*((volatile unsigned long *)0x40035018))
#define TIMER5_RIS_R            (*((volatile unsigned long *)0x4003501C))
#define TIMER5_MIS_R            (*((volatile unsigned long *)0x40035020))
#define TIMER5_ICR_R            (*((volatile unsigned long *)0x40035024))
#define TIMER5_TAILR_R          (*((volatile unsigned long *)0x40035028))
#define TIMER5_TBILR_R          (*((volatile unsigned long *)0x4003502C))
#define TIMER5_TAMATCHR_R       (*((volatile unsigned long *)0x40035030))
#define TIMER5_TBMATCHR_R       (*((volatile unsigned long *)0x40035034))
#define TIMER5_TAPR_R           (*((volatile unsigned long *)0x40035038))
#define TIMER5_TBPR_R           (*((volatile unsigned long *)0x4003503C))
#define TIMER5_TAPMR_R          (*((volatile unsigned long *)0x40035040))
#define TIMER5_TBPMR_R          (*((volatile unsigned long *)0x40035044))
#define TIMER5_TAR_R            (*((volatile unsigned long *)0x40035048))
#define TIMER5_TBR_R            (*((volatile unsigned long *)0x4003504C))
#define TIMER5_TAV_R            (*((volatile unsigned long *)0x40035050))
#define TIMER5_TBV_R            (*((volatile unsigned long *)0x40035054))
#define TIMER5_RTCPD_R          (*((volatile unsigned long *)0x40035058))
#define TIMER5_TAPS_R           (*((volatile unsigned long *)0x4003505C))
#define TIMER5_TBPS_R           (*((volatile unsigned long *)0x40035060))
#define TIMER5_TAPV_R           (*((volatile unsigned long *)0x40035064))
#define TIMER5_TBPV_R           (*((volatile unsigned long *)0x40035068))
#define TIMER5_PP_R             (*((volatile unsigned long *)0x40035FC0))

//*****************************************************************************
//
// Timer registers (WTIMER0)
//
//*****************************************************************************
#define WTIMER0_CFG_R           (*((volatile unsigned long *)0x40036000))
#define WTIMER0_TAMR_R          (*((volatile unsigned long *)0x40036004))
#define WTIMER0_TBMR_R          (*((volatile unsigned long *)0x40036008))
#define WTIMER0_CTL_R           (*((volatile unsigned long *)0x4003600C))
#define WTIMER0_SYNC_R          (*((volatile unsigned long *)0x40036010))
#define WTIMER0_IMR_R           (*((volatile unsigned long *)0x40036018))
#define WTIMER0_RIS_R           (*((volatile unsigned long *)0x4003601C))
#define WTIMER0_MIS_R           (*((volatile unsigned long *)0x40036020))
#define WTIMER0_ICR_R           (*((volatile unsigned long *)0x40036024))
#define WTIMER0_TAILR_R         (*((volatile unsigned long *)0x40036028))
#define WTIMER0_TBILR_R         (*((volatile unsigned long *)0x4003602C))
#define WTIMER0_TAMATCHR_R      (*((volatile unsigned long *)0x40036030))
#define WTIMER0_TBMATCHR_R      (*((volatile unsigned long *)0x40036034))
#define WTIMER0_TAPR_R          (*((volatile unsigned long *)0x40036038))
#define WTIMER0_TBPR_R          (*((volatile unsigned long *)0x4003603C))
#define WTIMER0_TAPMR_R         (*((volatile unsigned long *)0x40036040))
#define WTIMER0_TBPMR_R         (*((volatile unsigned long *)0x40036044))
#define WTIMER0_TAR_R           (*((volatile unsigned long *)0x40036048))
#define WTIMER0_TBR_R           (*((volatile unsigned long *)0x4003604C))
#define WTIMER0_TAV_R           (*((volatile unsigned long *)0x40036050))
#define WTIMER0_TBV_R           (*((volatile unsigned long *)0x40036054))
#define WTIMER0_RTCPD_R         (*((volatile unsigned long *)0x40036058))
#define WTIMER0_TAPS_R          (*((volatile unsigned long *)0x4003605C))
#define WTIMER0_TBPS_R          (*((volatile unsigned long *)0x40036060))
#define WTIMER0_TAPV_R          (*((volatile unsigned long *)0x40036064))
#define WTIMER0_TBPV_R          (*((volatile unsigned long *)0x40036068))
#define WTIMER0_PP_R            (*((volatile unsigned long *)0x40036FC0))

//*****************************************************************************
//
// Timer registers (WTIMER1)
//
//*****************************************************************************
#define WTIMER1_CFG_R           (*((volatile unsigned long *)0x40037000))
#define WTIMER1_TAMR_R          (*((volatile unsigned long *)0x40037004))
#define WTIMER1_TBMR_R          (*((volatile unsigned long *)0x40037008))
#define WTIMER1_CTL_R           (*((volatile unsigned long *)0x4003700C))
#define WTIMER1_SYNC_R          (*((volatile unsigned long *)0x40037010))
#define WTIMER1_IMR_R           (*((volatile unsigned long *)0x40037018))
#define WTIMER1_RIS_R           (*((volatile unsigned long *)0x4003701C))
#define WTIMER1_MIS_R           (*((volatile unsigned long *)0x40037020))
#define WTIMER1_ICR_R           (*((volatile unsigned long *)0x40037024))
#define WTIMER1_TAILR_R         (*((volatile unsigned long *)0x40037028))
#define WTIMER1_TBILR_R         (*((volatile unsigned long *)0x4003702C))
#define WTIMER1_TAMATCHR_R      (*((volatile unsigned long *)0x40037030))
#define WTIMER1_TBMATCHR_R      (*((volatile unsigned long *)0x40037034))
#define WTIMER1_TAPR_R          (*((volatile unsigned long *)0x40037038))
#define WTIMER1_TBPR_R          (*((volatile unsigned long *)0x4003703C))
#define WTIMER1_TAPMR_R         (*((volatile unsigned long *)0x40037040))
#define WTIMER1_TBPMR_R         (*((volatile unsigned long *)0x40037044))
#define WTIMER1_TAR_R           (*((volatile unsigned long *)0x40037048))
#define WTIMER1_TBR_R           (*((volatile unsigned long *)0x4003704C))
#define WTIMER1_TAV_R           (*((volatile unsigned long *)0x40037050))
#define WTIMER1_TBV_R           (*((volatile unsigned long *)0x40037054))
#define WTIMER1_RTCPD_R         (*((volatile unsigned long *)0x40037058))
#define WTIMER1_TAPS_R          (*((volatile unsigned long *)0x4003705C))
#define WTIMER1_TBPS_R          (*((volatile unsigned long *)0x40037060))
#define WTIMER1_TAPV_R          (*((volatile unsigned long *)0x40037064))
#define WTIMER1_TBPV_R          (*((volatile unsigned long *)0x40037068))
#define WTIMER1_PP_R            (*((volatile unsigned long *)0x40037FC0))

//*****************************************************************************
//
// ADC registers (ADC0)
//
//*****************************************************************************
#define ADC0_ACTSS_R            (*((volatile unsigned long *)0x40038000))
#define ADC0_RIS_R              (*((volatile unsigned long *)0x40038004))
#define ADC0_IM_R               (*((volatile unsigned long *)0x40038008))
#define ADC0_ISC_R              (*((volatile unsigned long *)0x4003800C))
#define ADC0_OSTAT_R            (*((volatile unsigned long *)0x40038010))
#define ADC0_EMUX_R             (*((volatile unsigned long *)0x40038014))
#define ADC0_USTAT_R            (*((volatile unsigned long *)0x40038018))
#define ADC0_SSPRI_R            (*((volatile unsigned long *)0x40038020))
#define ADC0_SPC_R              (*((volatile unsigned long *)0x40038024))
#define ADC0_PSSI_R             (*((volatile unsigned long *)0x40038028))
#define ADC0_SAC_R              (*((volatile unsigned long *)0x40038030))
#define ADC0_DCISC_R            (*((volatile unsigned long *)0x40038034))
#define ADC0_CTL_R              (*((volatile unsigned long *)0x40038038))
#define ADC0_SSMUX0_R           (*((volatile unsigned long *)0x40038040))
#define ADC0_SSCTL0_R           (*((volatile unsigned long *)0x40038044))
#define ADC0_SSFIFO0_R          (*((volatile unsigned long *)0x40038048))
#define ADC0_SSFSTAT0_R         (*((volatile unsigned long *)0x4003804C))
#define ADC0_SSOP0_R            (*((volatile unsigned long *)0x40038050))
#define ADC0_SSDC0_R            (*((volatile unsigned long *)0x40038054))
#define ADC0_SSEMUX0_R          (*((volatile unsigned long *)0x40038058))
#define ADC0_SSMUX1_R           (*((volatile unsigned long *)0x40038060))
#define ADC0_SSCTL1_R           (*((volatile unsigned long *)0x40038064))
#define ADC0_SSFIFO1_R          (*((volatile unsigned long *)0x40038068))
#define ADC0_SSFSTAT1_R         (*((volatile unsigned long *)0x4003806C))
#define ADC0_SSOP1_R            (*((volatile unsigned long *)0x40038070))
#define ADC0_SSDC1_R            (*((volatile unsigned long *)0x40038074))
#define ADC0_SSEMUX1_R          (*((volatile unsigned long *)0x40038078))
#define ADC0_SSMUX2_R           (*((volatile unsigned long *)0x40038080))
#define ADC0_SSCTL2_R           (*((volatile unsigned long *)0x40038084))
#define ADC0_SSFIFO2_R          (*((volatile unsigned long *)0x40038088))
#define ADC0_SSFSTAT2_R         (*((volatile unsigned long *)0x4003808C))
#define ADC0_SSOP2_R            (*((volatile unsigned long *)0x40038090))
#define ADC0_SSDC2_R            (*((volatile unsigned long *)0x40038094))
#define ADC0_SSEMUX2_R          (*((volatile unsigned long *)0x40038098))
#define ADC0_SSMUX3_R           (*((volatile unsigned long *)0x400380A0))
#define ADC0_SSCTL3_R           (*((volatile unsigned long *)0x400380A4))
#define ADC0_SSFIFO3_R          (*((volatile unsigned long *)0x400380A8))
#define ADC0_SSFSTAT3_R         (*((volatile unsigned long *)0x400380AC))
#define ADC0_SSOP3_R            (*((volatile unsigned long *)0x400380B0))
#define ADC0_SSDC3_R            (*((volatile unsigned long *)0x400380B4))
#define ADC0_SSEMUX3_R          (*((volatile unsigned long *)0x400380B8))
#define ADC0_DCRIC_R            (*((volatile unsigned long *)0x40038D00))
#define ADC0_DCCTL0_R           (*((volatile unsigned long *)0x40038E00))
#define ADC0_DCCTL1_R           (*((volatile unsigned long *)0x40038E04))
#define ADC0_DCCTL2_R           (*((volatile unsigned long *)0x40038E08))
#define ADC0_DCCTL3_R           (*((volatile unsigned long *)0x40038E0C))
#define ADC0_DCCTL4_R           (*((volatile unsigned long *)0x40038E10))
#define ADC0_DCCTL5_R           (*((volatile unsigned long *)0x40038E14))
#define ADC0_DCCTL6_R           (*((volatile unsigned long *)0x40038E18))
#define ADC0_DCCTL7_R           (*((volatile unsigned long *)0x40038E1C))
#define ADC0_DCCMP0_R           (*((volatile unsigned long *)0x40038E40))
#define ADC0_DCCMP1_R           (*((volatile unsigned long *)0x40038E44))
#define ADC0_DCCMP2_R           (*((volatile unsigned long *)0x40038E48))
#define ADC0_DCCMP3_R           (*((volatile unsigned long *)0x40038E4C))
#define ADC0_DCCMP4_R           (*((volatile unsigned long *)0x40038E50))
#define ADC0_DCCMP5_R           (*((volatile unsigned long *)0x40038E54))
#define ADC0_DCCMP6_R           (*((volatile unsigned long *)0x40038E58))
#define ADC0_DCCMP7_R           (*((volatile unsigned long *)0x40038E5C))
#define ADC0_PP_R               (*((volatile unsigned long *)0x40038FC0))
#define ADC0_PC_R               (*((volatile unsigned long *)0x40038FC4))
#define ADC0_CC_R               (*((volatile unsigned long *)0x40038FC8))

//*****************************************************************************
//
// ADC registers (ADC1)
//
//*****************************************************************************
#define ADC1_ACTSS_R            (*((volatile unsigned long *)0x40039000))
#define ADC1_RIS_R              (*((volatile unsigned long *)0x40039004))
#define ADC1_IM_R               (*((volatile unsigned long *)0x40039008))
#define ADC1_ISC_R              (*((volatile unsigned long *)0x4003900C))
#define ADC1_OSTAT_R            (*((volatile unsigned long *)0x40039010))
#define ADC1_EMUX_R             (*((volatile unsigned long *)0x40039014))
#define ADC1_USTAT_R            (*((volatile unsigned long *)0x40039018))
#define ADC1_SSPRI_R            (*((volatile unsigned long *)0x40039020))
#define ADC1_SPC_R              (*((volatile unsigned long *)0x40039024))
#define ADC1_PSSI_R             (*((volatile unsigned long *)0x40039028))
#define ADC1_SAC_R              (*((volatile unsigned long *)0x40039030))
#define ADC1_DCISC_R            (*((volatile unsigned long *)0x40039034))
#define ADC1_CTL_R              (*((volatile unsigned long *)0x40039038))
#define ADC1_SSMUX0_R           (*((volatile unsigned long *)0x40039040))
#define ADC1_SSCTL0_R           (*((volatile unsigned long *)0x40039044))
#define ADC1_SSFIFO0_R          (*((volatile unsigned long *)0x40039048))
#define ADC1_SSFSTAT0_R         (*((volatile unsigned long *)0x4003904C))
#define ADC1_SSOP0_R            (*((volatile unsigned long *)0x40039050))
#define ADC1_SSDC0_R            (*((volatile unsigned long *)0x40039054))
#define ADC1_SSEMUX0_R          (*((volatile unsigned long *)0x40039058))
#define ADC1_SSMUX1_R           (*((volatile unsigned long *)0x40039060))
#define ADC1_SSCTL1_R           (*((volatile unsigned long *)0x40039064))
#define ADC1_SSFIFO1_R          (*((volatile unsigned long *)0x40039068))
#define ADC1_SSFSTAT1_R         (*((volatile unsigned long *)0x4003906C))
#define ADC1_SSOP1_R            (*((volatile unsigned long *)0x40039070))
#define ADC1_SSDC1_R            (*((volatile unsigned long *)0x40039074))
#define ADC1_SSEMUX1_R          (*((volatile unsigned long *)0x40039078))
#define ADC1_SSMUX2_R           (*((volatile unsigned long *)0x40039080))
#define ADC1_SSCTL2_R           (*((volatile unsigned long *)0x40039084))
#define ADC1_SSFIFO2_R          (*((volatile unsigned long *)0x40039088))
#define ADC1_SSFSTAT2_R         (*((volatile unsigned long *)0x4003908C))
#define ADC1_SSOP2_R            (*((volatile unsigned long *)0x40039090))
#define ADC1_SSDC2_R            (*((volatile unsigned long *)0x40039094))
#define ADC1_SSEMUX2_R          (*((volatile unsigned long *)0x40039098))
#define ADC1_SSMUX3_R           (*((volatile unsigned long *)0x400390A0))
#define ADC1_SSCTL3_R           (*((volatile unsigned long *)0x400390A4))
#define ADC1_SSFIFO3_R          (*((volatile unsigned long *)0x400390A8))
#define ADC1_SSFSTAT3_R         (*((volatile unsigned long *)0x400390AC))
#define ADC1_SSOP3_R            (*((volatile unsigned long *)0x400390B0))
#define ADC1_SSDC3_R            (*((volatile unsigned long *)0x400390B4))
#define ADC1_SSEMUX3_R          (*((volatile unsigned long *)0x400390B8))
#define ADC1_DCRIC_R            (*((volatile unsigned long *)0x40039D00))
#define ADC1_DCCTL0_R           (*((volatile unsigned long *)0x40039E00))
#define ADC1_DCCTL1_R           (*((volatile unsigned long *)0x40039E04))
#define ADC1_DCCTL2_R           (*((volatile unsigned long *)0x40039E08))
#define ADC1_DCCTL3_R           (*((volatile unsigned long *)0x40039E0C))
#define ADC1_DCCTL4_R           (*((volatile unsigned long *)0x40039E10))
#define ADC1_DCCTL5_R           (*((volatile unsigned long *)0x40039E14))
#define ADC1_DCCTL6_R           (*((volatile unsigned long *)0x40039E18))
#define ADC1_DCCTL7_R           (*((volatile unsigned long *)0x40039E1C))
#define ADC1_DCCMP0_R           (*((volatile unsigned long *)0x40039E40))
#define ADC1_DCCMP1_R           (*((volatile unsigned long *)0x40039E44))
#define ADC1_DCCMP2_R           (*((volatile unsigned long *)0x40039E48))
#define ADC1_DCCMP3_R           (*((volatile unsigned long *)0x40039E4C))
#define ADC1_DCCMP4_R           (*((volatile unsigned long *)0x40039E50))
#define ADC1_DCCMP5_R           (*((volatile unsigned long *)0x40039E54))
#define ADC1_DCCMP6_R           (*((volatile unsigned long *)0x40039E58))
#define ADC1_DCCMP7_R           (*((volatile unsigned long *)0x40039E5C))
#define ADC1_PP_R               (*((volatile unsigned long *)0x40039FC0))
#define ADC1_PC_R               (*((volatile unsigned long *)0x40039FC4))
#define ADC1_CC_R               (*((volatile unsigned long *)0x40039FC8))

//*****************************************************************************
//
// Comparator registers (COMP)
//
//*****************************************************************************
#define COMP_ACMIS_R            (*((volatile unsigned long *)0x4003C000))
#define COMP_ACRIS_R            (*((volatile unsigned long *)0x4003C004))
#define COMP_ACINTEN_R          (*((volatile unsigned long *)0x4003C008))
#define COMP_ACREFCTL_R         (*((volatile unsigned long *)0x4003C010))
#define COMP_ACSTAT0_R          (*((volatile unsigned long *)0x4003C020))
#define COMP_ACCTL0_R           (*((volatile unsigned long *)0x4003C024))
#define COMP_ACSTAT1_R          (*((volatile unsigned long *)0x4003C040))
#define COMP_ACCTL1_R           (*((volatile unsigned long *)0x4003C044))
#define COMP_ACSTAT2_R          (*((volatile unsigned long *)0x4003C060))
#define COMP_ACCTL2_R           (*((volatile unsigned long *)0x4003C064))
#define COMP_PP_R               (*((volatile unsigned long *)0x4003CFC0))

//*****************************************************************************
//
// GPIO registers (PORTJ)
//
//*****************************************************************************
#define GPIO_PORTJ_DATA_BITS_R  ((volatile unsigned long *)0x4003D000)
#define GPIO_PORTJ_DATA_R       (*((volatile unsigned long *)0x4003D3FC))
#define GPIO_PORTJ_DIR_R        (*((volatile unsigned long *)0x4003D400))
#define GPIO_PORTJ_IS_R         (*((volatile unsigned long *)0x4003D404))
#define GPIO_PORTJ_IBE_R        (*((volatile unsigned long *)0x4003D408))
#define GPIO_PORTJ_IEV_R        (*((volatile unsigned long *)0x4003D40C))
#define GPIO_PORTJ_IM_R         (*((volatile unsigned long *)0x4003D410))
#define GPIO_PORTJ_RIS_R        (*((volatile unsigned long *)0x4003D414))
#define GPIO_PORTJ_MIS_R        (*((volatile unsigned long *)0x4003D418))
#define GPIO_PORTJ_ICR_R        (*((volatile unsigned long *)0x4003D41C))
#define GPIO_PORTJ_AFSEL_R      (*((volatile unsigned long *)0x4003D420))
#define GPIO_PORTJ_DR2R_R       (*((volatile unsigned long *)0x4003D500))
#define GPIO_PORTJ_DR4R_R       (*((volatile unsigned long *)0x4003D504))
#define GPIO_PORTJ_DR8R_R       (*((volatile unsigned long *)0x4003D508))
#define GPIO_PORTJ_ODR_R        (*((volatile unsigned long *)0x4003D50C))
#define GPIO_PORTJ_PUR_R        (*((volatile unsigned long *)0x4003D510))
#define GPIO_PORTJ_PDR_R        (*((volatile unsigned long *)0x4003D514))
#define GPIO_PORTJ_SLR_R        (*((volatile unsigned long *)0x4003D518))
#define GPIO_PORTJ_DEN_R        (*((volatile unsigned long *)0x4003D51C))
#define GPIO_PORTJ_LOCK_R       (*((volatile unsigned long *)0x4003D520))
#define GPIO_PORTJ_CR_R         (*((volatile unsigned long *)0x4003D524))
#define GPIO_PORTJ_AMSEL_R      (*((volatile unsigned long *)0x4003D528))
#define GPIO_PORTJ_PCTL_R       (*((volatile unsigned long *)0x4003D52C))
#define GPIO_PORTJ_ADCCTL_R     (*((volatile unsigned long *)0x4003D530))
#define GPIO_PORTJ_DMACTL_R     (*((volatile unsigned long *)0x4003D534))
#define GPIO_PORTJ_SI_R         (*((volatile unsigned long *)0x4003D538))

//*****************************************************************************
//
// Timer registers (WTIMER2)
//
//*****************************************************************************
#define WTIMER2_CFG_R           (*((volatile unsigned long *)0x4004C000))
#define WTIMER2_TAMR_R          (*((volatile unsigned long *)0x4004C004))
#define WTIMER2_TBMR_R          (*((volatile unsigned long *)0x4004C008))
#define WTIMER2_CTL_R           (*((volatile unsigned long *)0x4004C00C))
#define WTIMER2_SYNC_R          (*((volatile unsigned long *)0x4004C010))
#define WTIMER2_IMR_R           (*((volatile unsigned long *)0x4004C018))
#define WTIMER2_RIS_R           (*((volatile unsigned long *)0x4004C01C))
#define WTIMER2_MIS_R           (*((volatile unsigned long *)0x4004C020))
#define WTIMER2_ICR_R           (*((volatile unsigned long *)0x4004C024))
#define WTIMER2_TAILR_R         (*((volatile unsigned long *)0x4004C028))
#define WTIMER2_TBILR_R         (*((volatile unsigned long *)0x4004C02C))
#define WTIMER2_TAMATCHR_R      (*((volatile unsigned long *)0x4004C030))
#define WTIMER2_TBMATCHR_R      (*((volatile unsigned long *)0x4004C034))
#define WTIMER2_TAPR_R          (*((volatile unsigned long *)0x4004C038))
#define WTIMER2_TBPR_R          (*((volatile unsigned long *)0x4004C03C))
#define WTIMER2_TAPMR_R         (*((volatile unsigned long *)0x4004C040))
#define WTIMER2_TBPMR_R         (*((volatile unsigned long *)0x4004C044))
#define WTIMER2_TAR_R           (*((volatile unsigned long *)0x4004C048))
#define WTIMER2_TBR_R           (*((volatile unsigned long *)0x4004C04C))
#define WTIMER2_TAV_R           (*((volatile unsigned long *)0x4004C050))
#define WTIMER2_TBV_R           (*((volatile unsigned long *)0x4004C054))
#define WTIMER2_RTCPD_R         (*((volatile unsigned long *)0x4004C058))
#define WTIMER2_TAPS_R          (*((volatile unsigned long *)0x4004C05C))
#define WTIMER2_TBPS_R          (*((volatile unsigned long *)0x4004C060))
#define WTIMER2_TAPV_R          (*((volatile unsigned long *)0x4004C064))
#define WTIMER2_TBPV_R          (*((volatile unsigned long *)0x4004C068))
#define WTIMER2_PP_R            (*((volatile unsigned long *)0x4004CFC0))

//*****************************************************************************
//
// Timer registers (WTIMER3)
//
//*****************************************************************************
#define WTIMER3_CFG_R           (*((volatile unsigned long *)0x4004D000))
#define WTIMER3_TAMR_R          (*((volatile unsigned long *)0x4004D004))
#define WTIMER3_TBMR_R          (*((volatile unsigned long *)0x4004D008))
#define WTIMER3_CTL_R           (*((volatile unsigned long *)0x4004D00C))
#define WTIMER3_SYNC_R          (*((volatile unsigned long *)0x4004D010))
#define WTIMER3_IMR_R           (*((volatile unsigned long *)0x4004D018))
#define WTIMER3_RIS_R           (*((volatile unsigned long *)0x4004D01C))
#define WTIMER3_MIS_R           (*((volatile unsigned long *)0x4004D020))
#define WTIMER3_ICR_R           (*((volatile unsigned long *)0x4004D024))
#define WTIMER3_TAILR_R         (*((volatile unsigned long *)0x4004D028))
#define WTIMER3_TBILR_R         (*((volatile unsigned long *)0x4004D02C))
#define WTIMER3_TAMATCHR_R      (*((volatile unsigned long *)0x4004D030))
#define WTIMER3_TBMATCHR_R      (*((volatile unsigned long *)0x4004D034))
#define WTIMER3_TAPR_R          (*((volatile unsigned long *)0x4004D038))
#define WTIMER3_TBPR_R          (*((volatile unsigned long *)0x4004D03C))
#define WTIMER3_TAPMR_R         (*((volatile unsigned long *)0x4004D040))
#define WTIMER3_TBPMR_R         (*((volatile unsigned long *)0x4004D044))
#define WTIMER3_TAR_R           (*((volatile unsigned long *)0x4004D048))
#define WTIMER3_TBR_R           (*((volatile unsigned long *)0x4004D04C))
#define WTIMER3_TAV_R           (*((volatile unsigned long *)0x4004D050))
#define WTIMER3_TBV_R           (*((volatile unsigned long *)0x4004D054))
#define WTIMER3_RTCPD_R         (*((volatile unsigned long *)0x4004D058))
#define WTIMER3_TAPS_R          (*((volatile unsigned long *)0x4004D05C))
#define WTIMER3_TBPS_R          (*((volatile unsigned long *)0x4004D060))
#define WTIMER3_TAPV_R          (*((volatile unsigned long *)0x4004D064))
#define WTIMER3_TBPV_R          (*((volatile unsigned long *)0x4004D068))
#define WTIMER3_PP_R            (*((volatile unsigned long *)0x4004DFC0))

//*****************************************************************************
//
// Timer registers (WTIMER4)
//
//*****************************************************************************
#define WTIMER4_CFG_R           (*((volatile unsigned long *)0x4004E000))
#define WTIMER4_TAMR_R          (*((volatile unsigned long *)0x4004E004))
#define WTIMER4_TBMR_R          (*((volatile unsigned long *)0x4004E008))
#define WTIMER4_CTL_R           (*((volatile unsigned long *)0x4004E00C))
#define WTIMER4_SYNC_R          (*((volatile unsigned long *)0x4004E010))
#define WTIMER4_IMR_R           (*((volatile unsigned long *)0x4004E018))
#define WTIMER4_RIS_R           (*((volatile unsigned long *)0x4004E01C))
#define WTIMER4_MIS_R           (*((volatile unsigned long *)0x4004E020))
#define WTIMER4_ICR_R           (*((volatile unsigned long *)0x4004E024))
#define WTIMER4_TAILR_R         (*((volatile unsigned long *)0x4004E028))
#define WTIMER4_TBILR_R         (*((volatile unsigned long *)0x4004E02C))
#define WTIMER4_TAMATCHR_R      (*((volatile unsigned long *)0x4004E030))
#define WTIMER4_TBMATCHR_R      (*((volatile unsigned long *)0x4004E034))
#define WTIMER4_TAPR_R          (*((volatile unsigned long *)0x4004E038))
#define WTIMER4_TBPR_R          (*((volatile unsigned long *)0x4004E03C))
#define WTIMER4_TAPMR_R         (*((volatile unsigned long *)0x4004E040))
#define WTIMER4_TBPMR_R         (*((volatile unsigned long *)0x4004E044))
#define WTIMER4_TAR_R           (*((volatile unsigned long *)0x4004E048))
#define WTIMER4_TBR_R           (*((volatile unsigned long *)0x4004E04C))
#define WTIMER4_TAV_R           (*((volatile unsigned long *)0x4004E050))
#define WTIMER4_TBV_R           (*((volatile unsigned long *)0x4004E054))
#define WTIMER4_RTCPD_R         (*((volatile unsigned long *)0x4004E058))
#define WTIMER4_TAPS_R          (*((volatile unsigned long *)0x4004E05C))
#define WTIMER4_TBPS_R          (*((volatile unsigned long *)0x4004E060))
#define WTIMER4_TAPV_R          (*((volatile unsigned long *)0x4004E064))
#define WTIMER4_TBPV_R          (*((volatile unsigned long *)0x4004E068))
#define WTIMER4_PP_R            (*((volatile unsigned long *)0x4004EFC0))

//*****************************************************************************
//
// Timer registers (WTIMER5)
//
//*****************************************************************************
#define WTIMER5_CFG_R           (*((volatile unsigned long *)0x4004F000))
#define WTIMER5_TAMR_R          (*((volatile unsigned long *)0x4004F004))
#define WTIMER5_TBMR_R          (*((volatile unsigned long *)0x4004F008))
#define WTIMER5_CTL_R           (*((volatile unsigned long *)0x4004F00C))
#define WTIMER5_SYNC_R          (*((volatile unsigned long *)0x4004F010))
#define WTIMER5_IMR_R           (*((volatile unsigned long *)0x4004F018))
#define WTIMER5_RIS_R           (*((volatile unsigned long *)0x4004F01C))
#define WTIMER5_MIS_R           (*((volatile unsigned long *)0x4004F020))
#define WTIMER5_ICR_R           (*((volatile unsigned long *)0x4004F024))
#define WTIMER5_TAILR_R         (*((volatile unsigned long *)0x4004F028))
#define WTIMER5_TBILR_R         (*((volatile unsigned long *)0x4004F02C))
#define WTIMER5_TAMATCHR_R      (*((volatile unsigned long *)0x4004F030))
#define WTIMER5_TBMATCHR_R      (*((volatile unsigned long *)0x4004F034))
#define WTIMER5_TAPR_R          (*((volatile unsigned long *)0x4004F038))
#define WTIMER5_TBPR_R          (*((volatile unsigned long *)0x4004F03C))
#define WTIMER5_TAPMR_R         (*((volatile unsigned long *)0x4004F040))
#define WTIMER5_TBPMR_R         (*((volatile unsigned long *)0x4004F044))
#define WTIMER5_TAR_R           (*((volatile unsigned long *)0x4004F048))
#define WTIMER5_TBR_R           (*((volatile unsigned long *)0x4004F04C))
#define WTIMER5_TAV_R           (*((volatile unsigned long *)0x4004F050))
#define WTIMER5_TBV_R           (*((volatile unsigned long *)0x4004F054))
#define WTIMER5_RTCPD_R         (*((volatile unsigned long *)0x4004F058))
#define WTIMER5_TAPS_R          (*((volatile unsigned long *)0x4004F05C))
#define WTIMER5_TBPS_R          (*((volatile unsigned long *)0x4004F060))
#define WTIMER5_TAPV_R          (*((volatile unsigned long *)0x4004F064))
#define WTIMER5_TBPV_R          (*((volatile unsigned long *)0x4004F068))
#define WTIMER5_PP_R            (*((volatile unsigned long *)0x4004FFC0))

//*****************************************************************************
//
// GPIO registers (PORTA AHB)
//
//*****************************************************************************
#define GPIO_PORTA_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x40058000)
#define GPIO_PORTA_AHB_DATA_R   (*((volatile unsigned long *)0x400583FC))
#define GPIO_PORTA_AHB_DIR_R    (*((volatile unsigned long *)0x40058400))
#define GPIO_PORTA_AHB_IS_R     (*((volatile unsigned long *)0x40058404))
#define GPIO_PORTA_AHB_IBE_R    (*((volatile unsigned long *)0x40058408))
#define GPIO_PORTA_AHB_IEV_R    (*((volatile unsigned long *)0x4005840C))
#define GPIO_PORTA_AHB_IM_R     (*((volatile unsigned long *)0x40058410))
#define GPIO_PORTA_AHB_RIS_R    (*((volatile unsigned long *)0x40058414))
#define GPIO_PORTA_AHB_MIS_R    (*((volatile unsigned long *)0x40058418))
#define GPIO_PORTA_AHB_ICR_R    (*((volatile unsigned long *)0x4005841C))
#define GPIO_PORTA_AHB_AFSEL_R  (*((volatile unsigned long *)0x40058420))
#define GPIO_PORTA_AHB_DR2R_R   (*((volatile unsigned long *)0x40058500))
#define GPIO_PORTA_AHB_DR4R_R   (*((volatile unsigned long *)0x40058504))
#define GPIO_PORTA_AHB_DR8R_R   (*((volatile unsigned long *)0x40058508))
#define GPIO_PORTA_AHB_ODR_R    (*((volatile unsigned long *)0x4005850C))
#define GPIO_PORTA_AHB_PUR_R    (*((volatile unsigned long *)0x40058510))
#define GPIO_PORTA_AHB_PDR_R    (*((volatile unsigned long *)0x40058514))
#define GPIO_PORTA_AHB_SLR_R    (*((volatile unsigned long *)0x40058518))
#define GPIO_PORTA_AHB_DEN_R    (*((volatile unsigned long *)0x4005851C))
#define GPIO_PORTA_AHB_LOCK_R   (*((volatile unsigned long *)0x40058520))
#define GPIO_PORTA_AHB_CR_R     (*((volatile unsigned long *)0x40058524))
#define GPIO_PORTA_AHB_AMSEL_R  (*((volatile unsigned long *)0x40058528))
#define GPIO_PORTA_AHB_PCTL_R   (*((volatile unsigned long *)0x4005852C))
#define GPIO_PORTA_AHB_ADCCTL_R (*((volatile unsigned long *)0x40058530))
#define GPIO_PORTA_AHB_DMACTL_R (*((volatile unsigned long *)0x40058534))
#define GPIO_PORTA_AHB_SI_R     (*((volatile unsigned long *)0x40058538))

//*****************************************************************************
//
// GPIO registers (PORTB AHB)
//
//*****************************************************************************
#define GPIO_PORTB_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x40059000)
#define GPIO_PORTB_AHB_DATA_R   (*((volatile unsigned long *)0x400593FC))
#define GPIO_PORTB_AHB_DIR_R    (*((volatile unsigned long *)0x40059400))
#define GPIO_PORTB_AHB_IS_R     (*((volatile unsigned long *)0x40059404))
#define GPIO_PORTB_AHB_IBE_R    (*((volatile unsigned long *)0x40059408))
#define GPIO_PORTB_AHB_IEV_R    (*((volatile unsigned long *)0x4005940C))
#define GPIO_PORTB_AHB_IM_R     (*((volatile unsigned long *)0x40059410))
#define GPIO_PORTB_AHB_RIS_R    (*((volatile unsigned long *)0x40059414))
#define GPIO_PORTB_AHB_MIS_R    (*((volatile unsigned long *)0x40059418))
#define GPIO_PORTB_AHB_ICR_R    (*((volatile unsigned long *)0x4005941C))
#define GPIO_PORTB_AHB_AFSEL_R  (*((volatile unsigned long *)0x40059420))
#define GPIO_PORTB_AHB_DR2R_R   (*((volatile unsigned long *)0x40059500))
#define GPIO_PORTB_AHB_DR4R_R   (*((volatile unsigned long *)0x40059504))
#define GPIO_PORTB_AHB_DR8R_R   (*((volatile unsigned long *)0x40059508))
#define GPIO_PORTB_AHB_ODR_R    (*((volatile unsigned long *)0x4005950C))
#define GPIO_PORTB_AHB_PUR_R    (*((volatile unsigned long *)0x40059510))
#define GPIO_PORTB_AHB_PDR_R    (*((volatile unsigned long *)0x40059514))
#define GPIO_PORTB_AHB_SLR_R    (*((volatile unsigned long *)0x40059518))
#define GPIO_PORTB_AHB_DEN_R    (*((volatile unsigned long *)0x4005951C))
#define GPIO_PORTB_AHB_LOCK_R   (*((volatile unsigned long *)0x40059520))
#define GPIO_PORTB_AHB_CR_R     (*((volatile unsigned long *)0x40059524))
#define GPIO_PORTB_AHB_AMSEL_R  (*((volatile unsigned long *)0x40059528))
#define GPIO_PORTB_AHB_PCTL_R   (*((volatile unsigned long *)0x4005952C))
#define GPIO_PORTB_AHB_ADCCTL_R (*((volatile unsigned long *)0x40059530))
#define GPIO_PORTB_AHB_DMACTL_R (*((volatile unsigned long *)0x40059534))
#define GPIO_PORTB_AHB_SI_R     (*((volatile unsigned long *)0x40059538))

//*****************************************************************************
//
// GPIO registers (PORTC AHB)
//
//*****************************************************************************
#define GPIO_PORTC_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x4005A000)
#define GPIO_PORTC_AHB_DATA_R   (*((volatile unsigned long *)0x4005A3FC))
#define GPIO_PORTC_AHB_DIR_R    (*((volatile unsigned long *)0x4005A400))
#define GPIO_PORTC_AHB_IS_R     (*((volatile unsigned long *)0x4005A404))
#define GPIO_PORTC_AHB_IBE_R    (*((volatile unsigned long *)0x4005A408))
#define GPIO_PORTC_AHB_IEV_R    (*((volatile unsigned long *)0x4005A40C))
#define GPIO_PORTC_AHB_IM_R     (*((volatile unsigned long *)0x4005A410))
#define GPIO_PORTC_AHB_RIS_R    (*((volatile unsigned long *)0x4005A414))
#define GPIO_PORTC_AHB_MIS_R    (*((volatile unsigned long *)0x4005A418))
#define GPIO_PORTC_AHB_ICR_R    (*((volatile unsigned long *)0x4005A41C))
#define GPIO_PORTC_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005A420))
#define GPIO_PORTC_AHB_DR2R_R   (*((volatile unsigned long *)0x4005A500))
#define GPIO_PORTC_AHB_DR4R_R   (*((volatile unsigned long *)0x4005A504))
#define GPIO_PORTC_AHB_DR8R_R   (*((volatile unsigned long *)0x4005A508))
#define GPIO_PORTC_AHB_ODR_R    (*((volatile unsigned long *)0x4005A50C))
#define GPIO_PORTC_AHB_PUR_R    (*((volatile unsigned long *)0x4005A510))
#define GPIO_PORTC_AHB_PDR_R    (*((volatile unsigned long *)0x4005A514))
#define GPIO_PORTC_AHB_SLR_R    (*((volatile unsigned long *)0x4005A518))
#define GPIO_PORTC_AHB_DEN_R    (*((volatile unsigned long *)0x4005A51C))
#define GPIO_PORTC_AHB_LOCK_R   (*((volatile unsigned long *)0x4005A520))
#define GPIO_PORTC_AHB_CR_R     (*((volatile unsigned long *)0x4005A524))
#define GPIO_PORTC_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005A528))
#define GPIO_PORTC_AHB_PCTL_R   (*((volatile unsigned long *)0x4005A52C))
#define GPIO_PORTC_AHB_ADCCTL_R (*((volatile unsigned long *)0x4005A530))
#define GPIO_PORTC_AHB_DMACTL_R (*((volatile unsigned long *)0x4005A534))
#define GPIO_PORTC_AHB_SI_R     (*((volatile unsigned long *)0x4005A538))

//*****************************************************************************
//
// GPIO registers (PORTD AHB)
//
//*****************************************************************************
#define GPIO_PORTD_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x4005B000)
#define GPIO_PORTD_AHB_DATA_R   (*((volatile unsigned long *)0x4005B3FC))
#define GPIO_PORTD_AHB_DIR_R    (*((volatile unsigned long *)0x4005B400))
#define GPIO_PORTD_AHB_IS_R     (*((volatile unsigned long *)0x4005B404))
#define GPIO_PORTD_AHB_IBE_R    (*((volatile unsigned long *)0x4005B408))
#define GPIO_PORTD_AHB_IEV_R    (*((volatile unsigned long *)0x4005B40C))
#define GPIO_PORTD_AHB_IM_R     (*((volatile unsigned long *)0x4005B410))
#define GPIO_PORTD_AHB_RIS_R    (*((volatile unsigned long *)0x4005B414))
#define GPIO_PORTD_AHB_MIS_R    (*((volatile unsigned long *)0x4005B418))
#define GPIO_PORTD_AHB_ICR_R    (*((volatile unsigned long *)0x4005B41C))
#define GPIO_PORTD_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005B420))
#define GPIO_PORTD_AHB_DR2R_R   (*((volatile unsigned long *)0x4005B500))
#define GPIO_PORTD_AHB_DR4R_R   (*((volatile unsigned long *)0x4005B504))
#define GPIO_PORTD_AHB_DR8R_R   (*((volatile unsigned long *)0x4005B508))
#define GPIO_PORTD_AHB_ODR_R    (*((volatile unsigned long *)0x4005B50C))
#define GPIO_PORTD_AHB_PUR_R    (*((volatile unsigned long *)0x4005B510))
#define GPIO_PORTD_AHB_PDR_R    (*((volatile unsigned long *)0x4005B514))
#define GPIO_PORTD_AHB_SLR_R    (*((volatile unsigned long *)0x4005B518))
#define GPIO_PORTD_AHB_DEN_R    (*((volatile unsigned long *)0x4005B51C))
#define GPIO_PORTD_AHB_LOCK_R   (*((volatile unsigned long *)0x4005B520))
#define GPIO_PORTD_AHB_CR_R     (*((volatile unsigned long *)0x4005B524))
#define GPIO_PORTD_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005B528))
#define GPIO_PORTD_AHB_PCTL_R   (*((volatile unsigned long *)0x4005B52C))
#define GPIO_PORTD_AHB_ADCCTL_R (*((volatile unsigned long *)0x4005B530))
#define GPIO_PORTD_AHB_DMACTL_R (*((volatile unsigned long *)0x4005B534))
#define GPIO_PORTD_AHB_SI_R     (*((volatile unsigned long *)0x4005B538))

//*****************************************************************************
//
// GPIO registers (PORTE AHB)
//
//*****************************************************************************
#define GPIO_PORTE_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x4005C000)
#define GPIO_PORTE_AHB_DATA_R   (*((volatile unsigned long *)0x4005C3FC))
#define GPIO_PORTE_AHB_DIR_R    (*((volatile unsigned long *)0x4005C400))
#define GPIO_PORTE_AHB_IS_R     (*((volatile unsigned long *)0x4005C404))
#define GPIO_PORTE_AHB_IBE_R    (*((volatile unsigned long *)0x4005C408))
#define GPIO_PORTE_AHB_IEV_R    (*((volatile unsigned long *)0x4005C40C))
#define GPIO_PORTE_AHB_IM_R     (*((volatile unsigned long *)0x4005C410))
#define GPIO_PORTE_AHB_RIS_R    (*((volatile unsigned long *)0x4005C414))
#define GPIO_PORTE_AHB_MIS_R    (*((volatile unsigned long *)0x4005C418))
#define GPIO_PORTE_AHB_ICR_R    (*((volatile unsigned long *)0x4005C41C))
#define GPIO_PORTE_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005C420))
#define GPIO_PORTE_AHB_DR2R_R   (*((volatile unsigned long *)0x4005C500))
#define GPIO_PORTE_AHB_DR4R_R   (*((volatile unsigned long *)0x4005C504))
#define GPIO_PORTE_AHB_DR8R_R   (*((volatile unsigned long *)0x4005C508))
#define GPIO_PORTE_AHB_ODR_R    (*((volatile unsigned long *)0x4005C50C))
#define GPIO_PORTE_AHB_PUR_R    (*((volatile unsigned long *)0x4005C510))
#define GPIO_PORTE_AHB_PDR_R    (*((volatile unsigned long *)0x4005C514))
#define GPIO_PORTE_AHB_SLR_R    (*((volatile unsigned long *)0x4005C518))
#define GPIO_PORTE_AHB_DEN_R    (*((volatile unsigned long *)0x4005C51C))
#define GPIO_PORTE_AHB_LOCK_R   (*((volatile unsigned long *)0x4005C520))
#define GPIO_PORTE_AHB_CR_R     (*((volatile unsigned long *)0x4005C524))
#define GPIO_PORTE_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005C528))
#define GPIO_PORTE_AHB_PCTL_R   (*((volatile unsigned long *)0x4005C52C))
#define GPIO_PORTE_AHB_ADCCTL_R (*((volatile unsigned long *)0x4005C530))
#define GPIO_PORTE_AHB_DMACTL_R (*((volatile unsigned long *)0x4005C534))
#define GPIO_PORTE_AHB_SI_R     (*((volatile unsigned long *)0x4005C538))

//*****************************************************************************
//
// GPIO registers (PORTF AHB)
//
//*****************************************************************************
#define GPIO_PORTF_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x4005D000)
#define GPIO_PORTF_AHB_DATA_R   (*((volatile unsigned long *)0x4005D3FC))
#define GPIO_PORTF_AHB_DIR_R    (*((volatile unsigned long *)0x4005D400))
#define GPIO_PORTF_AHB_IS_R     (*((volatile unsigned long *)0x4005D404))
#define GPIO_PORTF_AHB_IBE_R    (*((volatile unsigned long *)0x4005D408))
#define GPIO_PORTF_AHB_IEV_R    (*((volatile unsigned long *)0x4005D40C))
#define GPIO_PORTF_AHB_IM_R     (*((volatile unsigned long *)0x4005D410))
#define GPIO_PORTF_AHB_RIS_R    (*((volatile unsigned long *)0x4005D414))
#define GPIO_PORTF_AHB_MIS_R    (*((volatile unsigned long *)0x4005D418))
#define GPIO_PORTF_AHB_ICR_R    (*((volatile unsigned long *)0x4005D41C))
#define GPIO_PORTF_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005D420))
#define GPIO_PORTF_AHB_DR2R_R   (*((volatile unsigned long *)0x4005D500))
#define GPIO_PORTF_AHB_DR4R_R   (*((volatile unsigned long *)0x4005D504))
#define GPIO_PORTF_AHB_DR8R_R   (*((volatile unsigned long *)0x4005D508))
#define GPIO_PORTF_AHB_ODR_R    (*((volatile unsigned long *)0x4005D50C))
#define GPIO_PORTF_AHB_PUR_R    (*((volatile unsigned long *)0x4005D510))
#define GPIO_PORTF_AHB_PDR_R    (*((volatile unsigned long *)0x4005D514))
#define GPIO_PORTF_AHB_SLR_R    (*((volatile unsigned long *)0x4005D518))
#define GPIO_PORTF_AHB_DEN_R    (*((volatile unsigned long *)0x4005D51C))
#define GPIO_PORTF_AHB_LOCK_R   (*((volatile unsigned long *)0x4005D520))
#define GPIO_PORTF_AHB_CR_R     (*((volatile unsigned long *)0x4005D524))
#define GPIO_PORTF_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005D528))
#define GPIO_PORTF_AHB_PCTL_R   (*((volatile unsigned long *)0x4005D52C))
#define GPIO_PORTF_AHB_ADCCTL_R (*((volatile unsigned long *)0x4005D530))
#define GPIO_PORTF_AHB_DMACTL_R (*((volatile unsigned long *)0x4005D534))
#define GPIO_PORTF_AHB_SI_R     (*((volatile unsigned long *)0x4005D538))

//*****************************************************************************
//
// GPIO registers (PORTG AHB)
//
//*****************************************************************************
#define GPIO_PORTG_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x4005E000)
#define GPIO_PORTG_AHB_DATA_R   (*((volatile unsigned long *)0x4005E3FC))
#define GPIO_PORTG_AHB_DIR_R    (*((volatile unsigned long *)0x4005E400))
#define GPIO_PORTG_AHB_IS_R     (*((volatile unsigned long *)0x4005E404))
#define GPIO_PORTG_AHB_IBE_R    (*((volatile unsigned long *)0x4005E408))
#define GPIO_PORTG_AHB_IEV_R    (*((volatile unsigned long *)0x4005E40C))
#define GPIO_PORTG_AHB_IM_R     (*((volatile unsigned long *)0x4005E410))
#define GPIO_PORTG_AHB_RIS_R    (*((volatile unsigned long *)0x4005E414))
#define GPIO_PORTG_AHB_MIS_R    (*((volatile unsigned long *)0x4005E418))
#define GPIO_PORTG_AHB_ICR_R    (*((volatile unsigned long *)0x4005E41C))
#define GPIO_PORTG_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005E420))
#define GPIO_PORTG_AHB_DR2R_R   (*((volatile unsigned long *)0x4005E500))
#define GPIO_PORTG_AHB_DR4R_R   (*((volatile unsigned long *)0x4005E504))
#define GPIO_PORTG_AHB_DR8R_R   (*((volatile unsigned long *)0x4005E508))
#define GPIO_PORTG_AHB_ODR_R    (*((volatile unsigned long *)0x4005E50C))
#define GPIO_PORTG_AHB_PUR_R    (*((volatile unsigned long *)0x4005E510))
#define GPIO_PORTG_AHB_PDR_R    (*((volatile unsigned long *)0x4005E514))
#define GPIO_PORTG_AHB_SLR_R    (*((volatile unsigned long *)0x4005E518))
#define GPIO_PORTG_AHB_DEN_R    (*((volatile unsigned long *)0x4005E51C))
#define GPIO_PORTG_AHB_LOCK_R   (*((volatile unsigned long *)0x4005E520))
#define GPIO_PORTG_AHB_CR_R     (*((volatile unsigned long *)0x4005E524))
#define GPIO_PORTG_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005E528))
#define GPIO_PORTG_AHB_PCTL_R   (*((volatile unsigned long *)0x4005E52C))
#define GPIO_PORTG_AHB_ADCCTL_R (*((volatile unsigned long *)0x4005E530))
#define GPIO_PORTG_AHB_DMACTL_R (*((volatile unsigned long *)0x4005E534))
#define GPIO_PORTG_AHB_SI_R     (*((volatile unsigned long *)0x4005E538))

//*****************************************************************************
//
// GPIO registers (PORTH AHB)
//
//*****************************************************************************
#define GPIO_PORTH_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x4005F000)
#define GPIO_PORTH_AHB_DATA_R   (*((volatile unsigned long *)0x4005F3FC))
#define GPIO_PORTH_AHB_DIR_R    (*((volatile unsigned long *)0x4005F400))
#define GPIO_PORTH_AHB_IS_R     (*((volatile unsigned long *)0x4005F404))
#define GPIO_PORTH_AHB_IBE_R    (*((volatile unsigned long *)0x4005F408))
#define GPIO_PORTH_AHB_IEV_R    (*((volatile unsigned long *)0x4005F40C))
#define GPIO_PORTH_AHB_IM_R     (*((volatile unsigned long *)0x4005F410))
#define GPIO_PORTH_AHB_RIS_R    (*((volatile unsigned long *)0x4005F414))
#define GPIO_PORTH_AHB_MIS_R    (*((volatile unsigned long *)0x4005F418))
#define GPIO_PORTH_AHB_ICR_R    (*((volatile unsigned long *)0x4005F41C))
#define GPIO_PORTH_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005F420))
#define GPIO_PORTH_AHB_DR2R_R   (*((volatile unsigned long *)0x4005F500))
#define GPIO_PORTH_AHB_DR4R_R   (*((volatile unsigned long *)0x4005F504))
#define GPIO_PORTH_AHB_DR8R_R   (*((volatile unsigned long *)0x4005F508))
#define GPIO_PORTH_AHB_ODR_R    (*((volatile unsigned long *)0x4005F50C))
#define GPIO_PORTH_AHB_PUR_R    (*((volatile unsigned long *)0x4005F510))
#define GPIO_PORTH_AHB_PDR_R    (*((volatile unsigned long *)0x4005F514))
#define GPIO_PORTH_AHB_SLR_R    (*((volatile unsigned long *)0x4005F518))
#define GPIO_PORTH_AHB_DEN_R    (*((volatile unsigned long *)0x4005F51C))
#define GPIO_PORTH_AHB_LOCK_R   (*((volatile unsigned long *)0x4005F520))
#define GPIO_PORTH_AHB_CR_R     (*((volatile unsigned long *)0x4005F524))
#define GPIO_PORTH_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005F528))
#define GPIO_PORTH_AHB_PCTL_R   (*((volatile unsigned long *)0x4005F52C))
#define GPIO_PORTH_AHB_ADCCTL_R (*((volatile unsigned long *)0x4005F530))
#define GPIO_PORTH_AHB_DMACTL_R (*((volatile unsigned long *)0x4005F534))
#define GPIO_PORTH_AHB_SI_R     (*((volatile unsigned long *)0x4005F538))

//*****************************************************************************
//
// GPIO registers (PORTJ AHB)
//
//*****************************************************************************
#define GPIO_PORTJ_AHB_DATA_BITS_R \
                                ((volatile unsigned long *)0x40060000)
#define GPIO_PORTJ_AHB_DATA_R   (*((volatile unsigned long *)0x400603FC))
#define GPIO_PORTJ_AHB_DIR_R    (*((volatile unsigned long *)0x40060400))
#define GPIO_PORTJ_AHB_IS_R     (*((volatile unsigned long *)0x40060404))
#define GPIO_PORTJ_AHB_IBE_R    (*((volatile unsigned long *)0x40060408))
#define GPIO_PORTJ_AHB_IEV_R    (*((volatile unsigned long *)0x4006040C))
#define GPIO_PORTJ_AHB_IM_R     (*((volatile unsigned long *)0x40060410))
#define GPIO_PORTJ_AHB_RIS_R    (*((volatile unsigned long *)0x40060414))
#define GPIO_PORTJ_AHB_MIS_R    (*((volatile unsigned long *)0x40060418))
#define GPIO_PORTJ_AHB_ICR_R    (*((volatile unsigned long *)0x4006041C))
#define GPIO_PORTJ_AHB_AFSEL_R  (*((volatile unsigned long *)0x40060420))
#define GPIO_PORTJ_AHB_DR2R_R   (*((volatile unsigned long *)0x40060500))
#define GPIO_PORTJ_AHB_DR4R_R   (*((volatile unsigned long *)0x40060504))
#define GPIO_PORTJ_AHB_DR8R_R   (*((volatile unsigned long *)0x40060508))
#define GPIO_PORTJ_AHB_ODR_R    (*((volatile unsigned long *)0x4006050C))
#define GPIO_PORTJ_AHB_PUR_R    (*((volatile unsigned long *)0x40060510))
#define GPIO_PORTJ_AHB_PDR_R    (*((volatile unsigned long *)0x40060514))
#define GPIO_PORTJ_AHB_SLR_R    (*((volatile unsigned long *)0x40060518))
#define GPIO_PORTJ_AHB_DEN_R    (*((volatile unsigned long *)0x4006051C))
#define GPIO_PORTJ_AHB_LOCK_R   (*((volatile unsigned long *)0x40060520))
#define GPIO_PORTJ_AHB_CR_R     (*((volatile unsigned long *)0x40060524))
#define GPIO_PORTJ_AHB_AMSEL_R  (*((volatile unsigned long *)0x40060528))
#define GPIO_PORTJ_AHB_PCTL_R   (*((volatile unsigned long *)0x4006052C))
#define GPIO_PORTJ_AHB_ADCCTL_R (*((volatile unsigned long *)0x40060530))
#define GPIO_PORTJ_AHB_DMACTL_R (*((volatile unsigned long *)0x40060534))
#define GPIO_PORTJ_AHB_SI_R     (*((volatile unsigned long *)0x40060538))

//*****************************************************************************
//
// GPIO registers (PORTK)
//
//*****************************************************************************
#define GPIO_PORTK_DATA_BITS_R  ((volatile unsigned long *)0x40061000)
#define GPIO_PORTK_DATA_R       (*((volatile unsigned long *)0x400613FC))
#define GPIO_PORTK_DIR_R        (*((volatile unsigned long *)0x40061400))
#define GPIO_PORTK_IS_R         (*((volatile unsigned long *)0x40061404))
#define GPIO_PORTK_IBE_R        (*((volatile unsigned long *)0x40061408))
#define GPIO_PORTK_IEV_R        (*((volatile unsigned long *)0x4006140C))
#define GPIO_PORTK_IM_R         (*((volatile unsigned long *)0x40061410))
#define GPIO_PORTK_RIS_R        (*((volatile unsigned long *)0x40061414))
#define GPIO_PORTK_MIS_R        (*((volatile unsigned long *)0x40061418))
#define GPIO_PORTK_ICR_R        (*((volatile unsigned long *)0x4006141C))
#define GPIO_PORTK_AFSEL_R      (*((volatile unsigned long *)0x40061420))
#define GPIO_PORTK_DR2R_R       (*((volatile unsigned long *)0x40061500))
#define GPIO_PORTK_DR4R_R       (*((volatile unsigned long *)0x40061504))
#define GPIO_PORTK_DR8R_R       (*((volatile unsigned long *)0x40061508))
#define GPIO_PORTK_ODR_R        (*((volatile unsigned long *)0x4006150C))
#define GPIO_PORTK_PUR_R        (*((volatile unsigned long *)0x40061510))
#define GPIO_PORTK_PDR_R        (*((volatile unsigned long *)0x40061514))
#define GPIO_PORTK_SLR_R        (*((volatile unsigned long *)0x40061518))
#define GPIO_PORTK_DEN_R        (*((volatile unsigned long *)0x4006151C))
#define GPIO_PORTK_LOCK_R       (*((volatile unsigned long *)0x40061520))
#define GPIO_PORTK_CR_R         (*((volatile unsigned long *)0x40061524))
#define GPIO_PORTK_AMSEL_R      (*((volatile unsigned long *)0x40061528))
#define GPIO_PORTK_PCTL_R       (*((volatile unsigned long *)0x4006152C))
#define GPIO_PORTK_ADCCTL_R     (*((volatile unsigned long *)0x40061530))
#define GPIO_PORTK_DMACTL_R     (*((volatile unsigned long *)0x40061534))
#define GPIO_PORTK_SI_R         (*((volatile unsigned long *)0x40061538))

//*****************************************************************************
//
// GPIO registers (PORTL)
//
//*****************************************************************************
#define GPIO_PORTL_DATA_BITS_R  ((volatile unsigned long *)0x40062000)
#define GPIO_PORTL_DATA_R       (*((volatile unsigned long *)0x400623FC))
#define GPIO_PORTL_DIR_R        (*((volatile unsigned long *)0x40062400))
#define GPIO_PORTL_IS_R         (*((volatile unsigned long *)0x40062404))
#define GPIO_PORTL_IBE_R        (*((volatile unsigned long *)0x40062408))
#define GPIO_PORTL_IEV_R        (*((volatile unsigned long *)0x4006240C))
#define GPIO_PORTL_IM_R         (*((volatile unsigned long *)0x40062410))
#define GPIO_PORTL_RIS_R        (*((volatile unsigned long *)0x40062414))
#define GPIO_PORTL_MIS_R        (*((volatile unsigned long *)0x40062418))
#define GPIO_PORTL_ICR_R        (*((volatile unsigned long *)0x4006241C))
#define GPIO_PORTL_AFSEL_R      (*((volatile unsigned long *)0x40062420))
#define GPIO_PORTL_DR2R_R       (*((volatile unsigned long *)0x40062500))
#define GPIO_PORTL_DR4R_R       (*((volatile unsigned long *)0x40062504))
#define GPIO_PORTL_DR8R_R       (*((volatile unsigned long *)0x40062508))
#define GPIO_PORTL_ODR_R        (*((volatile unsigned long *)0x4006250C))
#define GPIO_PORTL_PUR_R        (*((volatile unsigned long *)0x40062510))
#define GPIO_PORTL_PDR_R        (*((volatile unsigned long *)0x40062514))
#define GPIO_PORTL_SLR_R        (*((volatile unsigned long *)0x40062518))
#define GPIO_PORTL_DEN_R        (*((volatile unsigned long *)0x4006251C))
#define GPIO_PORTL_LOCK_R       (*((volatile unsigned long *)0x40062520))
#define GPIO_PORTL_CR_R         (*((volatile unsigned long *)0x40062524))
#define GPIO_PORTL_AMSEL_R      (*((volatile unsigned long *)0x40062528))
#define GPIO_PORTL_PCTL_R       (*((volatile unsigned long *)0x4006252C))
#define GPIO_PORTL_ADCCTL_R     (*((volatile unsigned long *)0x40062530))
#define GPIO_PORTL_DMACTL_R     (*((volatile unsigned long *)0x40062534))
#define GPIO_PORTL_SI_R         (*((volatile unsigned long *)0x40062538))

//*****************************************************************************
//
// GPIO registers (PORTM)
//
//*****************************************************************************
#define GPIO_PORTM_DATA_BITS_R  ((volatile unsigned long *)0x40063000)
#define GPIO_PORTM_DATA_R       (*((volatile unsigned long *)0x400633FC))
#define GPIO_PORTM_DIR_R        (*((volatile unsigned long *)0x40063400))
#define GPIO_PORTM_IS_R         (*((volatile unsigned long *)0x40063404))
#define GPIO_PORTM_IBE_R        (*((volatile unsigned long *)0x40063408))
#define GPIO_PORTM_IEV_R        (*((volatile unsigned long *)0x4006340C))
#define GPIO_PORTM_IM_R         (*((volatile unsigned long *)0x40063410))
#define GPIO_PORTM_RIS_R        (*((volatile unsigned long *)0x40063414))
#define GPIO_PORTM_MIS_R        (*((volatile unsigned long *)0x40063418))
#define GPIO_PORTM_ICR_R        (*((volatile unsigned long *)0x4006341C))
#define GPIO_PORTM_AFSEL_R      (*((volatile unsigned long *)0x40063420))
#define GPIO_PORTM_DR2R_R       (*((volatile unsigned long *)0x40063500))
#define GPIO_PORTM_DR4R_R       (*((volatile unsigned long *)0x40063504))
#define GPIO_PORTM_DR8R_R       (*((volatile unsigned long *)0x40063508))
#define GPIO_PORTM_ODR_R        (*((volatile unsigned long *)0x4006350C))
#define GPIO_PORTM_PUR_R        (*((volatile unsigned long *)0x40063510))
#define GPIO_PORTM_PDR_R        (*((volatile unsigned long *)0x40063514))
#define GPIO_PORTM_SLR_R        (*((volatile unsigned long *)0x40063518))
#define GPIO_PORTM_DEN_R        (*((volatile unsigned long *)0x4006351C))
#define GPIO_PORTM_LOCK_R       (*((volatile unsigned long *)0x40063520))
#define GPIO_PORTM_CR_R         (*((volatile unsigned long *)0x40063524))
#define GPIO_PORTM_AMSEL_R      (*((volatile unsigned long *)0x40063528))
#define GPIO_PORTM_PCTL_R       (*((volatile unsigned long *)0x4006352C))
#define GPIO_PORTM_ADCCTL_R     (*((volatile unsigned long *)0x40063530))
#define GPIO_PORTM_DMACTL_R     (*((volatile unsigned long *)0x40063534))
#define GPIO_PORTM_SI_R         (*((volatile unsigned long *)0x40063538))

//*****************************************************************************
//
// GPIO registers (PORTN)
//
//*****************************************************************************
#define GPIO_PORTN_DATA_BITS_R  ((volatile unsigned long *)0x40064000)
#define GPIO_PORTN_DATA_R       (*((volatile unsigned long *)0x400643FC))
#define GPIO_PORTN_DIR_R        (*((volatile unsigned long *)0x40064400))
#define GPIO_PORTN_IS_R         (*((volatile unsigned long *)0x40064404))
#define GPIO_PORTN_IBE_R        (*((volatile unsigned long *)0x40064408))
#define GPIO_PORTN_IEV_R        (*((volatile unsigned long *)0x4006440C))
#define GPIO_PORTN_IM_R         (*((volatile unsigned long *)0x40064410))
#define GPIO_PORTN_RIS_R        (*((volatile unsigned long *)0x40064414))
#define GPIO_PORTN_MIS_R        (*((volatile unsigned long *)0x40064418))
#define GPIO_PORTN_ICR_R        (*((volatile unsigned long *)0x4006441C))
#define GPIO_PORTN_AFSEL_R      (*((volatile unsigned long *)0x40064420))
#define GPIO_PORTN_DR2R_R       (*((volatile unsigned long *)0x40064500))
#define GPIO_PORTN_DR4R_R       (*((volatile unsigned long *)0x40064504))
#define GPIO_PORTN_DR8R_R       (*((volatile unsigned long *)0x40064508))
#define GPIO_PORTN_ODR_R        (*((volatile unsigned long *)0x4006450C))
#define GPIO_PORTN_PUR_R        (*((volatile unsigned long *)0x40064510))
#define GPIO_PORTN_PDR_R        (*((volatile unsigned long *)0x40064514))
#define GPIO_PORTN_SLR_R        (*((volatile unsigned long *)0x40064518))
#define GPIO_PORTN_DEN_R        (*((volatile unsigned long *)0x4006451C))
#define GPIO_PORTN_LOCK_R       (*((volatile unsigned long *)0x40064520))
#define GPIO_PORTN_CR_R         (*((volatile unsigned long *)0x40064524))
#define GPIO_PORTN_AMSEL_R      (*((volatile unsigned long *)0x40064528))
#define GPIO_PORTN_PCTL_R       (*((volatile unsigned long *)0x4006452C))
#define GPIO_PORTN_ADCCTL_R     (*((volatile unsigned long *)0x40064530))
#define GPIO_PORTN_DMACTL_R     (*((volatile unsigned long *)0x40064534))
#define GPIO_PORTN_SI_R         (*((volatile unsigned long *)0x40064538))

//*****************************************************************************
//
// GPIO registers (PORTP)
//
//*****************************************************************************
#define GPIO_PORTP_DATA_BITS_R  ((volatile unsigned long *)0x40065000)
#define GPIO_PORTP_DATA_R       (*((volatile unsigned long *)0x400653FC))
#define GPIO_PORTP_DIR_R        (*((volatile unsigned long *)0x40065400))
#define GPIO_PORTP_IS_R         (*((volatile unsigned long *)0x40065404))
#define GPIO_PORTP_IBE_R        (*((volatile unsigned long *)0x40065408))
#define GPIO_PORTP_IEV_R        (*((volatile unsigned long *)0x4006540C))
#define GPIO_PORTP_IM_R         (*((volatile unsigned long *)0x40065410))
#define GPIO_PORTP_RIS_R        (*((volatile unsigned long *)0x40065414))
#define GPIO_PORTP_MIS_R        (*((volatile unsigned long *)0x40065418))
#define GPIO_PORTP_ICR_R        (*((volatile unsigned long *)0x4006541C))
#define GPIO_PORTP_AFSEL_R      (*((volatile unsigned long *)0x40065420))
#define GPIO_PORTP_DR2R_R       (*((volatile unsigned long *)0x40065500))
#define GPIO_PORTP_DR4R_R       (*((volatile unsigned long *)0x40065504))
#define GPIO_PORTP_DR8R_R       (*((volatile unsigned long *)0x40065508))
#define GPIO_PORTP_ODR_R        (*((volatile unsigned long *)0x4006550C))
#define GPIO_PORTP_PUR_R        (*((volatile unsigned long *)0x40065510))
#define GPIO_PORTP_PDR_R        (*((volatile unsigned long *)0x40065514))
#define GPIO_PORTP_SLR_R        (*((volatile unsigned long *)0x40065518))
#define GPIO_PORTP_DEN_R        (*((volatile unsigned long *)0x4006551C))
#define GPIO_PORTP_LOCK_R       (*((volatile unsigned long *)0x40065520))
#define GPIO_PORTP_CR_R         (*((volatile unsigned long *)0x40065524))
#define GPIO_PORTP_AMSEL_R      (*((volatile unsigned long *)0x40065528))
#define GPIO_PORTP_PCTL_R       (*((volatile unsigned long *)0x4006552C))
#define GPIO_PORTP_ADCCTL_R     (*((volatile unsigned long *)0x40065530))
#define GPIO_PORTP_DMACTL_R     (*((volatile unsigned long *)0x40065534))
#define GPIO_PORTP_SI_R         (*((volatile unsigned long *)0x40065538))

//*****************************************************************************
//
// GPIO registers (PORTQ)
//
//*****************************************************************************
#define GPIO_PORTQ_DATA_BITS_R  ((volatile unsigned long *)0x40066000)
#define GPIO_PORTQ_DATA_R       (*((volatile unsigned long *)0x400663FC))
#define GPIO_PORTQ_DIR_R        (*((volatile unsigned long *)0x40066400))
#define GPIO_PORTQ_IS_R         (*((volatile unsigned long *)0x40066404))
#define GPIO_PORTQ_IBE_R        (*((volatile unsigned long *)0x40066408))
#define GPIO_PORTQ_IEV_R        (*((volatile unsigned long *)0x4006640C))
#define GPIO_PORTQ_IM_R         (*((volatile unsigned long *)0x40066410))
#define GPIO_PORTQ_RIS_R        (*((volatile unsigned long *)0x40066414))
#define GPIO_PORTQ_MIS_R        (*((volatile unsigned long *)0x40066418))
#define GPIO_PORTQ_ICR_R        (*((volatile unsigned long *)0x4006641C))
#define GPIO_PORTQ_AFSEL_R      (*((volatile unsigned long *)0x40066420))
#define GPIO_PORTQ_DR2R_R       (*((volatile unsigned long *)0x40066500))
#define GPIO_PORTQ_DR4R_R       (*((volatile unsigned long *)0x40066504))
#define GPIO_PORTQ_DR8R_R       (*((volatile unsigned long *)0x40066508))
#define GPIO_PORTQ_ODR_R        (*((volatile unsigned long *)0x4006650C))
#define GPIO_PORTQ_PUR_R        (*((volatile unsigned long *)0x40066510))
#define GPIO_PORTQ_PDR_R        (*((volatile unsigned long *)0x40066514))
#define GPIO_PORTQ_SLR_R        (*((volatile unsigned long *)0x40066518))
#define GPIO_PORTQ_DEN_R        (*((volatile unsigned long *)0x4006651C))
#define GPIO_PORTQ_LOCK_R       (*((volatile unsigned long *)0x40066520))
#define GPIO_PORTQ_CR_R         (*((volatile unsigned long *)0x40066524))
#define GPIO_PORTQ_AMSEL_R      (*((volatile unsigned long *)0x40066528))
#define GPIO_PORTQ_PCTL_R       (*((volatile unsigned long *)0x4006652C))
#define GPIO_PORTQ_ADCCTL_R     (*((volatile unsigned long *)0x40066530))
#define GPIO_PORTQ_DMACTL_R     (*((volatile unsigned long *)0x40066534))
#define GPIO_PORTQ_SI_R         (*((volatile unsigned long *)0x40066538))

//*****************************************************************************
//
// LPC registers (LPC0)
//
//*****************************************************************************
#define LPC0_CTL_R              (*((volatile unsigned long *)0x40080000))
#define LPC0_STS_R              (*((volatile unsigned long *)0x40080004))
#define LPC0_IRQCTL_R           (*((volatile unsigned long *)0x40080008))
#define LPC0_IRQST_R            (*((volatile unsigned long *)0x4008000C))
#define LPC0_CH0CTL_R           (*((volatile unsigned long *)0x40080010))
#define LPC0_CH0ST_R            (*((volatile unsigned long *)0x40080014))
#define LPC0_CH0ADR_R           (*((volatile unsigned long *)0x40080018))
#define LPC0_CH1CTL_R           (*((volatile unsigned long *)0x40080020))
#define LPC0_CH1ST_R            (*((volatile unsigned long *)0x40080024))
#define LPC0_CH1ADR_R           (*((volatile unsigned long *)0x40080028))
#define LPC0_CH2CTL_R           (*((volatile unsigned long *)0x40080030))
#define LPC0_CH2ST_R            (*((volatile unsigned long *)0x40080034))
#define LPC0_CH2ADR_R           (*((volatile unsigned long *)0x40080038))
#define LPC0_CH3CTL_R           (*((volatile unsigned long *)0x40080040))
#define LPC0_CH3ST_R            (*((volatile unsigned long *)0x40080044))
#define LPC0_CH3ADR_R           (*((volatile unsigned long *)0x40080048))
#define LPC0_CH4CTL_R           (*((volatile unsigned long *)0x40080050))
#define LPC0_CH4ST_R            (*((volatile unsigned long *)0x40080054))
#define LPC0_CH4ADR_R           (*((volatile unsigned long *)0x40080058))
#define LPC0_CH5CTL_R           (*((volatile unsigned long *)0x40080060))
#define LPC0_CH5ST_R            (*((volatile unsigned long *)0x40080064))
#define LPC0_CH5ADR_R           (*((volatile unsigned long *)0x40080068))
#define LPC0_CH6CTL_R           (*((volatile unsigned long *)0x40080070))
#define LPC0_CH6ST_R            (*((volatile unsigned long *)0x40080074))
#define LPC0_CH6ADR_R           (*((volatile unsigned long *)0x40080078))
#define LPC0_CH7CTL_R           (*((volatile unsigned long *)0x40080080))
#define LPC0_CH7ST_R            (*((volatile unsigned long *)0x40080084))
#define LPC0_CH7ADR_R           (*((volatile unsigned long *)0x40080088))
#define LPC0_STSADDR_R          (*((volatile unsigned long *)0x400800A0))
#define LPC0_IM_R               (*((volatile unsigned long *)0x40080100))
#define LPC0_RIS_R              (*((volatile unsigned long *)0x40080104))
#define LPC0_MIS_R              (*((volatile unsigned long *)0x40080108))
#define LPC0_IC_R               (*((volatile unsigned long *)0x4008010C))
#define LPC0_DMACX_R            (*((volatile unsigned long *)0x40080120))
#define LPC0_POOL_R             (*((volatile unsigned long *)0x40080400))
#define LPC0_PP_R               (*((volatile unsigned long *)0x40080FC0))

//*****************************************************************************
//
// Fan Control registers (FAN0)
//
//*****************************************************************************
#define FAN0_STS_R              (*((volatile unsigned long *)0x40084000))
#define FAN0_CTL_R              (*((volatile unsigned long *)0x40084004))
#define FAN0_CH0_R              (*((volatile unsigned long *)0x40084010))
#define FAN0_CMD0_R             (*((volatile unsigned long *)0x40084014))
#define FAN0_CST0_R             (*((volatile unsigned long *)0x40084018))
#define FAN0_CH1_R              (*((volatile unsigned long *)0x40084020))
#define FAN0_CMD1_R             (*((volatile unsigned long *)0x40084024))
#define FAN0_CST1_R             (*((volatile unsigned long *)0x40084028))
#define FAN0_CH2_R              (*((volatile unsigned long *)0x40084030))
#define FAN0_CMD2_R             (*((volatile unsigned long *)0x40084034))
#define FAN0_CST2_R             (*((volatile unsigned long *)0x40084038))
#define FAN0_CH3_R              (*((volatile unsigned long *)0x40084040))
#define FAN0_CMD3_R             (*((volatile unsigned long *)0x40084044))
#define FAN0_CST3_R             (*((volatile unsigned long *)0x40084048))
#define FAN0_CH4_R              (*((volatile unsigned long *)0x40084050))
#define FAN0_CMD4_R             (*((volatile unsigned long *)0x40084054))
#define FAN0_CST4_R             (*((volatile unsigned long *)0x40084058))
#define FAN0_CH5_R              (*((volatile unsigned long *)0x40084060))
#define FAN0_CMD5_R             (*((volatile unsigned long *)0x40084064))
#define FAN0_CST5_R             (*((volatile unsigned long *)0x40084068))
#define FAN0_IM_R               (*((volatile unsigned long *)0x40084090))
#define FAN0_RIS_R              (*((volatile unsigned long *)0x40084094))
#define FAN0_MIS_R              (*((volatile unsigned long *)0x40084098))
#define FAN0_IC_R               (*((volatile unsigned long *)0x4008409C))
#define FAN0_PP_R               (*((volatile unsigned long *)0x40084FC0))

//*****************************************************************************
//
// EEPROM registers (EEPROM)
//
//*****************************************************************************
#define EEPROM_EESIZE_R         (*((volatile unsigned long *)0x400AF000))
#define EEPROM_EEBLOCK_R        (*((volatile unsigned long *)0x400AF004))
#define EEPROM_EEOFFSET_R       (*((volatile unsigned long *)0x400AF008))
#define EEPROM_EERDWR_R         (*((volatile unsigned long *)0x400AF010))
#define EEPROM_EERDWRINC_R      (*((volatile unsigned long *)0x400AF014))
#define EEPROM_EEDONE_R         (*((volatile unsigned long *)0x400AF018))
#define EEPROM_EESUPP_R         (*((volatile unsigned long *)0x400AF01C))
#define EEPROM_EEUNLOCK_R       (*((volatile unsigned long *)0x400AF020))
#define EEPROM_EEPROT_R         (*((volatile unsigned long *)0x400AF030))
#define EEPROM_EEPASS0_R        (*((volatile unsigned long *)0x400AF034))
#define EEPROM_EEPASS1_R        (*((volatile unsigned long *)0x400AF038))
#define EEPROM_EEPASS2_R        (*((volatile unsigned long *)0x400AF03C))
#define EEPROM_EEINT_R          (*((volatile unsigned long *)0x400AF040))
#define EEPROM_EEHIDE_R         (*((volatile unsigned long *)0x400AF050))
#define EEPROM_EEDBGME_R        (*((volatile unsigned long *)0x400AF080))
#define EEPROM_PP_R             (*((volatile unsigned long *)0x400AFFC0))

//*****************************************************************************
//
// PECI registers (PECI0)
//
//*****************************************************************************
#define PECI0_CTL_R             (*((volatile unsigned long *)0x400B0000))
#define PECI0_DIV_R             (*((volatile unsigned long *)0x400B0004))
#define PECI0_M0D0C_R           (*((volatile unsigned long *)0x400B0010))
#define PECI0_M0D1C_R           (*((volatile unsigned long *)0x400B0014))
#define PECI0_M1D0C_R           (*((volatile unsigned long *)0x400B0018))
#define PECI0_M1D1C_R           (*((volatile unsigned long *)0x400B001C))
#define PECI0_M0D0_R            (*((volatile unsigned long *)0x400B0040))
#define PECI0_M0D1_R            (*((volatile unsigned long *)0x400B0044))
#define PECI0_M1D0_R            (*((volatile unsigned long *)0x400B0048))
#define PECI0_M1D1_R            (*((volatile unsigned long *)0x400B004C))
#define PECI0_IM_R              (*((volatile unsigned long *)0x400B0080))
#define PECI0_RIS_R             (*((volatile unsigned long *)0x400B0084))
#define PECI0_MIS_R             (*((volatile unsigned long *)0x400B0088))
#define PECI0_IC_R              (*((volatile unsigned long *)0x400B008C))
#define PECI0_ACADDR_R          (*((volatile unsigned long *)0x400B0100))
#define PECI0_ACARG_R           (*((volatile unsigned long *)0x400B0104))
#define PECI0_ACRDWR0_R         (*((volatile unsigned long *)0x400B0108))
#define PECI0_ACRDWR1_R         (*((volatile unsigned long *)0x400B010C))
#define PECI0_ACCMD_R           (*((volatile unsigned long *)0x400B0110))
#define PECI0_ACCODE_R          (*((volatile unsigned long *)0x400B0114))

//*****************************************************************************
//
// I2C registers (I2C4 MASTER)
//
//*****************************************************************************
#define I2C4_MASTER_MSA_R       (*((volatile unsigned long *)0x400C0000))
#define I2C4_MASTER_MCS_R       (*((volatile unsigned long *)0x400C0004))
#define I2C4_MASTER_MDR_R       (*((volatile unsigned long *)0x400C0008))
#define I2C4_MASTER_MTPR_R      (*((volatile unsigned long *)0x400C000C))
#define I2C4_MASTER_MIMR_R      (*((volatile unsigned long *)0x400C0010))
#define I2C4_MASTER_MRIS_R      (*((volatile unsigned long *)0x400C0014))
#define I2C4_MASTER_MMIS_R      (*((volatile unsigned long *)0x400C0018))
#define I2C4_MASTER_MICR_R      (*((volatile unsigned long *)0x400C001C))
#define I2C4_MASTER_MCR_R       (*((volatile unsigned long *)0x400C0020))
#define I2C4_MASTER_MCLKOCNT_R  (*((volatile unsigned long *)0x400C0024))
#define I2C4_MASTER_MBMON_R     (*((volatile unsigned long *)0x400C002C))

//*****************************************************************************
//
// I2C registers (I2C4 SLAVE)
//
//*****************************************************************************
#define I2C4_SLAVE_SOAR_R       (*((volatile unsigned long *)0x400C0800))
#define I2C4_SLAVE_SCSR_R       (*((volatile unsigned long *)0x400C0804))
#define I2C4_SLAVE_SDR_R        (*((volatile unsigned long *)0x400C0808))
#define I2C4_SLAVE_SIMR_R       (*((volatile unsigned long *)0x400C080C))
#define I2C4_SLAVE_SRIS_R       (*((volatile unsigned long *)0x400C0810))
#define I2C4_SLAVE_SMIS_R       (*((volatile unsigned long *)0x400C0814))
#define I2C4_SLAVE_SICR_R       (*((volatile unsigned long *)0x400C0818))
#define I2C4_SLAVE_SOAR2_R      (*((volatile unsigned long *)0x400C081C))
#define I2C4_SLAVE_SACKCTL_R    (*((volatile unsigned long *)0x400C0820))

//*****************************************************************************
//
// I2C registers (I2C5 MASTER)
//
//*****************************************************************************
#define I2C5_MASTER_MSA_R       (*((volatile unsigned long *)0x400C1000))
#define I2C5_MASTER_MCS_R       (*((volatile unsigned long *)0x400C1004))
#define I2C5_MASTER_MDR_R       (*((volatile unsigned long *)0x400C1008))
#define I2C5_MASTER_MTPR_R      (*((volatile unsigned long *)0x400C100C))
#define I2C5_MASTER_MIMR_R      (*((volatile unsigned long *)0x400C1010))
#define I2C5_MASTER_MRIS_R      (*((volatile unsigned long *)0x400C1014))
#define I2C5_MASTER_MMIS_R      (*((volatile unsigned long *)0x400C1018))
#define I2C5_MASTER_MICR_R      (*((volatile unsigned long *)0x400C101C))
#define I2C5_MASTER_MCR_R       (*((volatile unsigned long *)0x400C1020))
#define I2C5_MASTER_MCLKOCNT_R  (*((volatile unsigned long *)0x400C1024))
#define I2C5_MASTER_MBMON_R     (*((volatile unsigned long *)0x400C102C))

//*****************************************************************************
//
// I2C registers (I2C5 SLAVE)
//
//*****************************************************************************
#define I2C5_SLAVE_SOAR_R       (*((volatile unsigned long *)0x400C1800))
#define I2C5_SLAVE_SCSR_R       (*((volatile unsigned long *)0x400C1804))
#define I2C5_SLAVE_SDR_R        (*((volatile unsigned long *)0x400C1808))
#define I2C5_SLAVE_SIMR_R       (*((volatile unsigned long *)0x400C180C))
#define I2C5_SLAVE_SRIS_R       (*((volatile unsigned long *)0x400C1810))
#define I2C5_SLAVE_SMIS_R       (*((volatile unsigned long *)0x400C1814))
#define I2C5_SLAVE_SICR_R       (*((volatile unsigned long *)0x400C1818))
#define I2C5_SLAVE_SOAR2_R      (*((volatile unsigned long *)0x400C181C))
#define I2C5_SLAVE_SACKCTL_R    (*((volatile unsigned long *)0x400C1820))

//*****************************************************************************
//
// System Exception Module registers (SYSEXC)
//
//*****************************************************************************
#define SYSEXC_RIS_R            (*((volatile unsigned long *)0x400F9000))
#define SYSEXC_IM_R             (*((volatile unsigned long *)0x400F9004))
#define SYSEXC_MIS_R            (*((volatile unsigned long *)0x400F9008))
#define SYSEXC_IC_R             (*((volatile unsigned long *)0x400F900C))

//*****************************************************************************
//
// Hibernation module registers (HIB)
//
//*****************************************************************************
#define HIB_RTCC_R              (*((volatile unsigned long *)0x400FC000))
#define HIB_RTCM0_R             (*((volatile unsigned long *)0x400FC004))
#define HIB_RTCLD_R             (*((volatile unsigned long *)0x400FC00C))
#define HIB_CTL_R               (*((volatile unsigned long *)0x400FC010))
#define HIB_IM_R                (*((volatile unsigned long *)0x400FC014))
#define HIB_RIS_R               (*((volatile unsigned long *)0x400FC018))
#define HIB_MIS_R               (*((volatile unsigned long *)0x400FC01C))
#define HIB_IC_R                (*((volatile unsigned long *)0x400FC020))
#define HIB_RTCT_R              (*((volatile unsigned long *)0x400FC024))
#define HIB_RTCSS_R             (*((volatile unsigned long *)0x400FC028))
#define HIB_DATA_R              (*((volatile unsigned long *)0x400FC030))

//*****************************************************************************
//
// FLASH registers (FLASH CTRL)
//
//*****************************************************************************
#define FLASH_FMA_R             (*((volatile unsigned long *)0x400FD000))
#define FLASH_FMD_R             (*((volatile unsigned long *)0x400FD004))
#define FLASH_FMC_R             (*((volatile unsigned long *)0x400FD008))
#define FLASH_FCRIS_R           (*((volatile unsigned long *)0x400FD00C))
#define FLASH_FCIM_R            (*((volatile unsigned long *)0x400FD010))
#define FLASH_FCMISC_R          (*((volatile unsigned long *)0x400FD014))
#define FLASH_FMC2_R            (*((volatile unsigned long *)0x400FD020))
#define FLASH_FWBVAL_R          (*((volatile unsigned long *)0x400FD030))
#define FLASH_FWBN_R            (*((volatile unsigned long *)0x400FD100))
#define FLASH_FSIZE_R           (*((volatile unsigned long *)0x400FDFC0))
#define FLASH_SSIZE_R           (*((volatile unsigned long *)0x400FDFC4))
#define FLASH_ROMSWMAP_R        (*((volatile unsigned long *)0x400FDFCC))
#define FLASH_RMCTL_R           (*((volatile unsigned long *)0x400FE0F0))
#define FLASH_BOOTCFG_R         (*((volatile unsigned long *)0x400FE1D0))
#define FLASH_USERREG0_R        (*((volatile unsigned long *)0x400FE1E0))
#define FLASH_USERREG1_R        (*((volatile unsigned long *)0x400FE1E4))
#define FLASH_USERREG2_R        (*((volatile unsigned long *)0x400FE1E8))
#define FLASH_USERREG3_R        (*((volatile unsigned long *)0x400FE1EC))
#define FLASH_FMPRE0_R          (*((volatile unsigned long *)0x400FE200))
#define FLASH_FMPRE1_R          (*((volatile unsigned long *)0x400FE204))
#define FLASH_FMPRE2_R          (*((volatile unsigned long *)0x400FE208))
#define FLASH_FMPRE3_R          (*((volatile unsigned long *)0x400FE20C))
#define FLASH_FMPPE0_R          (*((volatile unsigned long *)0x400FE400))
#define FLASH_FMPPE1_R          (*((volatile unsigned long *)0x400FE404))
#define FLASH_FMPPE2_R          (*((volatile unsigned long *)0x400FE408))
#define FLASH_FMPPE3_R          (*((volatile unsigned long *)0x400FE40C))

//*****************************************************************************
//
// System Control registers (SYSCTL)
//
//*****************************************************************************
#define SYSCTL_DID0_R           (*((volatile unsigned long *)0x400FE000))
#define SYSCTL_DID1_R           (*((volatile unsigned long *)0x400FE004))
#define SYSCTL_DC0_R            (*((volatile unsigned long *)0x400FE008))
#define SYSCTL_DC1_R            (*((volatile unsigned long *)0x400FE010))
#define SYSCTL_DC2_R            (*((volatile unsigned long *)0x400FE014))
#define SYSCTL_DC3_R            (*((volatile unsigned long *)0x400FE018))
#define SYSCTL_DC4_R            (*((volatile unsigned long *)0x400FE01C))
#define SYSCTL_DC5_R            (*((volatile unsigned long *)0x400FE020))
#define SYSCTL_DC6_R            (*((volatile unsigned long *)0x400FE024))
#define SYSCTL_DC7_R            (*((volatile unsigned long *)0x400FE028))
#define SYSCTL_DC8_R            (*((volatile unsigned long *)0x400FE02C))
#define SYSCTL_PBORCTL_R        (*((volatile unsigned long *)0x400FE030))
#define SYSCTL_SRCR0_R          (*((volatile unsigned long *)0x400FE040))
#define SYSCTL_SRCR1_R          (*((volatile unsigned long *)0x400FE044))
#define SYSCTL_SRCR2_R          (*((volatile unsigned long *)0x400FE048))
#define SYSCTL_RIS_R            (*((volatile unsigned long *)0x400FE050))
#define SYSCTL_IMC_R            (*((volatile unsigned long *)0x400FE054))
#define SYSCTL_MISC_R           (*((volatile unsigned long *)0x400FE058))
#define SYSCTL_RESC_R           (*((volatile unsigned long *)0x400FE05C))
#define SYSCTL_RCC_R            (*((volatile unsigned long *)0x400FE060))
#define SYSCTL_GPIOHBCTL_R      (*((volatile unsigned long *)0x400FE06C))
#define SYSCTL_RCC2_R           (*((volatile unsigned long *)0x400FE070))
#define SYSCTL_MOSCCTL_R        (*((volatile unsigned long *)0x400FE07C))
#define SYSCTL_RCGC0_R          (*((volatile unsigned long *)0x400FE100))
#define SYSCTL_RCGC1_R          (*((volatile unsigned long *)0x400FE104))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_SCGC0_R          (*((volatile unsigned long *)0x400FE110))
#define SYSCTL_SCGC1_R          (*((volatile unsigned long *)0x400FE114))
#define SYSCTL_SCGC2_R          (*((volatile unsigned long *)0x400FE118))
#define SYSCTL_DCGC0_R          (*((volatile unsigned long *)0x400FE120))
#define SYSCTL_DCGC1_R          (*((volatile unsigned long *)0x400FE124))
#define SYSCTL_DCGC2_R          (*((volatile unsigned long *)0x400FE128))
#define SYSCTL_DSLPCLKCFG_R     (*((volatile unsigned long *)0x400FE144))
#define SYSCTL_SYSPROP_R        (*((volatile unsigned long *)0x400FE14C))
#define SYSCTL_PIOSCCAL_R       (*((volatile unsigned long *)0x400FE150))
#define SYSCTL_PIOSCSTAT_R      (*((volatile unsigned long *)0x400FE154))
#define SYSCTL_PLLFREQ0_R       (*((volatile unsigned long *)0x400FE160))
#define SYSCTL_PLLFREQ1_R       (*((volatile unsigned long *)0x400FE164))
#define SYSCTL_PLLSTAT_R        (*((volatile unsigned long *)0x400FE168))
#define SYSCTL_DC9_R            (*((volatile unsigned long *)0x400FE190))
#define SYSCTL_NVMSTAT_R        (*((volatile unsigned long *)0x400FE1A0))
#define SYSCTL_PPWD_R           (*((volatile unsigned long *)0x400FE300))
#define SYSCTL_PPTIMER_R        (*((volatile unsigned long *)0x400FE304))
#define SYSCTL_PPGPIO_R         (*((volatile unsigned long *)0x400FE308))
#define SYSCTL_PPDMA_R          (*((volatile unsigned long *)0x400FE30C))
#define SYSCTL_PPHIB_R          (*((volatile unsigned long *)0x400FE314))
#define SYSCTL_PPUART_R         (*((volatile unsigned long *)0x400FE318))
#define SYSCTL_PPSSI_R          (*((volatile unsigned long *)0x400FE31C))
#define SYSCTL_PPI2C_R          (*((volatile unsigned long *)0x400FE320))
#define SYSCTL_PPUSB_R          (*((volatile unsigned long *)0x400FE328))
#define SYSCTL_PPCAN_R          (*((volatile unsigned long *)0x400FE334))
#define SYSCTL_PPADC_R          (*((volatile unsigned long *)0x400FE338))
#define SYSCTL_PPACMP_R         (*((volatile unsigned long *)0x400FE33C))
#define SYSCTL_PPPWM_R          (*((volatile unsigned long *)0x400FE340))
#define SYSCTL_PPQEI_R          (*((volatile unsigned long *)0x400FE344))
#define SYSCTL_PPLPC_R          (*((volatile unsigned long *)0x400FE348))
#define SYSCTL_PPPECI_R         (*((volatile unsigned long *)0x400FE350))
#define SYSCTL_PPFAN_R          (*((volatile unsigned long *)0x400FE354))
#define SYSCTL_PPEEPROM_R       (*((volatile unsigned long *)0x400FE358))
#define SYSCTL_PPWTIMER_R       (*((volatile unsigned long *)0x400FE35C))
#define SYSCTL_SRWD_R           (*((volatile unsigned long *)0x400FE500))
#define SYSCTL_SRTIMER_R        (*((volatile unsigned long *)0x400FE504))
#define SYSCTL_SRGPIO_R         (*((volatile unsigned long *)0x400FE508))
#define SYSCTL_SRDMA_R          (*((volatile unsigned long *)0x400FE50C))
#define SYSCTL_SRHIB_R          (*((volatile unsigned long *)0x400FE514))
#define SYSCTL_SRUART_R         (*((volatile unsigned long *)0x400FE518))
#define SYSCTL_SRSSI_R          (*((volatile unsigned long *)0x400FE51C))
#define SYSCTL_SRI2C_R          (*((volatile unsigned long *)0x400FE520))
#define SYSCTL_SRADC_R          (*((volatile unsigned long *)0x400FE538))
#define SYSCTL_SRACMP_R         (*((volatile unsigned long *)0x400FE53C))
#define SYSCTL_SRLPC_R          (*((volatile unsigned long *)0x400FE548))
#define SYSCTL_SRPECI_R         (*((volatile unsigned long *)0x400FE550))
#define SYSCTL_SRFAN_R          (*((volatile unsigned long *)0x400FE554))
#define SYSCTL_SREEPROM_R       (*((volatile unsigned long *)0x400FE558))
#define SYSCTL_SRWTIMER_R       (*((volatile unsigned long *)0x400FE55C))
#define SYSCTL_RCGCWD_R         (*((volatile unsigned long *)0x400FE600))
#define SYSCTL_RCGCTIMER_R      (*((volatile unsigned long *)0x400FE604))
#define SYSCTL_RCGCGPIO_R       (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCDMA_R        (*((volatile unsigned long *)0x400FE60C))
#define SYSCTL_RCGCHIB_R        (*((volatile unsigned long *)0x400FE614))
#define SYSCTL_RCGCUART_R       (*((volatile unsigned long *)0x400FE618))
#define SYSCTL_RCGCSSI_R        (*((volatile unsigned long *)0x400FE61C))
#define SYSCTL_RCGCI2C_R        (*((volatile unsigned long *)0x400FE620))
#define SYSCTL_RCGCADC_R        (*((volatile unsigned long *)0x400FE638))
#define SYSCTL_RCGCACMP_R       (*((volatile unsigned long *)0x400FE63C))
#define SYSCTL_RCGCLPC_R        (*((volatile unsigned long *)0x400FE648))
#define SYSCTL_RCGCPECI_R       (*((volatile unsigned long *)0x400FE650))
#define SYSCTL_RCGCFAN_R        (*((volatile unsigned long *)0x400FE654))
#define SYSCTL_RCGCEEPROM_R     (*((volatile unsigned long *)0x400FE658))
#define SYSCTL_RCGCWTIMER_R     (*((volatile unsigned long *)0x400FE65C))
#define SYSCTL_SCGCWD_R         (*((volatile unsigned long *)0x400FE700))
#define SYSCTL_SCGCTIMER_R      (*((volatile unsigned long *)0x400FE704))
#define SYSCTL_SCGCGPIO_R       (*((volatile unsigned long *)0x400FE708))
#define SYSCTL_SCGCDMA_R        (*((volatile unsigned long *)0x400FE70C))
#define SYSCTL_SCGCHIB_R        (*((volatile unsigned long *)0x400FE714))
#define SYSCTL_SCGCUART_R       (*((volatile unsigned long *)0x400FE718))
#define SYSCTL_SCGCSSI_R        (*((volatile unsigned long *)0x400FE71C))
#define SYSCTL_SCGCI2C_R        (*((volatile unsigned long *)0x400FE720))
#define SYSCTL_SCGCADC_R        (*((volatile unsigned long *)0x400FE738))
#define SYSCTL_SCGCACMP_R       (*((volatile unsigned long *)0x400FE73C))
#define SYSCTL_SCGCLPC_R        (*((volatile unsigned long *)0x400FE748))
#define SYSCTL_SCGCPECI_R       (*((volatile unsigned long *)0x400FE750))
#define SYSCTL_SCGCFAN_R        (*((volatile unsigned long *)0x400FE754))
#define SYSCTL_SCGCEEPROM_R     (*((volatile unsigned long *)0x400FE758))
#define SYSCTL_SCGCWTIMER_R     (*((volatile unsigned long *)0x400FE75C))
#define SYSCTL_DCGCWD_R         (*((volatile unsigned long *)0x400FE800))
#define SYSCTL_DCGCTIMER_R      (*((volatile unsigned long *)0x400FE804))
#define SYSCTL_DCGCGPIO_R       (*((volatile unsigned long *)0x400FE808))
#define SYSCTL_DCGCDMA_R        (*((volatile unsigned long *)0x400FE80C))
#define SYSCTL_DCGCHIB_R        (*((volatile unsigned long *)0x400FE814))
#define SYSCTL_DCGCUART_R       (*((volatile unsigned long *)0x400FE818))
#define SYSCTL_DCGCSSI_R        (*((volatile unsigned long *)0x400FE81C))
#define SYSCTL_DCGCI2C_R        (*((volatile unsigned long *)0x400FE820))
#define SYSCTL_DCGCADC_R        (*((volatile unsigned long *)0x400FE838))
#define SYSCTL_DCGCACMP_R       (*((volatile unsigned long *)0x400FE83C))
#define SYSCTL_DCGCLPC_R        (*((volatile unsigned long *)0x400FE848))
#define SYSCTL_DCGCPECI_R       (*((volatile unsigned long *)0x400FE850))
#define SYSCTL_DCGCFAN_R        (*((volatile unsigned long *)0x400FE854))
#define SYSCTL_DCGCEEPROM_R     (*((volatile unsigned long *)0x400FE858))
#define SYSCTL_DCGCWTIMER_R     (*((volatile unsigned long *)0x400FE85C))
#define SYSCTL_PCWD_R           (*((volatile unsigned long *)0x400FE900))
#define SYSCTL_PCTIMER_R        (*((volatile unsigned long *)0x400FE904))
#define SYSCTL_PCGPIO_R         (*((volatile unsigned long *)0x400FE908))
#define SYSCTL_PCDMA_R          (*((volatile unsigned long *)0x400FE90C))
#define SYSCTL_PCHIB_R          (*((volatile unsigned long *)0x400FE914))
#define SYSCTL_PCUART_R         (*((volatile unsigned long *)0x400FE918))
#define SYSCTL_PCSSI_R          (*((volatile unsigned long *)0x400FE91C))
#define SYSCTL_PCI2C_R          (*((volatile unsigned long *)0x400FE920))
#define SYSCTL_PCADC_R          (*((volatile unsigned long *)0x400FE938))
#define SYSCTL_PCACMP_R         (*((volatile unsigned long *)0x400FE93C))
#define SYSCTL_PCLPC_R          (*((volatile unsigned long *)0x400FE948))
#define SYSCTL_PCPECI_R         (*((volatile unsigned long *)0x400FE950))
#define SYSCTL_PCFAN_R          (*((volatile unsigned long *)0x400FE954))
#define SYSCTL_PCEEPROM_R       (*((volatile unsigned long *)0x400FE958))
#define SYSCTL_PCWTIMER_R       (*((volatile unsigned long *)0x400FE95C))
#define SYSCTL_PRWD_R           (*((volatile unsigned long *)0x400FEA00))
#define SYSCTL_PRTIMER_R        (*((volatile unsigned long *)0x400FEA04))
#define SYSCTL_PRGPIO_R         (*((volatile unsigned long *)0x400FEA08))
#define SYSCTL_PRDMA_R          (*((volatile unsigned long *)0x400FEA0C))
#define SYSCTL_PRHIB_R          (*((volatile unsigned long *)0x400FEA14))
#define SYSCTL_PRUART_R         (*((volatile unsigned long *)0x400FEA18))
#define SYSCTL_PRSSI_R          (*((volatile unsigned long *)0x400FEA1C))
#define SYSCTL_PRI2C_R          (*((volatile unsigned long *)0x400FEA20))
#define SYSCTL_PRADC_R          (*((volatile unsigned long *)0x400FEA38))
#define SYSCTL_PRACMP_R         (*((volatile unsigned long *)0x400FEA3C))
#define SYSCTL_PRLPC_R          (*((volatile unsigned long *)0x400FEA48))
#define SYSCTL_PRPECI_R         (*((volatile unsigned long *)0x400FEA50))
#define SYSCTL_PRFAN_R          (*((volatile unsigned long *)0x400FEA54))
#define SYSCTL_PREEPROM_R       (*((volatile unsigned long *)0x400FEA58))
#define SYSCTL_PRWTIMER_R       (*((volatile unsigned long *)0x400FEA5C))

//*****************************************************************************
//
// Micro Direct Memory Access registers (UDMA)
//
//*****************************************************************************
#define UDMA_STAT_R             (*((volatile unsigned long *)0x400FF000))
#define UDMA_CFG_R              (*((volatile unsigned long *)0x400FF004))
#define UDMA_CTLBASE_R          (*((volatile unsigned long *)0x400FF008))
#define UDMA_ALTBASE_R          (*((volatile unsigned long *)0x400FF00C))
#define UDMA_WAITSTAT_R         (*((volatile unsigned long *)0x400FF010))
#define UDMA_SWREQ_R            (*((volatile unsigned long *)0x400FF014))
#define UDMA_USEBURSTSET_R      (*((volatile unsigned long *)0x400FF018))
#define UDMA_USEBURSTCLR_R      (*((volatile unsigned long *)0x400FF01C))
#define UDMA_REQMASKSET_R       (*((volatile unsigned long *)0x400FF020))
#define UDMA_REQMASKCLR_R       (*((volatile unsigned long *)0x400FF024))
#define UDMA_ENASET_R           (*((volatile unsigned long *)0x400FF028))
#define UDMA_ENACLR_R           (*((volatile unsigned long *)0x400FF02C))
#define UDMA_ALTSET_R           (*((volatile unsigned long *)0x400FF030))
#define UDMA_ALTCLR_R           (*((volatile unsigned long *)0x400FF034))
#define UDMA_PRIOSET_R          (*((volatile unsigned long *)0x400FF038))
#define UDMA_PRIOCLR_R          (*((volatile unsigned long *)0x400FF03C))
#define UDMA_ERRCLR_R           (*((volatile unsigned long *)0x400FF04C))
#define UDMA_CHASGN_R           (*((volatile unsigned long *)0x400FF500))
#define UDMA_CHIS_R             (*((volatile unsigned long *)0x400FF504))
#define UDMA_CHMAP0_R           (*((volatile unsigned long *)0x400FF510))
#define UDMA_CHMAP1_R           (*((volatile unsigned long *)0x400FF514))
#define UDMA_CHMAP2_R           (*((volatile unsigned long *)0x400FF518))
#define UDMA_CHMAP3_R           (*((volatile unsigned long *)0x400FF51C))

//*****************************************************************************
//
// Micro Direct Memory Access (uDMA) offsets (UDMA)
//
//*****************************************************************************
#define UDMA_SRCENDP            0x00000000  // DMA Channel Source Address End
                                            // Pointer
#define UDMA_DSTENDP            0x00000004  // DMA Channel Destination Address
                                            // End Pointer
#define UDMA_CHCTL              0x00000008  // DMA Channel Control Word

//*****************************************************************************
//
// NVIC registers (NVIC)
//
//*****************************************************************************
#define NVIC_INT_TYPE_R         (*((volatile unsigned long *)0xE000E004))
#define NVIC_ACTLR_R            (*((volatile unsigned long *)0xE000E008))
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CAL_R           (*((volatile unsigned long *)0xE000E01C))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))
#define NVIC_EN1_R              (*((volatile unsigned long *)0xE000E104))
#define NVIC_EN2_R              (*((volatile unsigned long *)0xE000E108))
#define NVIC_EN3_R              (*((volatile unsigned long *)0xE000E10C))
#define NVIC_EN4_R              (*((volatile unsigned long *)0xE000E110))
#define NVIC_DIS0_R             (*((volatile unsigned long *)0xE000E180))
#define NVIC_DIS1_R             (*((volatile unsigned long *)0xE000E184))
#define NVIC_DIS2_R             (*((volatile unsigned long *)0xE000E188))
#define NVIC_DIS3_R             (*((volatile unsigned long *)0xE000E18C))
#define NVIC_DIS4_R             (*((volatile unsigned long *)0xE000E190))
#define NVIC_PEND0_R            (*((volatile unsigned long *)0xE000E200))
#define NVIC_PEND1_R            (*((volatile unsigned long *)0xE000E204))
#define NVIC_PEND2_R            (*((volatile unsigned long *)0xE000E208))
#define NVIC_PEND3_R            (*((volatile unsigned long *)0xE000E20C))
#define NVIC_PEND4_R            (*((volatile unsigned long *)0xE000E210))
#define NVIC_UNPEND0_R          (*((volatile unsigned long *)0xE000E280))
#define NVIC_UNPEND1_R          (*((volatile unsigned long *)0xE000E284))
#define NVIC_UNPEND2_R          (*((volatile unsigned long *)0xE000E288))
#define NVIC_UNPEND3_R          (*((volatile unsigned long *)0xE000E28C))
#define NVIC_UNPEND4_R          (*((volatile unsigned long *)0xE000E290))
#define NVIC_ACTIVE0_R          (*((volatile unsigned long *)0xE000E300))
#define NVIC_ACTIVE1_R          (*((volatile unsigned long *)0xE000E304))
#define NVIC_ACTIVE2_R          (*((volatile unsigned long *)0xE000E308))
#define NVIC_ACTIVE3_R          (*((volatile unsigned long *)0xE000E30C))
#define NVIC_ACTIVE4_R          (*((volatile unsigned long *)0xE000E310))
#define NVIC_PRI0_R             (*((volatile unsigned long *)0xE000E400))
#define NVIC_PRI1_R             (*((volatile unsigned long *)0xE000E404))
#define NVIC_PRI2_R             (*((volatile unsigned long *)0xE000E408))
#define NVIC_PRI3_R             (*((volatile unsigned long *)0xE000E40C))
#define NVIC_PRI4_R             (*((volatile unsigned long *)0xE000E410))
#define NVIC_PRI5_R             (*((volatile unsigned long *)0xE000E414))
#define NVIC_PRI6_R             (*((volatile unsigned long *)0xE000E418))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define NVIC_PRI8_R             (*((volatile unsigned long *)0xE000E420))
#define NVIC_PRI9_R             (*((volatile unsigned long *)0xE000E424))
#define NVIC_PRI10_R            (*((volatile unsigned long *)0xE000E428))
#define NVIC_PRI11_R            (*((volatile unsigned long *)0xE000E42C))
#define NVIC_PRI12_R            (*((volatile unsigned long *)0xE000E430))
#define NVIC_PRI13_R            (*((volatile unsigned long *)0xE000E434))
#define NVIC_PRI14_R            (*((volatile unsigned long *)0xE000E438))
#define NVIC_PRI15_R            (*((volatile unsigned long *)0xE000E43C))
#define NVIC_PRI16_R            (*((volatile unsigned long *)0xE000E440))
#define NVIC_PRI17_R            (*((volatile unsigned long *)0xE000E444))
#define NVIC_PRI18_R            (*((volatile unsigned long *)0xE000E448))
#define NVIC_PRI19_R            (*((volatile unsigned long *)0xE000E44C))
#define NVIC_PRI20_R            (*((volatile unsigned long *)0xE000E450))
#define NVIC_PRI21_R            (*((volatile unsigned long *)0xE000E454))
#define NVIC_PRI22_R            (*((volatile unsigned long *)0xE000E458))
#define NVIC_PRI23_R            (*((volatile unsigned long *)0xE000E45C))
#define NVIC_PRI24_R            (*((volatile unsigned long *)0xE000E460))
#define NVIC_PRI25_R            (*((volatile unsigned long *)0xE000E464))
#define NVIC_PRI26_R            (*((volatile unsigned long *)0xE000E468))
#define NVIC_PRI27_R            (*((volatile unsigned long *)0xE000E46C))
#define NVIC_PRI28_R            (*((volatile unsigned long *)0xE000E470))
#define NVIC_PRI29_R            (*((volatile unsigned long *)0xE000E474))
#define NVIC_PRI30_R            (*((volatile unsigned long *)0xE000E478))
#define NVIC_PRI31_R            (*((volatile unsigned long *)0xE000E47C))
#define NVIC_PRI32_R            (*((volatile unsigned long *)0xE000E480))
#define NVIC_PRI33_R            (*((volatile unsigned long *)0xE000E484))
#define NVIC_PRI34_R            (*((volatile unsigned long *)0xE000E488))
#define NVIC_CPUID_R            (*((volatile unsigned long *)0xE000ED00))
#define NVIC_INT_CTRL_R         (*((volatile unsigned long *)0xE000ED04))
#define NVIC_VTABLE_R           (*((volatile unsigned long *)0xE000ED08))
#define NVIC_APINT_R            (*((volatile unsigned long *)0xE000ED0C))
#define NVIC_SYS_CTRL_R         (*((volatile unsigned long *)0xE000ED10))
#define NVIC_CFG_CTRL_R         (*((volatile unsigned long *)0xE000ED14))
#define NVIC_SYS_PRI1_R         (*((volatile unsigned long *)0xE000ED18))
#define NVIC_SYS_PRI2_R         (*((volatile unsigned long *)0xE000ED1C))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))
#define NVIC_SYS_HND_CTRL_R     (*((volatile unsigned long *)0xE000ED24))
#define NVIC_FAULT_STAT_R       (*((volatile unsigned long *)0xE000ED28))
#define NVIC_HFAULT_STAT_R      (*((volatile unsigned long *)0xE000ED2C))
#define NVIC_DEBUG_STAT_R       (*((volatile unsigned long *)0xE000ED30))
#define NVIC_MM_ADDR_R          (*((volatile unsigned long *)0xE000ED34))
#define NVIC_FAULT_ADDR_R       (*((volatile unsigned long *)0xE000ED38))
#define NVIC_CPAC_R             (*((volatile unsigned long *)0xE000ED88))
#define NVIC_MPU_TYPE_R         (*((volatile unsigned long *)0xE000ED90))
#define NVIC_MPU_CTRL_R         (*((volatile unsigned long *)0xE000ED94))
#define NVIC_MPU_NUMBER_R       (*((volatile unsigned long *)0xE000ED98))
#define NVIC_MPU_BASE_R         (*((volatile unsigned long *)0xE000ED9C))
#define NVIC_MPU_ATTR_R         (*((volatile unsigned long *)0xE000EDA0))
#define NVIC_MPU_BASE1_R        (*((volatile unsigned long *)0xE000EDA4))
#define NVIC_MPU_ATTR1_R        (*((volatile unsigned long *)0xE000EDA8))
#define NVIC_MPU_BASE2_R        (*((volatile unsigned long *)0xE000EDAC))
#define NVIC_MPU_ATTR2_R        (*((volatile unsigned long *)0xE000EDB0))
#define NVIC_MPU_BASE3_R        (*((volatile unsigned long *)0xE000EDB4))
#define NVIC_MPU_ATTR3_R        (*((volatile unsigned long *)0xE000EDB8))
#define NVIC_DBG_CTRL_R         (*((volatile unsigned long *)0xE000EDF0))
#define NVIC_DBG_XFER_R         (*((volatile unsigned long *)0xE000EDF4))
#define NVIC_DBG_DATA_R         (*((volatile unsigned long *)0xE000EDF8))
#define NVIC_DBG_INT_R          (*((volatile unsigned long *)0xE000EDFC))
#define NVIC_SW_TRIG_R          (*((volatile unsigned long *)0xE000EF00))
#define NVIC_FPCC_R             (*((volatile unsigned long *)0xE000EF34))
#define NVIC_FPCA_R             (*((volatile unsigned long *)0xE000EF38))
#define NVIC_FPDSC_R            (*((volatile unsigned long *)0xE000EF3C))

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_LOAD register.
//
//*****************************************************************************
#define WDT_LOAD_M              0xFFFFFFFF  // Watchdog Load Value
#define WDT_LOAD_S              0

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_VALUE register.
//
//*****************************************************************************
#define WDT_VALUE_M             0xFFFFFFFF  // Watchdog Value
#define WDT_VALUE_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_CTL register.
//
//*****************************************************************************
#define WDT_CTL_WRC             0x80000000  // Write Complete
#define WDT_CTL_INTTYPE         0x00000004  // Watchdog Interrupt Type
#define WDT_CTL_RESEN           0x00000002  // Watchdog Reset Enable
#define WDT_CTL_INTEN           0x00000001  // Watchdog Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_ICR register.
//
//*****************************************************************************
#define WDT_ICR_M               0xFFFFFFFF  // Watchdog Interrupt Clear
#define WDT_ICR_S               0

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_RIS register.
//
//*****************************************************************************
#define WDT_RIS_WDTRIS          0x00000001  // Watchdog Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_MIS register.
//
//*****************************************************************************
#define WDT_MIS_WDTMIS          0x00000001  // Watchdog Masked Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_TEST register.
//
//*****************************************************************************
#define WDT_TEST_STALL          0x00000100  // Watchdog Stall Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the WDT_O_LOCK register.
//
//*****************************************************************************
#define WDT_LOCK_M              0xFFFFFFFF  // Watchdog Lock
#define WDT_LOCK_UNLOCKED       0x00000000  // Unlocked
#define WDT_LOCK_LOCKED         0x00000001  // Locked

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_IM register.
//
//*****************************************************************************
#define GPIO_IM_GPIO_M          0x000000FF  // GPIO Interrupt Mask Enable
#define GPIO_IM_GPIO_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_RIS register.
//
//*****************************************************************************
#define GPIO_RIS_GPIO_M         0x000000FF  // GPIO Interrupt Raw Status
#define GPIO_RIS_GPIO_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_MIS register.
//
//*****************************************************************************
#define GPIO_MIS_GPIO_M         0x000000FF  // GPIO Masked Interrupt Status
#define GPIO_MIS_GPIO_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_ICR register.
//
//*****************************************************************************
#define GPIO_ICR_GPIO_M         0x000000FF  // GPIO Interrupt Clear
#define GPIO_ICR_GPIO_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_LOCK register.
//
//*****************************************************************************
#define GPIO_LOCK_M             0xFFFFFFFF  // GPIO Lock
#define GPIO_LOCK_UNLOCKED      0x00000000  // The GPIOCR register is unlocked
                                            // and may be modified
#define GPIO_LOCK_LOCKED        0x00000001  // The GPIOCR register is locked
                                            // and may not be modified
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_SI register.
//
//*****************************************************************************
#define GPIO_SI_SUM             0x00000001  // Summary Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port A.
//
//*****************************************************************************
#define GPIO_PCTL_PA7_M         0xF0000000  // PA7 mask
#define GPIO_PCTL_PA7_I2C1SDA   0x30000000  // I2C1SDA on PA7
#define GPIO_PCTL_PA6_M         0x0F000000  // PA6 mask
#define GPIO_PCTL_PA6_I2C1SCL   0x03000000  // I2C1SCL on PA6
#define GPIO_PCTL_PA5_M         0x00F00000  // PA5 mask
#define GPIO_PCTL_PA5_SSI0TX    0x00200000  // SSI0TX on PA5
#define GPIO_PCTL_PA4_M         0x000F0000  // PA4 mask
#define GPIO_PCTL_PA4_SSI0RX    0x00020000  // SSI0RX on PA4
#define GPIO_PCTL_PA3_M         0x0000F000  // PA3 mask
#define GPIO_PCTL_PA3_SSI0FSS   0x00002000  // SSI0FSS on PA3
#define GPIO_PCTL_PA2_M         0x00000F00  // PA2 mask
#define GPIO_PCTL_PA2_SSI0CLK   0x00000200  // SSI0CLK on PA2
#define GPIO_PCTL_PA1_M         0x000000F0  // PA1 mask
#define GPIO_PCTL_PA1_U0TX      0x00000010  // U0TX on PA1
#define GPIO_PCTL_PA0_M         0x0000000F  // PA0 mask
#define GPIO_PCTL_PA0_U0RX      0x00000001  // U0RX on PA0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port B.
//
//*****************************************************************************
#define GPIO_PCTL_PB7_M         0xF0000000  // PB7 mask
#define GPIO_PCTL_PB7_SSI2TX    0x20000000  // SSI2TX on PB7
#define GPIO_PCTL_PB7_I2C5SDA   0x30000000  // I2C5SDA on PB7
#define GPIO_PCTL_PB7_T0CCP1    0x70000000  // T0CCP1 on PB7
#define GPIO_PCTL_PB6_M         0x0F000000  // PB6 mask
#define GPIO_PCTL_PB6_SSI2RX    0x02000000  // SSI2RX on PB6
#define GPIO_PCTL_PB6_I2C5SCL   0x03000000  // I2C5SCL on PB6
#define GPIO_PCTL_PB6_T0CCP0    0x07000000  // T0CCP0 on PB6
#define GPIO_PCTL_PB5_M         0x00F00000  // PB5 mask
#define GPIO_PCTL_PB5_SSI2FSS   0x00200000  // SSI2FSS on PB5
#define GPIO_PCTL_PB5_T1CCP1    0x00700000  // T1CCP1 on PB5
#define GPIO_PCTL_PB4_M         0x000F0000  // PB4 mask
#define GPIO_PCTL_PB4_SSI2CLK   0x00020000  // SSI2CLK on PB4
#define GPIO_PCTL_PB4_T1CCP0    0x00070000  // T1CCP0 on PB4
#define GPIO_PCTL_PB3_M         0x0000F000  // PB3 mask
#define GPIO_PCTL_PB3_I2C0SDA   0x00003000  // I2C0SDA on PB3
#define GPIO_PCTL_PB3_T3CCP1    0x00007000  // T3CCP1 on PB3
#define GPIO_PCTL_PB2_M         0x00000F00  // PB2 mask
#define GPIO_PCTL_PB2_I2C0SCL   0x00000300  // I2C0SCL on PB2
#define GPIO_PCTL_PB2_T3CCP0    0x00000700  // T3CCP0 on PB2
#define GPIO_PCTL_PB1_M         0x000000F0  // PB1 mask
#define GPIO_PCTL_PB1_U1TX      0x00000010  // U1TX on PB1
#define GPIO_PCTL_PB1_T2CCP1    0x00000070  // T2CCP1 on PB1
#define GPIO_PCTL_PB0_M         0x0000000F  // PB0 mask
#define GPIO_PCTL_PB0_U1RX      0x00000001  // U1RX on PB0
#define GPIO_PCTL_PB0_T2CCP0    0x00000007  // T2CCP0 on PB0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port C.
//
//*****************************************************************************
#define GPIO_PCTL_PC7_M         0xF0000000  // PC7 mask
#define GPIO_PCTL_PC7_U3TX      0x10000000  // U3TX on PC7
#define GPIO_PCTL_PC7_WT1CCP1   0x70000000  // WT1CCP1 on PC7
#define GPIO_PCTL_PC6_M         0x0F000000  // PC6 mask
#define GPIO_PCTL_PC6_U3RX      0x01000000  // U3RX on PC6
#define GPIO_PCTL_PC6_WT1CCP0   0x07000000  // WT1CCP0 on PC6
#define GPIO_PCTL_PC5_M         0x00F00000  // PC5 mask
#define GPIO_PCTL_PC5_U4TX      0x00100000  // U4TX on PC5
#define GPIO_PCTL_PC5_U1TX      0x00200000  // U1TX on PC5
#define GPIO_PCTL_PC5_WT0CCP1   0x00700000  // WT0CCP1 on PC5
#define GPIO_PCTL_PC4_M         0x000F0000  // PC4 mask
#define GPIO_PCTL_PC4_U4RX      0x00010000  // U4RX on PC4
#define GPIO_PCTL_PC4_U1RX      0x00020000  // U1RX on PC4
#define GPIO_PCTL_PC4_WT0CCP0   0x00070000  // WT0CCP0 on PC4
#define GPIO_PCTL_PC3_M         0x0000F000  // PC3 mask
#define GPIO_PCTL_PC3_TDO       0x00001000  // TDO on PC3
#define GPIO_PCTL_PC3_T5CCP1    0x00007000  // T5CCP1 on PC3
#define GPIO_PCTL_PC2_M         0x00000F00  // PC2 mask
#define GPIO_PCTL_PC2_TDI       0x00000100  // TDI on PC2
#define GPIO_PCTL_PC2_T5CCP0    0x00000700  // T5CCP0 on PC2
#define GPIO_PCTL_PC1_M         0x000000F0  // PC1 mask
#define GPIO_PCTL_PC1_TMS       0x00000010  // TMS on PC1
#define GPIO_PCTL_PC1_T4CCP1    0x00000070  // T4CCP1 on PC1
#define GPIO_PCTL_PC0_M         0x0000000F  // PC0 mask
#define GPIO_PCTL_PC0_TCK       0x00000001  // TCK on PC0
#define GPIO_PCTL_PC0_T4CCP0    0x00000007  // T4CCP0 on PC0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port D.
//
//*****************************************************************************
#define GPIO_PCTL_PD7_M         0xF0000000  // PD7 mask
#define GPIO_PCTL_PD7_U2TX      0x10000000  // U2TX on PD7
#define GPIO_PCTL_PD7_WT5CCP1   0x70000000  // WT5CCP1 on PD7
#define GPIO_PCTL_PD7_NMI       0x80000000  // NMI on PD7
#define GPIO_PCTL_PD6_M         0x0F000000  // PD6 mask
#define GPIO_PCTL_PD6_U2RX      0x01000000  // U2RX on PD6
#define GPIO_PCTL_PD6_WT5CCP0   0x07000000  // WT5CCP0 on PD6
#define GPIO_PCTL_PD5_M         0x00F00000  // PD5 mask
#define GPIO_PCTL_PD5_U6TX      0x00100000  // U6TX on PD5
#define GPIO_PCTL_PD5_WT4CCP1   0x00700000  // WT4CCP1 on PD5
#define GPIO_PCTL_PD4_M         0x000F0000  // PD4 mask
#define GPIO_PCTL_PD4_U6RX      0x00010000  // U6RX on PD4
#define GPIO_PCTL_PD4_WT4CCP0   0x00070000  // WT4CCP0 on PD4
#define GPIO_PCTL_PD3_M         0x0000F000  // PD3 mask
#define GPIO_PCTL_PD3_SSI3TX    0x00001000  // SSI3TX on PD3
#define GPIO_PCTL_PD3_SSI1TX    0x00002000  // SSI1TX on PD3
#define GPIO_PCTL_PD3_WT3CCP1   0x00007000  // WT3CCP1 on PD3
#define GPIO_PCTL_PD2_M         0x00000F00  // PD2 mask
#define GPIO_PCTL_PD2_SSI3RX    0x00000100  // SSI3RX on PD2
#define GPIO_PCTL_PD2_SSI1RX    0x00000200  // SSI1RX on PD2
#define GPIO_PCTL_PD2_WT3CCP0   0x00000700  // WT3CCP0 on PD2
#define GPIO_PCTL_PD1_M         0x000000F0  // PD1 mask
#define GPIO_PCTL_PD1_SSI3FSS   0x00000010  // SSI3FSS on PD1
#define GPIO_PCTL_PD1_SSI1FSS   0x00000020  // SSI1FSS on PD1
#define GPIO_PCTL_PD1_I2C3SDA   0x00000030  // I2C3SDA on PD1
#define GPIO_PCTL_PD1_WT2CCP1   0x00000070  // WT2CCP1 on PD1
#define GPIO_PCTL_PD0_M         0x0000000F  // PD0 mask
#define GPIO_PCTL_PD0_SSI3CLK   0x00000001  // SSI3CLK on PD0
#define GPIO_PCTL_PD0_SSI1CLK   0x00000002  // SSI1CLK on PD0
#define GPIO_PCTL_PD0_I2C3SCL   0x00000003  // I2C3SCL on PD0
#define GPIO_PCTL_PD0_WT2CCP0   0x00000007  // WT2CCP0 on PD0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port E.
//
//*****************************************************************************
#define GPIO_PCTL_PE7_M         0xF0000000  // PE7 mask
#define GPIO_PCTL_PE6_M         0x0F000000  // PE6 mask
#define GPIO_PCTL_PE5_M         0x00F00000  // PE5 mask
#define GPIO_PCTL_PE5_U5TX      0x00100000  // U5TX on PE5
#define GPIO_PCTL_PE5_I2C2SDA   0x00300000  // I2C2SDA on PE5
#define GPIO_PCTL_PE4_M         0x000F0000  // PE4 mask
#define GPIO_PCTL_PE4_U5RX      0x00010000  // U5RX on PE4
#define GPIO_PCTL_PE4_I2C2SCL   0x00030000  // I2C2SCL on PE4
#define GPIO_PCTL_PE3_M         0x0000F000  // PE3 mask
#define GPIO_PCTL_PE2_M         0x00000F00  // PE2 mask
#define GPIO_PCTL_PE1_M         0x000000F0  // PE1 mask
#define GPIO_PCTL_PE1_U7TX      0x00000010  // U7TX on PE1
#define GPIO_PCTL_PE0_M         0x0000000F  // PE0 mask
#define GPIO_PCTL_PE0_U7RX      0x00000001  // U7RX on PE0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port F.
//
//*****************************************************************************
#define GPIO_PCTL_PF7_M         0xF0000000  // PF7 mask
#define GPIO_PCTL_PF7_I2C2SDA   0x30000000  // I2C2SDA on PF7
#define GPIO_PCTL_PF7_T3CCP1    0x70000000  // T3CCP1 on PF7
#define GPIO_PCTL_PF6_M         0x0F000000  // PF6 mask
#define GPIO_PCTL_PF6_I2C2SCL   0x03000000  // I2C2SCL on PF6
#define GPIO_PCTL_PF6_T3CCP0    0x07000000  // T3CCP0 on PF6
#define GPIO_PCTL_PF5_M         0x00F00000  // PF5 mask
#define GPIO_PCTL_PF5_T2CCP1    0x00700000  // T2CCP1 on PF5
#define GPIO_PCTL_PF4_M         0x000F0000  // PF4 mask
#define GPIO_PCTL_PF4_T2CCP0    0x00070000  // T2CCP0 on PF4
#define GPIO_PCTL_PF4_TRD3      0x000E0000  // TRD3 on PF4
#define GPIO_PCTL_PF3_M         0x0000F000  // PF3 mask
#define GPIO_PCTL_PF3_SSI1FSS   0x00002000  // SSI1FSS on PF3
#define GPIO_PCTL_PF3_T1CCP1    0x00007000  // T1CCP1 on PF3
#define GPIO_PCTL_PF3_TRCLK     0x0000E000  // TRCLK on PF3
#define GPIO_PCTL_PF2_M         0x00000F00  // PF2 mask
#define GPIO_PCTL_PF2_SSI1CLK   0x00000200  // SSI1CLK on PF2
#define GPIO_PCTL_PF2_T1CCP0    0x00000700  // T1CCP0 on PF2
#define GPIO_PCTL_PF2_TRD0      0x00000E00  // TRD0 on PF2
#define GPIO_PCTL_PF1_M         0x000000F0  // PF1 mask
#define GPIO_PCTL_PF1_SSI1TX    0x00000020  // SSI1TX on PF1
#define GPIO_PCTL_PF1_T0CCP1    0x00000070  // T0CCP1 on PF1
#define GPIO_PCTL_PF1_TRD1      0x000000E0  // TRD1 on PF1
#define GPIO_PCTL_PF0_M         0x0000000F  // PF0 mask
#define GPIO_PCTL_PF0_SSI1RX    0x00000002  // SSI1RX on PF0
#define GPIO_PCTL_PF0_T0CCP0    0x00000007  // T0CCP0 on PF0
#define GPIO_PCTL_PF0_NMI       0x00000008  // NMI on PF0
#define GPIO_PCTL_PF0_TRD2      0x0000000E  // TRD2 on PF0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port G.
//
//*****************************************************************************
#define GPIO_PCTL_PG7_M         0xF0000000  // PG7 mask
#define GPIO_PCTL_PG7_I2C5SDA   0x30000000  // I2C5SDA on PG7
#define GPIO_PCTL_PG7_WT1CCP1   0x70000000  // WT1CCP1 on PG7
#define GPIO_PCTL_PG6_M         0x0F000000  // PG6 mask
#define GPIO_PCTL_PG6_I2C5SCL   0x03000000  // I2C5SCL on PG6
#define GPIO_PCTL_PG6_WT1CCP0   0x07000000  // WT1CCP0 on PG6
#define GPIO_PCTL_PG5_M         0x00F00000  // PG5 mask
#define GPIO_PCTL_PG5_U2TX      0x00100000  // U2TX on PG5
#define GPIO_PCTL_PG5_I2C1SDA   0x00300000  // I2C1SDA on PG5
#define GPIO_PCTL_PG5_WT0CCP1   0x00700000  // WT0CCP1 on PG5
#define GPIO_PCTL_PG4_M         0x000F0000  // PG4 mask
#define GPIO_PCTL_PG4_U2RX      0x00010000  // U2RX on PG4
#define GPIO_PCTL_PG4_I2C1SCL   0x00030000  // I2C1SCL on PG4
#define GPIO_PCTL_PG4_WT0CCP0   0x00070000  // WT0CCP0 on PG4
#define GPIO_PCTL_PG3_M         0x0000F000  // PG3 mask
#define GPIO_PCTL_PG3_I2C4SDA   0x00003000  // I2C4SDA on PG3
#define GPIO_PCTL_PG3_T5CCP1    0x00007000  // T5CCP1 on PG3
#define GPIO_PCTL_PG2_M         0x00000F00  // PG2 mask
#define GPIO_PCTL_PG2_I2C4SCL   0x00000300  // I2C4SCL on PG2
#define GPIO_PCTL_PG2_T5CCP0    0x00000700  // T5CCP0 on PG2
#define GPIO_PCTL_PG1_M         0x000000F0  // PG1 mask
#define GPIO_PCTL_PG1_I2C3SDA   0x00000030  // I2C3SDA on PG1
#define GPIO_PCTL_PG1_T4CCP1    0x00000070  // T4CCP1 on PG1
#define GPIO_PCTL_PG0_M         0x0000000F  // PG0 mask
#define GPIO_PCTL_PG0_I2C3SCL   0x00000003  // I2C3SCL on PG0
#define GPIO_PCTL_PG0_T4CCP0    0x00000007  // T4CCP0 on PG0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port H.
//
//*****************************************************************************
#define GPIO_PCTL_PH7_M         0xF0000000  // PH7 mask
#define GPIO_PCTL_PH7_SSI2TX    0x20000000  // SSI2TX on PH7
#define GPIO_PCTL_PH7_WT4CCP1   0x70000000  // WT4CCP1 on PH7
#define GPIO_PCTL_PH6_M         0x0F000000  // PH6 mask
#define GPIO_PCTL_PH6_SSI2RX    0x02000000  // SSI2RX on PH6
#define GPIO_PCTL_PH6_WT4CCP0   0x07000000  // WT4CCP0 on PH6
#define GPIO_PCTL_PH5_M         0x00F00000  // PH5 mask
#define GPIO_PCTL_PH5_SSI2FSS   0x00200000  // SSI2FSS on PH5
#define GPIO_PCTL_PH5_WT3CCP1   0x00700000  // WT3CCP1 on PH5
#define GPIO_PCTL_PH4_M         0x000F0000  // PH4 mask
#define GPIO_PCTL_PH4_SSI2CLK   0x00020000  // SSI2CLK on PH4
#define GPIO_PCTL_PH4_WT3CCP0   0x00070000  // WT3CCP0 on PH4
#define GPIO_PCTL_PH3_M         0x0000F000  // PH3 mask
#define GPIO_PCTL_PH3_FAN0TACH5 0x00001000  // FAN0TACH5 on PH3
#define GPIO_PCTL_PH3_SSI3TX    0x00002000  // SSI3TX on PH3
#define GPIO_PCTL_PH3_WT5CCP1   0x00007000  // WT5CCP1 on PH3
#define GPIO_PCTL_PH2_M         0x00000F00  // PH2 mask
#define GPIO_PCTL_PH2_FAN0PWM5  0x00000100  // FAN0PWM5 on PH2
#define GPIO_PCTL_PH2_SSI3RX    0x00000200  // SSI3RX on PH2
#define GPIO_PCTL_PH2_WT5CCP0   0x00000700  // WT5CCP0 on PH2
#define GPIO_PCTL_PH1_M         0x000000F0  // PH1 mask
#define GPIO_PCTL_PH1_SSI3FSS   0x00000020  // SSI3FSS on PH1
#define GPIO_PCTL_PH1_WT2CCP1   0x00000070  // WT2CCP1 on PH1
#define GPIO_PCTL_PH0_M         0x0000000F  // PH0 mask
#define GPIO_PCTL_PH0_SSI3CLK   0x00000002  // SSI3CLK on PH0
#define GPIO_PCTL_PH0_WT2CCP0   0x00000007  // WT2CCP0 on PH0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port J.
//
//*****************************************************************************
#define GPIO_PCTL_PJ7_M         0xF0000000  // PJ7 mask
#define GPIO_PCTL_PJ6_M         0x0F000000  // PJ6 mask
#define GPIO_PCTL_PJ6_PECI0TX   0x01000000  // PECI0TX on PJ6
#define GPIO_PCTL_PJ5_M         0x00F00000  // PJ5 mask
#define GPIO_PCTL_PJ5_U6TX      0x00100000  // U6TX on PJ5
#define GPIO_PCTL_PJ5_T3CCP1    0x00700000  // T3CCP1 on PJ5
#define GPIO_PCTL_PJ4_M         0x000F0000  // PJ4 mask
#define GPIO_PCTL_PJ4_U6RX      0x00010000  // U6RX on PJ4
#define GPIO_PCTL_PJ4_T3CCP0    0x00070000  // T3CCP0 on PJ4
#define GPIO_PCTL_PJ3_M         0x0000F000  // PJ3 mask
#define GPIO_PCTL_PJ3_U5TX      0x00001000  // U5TX on PJ3
#define GPIO_PCTL_PJ3_T2CCP1    0x00007000  // T2CCP1 on PJ3
#define GPIO_PCTL_PJ2_M         0x00000F00  // PJ2 mask
#define GPIO_PCTL_PJ2_U5RX      0x00000100  // U5RX on PJ2
#define GPIO_PCTL_PJ2_T2CCP0    0x00000700  // T2CCP0 on PJ2
#define GPIO_PCTL_PJ1_M         0x000000F0  // PJ1 mask
#define GPIO_PCTL_PJ1_U4TX      0x00000010  // U4TX on PJ1
#define GPIO_PCTL_PJ1_T1CCP1    0x00000070  // T1CCP1 on PJ1
#define GPIO_PCTL_PJ0_M         0x0000000F  // PJ0 mask
#define GPIO_PCTL_PJ0_U4RX      0x00000001  // U4RX on PJ0
#define GPIO_PCTL_PJ0_T1CCP0    0x00000007  // T1CCP0 on PJ0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port K.
//
//*****************************************************************************
#define GPIO_PCTL_PK7_M         0xF0000000  // PK7 Mask
#define GPIO_PCTL_PK7_FAN0TACH1 0x10000000  // FAN0TACH1 on PK7
#define GPIO_PCTL_PK7_WT1CCP1   0x70000000  // WT1CCP1 on PK7
#define GPIO_PCTL_PK6_M         0x0F000000  // PK6 Mask
#define GPIO_PCTL_PK6_FAN0PWM1  0x01000000  // FAN0PWM1 on PK6
#define GPIO_PCTL_PK6_WT1CCP0   0x07000000  // WT1CCP0 on PK6
#define GPIO_PCTL_PK5_M         0x00F00000  // PK5 Mask
#define GPIO_PCTL_PK5_U7TX      0x00100000  // U7TX on PK5
#define GPIO_PCTL_PK4_M         0x000F0000  // PK4 Mask
#define GPIO_PCTL_PK4_U7RX      0x00010000  // U7RX on PK4
#define GPIO_PCTL_PK4_RTCCLK    0x00070000  // RTCCLK on PK4
#define GPIO_PCTL_PK3_M         0x0000F000  // PK3 Mask
#define GPIO_PCTL_PK3_SSI3TX    0x00002000  // SSI3TX on PK3
#define GPIO_PCTL_PK2_M         0x00000F00  // PK2 Mask
#define GPIO_PCTL_PK2_SSI3RX    0x00000200  // SSI3RX on PK2
#define GPIO_PCTL_PK1_M         0x000000F0  // PK1 Mask
#define GPIO_PCTL_PK1_SSI3FSS   0x00000020  // SSI3FSS on PK1
#define GPIO_PCTL_PK0_M         0x0000000F  // PK0 Mask
#define GPIO_PCTL_PK0_SSI3CLK   0x00000002  // SSI3CLK on PK0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port L.
//
//*****************************************************************************
#define GPIO_PCTL_PL7_M         0xF0000000  // PL7 Mask
#define GPIO_PCTL_PL7_T3CCP1    0x70000000  // T3CCP1 on PL7
#define GPIO_PCTL_PL7_WT3CCP1   0x80000000  // WT3CCP1 on PL7
#define GPIO_PCTL_PL6_M         0x0F000000  // PL6 Mask
#define GPIO_PCTL_PL6_T3CCP0    0x07000000  // T3CCP0 on PL6
#define GPIO_PCTL_PL6_WT3CCP0   0x08000000  // WT3CCP0 on PL6
#define GPIO_PCTL_PL5_M         0x00F00000  // PL5 Mask
#define GPIO_PCTL_PL5_T2CCP1    0x00700000  // T2CCP1 on PL5
#define GPIO_PCTL_PL5_WT2CCP1   0x00800000  // WT2CCP1 on PL5
#define GPIO_PCTL_PL5_LPC0RESET_N \
                                0x00F00000  // LPC0RESET_N on PL5
#define GPIO_PCTL_PL4_M         0x000F0000  // PL4 Mask
#define GPIO_PCTL_PL4_T2CCP0    0x00070000  // T2CCP0 on PL4
#define GPIO_PCTL_PL4_WT2CCP0   0x00080000  // WT2CCP0 on PL4
#define GPIO_PCTL_PL4_LPC0FRAME_N \
                                0x000F0000  // LPC0FRAME_N on PL4
#define GPIO_PCTL_PL3_M         0x0000F000  // PL3 Mask
#define GPIO_PCTL_PL3_T1CCP1    0x00007000  // T1CCP1 on PL3
#define GPIO_PCTL_PL3_WT1CCP1   0x00008000  // WT1CCP1 on PL3
#define GPIO_PCTL_PL3_LPC0AD0   0x0000F000  // LPC0AD0 on PL3
#define GPIO_PCTL_PL2_M         0x00000F00  // PL2 Mask
#define GPIO_PCTL_PL2_T1CCP0    0x00000700  // T1CCP0 on PL2
#define GPIO_PCTL_PL2_WT1CCP0   0x00000800  // WT1CCP0 on PL2
#define GPIO_PCTL_PL2_LPC0AD1   0x00000F00  // LPC0AD1 on PL2
#define GPIO_PCTL_PL1_M         0x000000F0  // PL1 Mask
#define GPIO_PCTL_PL1_T0CCP1    0x00000070  // T0CCP1 on PL1
#define GPIO_PCTL_PL1_WT0CCP1   0x00000080  // WT0CCP1 on PL1
#define GPIO_PCTL_PL1_LPC0AD2   0x000000F0  // LPC0AD2 on PL1
#define GPIO_PCTL_PL0_M         0x0000000F  // PL0 Mask
#define GPIO_PCTL_PL0_T0CCP0    0x00000007  // T0CCP0 on PL0
#define GPIO_PCTL_PL0_WT0CCP0   0x00000008  // WT0CCP0 on PL0
#define GPIO_PCTL_PL0_LPC0AD3   0x0000000F  // LPC0AD3 on PL0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port M.
//
//*****************************************************************************
#define GPIO_PCTL_PM7_M         0xF0000000  // PM7 Mask
#define GPIO_PCTL_PM7_FAN0TACH0 0x10000000  // FAN0TACH0 on PM7
#define GPIO_PCTL_PM7_WT0CCP1   0x70000000  // WT0CCP1 on PM7
#define GPIO_PCTL_PM6_M         0x0F000000  // PM6 Mask
#define GPIO_PCTL_PM6_FAN0PWM0  0x01000000  // FAN0PWM0 on PM6
#define GPIO_PCTL_PM6_WT0CCP0   0x07000000  // WT0CCP0 on PM6
#define GPIO_PCTL_PM5_M         0x00F00000  // PM5 Mask
#define GPIO_PCTL_PM5_LPC0CLK   0x00F00000  // LPC0CLK on PM5
#define GPIO_PCTL_PM4_M         0x000F0000  // PM4 Mask
#define GPIO_PCTL_PM4_LPC0SERIRQ \
                                0x000F0000  // LPC0SERIRQ on PM4
#define GPIO_PCTL_PM3_M         0x0000F000  // PM3 Mask
#define GPIO_PCTL_PM3_T5CCP1    0x00007000  // T5CCP1 on PM3
#define GPIO_PCTL_PM3_WT5CCP1   0x00008000  // WT5CCP1 on PM3
#define GPIO_PCTL_PM2_M         0x00000F00  // PM2 Mask
#define GPIO_PCTL_PM2_T5CCP0    0x00000700  // T5CCP0 on PM2
#define GPIO_PCTL_PM2_WT5CCP0   0x00000800  // WT5CCP0 on PM2
#define GPIO_PCTL_PM2_LPC0CLKRUN_N \
                                0x00000F00  // LPC0CLKRUN_N on PM2
#define GPIO_PCTL_PM1_M         0x000000F0  // PM1 Mask
#define GPIO_PCTL_PM1_T4CCP1    0x00000070  // T4CCP1 on PM1
#define GPIO_PCTL_PM1_WT4CCP1   0x00000080  // WT4CCP1 on PM1
#define GPIO_PCTL_PM1_LPC0SCI_N 0x000000F0  // LPC0SCI_N on PM1
#define GPIO_PCTL_PM0_M         0x0000000F  // PM0 Mask
#define GPIO_PCTL_PM0_T4CCP0    0x00000007  // T4CCP0 on PM0
#define GPIO_PCTL_PM0_WT4CCP0   0x00000008  // WT4CCP0 on PM0
#define GPIO_PCTL_PM0_LPC0PD_N  0x0000000F  // LPC0PD_N on PM0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port N.
//
//*****************************************************************************
#define GPIO_PCTL_PN7_M         0xF0000000  // PN7 Mask
#define GPIO_PCTL_PN7_FAN0TACH4 0x10000000  // FAN0TACH4 on PN7
#define GPIO_PCTL_PN7_WT4CCP1   0x70000000  // WT4CCP1 on PN7
#define GPIO_PCTL_PN6_M         0x0F000000  // PN6 Mask
#define GPIO_PCTL_PN6_FAN0PWM4  0x01000000  // FAN0PWM4 on PN6
#define GPIO_PCTL_PN6_WT4CCP0   0x07000000  // WT4CCP0 on PN6
#define GPIO_PCTL_PN5_M         0x00F00000  // PN5 Mask
#define GPIO_PCTL_PN5_FAN0TACH3 0x00100000  // FAN0TACH3 on PN5
#define GPIO_PCTL_PN5_WT3CCP1   0x00700000  // WT3CCP1 on PN5
#define GPIO_PCTL_PN4_M         0x000F0000  // PN4 Mask
#define GPIO_PCTL_PN4_FAN0PWM3  0x00010000  // FAN0PWM3 on PN4
#define GPIO_PCTL_PN4_WT3CCP0   0x00070000  // WT3CCP0 on PN4
#define GPIO_PCTL_PN3_M         0x0000F000  // PN3 Mask
#define GPIO_PCTL_PN3_FAN0TACH2 0x00001000  // FAN0TACH2 on PN3
#define GPIO_PCTL_PN3_WT2CCP1   0x00007000  // WT2CCP1 on PN3
#define GPIO_PCTL_PN2_M         0x00000F00  // PN2 Mask
#define GPIO_PCTL_PN2_FAN0PWM2  0x00000100  // FAN0PWM2 on PN2
#define GPIO_PCTL_PN2_WT2CCP0   0x00000700  // WT2CCP0 on PN2
#define GPIO_PCTL_PN1_M         0x000000F0  // PN1 Mask
#define GPIO_PCTL_PN0_M         0x0000000F  // PN0 Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port P.
//
//*****************************************************************************
#define GPIO_PCTL_PP7_M         0xF0000000  // PP7 Mask
#define GPIO_PCTL_PP7_WT1CCP1   0x70000000  // WT1CCP1 on PP7
#define GPIO_PCTL_PP6_M         0x0F000000  // PP6 Mask
#define GPIO_PCTL_PP6_WT1CCP0   0x07000000  // WT1CCP0 on PP6
#define GPIO_PCTL_PP5_M         0x00F00000  // PP5 Mask
#define GPIO_PCTL_PP5_WT0CCP1   0x00700000  // WT0CCP1 on PP5
#define GPIO_PCTL_PP4_M         0x000F0000  // PP4 Mask
#define GPIO_PCTL_PP4_WT0CCP0   0x00070000  // WT0CCP0 on PP4
#define GPIO_PCTL_PP3_M         0x0000F000  // PP3 Mask
#define GPIO_PCTL_PP3_T5CCP1    0x00007000  // T5CCP1 on PP3
#define GPIO_PCTL_PP2_M         0x00000F00  // PP2 Mask
#define GPIO_PCTL_PP2_T5CCP0    0x00000700  // T5CCP0 on PP2
#define GPIO_PCTL_PP1_M         0x000000F0  // PP1 Mask
#define GPIO_PCTL_PP1_T4CCP1    0x00000070  // T4CCP1 on PP1
#define GPIO_PCTL_PP0_M         0x0000000F  // PP0 Mask
#define GPIO_PCTL_PP0_T4CCP0    0x00000007  // T4CCP0 on PP0

//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_PCTL register for
// port Q.
//
//*****************************************************************************
#define GPIO_PCTL_PQ7_M         0xF0000000  // PQ7 Mask
#define GPIO_PCTL_PQ7_WT5CCP1   0x70000000  // WT5CCP1 on PQ7
#define GPIO_PCTL_PQ6_M         0x0F000000  // PQ6 Mask
#define GPIO_PCTL_PQ6_WT5CCP0   0x07000000  // WT5CCP0 on PQ6
#define GPIO_PCTL_PQ5_M         0x00F00000  // PQ5 Mask
#define GPIO_PCTL_PQ5_WT4CCP1   0x00700000  // WT4CCP1 on PQ5
#define GPIO_PCTL_PQ4_M         0x000F0000  // PQ4 Mask
#define GPIO_PCTL_PQ4_WT4CCP0   0x00070000  // WT4CCP0 on PQ4
#define GPIO_PCTL_PQ3_M         0x0000F000  // PQ3 Mask
#define GPIO_PCTL_PQ3_WT3CCP1   0x00007000  // WT3CCP1 on PQ3
#define GPIO_PCTL_PQ2_M         0x00000F00  // PQ2 Mask
#define GPIO_PCTL_PQ2_WT3CCP0   0x00000700  // WT3CCP0 on PQ2
#define GPIO_PCTL_PQ1_M         0x000000F0  // PQ1 Mask
#define GPIO_PCTL_PQ1_WT2CCP1   0x00000070  // WT2CCP1 on PQ1
#define GPIO_PCTL_PQ0_M         0x0000000F  // PQ0 Mask
#define GPIO_PCTL_PQ0_WT2CCP0   0x00000007  // WT2CCP0 on PQ0

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CR0 register.
//
//*****************************************************************************
#define SSI_CR0_SCR_M           0x0000FF00  // SSI Serial Clock Rate
#define SSI_CR0_SPH             0x00000080  // SSI Serial Clock Phase
#define SSI_CR0_SPO             0x00000040  // SSI Serial Clock Polarity
#define SSI_CR0_FRF_M           0x00000030  // SSI Frame Format Select
#define SSI_CR0_FRF_MOTO        0x00000000  // Freescale SPI Frame Format
#define SSI_CR0_FRF_TI          0x00000010  // Texas Instruments Synchronous
                                            // Serial Frame Format
#define SSI_CR0_FRF_NMW         0x00000020  // MICROWIRE Frame Format
#define SSI_CR0_DSS_M           0x0000000F  // SSI Data Size Select
#define SSI_CR0_DSS_4           0x00000003  // 4-bit data
#define SSI_CR0_DSS_5           0x00000004  // 5-bit data
#define SSI_CR0_DSS_6           0x00000005  // 6-bit data
#define SSI_CR0_DSS_7           0x00000006  // 7-bit data
#define SSI_CR0_DSS_8           0x00000007  // 8-bit data
#define SSI_CR0_DSS_9           0x00000008  // 9-bit data
#define SSI_CR0_DSS_10          0x00000009  // 10-bit data
#define SSI_CR0_DSS_11          0x0000000A  // 11-bit data
#define SSI_CR0_DSS_12          0x0000000B  // 12-bit data
#define SSI_CR0_DSS_13          0x0000000C  // 13-bit data
#define SSI_CR0_DSS_14          0x0000000D  // 14-bit data
#define SSI_CR0_DSS_15          0x0000000E  // 15-bit data
#define SSI_CR0_DSS_16          0x0000000F  // 16-bit data
#define SSI_CR0_SCR_S           8

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CR1 register.
//
//*****************************************************************************
#define SSI_CR1_EOT             0x00000010  // End of Transmission
#define SSI_CR1_SOD             0x00000008  // SSI Slave Mode Output Disable
#define SSI_CR1_MS              0x00000004  // SSI Master/Slave Select
#define SSI_CR1_SSE             0x00000002  // SSI Synchronous Serial Port
                                            // Enable
#define SSI_CR1_LBM             0x00000001  // SSI Loopback Mode

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_DR register.
//
//*****************************************************************************
#define SSI_DR_DATA_M           0x0000FFFF  // SSI Receive/Transmit Data
#define SSI_DR_DATA_S           0

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_SR register.
//
//*****************************************************************************
#define SSI_SR_BSY              0x00000010  // SSI Busy Bit
#define SSI_SR_RFF              0x00000008  // SSI Receive FIFO Full
#define SSI_SR_RNE              0x00000004  // SSI Receive FIFO Not Empty
#define SSI_SR_TNF              0x00000002  // SSI Transmit FIFO Not Full
#define SSI_SR_TFE              0x00000001  // SSI Transmit FIFO Empty

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CPSR register.
//
//*****************************************************************************
#define SSI_CPSR_CPSDVSR_M      0x000000FF  // SSI Clock Prescale Divisor
#define SSI_CPSR_CPSDVSR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_IM register.
//
//*****************************************************************************
#define SSI_IM_TXIM             0x00000008  // SSI Transmit FIFO Interrupt Mask
#define SSI_IM_RXIM             0x00000004  // SSI Receive FIFO Interrupt Mask
#define SSI_IM_RTIM             0x00000002  // SSI Receive Time-Out Interrupt
                                            // Mask
#define SSI_IM_RORIM            0x00000001  // SSI Receive Overrun Interrupt
                                            // Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_RIS register.
//
//*****************************************************************************
#define SSI_RIS_TXRIS           0x00000008  // SSI Transmit FIFO Raw Interrupt
                                            // Status
#define SSI_RIS_RXRIS           0x00000004  // SSI Receive FIFO Raw Interrupt
                                            // Status
#define SSI_RIS_RTRIS           0x00000002  // SSI Receive Time-Out Raw
                                            // Interrupt Status
#define SSI_RIS_RORRIS          0x00000001  // SSI Receive Overrun Raw
                                            // Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_MIS register.
//
//*****************************************************************************
#define SSI_MIS_TXMIS           0x00000008  // SSI Transmit FIFO Masked
                                            // Interrupt Status
#define SSI_MIS_RXMIS           0x00000004  // SSI Receive FIFO Masked
                                            // Interrupt Status
#define SSI_MIS_RTMIS           0x00000002  // SSI Receive Time-Out Masked
                                            // Interrupt Status
#define SSI_MIS_RORMIS          0x00000001  // SSI Receive Overrun Masked
                                            // Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_ICR register.
//
//*****************************************************************************
#define SSI_ICR_RTIC            0x00000002  // SSI Receive Time-Out Interrupt
                                            // Clear
#define SSI_ICR_RORIC           0x00000001  // SSI Receive Overrun Interrupt
                                            // Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_DMACTL register.
//
//*****************************************************************************
#define SSI_DMACTL_TXDMAE       0x00000002  // Transmit DMA Enable
#define SSI_DMACTL_RXDMAE       0x00000001  // Receive DMA Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CC register.
//
//*****************************************************************************
#define SSI_CC_CS_M             0x0000000F  // SSI Baud Clock Source
#define SSI_CC_CS_SYSPLL        0x00000000  // Either the system clock (if the
                                            // PLL bypass is in effect) or the
                                            // PLL output (default)
#define SSI_CC_CS_PIOSC         0x00000005  // PIOSC

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_DR register.
//
//*****************************************************************************
#define UART_DR_OE              0x00000800  // UART Overrun Error
#define UART_DR_BE              0x00000400  // UART Break Error
#define UART_DR_PE              0x00000200  // UART Parity Error
#define UART_DR_FE              0x00000100  // UART Framing Error
#define UART_DR_DATA_M          0x000000FF  // Data Transmitted or Received
#define UART_DR_DATA_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RSR register.
//
//*****************************************************************************
#define UART_RSR_OE             0x00000008  // UART Overrun Error
#define UART_RSR_BE             0x00000004  // UART Break Error
#define UART_RSR_PE             0x00000002  // UART Parity Error
#define UART_RSR_FE             0x00000001  // UART Framing Error

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_ECR register.
//
//*****************************************************************************
#define UART_ECR_DATA_M         0x000000FF  // Error Clear
#define UART_ECR_DATA_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FR register.
//
//*****************************************************************************
#define UART_FR_TXFE            0x00000080  // UART Transmit FIFO Empty
#define UART_FR_RXFF            0x00000040  // UART Receive FIFO Full
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_FR_BUSY            0x00000008  // UART Busy

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_ILPR register.
//
//*****************************************************************************
#define UART_ILPR_ILPDVSR_M     0x000000FF  // IrDA Low-Power Divisor
#define UART_ILPR_ILPDVSR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_IBRD register.
//
//*****************************************************************************
#define UART_IBRD_DIVINT_M      0x0000FFFF  // Integer Baud-Rate Divisor
#define UART_IBRD_DIVINT_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FBRD register.
//
//*****************************************************************************
#define UART_FBRD_DIVFRAC_M     0x0000003F  // Fractional Baud-Rate Divisor
#define UART_FBRD_DIVFRAC_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LCRH register.
//
//*****************************************************************************
#define UART_LCRH_SPS           0x00000080  // UART Stick Parity Select
#define UART_LCRH_WLEN_M        0x00000060  // UART Word Length
#define UART_LCRH_WLEN_5        0x00000000  // 5 bits (default)
#define UART_LCRH_WLEN_6        0x00000020  // 6 bits
#define UART_LCRH_WLEN_7        0x00000040  // 7 bits
#define UART_LCRH_WLEN_8        0x00000060  // 8 bits
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_LCRH_STP2          0x00000008  // UART Two Stop Bits Select
#define UART_LCRH_EPS           0x00000004  // UART Even Parity Select
#define UART_LCRH_PEN           0x00000002  // UART Parity Enable
#define UART_LCRH_BRK           0x00000001  // UART Send Break

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CTL register.
//
//*****************************************************************************
#define UART_CTL_RXE            0x00000200  // UART Receive Enable
#define UART_CTL_TXE            0x00000100  // UART Transmit Enable
#define UART_CTL_LBE            0x00000080  // UART Loop Back Enable
#define UART_CTL_LIN            0x00000040  // LIN Mode Enable
#define UART_CTL_HSE            0x00000020  // High-Speed Enable
#define UART_CTL_EOT            0x00000010  // End of Transmission
#define UART_CTL_SMART          0x00000008  // ISO 7816 Smart Card Support
#define UART_CTL_SIRLP          0x00000004  // UART SIR Low-Power Mode
#define UART_CTL_SIREN          0x00000002  // UART SIR Enable
#define UART_CTL_UARTEN         0x00000001  // UART Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_IFLS register.
//
//*****************************************************************************
#define UART_IFLS_RX_M          0x00000038  // UART Receive Interrupt FIFO
                                            // Level Select
#define UART_IFLS_RX1_8         0x00000000  // RX FIFO >= 1/8 full
#define UART_IFLS_RX2_8         0x00000008  // RX FIFO >= 1/4 full
#define UART_IFLS_RX4_8         0x00000010  // RX FIFO >= 1/2 full (default)
#define UART_IFLS_RX6_8         0x00000018  // RX FIFO >= 3/4 full
#define UART_IFLS_RX7_8         0x00000020  // RX FIFO >= 7/8 full
#define UART_IFLS_TX_M          0x00000007  // UART Transmit Interrupt FIFO
                                            // Level Select
#define UART_IFLS_TX1_8         0x00000000  // TX FIFO <= 1/8 full
#define UART_IFLS_TX2_8         0x00000001  // TX FIFO <= 1/4 full
#define UART_IFLS_TX4_8         0x00000002  // TX FIFO <= 1/2 full (default)
#define UART_IFLS_TX6_8         0x00000003  // TX FIFO <= 3/4 full
#define UART_IFLS_TX7_8         0x00000004  // TX FIFO <= 7/8 full

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_IM register.
//
//*****************************************************************************
#define UART_IM_LME5IM          0x00008000  // LIN Mode Edge 5 Interrupt Mask
#define UART_IM_LME1IM          0x00004000  // LIN Mode Edge 1 Interrupt Mask
#define UART_IM_LMSBIM          0x00002000  // LIN Mode Sync Break Interrupt
                                            // Mask
#define UART_IM_9BITIM          0x00001000  // 9-Bit Mode Interrupt Mask
#define UART_IM_OEIM            0x00000400  // UART Overrun Error Interrupt
                                            // Mask
#define UART_IM_BEIM            0x00000200  // UART Break Error Interrupt Mask
#define UART_IM_PEIM            0x00000100  // UART Parity Error Interrupt Mask
#define UART_IM_FEIM            0x00000080  // UART Framing Error Interrupt
                                            // Mask
#define UART_IM_RTIM            0x00000040  // UART Receive Time-Out Interrupt
                                            // Mask
#define UART_IM_TXIM            0x00000020  // UART Transmit Interrupt Mask
#define UART_IM_RXIM            0x00000010  // UART Receive Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RIS register.
//
//*****************************************************************************
#define UART_RIS_LME5RIS        0x00008000  // LIN Mode Edge 5 Raw Interrupt
                                            // Status
#define UART_RIS_LME1RIS        0x00004000  // LIN Mode Edge 1 Raw Interrupt
                                            // Status
#define UART_RIS_LMSBRIS        0x00002000  // LIN Mode Sync Break Raw
                                            // Interrupt Status
#define UART_RIS_9BITRIS        0x00001000  // 9-Bit Mode Raw Interrupt Status
#define UART_RIS_OERIS          0x00000400  // UART Overrun Error Raw Interrupt
                                            // Status
#define UART_RIS_BERIS          0x00000200  // UART Break Error Raw Interrupt
                                            // Status
#define UART_RIS_PERIS          0x00000100  // UART Parity Error Raw Interrupt
                                            // Status
#define UART_RIS_FERIS          0x00000080  // UART Framing Error Raw Interrupt
                                            // Status
#define UART_RIS_RTRIS          0x00000040  // UART Receive Time-Out Raw
                                            // Interrupt Status
#define UART_RIS_TXRIS          0x00000020  // UART Transmit Raw Interrupt
                                            // Status
#define UART_RIS_RXRIS          0x00000010  // UART Receive Raw Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_MIS register.
//
//*****************************************************************************
#define UART_MIS_LME5MIS        0x00008000  // LIN Mode Edge 5 Masked Interrupt
                                            // Status
#define UART_MIS_LME1MIS        0x00004000  // LIN Mode Edge 1 Masked Interrupt
                                            // Status
#define UART_MIS_LMSBMIS        0x00002000  // LIN Mode Sync Break Masked
                                            // Interrupt Status
#define UART_MIS_9BITMIS        0x00001000  // 9-Bit Mode Masked Interrupt
                                            // Status
#define UART_MIS_OEMIS          0x00000400  // UART Overrun Error Masked
                                            // Interrupt Status
#define UART_MIS_BEMIS          0x00000200  // UART Break Error Masked
                                            // Interrupt Status
#define UART_MIS_PEMIS          0x00000100  // UART Parity Error Masked
                                            // Interrupt Status
#define UART_MIS_FEMIS          0x00000080  // UART Framing Error Masked
                                            // Interrupt Status
#define UART_MIS_RTMIS          0x00000040  // UART Receive Time-Out Masked
                                            // Interrupt Status
#define UART_MIS_TXMIS          0x00000020  // UART Transmit Masked Interrupt
                                            // Status
#define UART_MIS_RXMIS          0x00000010  // UART Receive Masked Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_ICR register.
//
//*****************************************************************************
#define UART_ICR_LME5IC         0x00008000  // LIN Mode Edge 5 Interrupt Clear
#define UART_ICR_LME1IC         0x00004000  // LIN Mode Edge 1 Interrupt Clear
#define UART_ICR_LMSBIC         0x00002000  // LIN Mode Sync Break Interrupt
                                            // Clear
#define UART_ICR_9BITIC         0x00001000  // 9-Bit Mode Interrupt Clear
#define UART_ICR_OEIC           0x00000400  // Overrun Error Interrupt Clear
#define UART_ICR_BEIC           0x00000200  // Break Error Interrupt Clear
#define UART_ICR_PEIC           0x00000100  // Parity Error Interrupt Clear
#define UART_ICR_FEIC           0x00000080  // Framing Error Interrupt Clear
#define UART_ICR_RTIC           0x00000040  // Receive Time-Out Interrupt Clear
#define UART_ICR_TXIC           0x00000020  // Transmit Interrupt Clear
#define UART_ICR_RXIC           0x00000010  // Receive Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_DMACTL register.
//
//*****************************************************************************
#define UART_DMACTL_DMAERR      0x00000004  // DMA on Error
#define UART_DMACTL_TXDMAE      0x00000002  // Transmit DMA Enable
#define UART_DMACTL_RXDMAE      0x00000001  // Receive DMA Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LCTL register.
//
//*****************************************************************************
#define UART_LCTL_BLEN_M        0x00000030  // Sync Break Length
#define UART_LCTL_BLEN_13T      0x00000000  // Sync break length is 13T bits
                                            // (default)
#define UART_LCTL_BLEN_14T      0x00000010  // Sync break length is 14T bits
#define UART_LCTL_BLEN_15T      0x00000020  // Sync break length is 15T bits
#define UART_LCTL_BLEN_16T      0x00000030  // Sync break length is 16T bits
#define UART_LCTL_MASTER        0x00000001  // LIN Master Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LSS register.
//
//*****************************************************************************
#define UART_LSS_TSS_M          0x0000FFFF  // Timer Snap Shot
#define UART_LSS_TSS_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LTIM register.
//
//*****************************************************************************
#define UART_LTIM_TIMER_M       0x0000FFFF  // Timer Value
#define UART_LTIM_TIMER_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_9BITADDR
// register.
//
//*****************************************************************************
#define UART_9BITADDR_9BITEN    0x00008000  // Enable 9-Bit Mode
#define UART_9BITADDR_ADDR_M    0x000000FF  // Self Address for 9-Bit Mode
#define UART_9BITADDR_ADDR_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_9BITAMASK
// register.
//
//*****************************************************************************
#define UART_9BITAMASK_MASK_M   0x000000FF  // Self Address Mask for 9-Bit Mode
#define UART_9BITAMASK_MASK_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_PP register.
//
//*****************************************************************************
#define UART_PP_NB              0x00000002  // 9-Bit Support
#define UART_PP_SC              0x00000001  // Smart Card Support

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CC register.
//
//*****************************************************************************
#define UART_CC_CS_M            0x0000000F  // UART Baud Clock Source
#define UART_CC_CS_SYSCLK       0x00000000  // The system clock (default)
#define UART_CC_CS_PIOSC        0x00000005  // PIOSC

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MSA register.
//
//*****************************************************************************
#define I2C_MSA_SA_M            0x000000FE  // I2C Slave Address
#define I2C_MSA_RS              0x00000001  // Receive not send
#define I2C_MSA_SA_S            1

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SOAR register.
//
//*****************************************************************************
#define I2C_SOAR_OAR_M          0x0000007F  // I2C Slave Own Address
#define I2C_SOAR_OAR_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SCSR register.
//
//*****************************************************************************
#define I2C_SCSR_QCMDRW         0x00000020  // Quick Command Read / Write
#define I2C_SCSR_QCMDST         0x00000010  // Quick Command Status
#define I2C_SCSR_OAR2SEL        0x00000008  // OAR2 Address Matched
#define I2C_SCSR_FBR            0x00000004  // First Byte Received
#define I2C_SCSR_TREQ           0x00000002  // Transmit Request
#define I2C_SCSR_DA             0x00000001  // Device Active
#define I2C_SCSR_RREQ           0x00000001  // Receive Request

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCS register.
//
//*****************************************************************************
#define I2C_MCS_CLKTO           0x00000080  // Clock Timeout Error
#define I2C_MCS_BUSBSY          0x00000040  // Bus Busy
#define I2C_MCS_IDLE            0x00000020  // I2C Idle
#define I2C_MCS_QCMD            0x00000020  // Quick Command
#define I2C_MCS_ARBLST          0x00000010  // Arbitration Lost
#define I2C_MCS_HS              0x00000010  // High-Speed Enable
#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SDR register.
//
//*****************************************************************************
#define I2C_SDR_DATA_M          0x000000FF  // Data for Transfer
#define I2C_SDR_DATA_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MDR register.
//
//*****************************************************************************
#define I2C_MDR_DATA_M          0x000000FF  // Data Transferred
#define I2C_MDR_DATA_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MTPR register.
//
//*****************************************************************************
#define I2C_MTPR_HS             0x00000080  // High-Speed Enable
#define I2C_MTPR_TPR_M          0x0000007F  // SCL Clock Period
#define I2C_MTPR_TPR_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SIMR register.
//
//*****************************************************************************
#define I2C_SIMR_STOPIM         0x00000004  // Stop Condition Interrupt Mask
#define I2C_SIMR_STARTIM        0x00000002  // Start Condition Interrupt Mask
#define I2C_SIMR_DATAIM         0x00000001  // Data Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SRIS register.
//
//*****************************************************************************
#define I2C_SRIS_STOPRIS        0x00000004  // Stop Condition Raw Interrupt
                                            // Status
#define I2C_SRIS_STARTRIS       0x00000002  // Start Condition Raw Interrupt
                                            // Status
#define I2C_SRIS_DATARIS        0x00000001  // Data Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MIMR register.
//
//*****************************************************************************
#define I2C_MIMR_CLKIM          0x00000002  // Clock Timeout Interrupt Mask
#define I2C_MIMR_IM             0x00000001  // Master Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MRIS register.
//
//*****************************************************************************
#define I2C_MRIS_CLKRIS         0x00000002  // Clock Timeout Raw Interrupt
                                            // Status
#define I2C_MRIS_RIS            0x00000001  // Master Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SMIS register.
//
//*****************************************************************************
#define I2C_SMIS_STOPMIS        0x00000004  // Stop Condition Masked Interrupt
                                            // Status
#define I2C_SMIS_STARTMIS       0x00000002  // Start Condition Masked Interrupt
                                            // Status
#define I2C_SMIS_DATAMIS        0x00000001  // Data Masked Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SICR register.
//
//*****************************************************************************
#define I2C_SICR_STOPIC         0x00000004  // Stop Condition Interrupt Clear
#define I2C_SICR_STARTIC        0x00000002  // Start Condition Interrupt Clear
#define I2C_SICR_DATAIC         0x00000001  // Data Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MMIS register.
//
//*****************************************************************************
#define I2C_MMIS_CLKMIS         0x00000002  // Clock Timeout Masked Interrupt
                                            // Status
#define I2C_MMIS_MIS            0x00000001  // Masked Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MICR register.
//
//*****************************************************************************
#define I2C_MICR_CLKIC          0x00000002  // Clock Timeout Interrupt Clear
#define I2C_MICR_IC             0x00000001  // Master Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SOAR2 register.
//
//*****************************************************************************
#define I2C_SOAR2_OAR2EN        0x00000080  // I2C Slave Own Address 2 Enable
#define I2C_SOAR2_OAR2_M        0x0000007F  // I2C Slave Own Address 2
#define I2C_SOAR2_OAR2_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCR register.
//
//*****************************************************************************
#define I2C_MCR_SFE             0x00000020  // I2C Slave Function Enable
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable
#define I2C_MCR_LPBK            0x00000001  // I2C Loopback

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SACKCTL register.
//
//*****************************************************************************
#define I2C_SACKCTL_ACKOVAL     0x00000002  // I2C Slave ACK Override Value
#define I2C_SACKCTL_ACKOEN      0x00000001  // I2C Slave ACK Override Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCLKOCNT register.
//
//*****************************************************************************
#define I2C_MCLKOCNT_CNTL_M     0x000000FF  // I2C Master Count
#define I2C_MCLKOCNT_CNTL_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MBMON register.
//
//*****************************************************************************
#define I2C_MBMON_SDA           0x00000002  // I2C SDA Status
#define I2C_MBMON_SCL           0x00000001  // I2C SCL Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PP register.
//
//*****************************************************************************
#define I2C_PP_HS               0x00000001  // High-Speed Capable

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PC register.
//
//*****************************************************************************
#define I2C_PC_HS               0x00000001  // High-Speed Capable

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_CFG register.
//
//*****************************************************************************
#define TIMER_CFG_M             0x00000007  // GPTM Configuration
#define TIMER_CFG_32_BIT_TIMER  0x00000000  // 32-bit timer configuration
#define TIMER_CFG_32_BIT_RTC    0x00000001  // 32-bit real-time clock (RTC)
                                            // counter configuration
#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration. The
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAMR register.
//
//*****************************************************************************
#define TIMER_TAMR_TAPLO        0x00000800  // GPTM Timer A PWM Legacy
                                            // Operation
#define TIMER_TAMR_TAMRSU       0x00000400  // GPTM Timer A Match Register
                                            // Update
#define TIMER_TAMR_TAPWMIE      0x00000200  // GPTM Timer A PWM Interrupt
                                            // Enable
#define TIMER_TAMR_TAILD        0x00000100  // GPTM Timer A Interval Load Write
#define TIMER_TAMR_TASNAPS      0x00000080  // GPTM Timer A Snap-Shot Mode
#define TIMER_TAMR_TAWOT        0x00000040  // GPTM Timer A Wait-on-Trigger
#define TIMER_TAMR_TAMIE        0x00000020  // GPTM Timer A Match Interrupt
                                            // Enable
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAAMS        0x00000008  // GPTM Timer A Alternate Mode
                                            // Select
#define TIMER_TAMR_TACMR        0x00000004  // GPTM Timer A Capture Mode
#define TIMER_TAMR_TAMR_M       0x00000003  // GPTM Timer A Mode
#define TIMER_TAMR_TAMR_1_SHOT  0x00000001  // One-Shot Timer mode
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBMR register.
//
//*****************************************************************************
#define TIMER_TBMR_TBPLO        0x00000800  // GPTM Timer B PWM Legacy
                                            // Operation
#define TIMER_TBMR_TBMRSU       0x00000400  // GPTM Timer B Match Register
                                            // Update
#define TIMER_TBMR_TBPWMIE      0x00000200  // GPTM Timer B PWM Interrupt
                                            // Enable
#define TIMER_TBMR_TBILD        0x00000100  // GPTM Timer B Interval Load Write
#define TIMER_TBMR_TBSNAPS      0x00000080  // GPTM Timer B Snap-Shot Mode
#define TIMER_TBMR_TBWOT        0x00000040  // GPTM Timer B Wait-on-Trigger
#define TIMER_TBMR_TBMIE        0x00000020  // GPTM Timer B Match Interrupt
                                            // Enable
#define TIMER_TBMR_TBCDIR       0x00000010  // GPTM Timer B Count Direction
#define TIMER_TBMR_TBAMS        0x00000008  // GPTM Timer B Alternate Mode
                                            // Select
#define TIMER_TBMR_TBCMR        0x00000004  // GPTM Timer B Capture Mode
#define TIMER_TBMR_TBMR_M       0x00000003  // GPTM Timer B Mode
#define TIMER_TBMR_TBMR_1_SHOT  0x00000001  // One-Shot Timer mode
#define TIMER_TBMR_TBMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_TBMR_TBMR_CAP     0x00000003  // Capture mode

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_CTL register.
//
//*****************************************************************************
#define TIMER_CTL_TBPWML        0x00004000  // GPTM Timer B PWM Output Level
#define TIMER_CTL_TBOTE         0x00002000  // GPTM Timer B Output Trigger
                                            // Enable
#define TIMER_CTL_TBEVENT_M     0x00000C00  // GPTM Timer B Event Mode
#define TIMER_CTL_TBEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TBEVENT_NEG   0x00000400  // Negative edge
#define TIMER_CTL_TBEVENT_BOTH  0x00000C00  // Both edges
#define TIMER_CTL_TBSTALL       0x00000200  // GPTM Timer B Stall Enable
#define TIMER_CTL_TBEN          0x00000100  // GPTM Timer B Enable
#define TIMER_CTL_TAPWML        0x00000040  // GPTM Timer A PWM Output Level
#define TIMER_CTL_TAOTE         0x00000020  // GPTM Timer A Output Trigger
                                            // Enable
#define TIMER_CTL_RTCEN         0x00000010  // GPTM RTC Stall Enable
#define TIMER_CTL_TAEVENT_M     0x0000000C  // GPTM Timer A Event Mode
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TAEVENT_NEG   0x00000004  // Negative edge
#define TIMER_CTL_TAEVENT_BOTH  0x0000000C  // Both edges
#define TIMER_CTL_TASTALL       0x00000002  // GPTM Timer A Stall Enable
#define TIMER_CTL_TAEN          0x00000001  // GPTM Timer A Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_SYNC register.
//
//*****************************************************************************
#define TIMER_SYNC_SYNCWT5_M    0x00C00000  // Synchronize GPTM 32/64-Bit Timer
                                            // 5
#define TIMER_SYNC_SYNCWT5_NONE 0x00000000  // GPTM 32/64-Bit Timer 5 is not
                                            // affected
#define TIMER_SYNC_SYNCWT5_TA   0x00400000  // A timeout event for Timer A of
                                            // GPTM 32/64-Bit Timer 5 is
                                            // triggered
#define TIMER_SYNC_SYNCWT5_TB   0x00800000  // A timeout event for Timer B of
                                            // GPTM 32/64-Bit Timer 5 is
                                            // triggered
#define TIMER_SYNC_SYNCWT5_TATB 0x00C00000  // A timeout event for both Timer A
                                            // and Timer B of GPTM 32/64-Bit
                                            // Timer 5 is triggered
#define TIMER_SYNC_SYNCWT4_M    0x00300000  // Synchronize GPTM 32/64-Bit Timer
                                            // 4
#define TIMER_SYNC_SYNCWT4_NONE 0x00000000  // GPTM 32/64-Bit Timer 4 is not
                                            // affected
#define TIMER_SYNC_SYNCWT4_TA   0x00100000  // A timeout event for Timer A of
                                            // GPTM 32/64-Bit Timer 4 is
                                            // triggered
#define TIMER_SYNC_SYNCWT4_TB   0x00200000  // A timeout event for Timer B of
                                            // GPTM 32/64-Bit Timer 4 is
                                            // triggered
#define TIMER_SYNC_SYNCWT4_TATB 0x00300000  // A timeout event for both Timer A
                                            // and Timer B of GPTM 32/64-Bit
                                            // Timer 4 is triggered
#define TIMER_SYNC_SYNCWT3_M    0x000C0000  // Synchronize GPTM 32/64-Bit Timer
                                            // 3
#define TIMER_SYNC_SYNCWT3_NONE 0x00000000  // GPTM 32/64-Bit Timer 3 is not
                                            // affected
#define TIMER_SYNC_SYNCWT3_TA   0x00040000  // A timeout event for Timer A of
                                            // GPTM 32/64-Bit Timer 3 is
                                            // triggered
#define TIMER_SYNC_SYNCWT3_TB   0x00080000  // A timeout event for Timer B of
                                            // GPTM 32/64-Bit Timer 3 is
                                            // triggered
#define TIMER_SYNC_SYNCWT3_TATB 0x000C0000  // A timeout event for both Timer A
                                            // and Timer B of GPTM 32/64-Bit
                                            // Timer 3 is triggered
#define TIMER_SYNC_SYNCWT2_M    0x00030000  // Synchronize GPTM 32/64-Bit Timer
                                            // 2
#define TIMER_SYNC_SYNCWT2_NONE 0x00000000  // GPTM 32/64-Bit Timer 2 is not
                                            // affected
#define TIMER_SYNC_SYNCWT2_TA   0x00010000  // A timeout event for Timer A of
                                            // GPTM 32/64-Bit Timer 2 is
                                            // triggered
#define TIMER_SYNC_SYNCWT2_TB   0x00020000  // A timeout event for Timer B of
                                            // GPTM 32/64-Bit Timer 2 is
                                            // triggered
#define TIMER_SYNC_SYNCWT2_TATB 0x00030000  // A timeout event for both Timer A
                                            // and Timer B of GPTM 32/64-Bit
                                            // Timer 2 is triggered
#define TIMER_SYNC_SYNCWT1_M    0x0000C000  // Synchronize GPTM 32/64-Bit Timer
                                            // 1
#define TIMER_SYNC_SYNCWT1_NONE 0x00000000  // GPTM 32/64-Bit Timer 1 is not
                                            // affected
#define TIMER_SYNC_SYNCWT1_TA   0x00004000  // A timeout event for Timer A of
                                            // GPTM 32/64-Bit Timer 1 is
                                            // triggered
#define TIMER_SYNC_SYNCWT1_TB   0x00008000  // A timeout event for Timer B of
                                            // GPTM 32/64-Bit Timer 1 is
                                            // triggered
#define TIMER_SYNC_SYNCWT1_TATB 0x0000C000  // A timeout event for both Timer A
                                            // and Timer B of GPTM 32/64-Bit
                                            // Timer 1 is triggered
#define TIMER_SYNC_SYNCWT0_M    0x00003000  // Synchronize GPTM 32/64-Bit Timer
                                            // 0
#define TIMER_SYNC_SYNCWT0_NONE 0x00000000  // GPTM 32/64-Bit Timer 0 is not
                                            // affected
#define TIMER_SYNC_SYNCWT0_TA   0x00001000  // A timeout event for Timer A of
                                            // GPTM 32/64-Bit Timer 0 is
                                            // triggered
#define TIMER_SYNC_SYNCWT0_TB   0x00002000  // A timeout event for Timer B of
                                            // GPTM 32/64-Bit Timer 0 is
                                            // triggered
#define TIMER_SYNC_SYNCWT0_TATB 0x00003000  // A timeout event for both Timer A
                                            // and Timer B of GPTM 32/64-Bit
                                            // Timer 0 is triggered
#define TIMER_SYNC_SYNCT5_M     0x00000C00  // Synchronize GPTM 16/32-Bit Timer
                                            // 5
#define TIMER_SYNC_SYNCT5_NONE  0x00000000  // GPTM 16/32-Bit Timer 5 is not
                                            // affected
#define TIMER_SYNC_SYNCT5_TA    0x00000400  // A timeout event for Timer A of
                                            // GPTM 16/32-Bit Timer 5 is
                                            // triggered
#define TIMER_SYNC_SYNCT5_TB    0x00000800  // A timeout event for Timer B of
                                            // GPTM 16/32-Bit Timer 5 is
                                            // triggered
#define TIMER_SYNC_SYNCT5_TATB  0x00000C00  // A timeout event for both Timer A
                                            // and Timer B of GPTM 16/32-Bit
                                            // Timer 5 is triggered
#define TIMER_SYNC_SYNCT4_M     0x00000300  // Synchronize GPTM 16/32-Bit Timer
                                            // 4
#define TIMER_SYNC_SYNCT4_NONE  0x00000000  // GPTM 16/32-Bit Timer 4 is not
                                            // affected
#define TIMER_SYNC_SYNCT4_TA    0x00000100  // A timeout event for Timer A of
                                            // GPTM 16/32-Bit Timer 4 is
                                            // triggered
#define TIMER_SYNC_SYNCT4_TB    0x00000200  // A timeout event for Timer B of
                                            // GPTM 16/32-Bit Timer 4 is
                                            // triggered
#define TIMER_SYNC_SYNCT4_TATB  0x00000300  // A timeout event for both Timer A
                                            // and Timer B of GPTM 16/32-Bit
                                            // Timer 4 is triggered
#define TIMER_SYNC_SYNCT3_M     0x000000C0  // Synchronize GPTM 16/32-Bit Timer
                                            // 3
#define TIMER_SYNC_SYNCT3_NONE  0x00000000  // GPTM 16/32-Bit Timer 3 is not
                                            // affected
#define TIMER_SYNC_SYNCT3_TA    0x00000040  // A timeout event for Timer A of
                                            // GPTM 16/32-Bit Timer 3 is
                                            // triggered
#define TIMER_SYNC_SYNCT3_TB    0x00000080  // A timeout event for Timer B of
                                            // GPTM 16/32-Bit Timer 3 is
                                            // triggered
#define TIMER_SYNC_SYNCT3_TATB  0x000000C0  // A timeout event for both Timer A
                                            // and Timer B of GPTM 16/32-Bit
                                            // Timer 3 is triggered
#define TIMER_SYNC_SYNCT2_M     0x00000030  // Synchronize GPTM 16/32-Bit Timer
                                            // 2
#define TIMER_SYNC_SYNCT2_NONE  0x00000000  // GPTM 16/32-Bit Timer 2 is not
                                            // affected
#define TIMER_SYNC_SYNCT2_TA    0x00000010  // A timeout event for Timer A of
                                            // GPTM 16/32-Bit Timer 2 is
                                            // triggered
#define TIMER_SYNC_SYNCT2_TB    0x00000020  // A timeout event for Timer B of
                                            // GPTM 16/32-Bit Timer 2 is
                                            // triggered
#define TIMER_SYNC_SYNCT2_TATB  0x00000030  // A timeout event for both Timer A
                                            // and Timer B of GPTM 16/32-Bit
                                            // Timer 2 is triggered
#define TIMER_SYNC_SYNCT1_M     0x0000000C  // Synchronize GPTM 16/32-Bit Timer
                                            // 1
#define TIMER_SYNC_SYNCT1_NONE  0x00000000  // GPTM 16/32-Bit Timer 1 is not
                                            // affected
#define TIMER_SYNC_SYNCT1_TA    0x00000004  // A timeout event for Timer A of
                                            // GPTM 16/32-Bit Timer 1 is
                                            // triggered
#define TIMER_SYNC_SYNCT1_TB    0x00000008  // A timeout event for Timer B of
                                            // GPTM 16/32-Bit Timer 1 is
                                            // triggered
#define TIMER_SYNC_SYNCT1_TATB  0x0000000C  // A timeout event for both Timer A
                                            // and Timer B of GPTM 16/32-Bit
                                            // Timer 1 is triggered
#define TIMER_SYNC_SYNCT0_M     0x00000003  // Synchronize GPTM 16/32-Bit Timer
                                            // 0
#define TIMER_SYNC_SYNCT0_NONE  0x00000000  // GPTM 16/32-Bit Timer 0 is not
                                            // affected
#define TIMER_SYNC_SYNCT0_TA    0x00000001  // A timeout event for Timer A of
                                            // GPTM 16/32-Bit Timer 0 is
                                            // triggered
#define TIMER_SYNC_SYNCT0_TB    0x00000002  // A timeout event for Timer B of
                                            // GPTM 16/32-Bit Timer 0 is
                                            // triggered
#define TIMER_SYNC_SYNCT0_TATB  0x00000003  // A timeout event for both Timer A
                                            // and Timer B of GPTM 16/32-Bit
                                            // Timer 0 is triggered

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_IMR register.
//
//*****************************************************************************
#define TIMER_IMR_WUEIM         0x00010000  // GPTM Write Update Error
                                            // Interrupt Mask
#define TIMER_IMR_TBMIM         0x00000800  // GPTM Timer B Match Interrupt
                                            // Mask
#define TIMER_IMR_CBEIM         0x00000400  // GPTM Timer B Capture Mode Event
                                            // Interrupt Mask
#define TIMER_IMR_CBMIM         0x00000200  // GPTM Timer B Capture Mode Match
                                            // Interrupt Mask
#define TIMER_IMR_TBTOIM        0x00000100  // GPTM Timer B Time-Out Interrupt
                                            // Mask
#define TIMER_IMR_TAMIM         0x00000010  // GPTM Timer A Match Interrupt
                                            // Mask
#define TIMER_IMR_RTCIM         0x00000008  // GPTM RTC Interrupt Mask
#define TIMER_IMR_CAEIM         0x00000004  // GPTM Timer A Capture Mode Event
                                            // Interrupt Mask
#define TIMER_IMR_CAMIM         0x00000002  // GPTM Timer A Capture Mode Match
                                            // Interrupt Mask
#define TIMER_IMR_TATOIM        0x00000001  // GPTM Timer A Time-Out Interrupt
                                            // Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_RIS register.
//
//*****************************************************************************
#define TIMER_RIS_WUERIS        0x00010000  // GPTM Write Update Error Raw
                                            // Interrupt
#define TIMER_RIS_TBMRIS        0x00000800  // GPTM Timer B Match Raw Interrupt
#define TIMER_RIS_CBERIS        0x00000400  // GPTM Timer B Capture Mode Event
                                            // Raw Interrupt
#define TIMER_RIS_CBMRIS        0x00000200  // GPTM Timer B Capture Mode Match
                                            // Raw Interrupt
#define TIMER_RIS_TBTORIS       0x00000100  // GPTM Timer B Time-Out Raw
                                            // Interrupt
#define TIMER_RIS_TAMRIS        0x00000010  // GPTM Timer A Match Raw Interrupt
#define TIMER_RIS_RTCRIS        0x00000008  // GPTM RTC Raw Interrupt
#define TIMER_RIS_CAERIS        0x00000004  // GPTM Timer A Capture Mode Event
                                            // Raw Interrupt
#define TIMER_RIS_CAMRIS        0x00000002  // GPTM Timer A Capture Mode Match
                                            // Raw Interrupt
#define TIMER_RIS_TATORIS       0x00000001  // GPTM Timer A Time-Out Raw
                                            // Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_MIS register.
//
//*****************************************************************************
#define TIMER_MIS_WUEMIS        0x00010000  // GPTM Write Update Error Masked
                                            // Interrupt
#define TIMER_MIS_TBMMIS        0x00000800  // GPTM Timer B Match Masked
                                            // Interrupt
#define TIMER_MIS_CBEMIS        0x00000400  // GPTM Timer B Capture Mode Event
                                            // Masked Interrupt
#define TIMER_MIS_CBMMIS        0x00000200  // GPTM Timer B Capture Mode Match
                                            // Masked Interrupt
#define TIMER_MIS_TBTOMIS       0x00000100  // GPTM Timer B Time-Out Masked
                                            // Interrupt
#define TIMER_MIS_TAMMIS        0x00000010  // GPTM Timer A Match Masked
                                            // Interrupt
#define TIMER_MIS_RTCMIS        0x00000008  // GPTM RTC Masked Interrupt
#define TIMER_MIS_CAEMIS        0x00000004  // GPTM Timer A Capture Mode Event
                                            // Masked Interrupt
#define TIMER_MIS_CAMMIS        0x00000002  // GPTM Timer A Capture Mode Match
                                            // Masked Interrupt
#define TIMER_MIS_TATOMIS       0x00000001  // GPTM Timer A Time-Out Masked
                                            // Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_ICR register.
//
//*****************************************************************************
#define TIMER_ICR_WUECINT       0x00010000  // 32/64-Bit GPTM Write Update
                                            // Error Interrupt Clear
#define TIMER_ICR_TBMCINT       0x00000800  // GPTM Timer B Match Interrupt
                                            // Clear
#define TIMER_ICR_CBECINT       0x00000400  // GPTM Timer B Capture Mode Event
                                            // Interrupt Clear
#define TIMER_ICR_CBMCINT       0x00000200  // GPTM Timer B Capture Mode Match
                                            // Interrupt Clear
#define TIMER_ICR_TBTOCINT      0x00000100  // GPTM Timer B Time-Out Interrupt
                                            // Clear
#define TIMER_ICR_TAMCINT       0x00000010  // GPTM Timer A Match Interrupt
                                            // Clear
#define TIMER_ICR_RTCCINT       0x00000008  // GPTM RTC Interrupt Clear
#define TIMER_ICR_CAECINT       0x00000004  // GPTM Timer A Capture Mode Event
                                            // Interrupt Clear
#define TIMER_ICR_CAMCINT       0x00000002  // GPTM Timer A Capture Mode Match
                                            // Interrupt Clear
#define TIMER_ICR_TATOCINT      0x00000001  // GPTM Timer A Time-Out Raw
                                            // Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAILR register.
//
//*****************************************************************************
#define TIMER_TAILR_M           0xFFFFFFFF  // GPTM Timer A Interval Load
                                            // Register
#define TIMER_TAILR_S           0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBILR register.
//
//*****************************************************************************
#define TIMER_TBILR_M           0xFFFFFFFF  // GPTM Timer B Interval Load
                                            // Register
#define TIMER_TBILR_S           0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAMATCHR
// register.
//
//*****************************************************************************
#define TIMER_TAMATCHR_TAMR_M   0xFFFFFFFF  // GPTM Timer A Match Register
#define TIMER_TAMATCHR_TAMR_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBMATCHR
// register.
//
//*****************************************************************************
#define TIMER_TBMATCHR_TBMR_M   0xFFFFFFFF  // GPTM Timer B Match Register
#define TIMER_TBMATCHR_TBMR_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPR register.
//
//*****************************************************************************
#define TIMER_TAPR_TAPSRH_M     0x0000FF00  // GPTM Timer A Prescale High Byte
#define TIMER_TAPR_TAPSR_M      0x000000FF  // GPTM Timer A Prescale
#define TIMER_TAPR_TAPSRH_S     8
#define TIMER_TAPR_TAPSR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPR register.
//
//*****************************************************************************
#define TIMER_TBPR_TBPSRH_M     0x0000FF00  // GPTM Timer B Prescale High Byte
#define TIMER_TBPR_TBPSR_M      0x000000FF  // GPTM Timer B Prescale
#define TIMER_TBPR_TBPSRH_S     8
#define TIMER_TBPR_TBPSR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPMR register.
//
//*****************************************************************************
#define TIMER_TAPMR_TAPSMRH_M   0x0000FF00  // GPTM Timer A Prescale Match High
                                            // Byte
#define TIMER_TAPMR_TAPSMR_M    0x000000FF  // GPTM TimerA Prescale Match
#define TIMER_TAPMR_TAPSMRH_S   8
#define TIMER_TAPMR_TAPSMR_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPMR register.
//
//*****************************************************************************
#define TIMER_TBPMR_TBPSMRH_M   0x0000FF00  // GPTM Timer B Prescale Match High
                                            // Byte
#define TIMER_TBPMR_TBPSMR_M    0x000000FF  // GPTM TimerB Prescale Match
#define TIMER_TBPMR_TBPSMRH_S   8
#define TIMER_TBPMR_TBPSMR_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAR register.
//
//*****************************************************************************
#define TIMER_TAR_M             0xFFFFFFFF  // GPTM Timer A Register
#define TIMER_TAR_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBR register.
//
//*****************************************************************************
#define TIMER_TBR_M             0xFFFFFFFF  // GPTM Timer B Register
#define TIMER_TBR_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAV register.
//
//*****************************************************************************
#define TIMER_TAV_M             0xFFFFFFFF  // GPTM Timer A Value
#define TIMER_TAV_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBV register.
//
//*****************************************************************************
#define TIMER_TBV_M             0xFFFFFFFF  // GPTM Timer B Value
#define TIMER_TBV_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_RTCPD register.
//
//*****************************************************************************
#define TIMER_RTCPD_RTCPD_M     0x0000FFFF  // RTC Predivide Counter Value
#define TIMER_RTCPD_RTCPD_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPS register.
//
//*****************************************************************************
#define TIMER_TAPS_PSS_M        0x0000FFFF  // GPTM Timer A Prescaler Snapshot
#define TIMER_TAPS_PSS_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPS register.
//
//*****************************************************************************
#define TIMER_TBPS_PSS_M        0x0000FFFF  // GPTM Timer A Prescaler Value
#define TIMER_TBPS_PSS_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TAPV register.
//
//*****************************************************************************
#define TIMER_TAPV_PSV_M        0x0000FFFF  // GPTM Timer A Prescaler Value
#define TIMER_TAPV_PSV_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_TBPV register.
//
//*****************************************************************************
#define TIMER_TBPV_PSV_M        0x0000FFFF  // GPTM Timer B Prescaler Value
#define TIMER_TBPV_PSV_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the TIMER_O_PP register.
//
//*****************************************************************************
#define TIMER_PP_SIZE_M         0x0000000F  // Count Size
#define TIMER_PP_SIZE_16        0x00000000  // Timer A and Timer B counters are
                                            // 16 bits each with an 8-bit
                                            // prescale counter
#define TIMER_PP_SIZE_32        0x00000001  // Timer A and Timer B counters are
                                            // 32 bits each with a 16-bit
                                            // prescale counter

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_ACTSS register.
//
//*****************************************************************************
#define ADC_ACTSS_ASEN3         0x00000008  // ADC SS3 Enable
#define ADC_ACTSS_ASEN2         0x00000004  // ADC SS2 Enable
#define ADC_ACTSS_ASEN1         0x00000002  // ADC SS1 Enable
#define ADC_ACTSS_ASEN0         0x00000001  // ADC SS0 Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_RIS register.
//
//*****************************************************************************
#define ADC_RIS_INRDC           0x00010000  // Digital Comparator Raw Interrupt
                                            // Status
#define ADC_RIS_INR3            0x00000008  // SS3 Raw Interrupt Status
#define ADC_RIS_INR2            0x00000004  // SS2 Raw Interrupt Status
#define ADC_RIS_INR1            0x00000002  // SS1 Raw Interrupt Status
#define ADC_RIS_INR0            0x00000001  // SS0 Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_IM register.
//
//*****************************************************************************
#define ADC_IM_DCONSS3          0x00080000  // Digital Comparator Interrupt on
                                            // SS3
#define ADC_IM_DCONSS2          0x00040000  // Digital Comparator Interrupt on
                                            // SS2
#define ADC_IM_DCONSS1          0x00020000  // Digital Comparator Interrupt on
                                            // SS1
#define ADC_IM_DCONSS0          0x00010000  // Digital Comparator Interrupt on
                                            // SS0
#define ADC_IM_MASK3            0x00000008  // SS3 Interrupt Mask
#define ADC_IM_MASK2            0x00000004  // SS2 Interrupt Mask
#define ADC_IM_MASK1            0x00000002  // SS1 Interrupt Mask
#define ADC_IM_MASK0            0x00000001  // SS0 Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_ISC register.
//
//*****************************************************************************
#define ADC_ISC_DCINSS3         0x00080000  // Digital Comparator Interrupt
                                            // Status on SS3
#define ADC_ISC_DCINSS2         0x00040000  // Digital Comparator Interrupt
                                            // Status on SS2
#define ADC_ISC_DCINSS1         0x00020000  // Digital Comparator Interrupt
                                            // Status on SS1
#define ADC_ISC_DCINSS0         0x00010000  // Digital Comparator Interrupt
                                            // Status on SS0
#define ADC_ISC_IN3             0x00000008  // SS3 Interrupt Status and Clear
#define ADC_ISC_IN2             0x00000004  // SS2 Interrupt Status and Clear
#define ADC_ISC_IN1             0x00000002  // SS1 Interrupt Status and Clear
#define ADC_ISC_IN0             0x00000001  // SS0 Interrupt Status and Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_OSTAT register.
//
//*****************************************************************************
#define ADC_OSTAT_OV3           0x00000008  // SS3 FIFO Overflow
#define ADC_OSTAT_OV2           0x00000004  // SS2 FIFO Overflow
#define ADC_OSTAT_OV1           0x00000002  // SS1 FIFO Overflow
#define ADC_OSTAT_OV0           0x00000001  // SS0 FIFO Overflow

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_EMUX register.
//
//*****************************************************************************
#define ADC_EMUX_EM3_M          0x0000F000  // SS3 Trigger Select
#define ADC_EMUX_EM3_PROCESSOR  0x00000000  // Processor (default)
#define ADC_EMUX_EM3_COMP0      0x00001000  // Analog Comparator 0
#define ADC_EMUX_EM3_COMP1      0x00002000  // Analog Comparator 1
#define ADC_EMUX_EM3_COMP2      0x00003000  // Analog Comparator 2
#define ADC_EMUX_EM3_EXTERNAL   0x00004000  // External (GPIO PB4)
#define ADC_EMUX_EM3_TIMER      0x00005000  // Timer
#define ADC_EMUX_EM3_ALWAYS     0x0000F000  // Always (continuously sample)
#define ADC_EMUX_EM2_M          0x00000F00  // SS2 Trigger Select
#define ADC_EMUX_EM2_PROCESSOR  0x00000000  // Processor (default)
#define ADC_EMUX_EM2_COMP0      0x00000100  // Analog Comparator 0
#define ADC_EMUX_EM2_COMP1      0x00000200  // Analog Comparator 1
#define ADC_EMUX_EM2_COMP2      0x00000300  // Analog Comparator 2
#define ADC_EMUX_EM2_EXTERNAL   0x00000400  // External (GPIO PB4)
#define ADC_EMUX_EM2_TIMER      0x00000500  // Timer
#define ADC_EMUX_EM2_ALWAYS     0x00000F00  // Always (continuously sample)
#define ADC_EMUX_EM1_M          0x000000F0  // SS1 Trigger Select
#define ADC_EMUX_EM1_PROCESSOR  0x00000000  // Processor (default)
#define ADC_EMUX_EM1_COMP0      0x00000010  // Analog Comparator 0
#define ADC_EMUX_EM1_COMP1      0x00000020  // Analog Comparator 1
#define ADC_EMUX_EM1_COMP2      0x00000030  // Analog Comparator 2
#define ADC_EMUX_EM1_EXTERNAL   0x00000040  // External (GPIO PB4)
#define ADC_EMUX_EM1_TIMER      0x00000050  // Timer
#define ADC_EMUX_EM1_ALWAYS     0x000000F0  // Always (continuously sample)
#define ADC_EMUX_EM0_M          0x0000000F  // SS0 Trigger Select
#define ADC_EMUX_EM0_PROCESSOR  0x00000000  // Processor (default)
#define ADC_EMUX_EM0_COMP0      0x00000001  // Analog Comparator 0
#define ADC_EMUX_EM0_COMP1      0x00000002  // Analog Comparator 1
#define ADC_EMUX_EM0_COMP2      0x00000003  // Analog Comparator 2
#define ADC_EMUX_EM0_EXTERNAL   0x00000004  // External (GPIO PB4)
#define ADC_EMUX_EM0_TIMER      0x00000005  // Timer
#define ADC_EMUX_EM0_ALWAYS     0x0000000F  // Always (continuously sample)

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_USTAT register.
//
//*****************************************************************************
#define ADC_USTAT_UV3           0x00000008  // SS3 FIFO Underflow
#define ADC_USTAT_UV2           0x00000004  // SS2 FIFO Underflow
#define ADC_USTAT_UV1           0x00000002  // SS1 FIFO Underflow
#define ADC_USTAT_UV0           0x00000001  // SS0 FIFO Underflow

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSPRI register.
//
//*****************************************************************************
#define ADC_SSPRI_SS3_M         0x00003000  // SS3 Priority
#define ADC_SSPRI_SS3_1ST       0x00000000  // First priority
#define ADC_SSPRI_SS3_2ND       0x00001000  // Second priority
#define ADC_SSPRI_SS3_3RD       0x00002000  // Third priority
#define ADC_SSPRI_SS3_4TH       0x00003000  // Fourth priority
#define ADC_SSPRI_SS2_M         0x00000300  // SS2 Priority
#define ADC_SSPRI_SS2_1ST       0x00000000  // First priority
#define ADC_SSPRI_SS2_2ND       0x00000100  // Second priority
#define ADC_SSPRI_SS2_3RD       0x00000200  // Third priority
#define ADC_SSPRI_SS2_4TH       0x00000300  // Fourth priority
#define ADC_SSPRI_SS1_M         0x00000030  // SS1 Priority
#define ADC_SSPRI_SS1_1ST       0x00000000  // First priority
#define ADC_SSPRI_SS1_2ND       0x00000010  // Second priority
#define ADC_SSPRI_SS1_3RD       0x00000020  // Third priority
#define ADC_SSPRI_SS1_4TH       0x00000030  // Fourth priority
#define ADC_SSPRI_SS0_M         0x00000003  // SS0 Priority
#define ADC_SSPRI_SS0_1ST       0x00000000  // First priority
#define ADC_SSPRI_SS0_2ND       0x00000001  // Second priority
#define ADC_SSPRI_SS0_3RD       0x00000002  // Third priority
#define ADC_SSPRI_SS0_4TH       0x00000003  // Fourth priority

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SPC register.
//
//*****************************************************************************
#define ADC_SPC_PHASE_M         0x0000000F  // Phase Difference
#define ADC_SPC_PHASE_0         0x00000000  // ADC sample lags by 0.0
#define ADC_SPC_PHASE_22_5      0x00000001  // ADC sample lags by 22.5
#define ADC_SPC_PHASE_45        0x00000002  // ADC sample lags by 45.0
#define ADC_SPC_PHASE_67_5      0x00000003  // ADC sample lags by 67.5
#define ADC_SPC_PHASE_90        0x00000004  // ADC sample lags by 90.0
#define ADC_SPC_PHASE_112_5     0x00000005  // ADC sample lags by 112.5
#define ADC_SPC_PHASE_135       0x00000006  // ADC sample lags by 135.0
#define ADC_SPC_PHASE_157_5     0x00000007  // ADC sample lags by 157.5
#define ADC_SPC_PHASE_180       0x00000008  // ADC sample lags by 180.0
#define ADC_SPC_PHASE_202_5     0x00000009  // ADC sample lags by 202.5
#define ADC_SPC_PHASE_225       0x0000000A  // ADC sample lags by 225.0
#define ADC_SPC_PHASE_247_5     0x0000000B  // ADC sample lags by 247.5
#define ADC_SPC_PHASE_270       0x0000000C  // ADC sample lags by 270.0
#define ADC_SPC_PHASE_292_5     0x0000000D  // ADC sample lags by 292.5
#define ADC_SPC_PHASE_315       0x0000000E  // ADC sample lags by 315.0
#define ADC_SPC_PHASE_337_5     0x0000000F  // ADC sample lags by 337.5

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_PSSI register.
//
//*****************************************************************************
#define ADC_PSSI_GSYNC          0x80000000  // Global Synchronize
#define ADC_PSSI_SYNCWAIT       0x08000000  // Synchronize Wait
#define ADC_PSSI_SS3            0x00000008  // SS3 Initiate
#define ADC_PSSI_SS2            0x00000004  // SS2 Initiate
#define ADC_PSSI_SS1            0x00000002  // SS1 Initiate
#define ADC_PSSI_SS0            0x00000001  // SS0 Initiate

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SAC register.
//
//*****************************************************************************
#define ADC_SAC_AVG_M           0x00000007  // Hardware Averaging Control
#define ADC_SAC_AVG_OFF         0x00000000  // No hardware oversampling
#define ADC_SAC_AVG_2X          0x00000001  // 2x hardware oversampling
#define ADC_SAC_AVG_4X          0x00000002  // 4x hardware oversampling
#define ADC_SAC_AVG_8X          0x00000003  // 8x hardware oversampling
#define ADC_SAC_AVG_16X         0x00000004  // 16x hardware oversampling
#define ADC_SAC_AVG_32X         0x00000005  // 32x hardware oversampling
#define ADC_SAC_AVG_64X         0x00000006  // 64x hardware oversampling

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCISC register.
//
//*****************************************************************************
#define ADC_DCISC_DCINT7        0x00000080  // Digital Comparator 7 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT6        0x00000040  // Digital Comparator 6 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT5        0x00000020  // Digital Comparator 5 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT4        0x00000010  // Digital Comparator 4 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT3        0x00000008  // Digital Comparator 3 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT2        0x00000004  // Digital Comparator 2 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT1        0x00000002  // Digital Comparator 1 Interrupt
                                            // Status and Clear
#define ADC_DCISC_DCINT0        0x00000001  // Digital Comparator 0 Interrupt
                                            // Status and Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_CTL register.
//
//*****************************************************************************
#define ADC_CTL_VREF_M          0x00000003  // Voltage Reference Select
#define ADC_CTL_VREF_INTERNAL   0x00000000  // The internal reference as the
                                            // voltage reference
#define ADC_CTL_VREF_EXT_3V     0x00000001  // A 3.0 V external VREFA input is
                                            // the voltage reference. The ADC
                                            // conversion range is 0.0 V to the
                                            // external reference value

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSMUX0 register.
//
//*****************************************************************************
#define ADC_SSMUX0_MUX7_M       0xF0000000  // 8th Sample Input Select
#define ADC_SSMUX0_MUX6_M       0x0F000000  // 7th Sample Input Select
#define ADC_SSMUX0_MUX5_M       0x00F00000  // 6th Sample Input Select
#define ADC_SSMUX0_MUX4_M       0x000F0000  // 5th Sample Input Select
#define ADC_SSMUX0_MUX3_M       0x0000F000  // 4th Sample Input Select
#define ADC_SSMUX0_MUX2_M       0x00000F00  // 3rd Sample Input Select
#define ADC_SSMUX0_MUX1_M       0x000000F0  // 2nd Sample Input Select
#define ADC_SSMUX0_MUX0_M       0x0000000F  // 1st Sample Input Select
#define ADC_SSMUX0_MUX7_S       28
#define ADC_SSMUX0_MUX6_S       24
#define ADC_SSMUX0_MUX5_S       20
#define ADC_SSMUX0_MUX4_S       16
#define ADC_SSMUX0_MUX3_S       12
#define ADC_SSMUX0_MUX2_S       8
#define ADC_SSMUX0_MUX1_S       4
#define ADC_SSMUX0_MUX0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSCTL0 register.
//
//*****************************************************************************
#define ADC_SSCTL0_TS7          0x80000000  // 8th Sample Temp Sensor Select
#define ADC_SSCTL0_IE7          0x40000000  // 8th Sample Interrupt Enable
#define ADC_SSCTL0_END7         0x20000000  // 8th Sample is End of Sequence
#define ADC_SSCTL0_D7           0x10000000  // 8th Sample Diff Input Select
#define ADC_SSCTL0_TS6          0x08000000  // 7th Sample Temp Sensor Select
#define ADC_SSCTL0_IE6          0x04000000  // 7th Sample Interrupt Enable
#define ADC_SSCTL0_END6         0x02000000  // 7th Sample is End of Sequence
#define ADC_SSCTL0_D6           0x01000000  // 7th Sample Diff Input Select
#define ADC_SSCTL0_TS5          0x00800000  // 6th Sample Temp Sensor Select
#define ADC_SSCTL0_IE5          0x00400000  // 6th Sample Interrupt Enable
#define ADC_SSCTL0_END5         0x00200000  // 6th Sample is End of Sequence
#define ADC_SSCTL0_D5           0x00100000  // 6th Sample Diff Input Select
#define ADC_SSCTL0_TS4          0x00080000  // 5th Sample Temp Sensor Select
#define ADC_SSCTL0_IE4          0x00040000  // 5th Sample Interrupt Enable
#define ADC_SSCTL0_END4         0x00020000  // 5th Sample is End of Sequence
#define ADC_SSCTL0_D4           0x00010000  // 5th Sample Diff Input Select
#define ADC_SSCTL0_TS3          0x00008000  // 4th Sample Temp Sensor Select
#define ADC_SSCTL0_IE3          0x00004000  // 4th Sample Interrupt Enable
#define ADC_SSCTL0_END3         0x00002000  // 4th Sample is End of Sequence
#define ADC_SSCTL0_D3           0x00001000  // 4th Sample Diff Input Select
#define ADC_SSCTL0_TS2          0x00000800  // 3rd Sample Temp Sensor Select
#define ADC_SSCTL0_IE2          0x00000400  // 3rd Sample Interrupt Enable
#define ADC_SSCTL0_END2         0x00000200  // 3rd Sample is End of Sequence
#define ADC_SSCTL0_D2           0x00000100  // 3rd Sample Diff Input Select
#define ADC_SSCTL0_TS1          0x00000080  // 2nd Sample Temp Sensor Select
#define ADC_SSCTL0_IE1          0x00000040  // 2nd Sample Interrupt Enable
#define ADC_SSCTL0_END1         0x00000020  // 2nd Sample is End of Sequence
#define ADC_SSCTL0_D1           0x00000010  // 2nd Sample Diff Input Select
#define ADC_SSCTL0_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL0_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL0_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL0_D0           0x00000001  // 1st Sample Diff Input Select

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFIFO0 register.
//
//*****************************************************************************
#define ADC_SSFIFO0_DATA_M      0x00000FFF  // Conversion Result Data
#define ADC_SSFIFO0_DATA_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFSTAT0 register.
//
//*****************************************************************************
#define ADC_SSFSTAT0_FULL       0x00001000  // FIFO Full
#define ADC_SSFSTAT0_EMPTY      0x00000100  // FIFO Empty
#define ADC_SSFSTAT0_HPTR_M     0x000000F0  // FIFO Head Pointer
#define ADC_SSFSTAT0_TPTR_M     0x0000000F  // FIFO Tail Pointer
#define ADC_SSFSTAT0_HPTR_S     4
#define ADC_SSFSTAT0_TPTR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSOP0 register.
//
//*****************************************************************************
#define ADC_SSOP0_S7DCOP        0x10000000  // Sample 7 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S6DCOP        0x01000000  // Sample 6 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S5DCOP        0x00100000  // Sample 5 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S4DCOP        0x00010000  // Sample 4 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S3DCOP        0x00001000  // Sample 3 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S2DCOP        0x00000100  // Sample 2 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S1DCOP        0x00000010  // Sample 1 Digital Comparator
                                            // Operation
#define ADC_SSOP0_S0DCOP        0x00000001  // Sample 0 Digital Comparator
                                            // Operation

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSDC0 register.
//
//*****************************************************************************
#define ADC_SSDC0_S7DCSEL_M     0xF0000000  // Sample 7 Digital Comparator
                                            // Select
#define ADC_SSDC0_S6DCSEL_M     0x0F000000  // Sample 6 Digital Comparator
                                            // Select
#define ADC_SSDC0_S5DCSEL_M     0x00F00000  // Sample 5 Digital Comparator
                                            // Select
#define ADC_SSDC0_S4DCSEL_M     0x000F0000  // Sample 4 Digital Comparator
                                            // Select
#define ADC_SSDC0_S3DCSEL_M     0x0000F000  // Sample 3 Digital Comparator
                                            // Select
#define ADC_SSDC0_S2DCSEL_M     0x00000F00  // Sample 2 Digital Comparator
                                            // Select
#define ADC_SSDC0_S1DCSEL_M     0x000000F0  // Sample 1 Digital Comparator
                                            // Select
#define ADC_SSDC0_S0DCSEL_M     0x0000000F  // Sample 0 Digital Comparator
                                            // Select
#define ADC_SSDC0_S6DCSEL_S     24
#define ADC_SSDC0_S5DCSEL_S     20
#define ADC_SSDC0_S4DCSEL_S     16
#define ADC_SSDC0_S3DCSEL_S     12
#define ADC_SSDC0_S2DCSEL_S     8
#define ADC_SSDC0_S1DCSEL_S     4
#define ADC_SSDC0_S0DCSEL_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSEMUX0 register.
//
//*****************************************************************************
#define ADC_SSEMUX0_EMUX7       0x10000000  // 8th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX6       0x01000000  // 7th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX5       0x00100000  // 6th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX4       0x00010000  // 5th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX3       0x00001000  // 4th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX2       0x00000100  // 3rd Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX1       0x00000010  // 2th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX0_EMUX0       0x00000001  // 1st Sample Input Select (Upper
                                            // Bit)

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSMUX1 register.
//
//*****************************************************************************
#define ADC_SSMUX1_MUX3_M       0x0000F000  // 4th Sample Input Select
#define ADC_SSMUX1_MUX2_M       0x00000F00  // 3rd Sample Input Select
#define ADC_SSMUX1_MUX1_M       0x000000F0  // 2nd Sample Input Select
#define ADC_SSMUX1_MUX0_M       0x0000000F  // 1st Sample Input Select
#define ADC_SSMUX1_MUX3_S       12
#define ADC_SSMUX1_MUX2_S       8
#define ADC_SSMUX1_MUX1_S       4
#define ADC_SSMUX1_MUX0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSCTL1 register.
//
//*****************************************************************************
#define ADC_SSCTL1_TS3          0x00008000  // 4th Sample Temp Sensor Select
#define ADC_SSCTL1_IE3          0x00004000  // 4th Sample Interrupt Enable
#define ADC_SSCTL1_END3         0x00002000  // 4th Sample is End of Sequence
#define ADC_SSCTL1_D3           0x00001000  // 4th Sample Diff Input Select
#define ADC_SSCTL1_TS2          0x00000800  // 3rd Sample Temp Sensor Select
#define ADC_SSCTL1_IE2          0x00000400  // 3rd Sample Interrupt Enable
#define ADC_SSCTL1_END2         0x00000200  // 3rd Sample is End of Sequence
#define ADC_SSCTL1_D2           0x00000100  // 3rd Sample Diff Input Select
#define ADC_SSCTL1_TS1          0x00000080  // 2nd Sample Temp Sensor Select
#define ADC_SSCTL1_IE1          0x00000040  // 2nd Sample Interrupt Enable
#define ADC_SSCTL1_END1         0x00000020  // 2nd Sample is End of Sequence
#define ADC_SSCTL1_D1           0x00000010  // 2nd Sample Diff Input Select
#define ADC_SSCTL1_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL1_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL1_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL1_D0           0x00000001  // 1st Sample Diff Input Select

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFIFO1 register.
//
//*****************************************************************************
#define ADC_SSFIFO1_DATA_M      0x00000FFF  // Conversion Result Data
#define ADC_SSFIFO1_DATA_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFSTAT1 register.
//
//*****************************************************************************
#define ADC_SSFSTAT1_FULL       0x00001000  // FIFO Full
#define ADC_SSFSTAT1_EMPTY      0x00000100  // FIFO Empty
#define ADC_SSFSTAT1_HPTR_M     0x000000F0  // FIFO Head Pointer
#define ADC_SSFSTAT1_TPTR_M     0x0000000F  // FIFO Tail Pointer
#define ADC_SSFSTAT1_HPTR_S     4
#define ADC_SSFSTAT1_TPTR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSOP1 register.
//
//*****************************************************************************
#define ADC_SSOP1_S3DCOP        0x00001000  // Sample 3 Digital Comparator
                                            // Operation
#define ADC_SSOP1_S2DCOP        0x00000100  // Sample 2 Digital Comparator
                                            // Operation
#define ADC_SSOP1_S1DCOP        0x00000010  // Sample 1 Digital Comparator
                                            // Operation
#define ADC_SSOP1_S0DCOP        0x00000001  // Sample 0 Digital Comparator
                                            // Operation

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSDC1 register.
//
//*****************************************************************************
#define ADC_SSDC1_S3DCSEL_M     0x0000F000  // Sample 3 Digital Comparator
                                            // Select
#define ADC_SSDC1_S2DCSEL_M     0x00000F00  // Sample 2 Digital Comparator
                                            // Select
#define ADC_SSDC1_S1DCSEL_M     0x000000F0  // Sample 1 Digital Comparator
                                            // Select
#define ADC_SSDC1_S0DCSEL_M     0x0000000F  // Sample 0 Digital Comparator
                                            // Select
#define ADC_SSDC1_S2DCSEL_S     8
#define ADC_SSDC1_S1DCSEL_S     4
#define ADC_SSDC1_S0DCSEL_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSEMUX1 register.
//
//*****************************************************************************
#define ADC_SSEMUX1_EMUX3       0x00001000  // 4th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX1_EMUX2       0x00000100  // 3rd Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX1_EMUX1       0x00000010  // 2th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX1_EMUX0       0x00000001  // 1st Sample Input Select (Upper
                                            // Bit)

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSMUX2 register.
//
//*****************************************************************************
#define ADC_SSMUX2_MUX3_M       0x0000F000  // 4th Sample Input Select
#define ADC_SSMUX2_MUX2_M       0x00000F00  // 3rd Sample Input Select
#define ADC_SSMUX2_MUX1_M       0x000000F0  // 2nd Sample Input Select
#define ADC_SSMUX2_MUX0_M       0x0000000F  // 1st Sample Input Select
#define ADC_SSMUX2_MUX3_S       12
#define ADC_SSMUX2_MUX2_S       8
#define ADC_SSMUX2_MUX1_S       4
#define ADC_SSMUX2_MUX0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSCTL2 register.
//
//*****************************************************************************
#define ADC_SSCTL2_TS3          0x00008000  // 4th Sample Temp Sensor Select
#define ADC_SSCTL2_IE3          0x00004000  // 4th Sample Interrupt Enable
#define ADC_SSCTL2_END3         0x00002000  // 4th Sample is End of Sequence
#define ADC_SSCTL2_D3           0x00001000  // 4th Sample Diff Input Select
#define ADC_SSCTL2_TS2          0x00000800  // 3rd Sample Temp Sensor Select
#define ADC_SSCTL2_IE2          0x00000400  // 3rd Sample Interrupt Enable
#define ADC_SSCTL2_END2         0x00000200  // 3rd Sample is End of Sequence
#define ADC_SSCTL2_D2           0x00000100  // 3rd Sample Diff Input Select
#define ADC_SSCTL2_TS1          0x00000080  // 2nd Sample Temp Sensor Select
#define ADC_SSCTL2_IE1          0x00000040  // 2nd Sample Interrupt Enable
#define ADC_SSCTL2_END1         0x00000020  // 2nd Sample is End of Sequence
#define ADC_SSCTL2_D1           0x00000010  // 2nd Sample Diff Input Select
#define ADC_SSCTL2_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL2_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL2_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL2_D0           0x00000001  // 1st Sample Diff Input Select

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFIFO2 register.
//
//*****************************************************************************
#define ADC_SSFIFO2_DATA_M      0x00000FFF  // Conversion Result Data
#define ADC_SSFIFO2_DATA_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFSTAT2 register.
//
//*****************************************************************************
#define ADC_SSFSTAT2_FULL       0x00001000  // FIFO Full
#define ADC_SSFSTAT2_EMPTY      0x00000100  // FIFO Empty
#define ADC_SSFSTAT2_HPTR_M     0x000000F0  // FIFO Head Pointer
#define ADC_SSFSTAT2_TPTR_M     0x0000000F  // FIFO Tail Pointer
#define ADC_SSFSTAT2_HPTR_S     4
#define ADC_SSFSTAT2_TPTR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSOP2 register.
//
//*****************************************************************************
#define ADC_SSOP2_S3DCOP        0x00001000  // Sample 3 Digital Comparator
                                            // Operation
#define ADC_SSOP2_S2DCOP        0x00000100  // Sample 2 Digital Comparator
                                            // Operation
#define ADC_SSOP2_S1DCOP        0x00000010  // Sample 1 Digital Comparator
                                            // Operation
#define ADC_SSOP2_S0DCOP        0x00000001  // Sample 0 Digital Comparator
                                            // Operation

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSDC2 register.
//
//*****************************************************************************
#define ADC_SSDC2_S3DCSEL_M     0x0000F000  // Sample 3 Digital Comparator
                                            // Select
#define ADC_SSDC2_S2DCSEL_M     0x00000F00  // Sample 2 Digital Comparator
                                            // Select
#define ADC_SSDC2_S1DCSEL_M     0x000000F0  // Sample 1 Digital Comparator
                                            // Select
#define ADC_SSDC2_S0DCSEL_M     0x0000000F  // Sample 0 Digital Comparator
                                            // Select
#define ADC_SSDC2_S2DCSEL_S     8
#define ADC_SSDC2_S1DCSEL_S     4
#define ADC_SSDC2_S0DCSEL_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSEMUX2 register.
//
//*****************************************************************************
#define ADC_SSEMUX2_EMUX3       0x00001000  // 4th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX2_EMUX2       0x00000100  // 3rd Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX2_EMUX1       0x00000010  // 2th Sample Input Select (Upper
                                            // Bit)
#define ADC_SSEMUX2_EMUX0       0x00000001  // 1st Sample Input Select (Upper
                                            // Bit)

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSMUX3 register.
//
//*****************************************************************************
#define ADC_SSMUX3_MUX0_M       0x0000000F  // 1st Sample Input Select
#define ADC_SSMUX3_MUX0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSCTL3 register.
//
//*****************************************************************************
#define ADC_SSCTL3_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL3_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL3_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL3_D0           0x00000001  // 1st Sample Diff Input Select

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFIFO3 register.
//
//*****************************************************************************
#define ADC_SSFIFO3_DATA_M      0x00000FFF  // Conversion Result Data
#define ADC_SSFIFO3_DATA_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSFSTAT3 register.
//
//*****************************************************************************
#define ADC_SSFSTAT3_FULL       0x00001000  // FIFO Full
#define ADC_SSFSTAT3_EMPTY      0x00000100  // FIFO Empty
#define ADC_SSFSTAT3_HPTR_M     0x000000F0  // FIFO Head Pointer
#define ADC_SSFSTAT3_TPTR_M     0x0000000F  // FIFO Tail Pointer
#define ADC_SSFSTAT3_HPTR_S     4
#define ADC_SSFSTAT3_TPTR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSOP3 register.
//
//*****************************************************************************
#define ADC_SSOP3_S0DCOP        0x00000001  // Sample 0 Digital Comparator
                                            // Operation

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSDC3 register.
//
//*****************************************************************************
#define ADC_SSDC3_S0DCSEL_M     0x0000000F  // Sample 0 Digital Comparator
                                            // Select

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_SSEMUX3 register.
//
//*****************************************************************************
#define ADC_SSEMUX3_EMUX0       0x00000001  // 1st Sample Input Select (Upper
                                            // Bit)

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCRIC register.
//
//*****************************************************************************
#define ADC_DCRIC_DCTRIG7       0x00800000  // Digital Comparator Trigger 7
#define ADC_DCRIC_DCTRIG6       0x00400000  // Digital Comparator Trigger 6
#define ADC_DCRIC_DCTRIG5       0x00200000  // Digital Comparator Trigger 5
#define ADC_DCRIC_DCTRIG4       0x00100000  // Digital Comparator Trigger 4
#define ADC_DCRIC_DCTRIG3       0x00080000  // Digital Comparator Trigger 3
#define ADC_DCRIC_DCTRIG2       0x00040000  // Digital Comparator Trigger 2
#define ADC_DCRIC_DCTRIG1       0x00020000  // Digital Comparator Trigger 1
#define ADC_DCRIC_DCTRIG0       0x00010000  // Digital Comparator Trigger 0
#define ADC_DCRIC_DCINT7        0x00000080  // Digital Comparator Interrupt 7
#define ADC_DCRIC_DCINT6        0x00000040  // Digital Comparator Interrupt 6
#define ADC_DCRIC_DCINT5        0x00000020  // Digital Comparator Interrupt 5
#define ADC_DCRIC_DCINT4        0x00000010  // Digital Comparator Interrupt 4
#define ADC_DCRIC_DCINT3        0x00000008  // Digital Comparator Interrupt 3
#define ADC_DCRIC_DCINT2        0x00000004  // Digital Comparator Interrupt 2
#define ADC_DCRIC_DCINT1        0x00000002  // Digital Comparator Interrupt 1
#define ADC_DCRIC_DCINT0        0x00000001  // Digital Comparator Interrupt 0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL0 register.
//
//*****************************************************************************
#define ADC_DCCTL0_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL0_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL0_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL0_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL0_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL0_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL0_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL0_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL0_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL0_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL1 register.
//
//*****************************************************************************
#define ADC_DCCTL1_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL1_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL1_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL1_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL1_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL1_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL1_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL1_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL1_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL1_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL2 register.
//
//*****************************************************************************
#define ADC_DCCTL2_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL2_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL2_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL2_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL2_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL2_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL2_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL2_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL2_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL2_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL3 register.
//
//*****************************************************************************
#define ADC_DCCTL3_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL3_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL3_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL3_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL3_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL3_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL3_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL3_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL3_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL3_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL4 register.
//
//*****************************************************************************
#define ADC_DCCTL4_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL4_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL4_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL4_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL4_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL4_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL4_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL4_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL4_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL4_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL5 register.
//
//*****************************************************************************
#define ADC_DCCTL5_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL5_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL5_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL5_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL5_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL5_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL5_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL5_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL5_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL5_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL6 register.
//
//*****************************************************************************
#define ADC_DCCTL6_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL6_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL6_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL6_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL6_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL6_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL6_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL6_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL6_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL6_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCTL7 register.
//
//*****************************************************************************
#define ADC_DCCTL7_CIE          0x00000010  // Comparison Interrupt Enable
#define ADC_DCCTL7_CIC_M        0x0000000C  // Comparison Interrupt Condition
#define ADC_DCCTL7_CIC_LOW      0x00000000  // Low Band
#define ADC_DCCTL7_CIC_MID      0x00000004  // Mid Band
#define ADC_DCCTL7_CIC_HIGH     0x0000000C  // High Band
#define ADC_DCCTL7_CIM_M        0x00000003  // Comparison Interrupt Mode
#define ADC_DCCTL7_CIM_ALWAYS   0x00000000  // Always
#define ADC_DCCTL7_CIM_ONCE     0x00000001  // Once
#define ADC_DCCTL7_CIM_HALWAYS  0x00000002  // Hysteresis Always
#define ADC_DCCTL7_CIM_HONCE    0x00000003  // Hysteresis Once

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP0 register.
//
//*****************************************************************************
#define ADC_DCCMP0_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP0_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP0_COMP1_S      16
#define ADC_DCCMP0_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP1 register.
//
//*****************************************************************************
#define ADC_DCCMP1_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP1_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP1_COMP1_S      16
#define ADC_DCCMP1_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP2 register.
//
//*****************************************************************************
#define ADC_DCCMP2_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP2_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP2_COMP1_S      16
#define ADC_DCCMP2_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP3 register.
//
//*****************************************************************************
#define ADC_DCCMP3_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP3_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP3_COMP1_S      16
#define ADC_DCCMP3_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP4 register.
//
//*****************************************************************************
#define ADC_DCCMP4_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP4_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP4_COMP1_S      16
#define ADC_DCCMP4_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP5 register.
//
//*****************************************************************************
#define ADC_DCCMP5_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP5_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP5_COMP1_S      16
#define ADC_DCCMP5_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP6 register.
//
//*****************************************************************************
#define ADC_DCCMP6_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP6_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP6_COMP1_S      16
#define ADC_DCCMP6_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_DCCMP7 register.
//
//*****************************************************************************
#define ADC_DCCMP7_COMP1_M      0x0FFF0000  // Compare 1
#define ADC_DCCMP7_COMP0_M      0x00000FFF  // Compare 0
#define ADC_DCCMP7_COMP1_S      16
#define ADC_DCCMP7_COMP0_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_PP register.
//
//*****************************************************************************
#define ADC_PP_TS               0x00800000  // Temperature Sensor
#define ADC_PP_RSL_M            0x007C0000  // Resolution
#define ADC_PP_TYPE_M           0x00030000  // ADC Architecture
#define ADC_PP_TYPE_SAR         0x00000000  // SAR
#define ADC_PP_DC_M             0x0000FC00  // Digital Comparator Count
#define ADC_PP_CH_M             0x000003F0  // ADC Channel Count
#define ADC_PP_MSR_M            0x0000000F  // Maximum ADC Sample Rate
#define ADC_PP_MSR_125K         0x00000001  // 125 ksps
#define ADC_PP_MSR_250K         0x00000003  // 250 ksps
#define ADC_PP_MSR_500K         0x00000005  // 500 ksps
#define ADC_PP_MSR_1M           0x00000007  // 1 Msps
#define ADC_PP_RSL_S            18
#define ADC_PP_DC_S             10
#define ADC_PP_CH_S             4

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_PC register.
//
//*****************************************************************************
#define ADC_PC_SR_M             0x0000000F  // ADC Sample Rate
#define ADC_PC_SR_125K          0x00000001  // 125 ksps
#define ADC_PC_SR_250K          0x00000003  // 250 ksps
#define ADC_PC_SR_500K          0x00000005  // 500 ksps
#define ADC_PC_SR_1M            0x00000007  // 1 Msps

//*****************************************************************************
//
// The following are defines for the bit fields in the ADC_O_CC register.
//
//*****************************************************************************
#define ADC_CC_CS_M             0x0000000F  // ADC Clock Source
#define ADC_CC_CS_SYSPLL        0x00000000  // Either the system clock (if the
                                            // PLL bypass is in effect) or the
                                            // 16 MHz clock derived from PLL /
                                            // 25 (default)
#define ADC_CC_CS_PIOSC         0x00000001  // PIOSC

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACMIS register.
//
//*****************************************************************************
#define COMP_ACMIS_IN2          0x00000004  // Comparator 2 Masked Interrupt
                                            // Status
#define COMP_ACMIS_IN1          0x00000002  // Comparator 1 Masked Interrupt
                                            // Status
#define COMP_ACMIS_IN0          0x00000001  // Comparator 0 Masked Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACRIS register.
//
//*****************************************************************************
#define COMP_ACRIS_IN2          0x00000004  // Comparator 2 Interrupt Status
#define COMP_ACRIS_IN1          0x00000002  // Comparator 1 Interrupt Status
#define COMP_ACRIS_IN0          0x00000001  // Comparator 0 Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACINTEN register.
//
//*****************************************************************************
#define COMP_ACINTEN_IN2        0x00000004  // Comparator 2 Interrupt Enable
#define COMP_ACINTEN_IN1        0x00000002  // Comparator 1 Interrupt Enable
#define COMP_ACINTEN_IN0        0x00000001  // Comparator 0 Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACREFCTL
// register.
//
//*****************************************************************************
#define COMP_ACREFCTL_EN        0x00000200  // Resistor Ladder Enable
#define COMP_ACREFCTL_RNG       0x00000100  // Resistor Ladder Range
#define COMP_ACREFCTL_VREF_M    0x0000000F  // Resistor Ladder Voltage Ref
#define COMP_ACREFCTL_VREF_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACSTAT0 register.
//
//*****************************************************************************
#define COMP_ACSTAT0_OVAL       0x00000002  // Comparator Output Value

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACCTL0 register.
//
//*****************************************************************************
#define COMP_ACCTL0_TOEN        0x00000800  // Trigger Output Enable
#define COMP_ACCTL0_ASRCP_M     0x00000600  // Analog Source Positive
#define COMP_ACCTL0_ASRCP_PIN   0x00000000  // Pin value of Cn+
#define COMP_ACCTL0_ASRCP_PIN0  0x00000200  // Pin value of C0+
#define COMP_ACCTL0_ASRCP_REF   0x00000400  // Internal voltage reference
#define COMP_ACCTL0_TSLVAL      0x00000080  // Trigger Sense Level Value
#define COMP_ACCTL0_TSEN_M      0x00000060  // Trigger Sense
#define COMP_ACCTL0_TSEN_LEVEL  0x00000000  // Level sense, see TSLVAL
#define COMP_ACCTL0_TSEN_FALL   0x00000020  // Falling edge
#define COMP_ACCTL0_TSEN_RISE   0x00000040  // Rising edge
#define COMP_ACCTL0_TSEN_BOTH   0x00000060  // Either edge
#define COMP_ACCTL0_ISLVAL      0x00000010  // Interrupt Sense Level Value
#define COMP_ACCTL0_ISEN_M      0x0000000C  // Interrupt Sense
#define COMP_ACCTL0_ISEN_LEVEL  0x00000000  // Level sense, see ISLVAL
#define COMP_ACCTL0_ISEN_FALL   0x00000004  // Falling edge
#define COMP_ACCTL0_ISEN_RISE   0x00000008  // Rising edge
#define COMP_ACCTL0_ISEN_BOTH   0x0000000C  // Either edge
#define COMP_ACCTL0_CINV        0x00000002  // Comparator Output Invert

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACSTAT1 register.
//
//*****************************************************************************
#define COMP_ACSTAT1_OVAL       0x00000002  // Comparator Output Value

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACCTL1 register.
//
//*****************************************************************************
#define COMP_ACCTL1_TOEN        0x00000800  // Trigger Output Enable
#define COMP_ACCTL1_ASRCP_M     0x00000600  // Analog Source Positive
#define COMP_ACCTL1_ASRCP_PIN   0x00000000  // Pin value of Cn+
#define COMP_ACCTL1_ASRCP_PIN0  0x00000200  // Pin value of C0+
#define COMP_ACCTL1_ASRCP_REF   0x00000400  // Internal voltage reference
                                            // (VIREF)
#define COMP_ACCTL1_TSLVAL      0x00000080  // Trigger Sense Level Value
#define COMP_ACCTL1_TSEN_M      0x00000060  // Trigger Sense
#define COMP_ACCTL1_TSEN_LEVEL  0x00000000  // Level sense, see TSLVAL
#define COMP_ACCTL1_TSEN_FALL   0x00000020  // Falling edge
#define COMP_ACCTL1_TSEN_RISE   0x00000040  // Rising edge
#define COMP_ACCTL1_TSEN_BOTH   0x00000060  // Either edge
#define COMP_ACCTL1_ISLVAL      0x00000010  // Interrupt Sense Level Value
#define COMP_ACCTL1_ISEN_M      0x0000000C  // Interrupt Sense
#define COMP_ACCTL1_ISEN_LEVEL  0x00000000  // Level sense, see ISLVAL
#define COMP_ACCTL1_ISEN_FALL   0x00000004  // Falling edge
#define COMP_ACCTL1_ISEN_RISE   0x00000008  // Rising edge
#define COMP_ACCTL1_ISEN_BOTH   0x0000000C  // Either edge
#define COMP_ACCTL1_CINV        0x00000002  // Comparator Output Invert

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACSTAT2 register.
//
//*****************************************************************************
#define COMP_ACSTAT2_OVAL       0x00000002  // Comparator Output Value

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_ACCTL2 register.
//
//*****************************************************************************
#define COMP_ACCTL2_TOEN        0x00000800  // Trigger Output Enable
#define COMP_ACCTL2_ASRCP_M     0x00000600  // Analog Source Positive
#define COMP_ACCTL2_ASRCP_PIN   0x00000000  // Pin value of Cn+
#define COMP_ACCTL2_ASRCP_PIN0  0x00000200  // Pin value of C0+
#define COMP_ACCTL2_ASRCP_REF   0x00000400  // Internal voltage reference
                                            // (VIREF)
#define COMP_ACCTL2_TSLVAL      0x00000080  // Trigger Sense Level Value
#define COMP_ACCTL2_TSEN_M      0x00000060  // Trigger Sense
#define COMP_ACCTL2_TSEN_LEVEL  0x00000000  // Level sense, see TSLVAL
#define COMP_ACCTL2_TSEN_FALL   0x00000020  // Falling edge
#define COMP_ACCTL2_TSEN_RISE   0x00000040  // Rising edge
#define COMP_ACCTL2_TSEN_BOTH   0x00000060  // Either edge
#define COMP_ACCTL2_ISLVAL      0x00000010  // Interrupt Sense Level Value
#define COMP_ACCTL2_ISEN_M      0x0000000C  // Interrupt Sense
#define COMP_ACCTL2_ISEN_LEVEL  0x00000000  // Level sense, see ISLVAL
#define COMP_ACCTL2_ISEN_FALL   0x00000004  // Falling edge
#define COMP_ACCTL2_ISEN_RISE   0x00000008  // Rising edge
#define COMP_ACCTL2_ISEN_BOTH   0x0000000C  // Either edge
#define COMP_ACCTL2_CINV        0x00000002  // Comparator Output Invert

//*****************************************************************************
//
// The following are defines for the bit fields in the COMP_O_PP register.
//
//*****************************************************************************
#define COMP_PP_CMP2            0x00000004  // Comparator 2 Present
#define COMP_PP_CMP1            0x00000002  // Comparator 1 Present
#define COMP_PP_CMP0            0x00000001  // Comparator 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CTL register.
//
//*****************************************************************************
#define LPC_CTL_SCICNT_M        0x00000C00  // LPC0SCI Pulse Length
#define LPC_CTL_SCICNT_0        0x00000000  // No pulse occurs on the LPC0SCI
                                            // pin
#define LPC_CTL_SCICNT_2        0x00000400  // The pulse on the LPC0SCI pin is
                                            // 2 LPC0CLK periods
#define LPC_CTL_SCICNT_4        0x00000800  // The pulse on the LPC0SCI pin is
                                            // 4 LPC0CLK periods
#define LPC_CTL_SCICNT_8        0x00000C00  // The pulse on the LPC0SCI pin is
                                            // 8 LPC0CLK periods
#define LPC_CTL_SCI             0x00000200  // Start SCI Pulse
#define LPC_CTL_WAKE            0x00000100  // Wake or Keep Awake the LPC Bus
#define LPC_CTL_CE7             0x00000080  // Enable Channel 7
#define LPC_CTL_CE6             0x00000040  // Enable Channel 6
#define LPC_CTL_CE5             0x00000020  // Enable Channel 5
#define LPC_CTL_CE4             0x00000010  // Enable Channel 4
#define LPC_CTL_CE3             0x00000008  // Enable Channel 3
#define LPC_CTL_CE2             0x00000004  // Enable Channel 2
#define LPC_CTL_CE1             0x00000002  // Enable Channel 1
#define LPC_CTL_CE0             0x00000001  // Enable Channel 0

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_STS register.
//
//*****************************************************************************
#define LPC_STS_CHCNT_M         0x00700000  // Number of Channels
#define LPC_STS_POOLSZ_M        0x00070000  // Register Pool Size
#define LPC_STS_POOLSZ_256      0x00010000  // 256 bytes
#define LPC_STS_POOLSZ_512      0x00020000  // 512 bytes
#define LPC_STS_POOLSZ_768      0x00030000  // 768 bytes
#define LPC_STS_POOLSZ_1024     0x00040000  // 1024 bytes
#define LPC_STS_RST             0x00000400  // LPC Bus is in Reset
#define LPC_STS_BUSY            0x00000200  // LPC is Busy
#define LPC_STS_SLEEP           0x00000100  // LPC is in Sleep Mode
#define LPC_STS_CA7             0x00000080  // Channel 7 Active
#define LPC_STS_CA6             0x00000040  // Channel 6 Active
#define LPC_STS_CA5             0x00000020  // Channel 5 Active
#define LPC_STS_CA4             0x00000010  // Channel 4 Active
#define LPC_STS_CA3             0x00000008  // Channel 3 Active
#define LPC_STS_CA2             0x00000004  // Channel 2 Active
#define LPC_STS_CA1             0x00000002  // Channel 1 Active
#define LPC_STS_CA0             0x00000001  // Channel 0 Active
#define LPC_STS_CHCNT_S         20

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_IRQCTL register.
//
//*****************************************************************************
#define LPC_IRQCTL_I15          0x80000000  // Manual IRQ15 State
#define LPC_IRQCTL_I14          0x40000000  // Manual IRQ14 State
#define LPC_IRQCTL_I13          0x20000000  // Manual IRQ13 State
#define LPC_IRQCTL_I12          0x10000000  // Manual IRQ12 State
#define LPC_IRQCTL_I11          0x08000000  // Manual IRQ11 State
#define LPC_IRQCTL_I10          0x04000000  // Manual IRQ10 State
#define LPC_IRQCTL_I9           0x02000000  // Manual IRQ9 State
#define LPC_IRQCTL_I8           0x01000000  // Manual IRQ8 State
#define LPC_IRQCTL_I7           0x00800000  // Manual IRQ7 State
#define LPC_IRQCTL_I6           0x00400000  // Manual IRQ6 State
#define LPC_IRQCTL_I5           0x00200000  // Manual IRQ5 State
#define LPC_IRQCTL_I4           0x00100000  // Manual IRQ4 State
#define LPC_IRQCTL_I3           0x00080000  // Manual IRQ3 State
#define LPC_IRQCTL_I2           0x00040000  // Manual IRQ2 State
#define LPC_IRQCTL_I1           0x00020000  // Manual IRQ1 State
#define LPC_IRQCTL_AH           0x00010000  // Active High Control
#define LPC_IRQCTL_PULSE        0x00000004  // Pulse IRQ States
#define LPC_IRQCTL_ONCHG        0x00000002  // Initiate on Change
#define LPC_IRQCTL_SND          0x00000001  // Initiate Immediately

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_IRQST register.
//
//*****************************************************************************
#define LPC_IRQST_I15           0x80000000  // Host IRQ15 State
#define LPC_IRQST_I14           0x40000000  // IRQ14 State
#define LPC_IRQST_I13           0x20000000  // IRQ13 State
#define LPC_IRQST_I12           0x10000000  // IRQ12 State
#define LPC_IRQST_I11           0x08000000  // IRQ11 State
#define LPC_IRQST_I10           0x04000000  // IRQ10 State
#define LPC_IRQST_I9            0x02000000  // IRQ9 State
#define LPC_IRQST_I8            0x01000000  // IRQ8 State
#define LPC_IRQST_I7            0x00800000  // IRQ7 State
#define LPC_IRQST_I6            0x00400000  // IRQ6 State
#define LPC_IRQST_I5            0x00200000  // IRQ5 State
#define LPC_IRQST_I4            0x00100000  // IRQ4 State
#define LPC_IRQST_I3            0x00080000  // IRQ3 State
#define LPC_IRQST_I2            0x00040000  // IRQ2 State
#define LPC_IRQST_I1            0x00020000  // IRQ1 State
#define LPC_IRQST_I0            0x00010000  // IRQ0 State
#define LPC_IRQST_SIRQ          0x00000004  // Pulse IRQ States
#define LPC_IRQST_CONT          0x00000001  // Initiate Immediately

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH0CTL register.
//
//*****************************************************************************
#define LPC_CH0CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH0CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH0CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH0CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH0CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH0CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH0CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH0CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH0CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH0CTL_IRQEN0_TRIG2 0x00020000  // Trigger 2
#define LPC_CH0CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH0CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH0CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH0CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH0CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH0CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH0CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH0CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH0CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH0CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH0CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH0CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH0CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH0CTL_IRQSEL2_S    28
#define LPC_CH0CTL_IRQSEL1_S    24
#define LPC_CH0CTL_IRQSEL0_S    20
#define LPC_CH0CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH0ST register.
//
//*****************************************************************************
#define LPC_CH0ST_USER_M        0x00001F00  // User Data
#define LPC_CH0ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH0ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH0ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH0ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH0ST_CMD           0x00000008  // Command or Data
#define LPC_CH0ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH0ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH0ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH0ADR register.
//
//*****************************************************************************
#define LPC_CH0ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH0ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH0ADR_ADDRL1       0x00000002  // Endpoint Match Bit 1
#define LPC_CH0ADR_ADDRH_S      16
#define LPC_CH0ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH1CTL register.
//
//*****************************************************************************
#define LPC_CH1CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH1CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH1CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH1CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH1CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH1CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH1CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH1CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH1CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH1CTL_IRQEN0_TRGI2 0x00020000  // Trigger 2
#define LPC_CH1CTL_IRQEN0_TRGI3 0x00030000  // Trigger 3
#define LPC_CH1CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH1CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH1CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH1CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH1CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH1CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH1CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH1CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH1CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH1CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH1CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH1CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH1CTL_IRQSEL2_S    28
#define LPC_CH1CTL_IRQSEL1_S    24
#define LPC_CH1CTL_IRQSEL0_S    20
#define LPC_CH1CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH1ST register.
//
//*****************************************************************************
#define LPC_CH1ST_USER_M        0x00001F00  // User Data
#define LPC_CH1ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH1ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH1ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH1ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH1ST_CMD           0x00000008  // Command or Data
#define LPC_CH1ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH1ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH1ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH1ADR register.
//
//*****************************************************************************
#define LPC_CH1ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH1ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH1ADR_ADDRL1       0x00000002  // Endpoint Match Bit 1
#define LPC_CH1ADR_ADDRH_S      16
#define LPC_CH1ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH2CTL register.
//
//*****************************************************************************
#define LPC_CH2CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH2CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH2CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH2CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH2CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH2CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH2CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH2CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH2CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH2CTL_IRQEN0_TRIG2 0x00020000  // Trigger 2
#define LPC_CH2CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH2CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH2CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH2CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH2CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH2CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH2CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH2CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH2CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH2CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH2CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH2CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH2CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH2CTL_IRQSEL2_S    28
#define LPC_CH2CTL_IRQSEL1_S    24
#define LPC_CH2CTL_IRQSEL0_S    20
#define LPC_CH2CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH2ST register.
//
//*****************************************************************************
#define LPC_CH2ST_USER_M        0x00001F00  // User Data
#define LPC_CH2ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH2ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH2ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH2ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH2ST_CMD           0x00000008  // Command or Data
#define LPC_CH2ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH2ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH2ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH2ADR register.
//
//*****************************************************************************
#define LPC_CH2ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH2ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH2ADR_ADDRL1       0x00000002  // Endpoint Match Bit 1
#define LPC_CH2ADR_ADDRH_S      16
#define LPC_CH2ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH3CTL register.
//
//*****************************************************************************
#define LPC_CH3CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH3CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH3CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH3CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH3CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH3CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH3CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH3CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH3CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH3CTL_IRQEN0_TRIG2 0x00020000  // Trigger 2
#define LPC_CH3CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH3CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH3CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH3CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH3CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH3CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH3CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH3CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH3CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH3CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH3CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH3CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH3CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH3CTL_IRQSEL2_S    28
#define LPC_CH3CTL_IRQSEL1_S    24
#define LPC_CH3CTL_IRQSEL0_S    20
#define LPC_CH3CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH3ST register.
//
//*****************************************************************************
#define LPC_CH3ST_USER_M        0x00001F00  // User Data
#define LPC_CH3ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH3ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH3ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH3ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH3ST_CMD           0x00000008  // Command or Data
#define LPC_CH3ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH3ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH3ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH3ADR register.
//
//*****************************************************************************
#define LPC_CH3ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH3ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH3ADR_ADDRL1       0x00000002  // Endpoint Match Bit 1
#define LPC_CH3ADR_ADDRH_S      16
#define LPC_CH3ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH4CTL register.
//
//*****************************************************************************
#define LPC_CH4CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH4CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH4CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH4CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH4CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH4CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH4CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH4CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH4CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH4CTL_IRQEN0_TRIG2 0x00020000  // Trigger 2
#define LPC_CH4CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH4CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH4CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH4CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH4CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH4CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH4CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH4CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH4CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH4CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH4CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH4CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH4CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH4CTL_IRQSEL2_S    28
#define LPC_CH4CTL_IRQSEL1_S    24
#define LPC_CH4CTL_IRQSEL0_S    20
#define LPC_CH4CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH4ST register.
//
//*****************************************************************************
#define LPC_CH4ST_USER_M        0x00001F00  // User Data
#define LPC_CH4ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH4ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH4ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH4ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH4ST_CMD           0x00000008  // Command or Data
#define LPC_CH4ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH4ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH4ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH4ADR register.
//
//*****************************************************************************
#define LPC_CH4ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH4ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH4ADR_ADDRH_S      16
#define LPC_CH4ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH5CTL register.
//
//*****************************************************************************
#define LPC_CH5CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH5CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH5CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH5CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH5CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH5CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH5CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH5CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH5CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH5CTL_IRQEN0_TRIG2 0x00020000  // Trigger 2
#define LPC_CH5CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH5CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH5CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH5CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH5CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH5CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH5CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH5CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH5CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH5CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH5CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH5CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH5CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH5CTL_IRQSEL2_S    28
#define LPC_CH5CTL_IRQSEL1_S    24
#define LPC_CH5CTL_IRQSEL0_S    20
#define LPC_CH5CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH5ST register.
//
//*****************************************************************************
#define LPC_CH5ST_USER_M        0x00001F00  // User Data
#define LPC_CH5ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH5ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH5ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH5ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH5ST_CMD           0x00000008  // Command or Data
#define LPC_CH5ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH5ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH5ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH5ADR register.
//
//*****************************************************************************
#define LPC_CH5ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH5ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH5ADR_ADDRH_S      16
#define LPC_CH5ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH6CTL register.
//
//*****************************************************************************
#define LPC_CH6CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH6CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH6CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH6CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH6CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH6CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH6CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH6CTL_IRQEN0_DIS   0x00000000  // Trigger disabled
#define LPC_CH6CTL_IRQEN0_TRIG1 0x00010000  // Trigger 1
#define LPC_CH6CTL_IRQEN0_TRIG2 0x00020000  // Trigger 2
#define LPC_CH6CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH6CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH6CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH6CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH6CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH6CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH6CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH6CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH6CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH6CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH6CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH6CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH6CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH6CTL_IRQSEL2_S    28
#define LPC_CH6CTL_IRQSEL1_S    24
#define LPC_CH6CTL_IRQSEL0_S    20
#define LPC_CH6CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH6ST register.
//
//*****************************************************************************
#define LPC_CH6ST_USER_M        0x00001F00  // User Data
#define LPC_CH6ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH6ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH6ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH6ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH6ST_CMD           0x00000008  // Command or Data
#define LPC_CH6ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH6ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH6ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH6ADR register.
//
//*****************************************************************************
#define LPC_CH6ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH6ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH6ADR_ADDRH_S      16
#define LPC_CH6ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH7CTL register.
//
//*****************************************************************************
#define LPC_CH7CTL_IRQSEL2_M    0xF0000000  // IRQ Select 2
#define LPC_CH7CTL_IRQSEL1_M    0x0F000000  // IRQ Select 1
#define LPC_CH7CTL_IRQSEL0_M    0x00F00000  // IRQ Select 0
#define LPC_CH7CTL_CX           0x00080000  // IRQ Enable 2
#define LPC_CH7CTL_IRQEN2       0x00080000  // IRQ Enable 2
#define LPC_CH7CTL_IRQEN1       0x00040000  // IRQ Enable 1
#define LPC_CH7CTL_IRQEN0_M     0x00030000  // IRQ Enable 0
#define LPC_CH7CTL_IRQEN0_AUTO  0x00000000  // The automatic IRQ trigger is
                                            // disabled
#define LPC_CH7CTL_IRQEN0_MST   0x00010000  // If TYPE is set, the IRQ selected
                                            // by IRQSEL0 is triggered when the
                                            // master wins arbitration (the
                                            // HW1ST bit is set)
#define LPC_CH7CTL_IRQEN0_SLV   0x00020000  // If TYPE is set, the IRQ selected
                                            // by IRQSEL0 is triggered when the
                                            // slave wins arbitration (the
                                            // SW1ST bit is set)
#define LPC_CH7CTL_IRQEN0_TRIG3 0x00030000  // Trigger 3
#define LPC_CH7CTL_ARBDIS       0x00008000  // Arbitration Disabled
#define LPC_CH7CTL_OFFSET_M     0x00003FE0  // Base Offset in Register Pool
#define LPC_CH7CTL_AMASK_M      0x0000001C  // Address Mask for Ranges
#define LPC_CH7CTL_AMASK_4      0x00000000  // 4 bytes
#define LPC_CH7CTL_AMASK_8      0x00000004  // 8 bytes
#define LPC_CH7CTL_AMASK_16     0x00000008  // 16 bytes
#define LPC_CH7CTL_AMASK_32     0x0000000C  // 32 bytes
#define LPC_CH7CTL_AMASK_64     0x00000010  // 64 bytes
#define LPC_CH7CTL_AMASK_128    0x00000014  // 128 bytes
#define LPC_CH7CTL_AMASK_256    0x00000018  // 256 bytes
#define LPC_CH7CTL_AMASK_512    0x0000001C  // 512 bytes
#define LPC_CH7CTL_TYPE         0x00000001  // Channel Type
#define LPC_CH7CTL_IRQSEL2_S    28
#define LPC_CH7CTL_IRQSEL1_S    24
#define LPC_CH7CTL_IRQSEL0_S    20
#define LPC_CH7CTL_OFFSET_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH7ST register.
//
//*****************************************************************************
#define LPC_CH7ST_USER_M        0x00001F00  // User Data
#define LPC_CH7ST_LASTHW        0x00000080  // Last Host Write
#define LPC_CH7ST_HW1ST         0x00000040  // Host Wrote First
#define LPC_CH7ST_LASTSW        0x00000020  // Last Slave Write
#define LPC_CH7ST_SW1ST         0x00000010  // Slave Wrote First
#define LPC_CH7ST_CMD           0x00000008  // Command or Data
#define LPC_CH7ST_FRMH          0x00000002  // From-Host Transaction
#define LPC_CH7ST_TOH           0x00000001  // To-Host Transaction
#define LPC_CH7ST_USER_S        8

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_CH7ADR register.
//
//*****************************************************************************
#define LPC_CH7ADR_ADDRH_M      0xFFFF0000  // Upper Address Match
#define LPC_CH7ADR_ADDRL_M      0x0000FFF8  // Lower Address Match
#define LPC_CH7ADR_ADDRH_S      16
#define LPC_CH7ADR_ADDRL_S      3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_STSADDR register.
//
//*****************************************************************************
#define LPC_STSADDR_ADDRH_M     0xFFFF0000  // Upper Address Match
#define LPC_STSADDR_ADDRL_M     0x0000FFF8  // Lower Address Match
#define LPC_STSADDR_ENA         0x00000001  // Enable Status Block
#define LPC_STSADDR_ADDRH_S     16
#define LPC_STSADDR_ADDRL_S     3

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_IM register.
//
//*****************************************************************************
#define LPC_IM_RSTIM            0x80000000  // Reset State Interrupt Mask
#define LPC_IM_SLEEPIM          0x40000000  // Sleep State Interrupt Mask
#define LPC_IM_COMXIM           0x20000000  // COMx Interrupt Mask
#define LPC_IM_SIRQIM           0x10000000  // SERIRQ Frame Complete Interrupt
                                            // Mask
#define LPC_IM_CH6IM3           0x08000000  // Channel 6 Interrupt Mask 3
#define LPC_IM_CH6IM2           0x04000000  // Channel 6 Interrupt Mask 2
#define LPC_IM_CH6IM1           0x02000000  // Channel 6 Interrupt Mask 1
#define LPC_IM_CH6IM0           0x01000000  // Channel 6 Interrupt Mask 0
#define LPC_IM_CH5IM3           0x00800000  // Channel 5 Interrupt Mask 3
#define LPC_IM_CH5IM2           0x00400000  // Channel 5 Interrupt Mask 2
#define LPC_IM_CH5IM1           0x00200000  // Channel 5 Interrupt Mask 1
#define LPC_IM_CH5IM0           0x00100000  // Channel 5 Interrupt Mask 0
#define LPC_IM_CH4IM3           0x00080000  // Channel 4 Interrupt Mask 3
#define LPC_IM_CH4IM2           0x00040000  // Channel 4 Interrupt Mask 2
#define LPC_IM_CH4IM1           0x00020000  // Channel 4 Interrupt Mask 1
#define LPC_IM_CH4IM0           0x00010000  // Channel 4 Interrupt Mask 0
#define LPC_IM_CH3IM3           0x00008000  // Channel 3 Interrupt Mask 3
#define LPC_IM_CH3IM2           0x00004000  // Channel 3 Interrupt Mask 2
#define LPC_IM_CH3IM1           0x00002000  // Channel 3 Interrupt Mask 1
#define LPC_IM_CH3IM0           0x00001000  // Channel 3 Interrupt Mask 0
#define LPC_IM_CH2IM3           0x00000800  // Channel 2 Interrupt Mask 3
#define LPC_IM_CH2IM2           0x00000400  // Channel 2 Interrupt Mask 2
#define LPC_IM_CH2IM1           0x00000200  // Channel 2 Interrupt Mask 1
#define LPC_IM_CH2IM0           0x00000100  // Channel 2 Interrupt Mask 0
#define LPC_IM_CH1IM3           0x00000080  // Channel 1 Interrupt Mask 3
#define LPC_IM_CH1IM2           0x00000040  // Channel 1 Interrupt Mask 2
#define LPC_IM_CH1IM1           0x00000020  // Channel 1 Interrupt Mask 1
#define LPC_IM_CH1IM0           0x00000010  // Channel 1 Interrupt Mask 0
#define LPC_IM_CH0IM3           0x00000008  // Channel 0 Interrupt Mask 3
#define LPC_IM_CH0IM2           0x00000004  // Channel 0 Interrupt Mask 2
#define LPC_IM_CH0IM1           0x00000002  // Channel 0 Interrupt Mask 1
#define LPC_IM_CH0IM0           0x00000001  // Channel 0 Interrupt Mask 0

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_RIS register.
//
//*****************************************************************************
#define LPC_RIS_RSTRIS          0x80000000  // Reset State Raw Interrupt Status
#define LPC_RIS_SLEEPRIS        0x40000000  // Sleep State Raw Interrupt Status
#define LPC_RIS_COMXRIS         0x20000000  // COMx Raw Interrupt Status
#define LPC_RIS_SIRQRIS         0x10000000  // SERIRQ Frame Complete Raw
                                            // Interrupt Status
#define LPC_RIS_CH6RIS3         0x08000000  // Channel 6 Raw Interrupt Status 3
#define LPC_RIS_CH6RIS2         0x04000000  // Channel 6 Raw Interrupt Status 2
#define LPC_RIS_CH6RIS1         0x02000000  // Channel 6 Raw Interrupt Status 1
#define LPC_RIS_CH6RIS0         0x01000000  // Channel 6 Raw Interrupt Status 0
#define LPC_RIS_CH5RIS3         0x00800000  // Channel 5 Raw Interrupt Status 3
#define LPC_RIS_CH5RIS2         0x00400000  // Channel 5 Raw Interrupt Status 2
#define LPC_RIS_CH5RIS1         0x00200000  // Channel 5 Raw Interrupt Status 1
#define LPC_RIS_CH5RIS0         0x00100000  // Channel 5 Raw Interrupt Status 0
#define LPC_RIS_CH4RIS3         0x00080000  // Channel 4 Raw Interrupt Status 3
#define LPC_RIS_CH4RIS2         0x00040000  // Channel 4 Raw Interrupt Status 2
#define LPC_RIS_CH4RIS1         0x00020000  // Channel 4 Raw Interrupt Status 1
#define LPC_RIS_CH4RIS0         0x00010000  // Channel 4 Raw Interrupt Status 0
#define LPC_RIS_CH3RIS3         0x00008000  // Channel 3 Raw Interrupt Status 3
#define LPC_RIS_CH3RIS2         0x00004000  // Channel 3 Raw Interrupt Status 2
#define LPC_RIS_CH3RIS1         0x00002000  // Channel 3 Raw Interrupt Status 1
#define LPC_RIS_CH3RIS0         0x00001000  // Channel 3 Raw Interrupt Status 0
#define LPC_RIS_CH2RIS3         0x00000800  // Channel 2 Raw Interrupt Status 3
#define LPC_RIS_CH2RIS2         0x00000400  // Channel 2 Raw Interrupt Status 2
#define LPC_RIS_CH2RIS1         0x00000200  // Channel 2 Raw Interrupt Status 1
#define LPC_RIS_CH2RIS0         0x00000100  // Channel 2 Raw Interrupt Status 0
#define LPC_RIS_CH1RIS3         0x00000080  // Channel 1 Raw Interrupt Status 3
#define LPC_RIS_CH1RIS2         0x00000040  // Channel 1 Raw Interrupt Status 2
#define LPC_RIS_CH1RIS1         0x00000020  // Channel 1 Raw Interrupt Status 1
#define LPC_RIS_CH1RIS0         0x00000010  // Channel 1 Raw Interrupt Status 0
#define LPC_RIS_CH0RIS3         0x00000008  // Channel 0 Raw Interrupt Status 3
#define LPC_RIS_CH0RIS2         0x00000004  // Channel 0 Raw Interrupt Status 2
#define LPC_RIS_CH0RIS1         0x00000002  // Channel 0 Raw Interrupt Status 1
#define LPC_RIS_CH0RIS0         0x00000001  // Channel 0 Raw Interrupt Status 0

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_MIS register.
//
//*****************************************************************************
#define LPC_MIS_RSTMIS          0x80000000  // Reset State Masked Interrupt
                                            // Status
#define LPC_MIS_SLEEPMIS        0x40000000  // Sleep State Masked Interrupt
                                            // Status
#define LPC_MIS_COMXMIS         0x20000000  // COMx Masked Interrupt Status
#define LPC_MIS_SIRQMIS         0x10000000  // SERIRQ Frame Complete Masked
                                            // Interrupt Status
#define LPC_MIS_CH6MIS3         0x08000000  // Channel 6 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH6MIS2         0x04000000  // Channel 6 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH6MIS1         0x02000000  // Channel 6 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH6MIS0         0x01000000  // Channel 6 Masked Interrupt
                                            // Status 0
#define LPC_MIS_CH5MIS3         0x00800000  // Channel 5 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH5MIS2         0x00400000  // Channel 5 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH5MIS1         0x00200000  // Channel 5 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH5MIS0         0x00100000  // Channel 5 Masked Interrupt
                                            // Status 0
#define LPC_MIS_CH4MIS3         0x00080000  // Channel 4 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH4MIS2         0x00040000  // Channel 4 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH4MIS1         0x00020000  // Channel 4 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH4MIS0         0x00010000  // Channel 4 Masked Interrupt
                                            // Status 0
#define LPC_MIS_CH3MIS3         0x00008000  // Channel 3 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH3MIS2         0x00004000  // Channel 3 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH3MIS1         0x00002000  // Channel 3 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH3MIS0         0x00001000  // Channel 3 Masked Interrupt
                                            // Status 0
#define LPC_MIS_CH2MIS3         0x00000800  // Channel 2 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH2MIS2         0x00000400  // Channel 2 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH2MIS1         0x00000200  // Channel 2 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH2MIS0         0x00000100  // Channel 2 Masked Interrupt
                                            // Status 0
#define LPC_MIS_CH1MIS3         0x00000080  // Channel 1 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH1MIS2         0x00000040  // Channel 1 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH1MIS1         0x00000020  // Channel 1 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH1MIS0         0x00000010  // Channel 1 Masked Interrupt
                                            // Status 0
#define LPC_MIS_CH0MIS3         0x00000008  // Channel 0 Masked Interrupt
                                            // Status 3
#define LPC_MIS_CH0MIS2         0x00000004  // Channel 0 Masked Interrupt
                                            // Status 2
#define LPC_MIS_CH0MIS1         0x00000002  // Channel 0 Masked Interrupt
                                            // Status 1
#define LPC_MIS_CH0MIS0         0x00000001  // Channel 0 Masked Interrupt
                                            // Status 0

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_IC register.
//
//*****************************************************************************
#define LPC_IC_RSTIC            0x80000000  // Reset State Interrupt Clear
#define LPC_IC_SLEEPIC          0x40000000  // Sleep State Interrupt Clear
#define LPC_IC_COMXIC           0x20000000  // COMx Interrupt Clear
#define LPC_IC_SIRQRIC          0x10000000  // SERIRQ Frame Complete Interrupt
                                            // Clear
#define LPC_IC_CH6IC3           0x08000000  // Channel 6 Interrupt Clear 3
#define LPC_IC_CH6IC2           0x04000000  // Channel 6 Interrupt Clear 2
#define LPC_IC_CH6IC1           0x02000000  // Channel 6 Interrupt Clear 1
#define LPC_IC_CH6IC0           0x01000000  // Channel 6 Interrupt Clear 0
#define LPC_IC_CH5IC3           0x00800000  // Channel 5 Interrupt Clear 3
#define LPC_IC_CH5IC2           0x00400000  // Channel 5 Interrupt Clear 2
#define LPC_IC_CH5IC1           0x00200000  // Channel 5 Interrupt Clear 1
#define LPC_IC_CH5IC0           0x00100000  // Channel 5 Interrupt Clear 0
#define LPC_IC_CH4IC3           0x00080000  // Channel 4 Interrupt Clear 3
#define LPC_IC_CH4IC2           0x00040000  // Channel 4 Interrupt Clear 2
#define LPC_IC_CH4IC1           0x00020000  // Channel 4 Interrupt Clear 1
#define LPC_IC_CH4IC0           0x00010000  // Channel 4 Interrupt Clear 0
#define LPC_IC_CH3IC3           0x00008000  // Channel 3 Interrupt Clear 3
#define LPC_IC_CH3IC2           0x00004000  // Channel 3 Interrupt Clear 2
#define LPC_IC_CH3IC1           0x00002000  // Channel 3 Interrupt Clear 1
#define LPC_IC_CH3IC0           0x00001000  // Channel 3 Interrupt Clear 0
#define LPC_IC_CH2IC3           0x00000800  // Channel 2 Interrupt Clear 3
#define LPC_IC_CH2IC2           0x00000400  // Channel 2 Interrupt Clear 2
#define LPC_IC_CH2IC1           0x00000200  // Channel 2 Interrupt Clear 1
#define LPC_IC_CH2IC0           0x00000100  // Channel 2 Interrupt Clear 0
#define LPC_IC_CH1IC3           0x00000080  // Channel 1 Interrupt Clear 3
#define LPC_IC_CH1IC2           0x00000040  // Channel 1 Interrupt Clear 2
#define LPC_IC_CH1IC1           0x00000020  // Channel 1 Interrupt Clear 1
#define LPC_IC_CH1IC0           0x00000010  // Channel 1 Interrupt Clear 0
#define LPC_IC_CH0IC3           0x00000008  // Channel 0 Interrupt Clear 3
#define LPC_IC_CH0IC2           0x00000004  // Channel 0 Interrupt Clear 2
#define LPC_IC_CH0IC1           0x00000002  // Channel 0 Interrupt Clear 1
#define LPC_IC_CH0IC0           0x00000001  // Channel 0 Interrupt Clear 0

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_DMACX register.
//
//*****************************************************************************
#define LPC_DMACX_CXRES         0x02000000  // Raw Event State for COMx
#define LPC_DMACX_CXTXRES       0x01000000  // Raw Event State for COMx TX
#define LPC_DMACX_CXRXRES       0x00800000  // Raw Event State for COMx RX
#define LPC_DMACX_CXEM          0x00200000  // Event Mask for COMx
#define LPC_DMACX_CXTXEM        0x00100000  // Event Mask for COMx TX
#define LPC_DMACX_CXRXEM        0x00080000  // Event Mask for COMx RX
#define LPC_DMACX_CXACT_M       0x00060000  // COMx Action
#define LPC_DMACX_CXACT_FRMHNML 0x00000000  // Treat as normal FRMH model and
                                            // let be full (and so marked as
                                            // full)
#define LPC_DMACX_CXACT_FRMHIGN 0x00020000  // Ignore FRMH bytes and continue
                                            // to mark FRMH as empty
#define LPC_DMACX_CXACT_FRMHDMA 0x00040000  // COMx DMA on FRMH byte (e.g. to
                                            // memory)
#define LPC_DMACX_CXACT_UARTDMA 0x00060000  // COMx DMA model with UART
#define LPC_DMACX_COMX          0x00010000  // COMx Handling
#define LPC_DMACX_C3W           0x00000080  // Channel 3 Write Control
#define LPC_DMACX_C3R           0x00000040  // Channel 3 Read Control
#define LPC_DMACX_C2W           0x00000020  // Channel 2 Write Control
#define LPC_DMACX_C2R           0x00000010  // Channel 2 Read Control
#define LPC_DMACX_C1W           0x00000008  // Channel 1 Write Control
#define LPC_DMACX_C1R           0x00000004  // Channel 1 Read Control
#define LPC_DMACX_C0W           0x00000002  // Channel 0 Write Control
#define LPC_DMACX_C0R           0x00000001  // Channel 0 Read Control

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_POOL register.
//
//*****************************************************************************
#define LPC_POOL_BYTE3_M        0xFF000000  // Byte 3
#define LPC_POOL_BYTE2_M        0x00FF0000  // Byte 2
#define LPC_POOL_BYTE1_M        0x0000FF00  // Byte 1
#define LPC_POOL_BYTE0_M        0x000000FF  // Byte 0
#define LPC_POOL_BYTE3_S        24
#define LPC_POOL_BYTE2_S        16
#define LPC_POOL_BYTE1_S        8
#define LPC_POOL_BYTE0_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the LPC_O_PP register.
//
//*****************************************************************************
#define LPC_PP_COMX             0x00000010  // COMx Support Available
#define LPC_PP_CHANCNT_M        0x0000000F  // Number of Channels (Excluding
                                            // COMx)
#define LPC_PP_CHANCNT_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_STS register.
//
//*****************************************************************************
#define FAN_STS_FANCNT_M        0x000F0000  // Fan Count
#define FAN_STS_ST5_M           0x00000C00  // Fan 5 Status
#define FAN_STS_ST5_STALLED     0x00000000  // Stalled
#define FAN_STS_ST5_CHANGING    0x00000400  // Changing
#define FAN_STS_ST5_LOCKED      0x00000800  // Locked
#define FAN_STS_ST5_NOATTAIN    0x00000C00  // No Attain
#define FAN_STS_ST4_M           0x00000300  // Fan 4 Status
#define FAN_STS_ST4_STALLED     0x00000000  // Stalled
#define FAN_STS_ST4_CHANGING    0x00000100  // Changing
#define FAN_STS_ST4_LOCKED      0x00000200  // Locked
#define FAN_STS_ST4_NOATTAIN    0x00000300  // No Attain
#define FAN_STS_ST3_M           0x000000C0  // Fan 3 Status
#define FAN_STS_ST3_STALLED     0x00000000  // Stalled
#define FAN_STS_ST3_CHANGING    0x00000040  // Changing
#define FAN_STS_ST3_LOCKED      0x00000080  // Locked
#define FAN_STS_ST3_NOATTAIN    0x000000C0  // No Attain
#define FAN_STS_ST2_M           0x00000030  // Fan 2 Status
#define FAN_STS_ST2_STALLED     0x00000000  // Stalled
#define FAN_STS_ST2_CHANGING    0x00000010  // Changing
#define FAN_STS_ST2_LOCKED      0x00000020  // Locked
#define FAN_STS_ST2_NOATTAIN    0x00000030  // No Attain
#define FAN_STS_ST1_M           0x0000000C  // Fan 1 Status
#define FAN_STS_ST1_STALLED     0x00000000  // Stalled
#define FAN_STS_ST1_CHANGING    0x00000004  // Changing
#define FAN_STS_ST1_LOCKED      0x00000008  // Locked
#define FAN_STS_ST1_NOATTAIN    0x0000000C  // No Attain
#define FAN_STS_ST0_M           0x00000003  // Fan 0 Status
#define FAN_STS_ST0_STALLED     0x00000000  // Stalled
#define FAN_STS_ST0_CHANGING    0x00000001  // Changing
#define FAN_STS_ST0_LOCKED      0x00000002  // Locked
#define FAN_STS_ST0_NOATTAIN    0x00000003  // No Attain
#define FAN_STS_FANCNT_S        16

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CTL register.
//
//*****************************************************************************
#define FAN_CTL_E5              0x00000020  // Fan 5 Enable
#define FAN_CTL_E4              0x00000010  // Fan 4 Enable
#define FAN_CTL_E3              0x00000008  // Fan 3 Enable
#define FAN_CTL_E2              0x00000004  // Fan 2 Enable
#define FAN_CTL_E1              0x00000002  // Fan 1 Enable
#define FAN_CTL_E0              0x00000001  // Fan 0 Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CH0 register.
//
//*****************************************************************************
#define FAN_CH0_ASTART          0x00008000  // Auto Restart
#define FAN_CH0_ACCEL           0x00004000  // Acceleration and Deceleration
                                            // Rule
#define FAN_CH0_HYST_M          0x00003800  // Hysteresis Adjustment on PID
                                            // Adjust
#define FAN_CH0_STPER_M         0x00000700  // Start Period
#define FAN_CH0_START_M         0x000000C0  // Fast Start Rule
#define FAN_CH0_START_NOFAST    0x00000000  // No fast start
#define FAN_CH0_START_50DC      0x00000040  // 50% duty cycle
#define FAN_CH0_START_75DC      0x00000080  // 75% duty cycle
#define FAN_CH0_START_100DC     0x000000C0  // 100% duty cycle
#define FAN_CH0_AVG_M           0x00000030  // Averaging of Tachometer
#define FAN_CH0_AVG_0           0x00000000  // No averaging
#define FAN_CH0_AVG_2           0x00000010  // Average 2 edges
#define FAN_CH0_AVG_4           0x00000020  // Average 4 edges
#define FAN_CH0_AVG_8           0x00000030  // Average 8 edges
#define FAN_CH0_PPR_M           0x0000000C  // Pulse per Revolution on
                                            // Tachometer
#define FAN_CH0_PPR_1           0x00000000  // 1 pulse per revolution
#define FAN_CH0_PPR_2           0x00000004  // 2 pulses per revolution
#define FAN_CH0_PPR_4           0x00000008  // 4 pulses per revolution
#define FAN_CH0_PPR_8           0x0000000C  // 8 pulses per revolution
#define FAN_CH0_MAN             0x00000001  // Control Type
#define FAN_CH0_HYST_S          11
#define FAN_CH0_STPER_S         8

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CMD0 register.
//
//*****************************************************************************
#define FAN_CMD0_DC_M           0x01FF0000  // PWM Duty Cycle
#define FAN_CMD0_RPM_M          0x00001FFF  // Fan Speed (in RPM)
#define FAN_CMD0_DC_S           16
#define FAN_CMD0_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CST0 register.
//
//*****************************************************************************
#define FAN_CST0_COUNT_M        0x07FF0000  // Last Read Tachometer Count
#define FAN_CST0_RPM_M          0x00001FFF  // Last Read RPM Speed Computed
#define FAN_CST0_COUNT_S        16
#define FAN_CST0_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CH1 register.
//
//*****************************************************************************
#define FAN_CH1_ASTART          0x00008000  // Auto Restart
#define FAN_CH1_ACCEL           0x00004000  // Acceleration and Deceleration
                                            // Rule
#define FAN_CH1_HYST_M          0x00003800  // Hysteresis Adjustment on PID
                                            // Adjust
#define FAN_CH1_STPER_M         0x00000700  // Start Period
#define FAN_CH1_START_M         0x000000C0  // Fast Start Rule
#define FAN_CH1_START_NOFAST    0x00000000  // No fast start
#define FAN_CH1_START_50DC      0x00000040  // 50% duty cycle
#define FAN_CH1_START_75DC      0x00000080  // 75% duty cycle
#define FAN_CH1_START_100DC     0x000000C0  // 100% duty cycle
#define FAN_CH1_AVG_M           0x00000030  // Averaging of Tachometer
#define FAN_CH1_AVG_0           0x00000000  // No averaging
#define FAN_CH1_AVG_2           0x00000010  // Average 2 edges
#define FAN_CH1_AVG_4           0x00000020  // Average 4 edges
#define FAN_CH1_AVG_8           0x00000030  // Average 8 edges
#define FAN_CH1_PPR_M           0x0000000C  // Pulse per Revolution on
                                            // Tachometer
#define FAN_CH1_PPR_1           0x00000000  // 1 pulse per revolution
#define FAN_CH1_PPR_2           0x00000004  // 2 pulses per revolution
#define FAN_CH1_PPR_4           0x00000008  // 4 pulses per revolution
#define FAN_CH1_PPR_8           0x0000000C  // 8 pulses per revolution
#define FAN_CH1_MAN             0x00000001  // Control Type
#define FAN_CH1_HYST_S          11
#define FAN_CH1_STPER_S         8

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CMD1 register.
//
//*****************************************************************************
#define FAN_CMD1_DC_M           0x01FF0000  // PWM Duty Cycle
#define FAN_CMD1_RPM_M          0x00001FFF  // Fan Speed (in RPM)
#define FAN_CMD1_DC_S           16
#define FAN_CMD1_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CST1 register.
//
//*****************************************************************************
#define FAN_CST1_COUNT_M        0x07FF0000  // Last Read Tachometer Count
#define FAN_CST1_RPM_M          0x00001FFF  // Last Read RPM Speed Computed
#define FAN_CST1_COUNT_S        16
#define FAN_CST1_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CH2 register.
//
//*****************************************************************************
#define FAN_CH2_ASTART          0x00008000  // Auto Restart
#define FAN_CH2_ACCEL           0x00004000  // Acceleration and Deceleration
                                            // Rule
#define FAN_CH2_HYST_M          0x00003800  // Hysteresis Adjustment on PID
                                            // Adjust
#define FAN_CH2_STPER_M         0x00000700  // Start Period
#define FAN_CH2_START_M         0x000000C0  // Fast Start Rule
#define FAN_CH2_START_NOFAST    0x00000000  // No fast start
#define FAN_CH2_START_50DC      0x00000040  // 50% duty cycle
#define FAN_CH2_START_75DC      0x00000080  // 75% duty cycle
#define FAN_CH2_START_100DC     0x000000C0  // 100% duty cycle
#define FAN_CH2_AVG_M           0x00000030  // Averaging of Tachometer
#define FAN_CH2_AVG_0           0x00000000  // No averaging
#define FAN_CH2_AVG_2           0x00000010  // Average 2 edges
#define FAN_CH2_AVG_4           0x00000020  // Average 4 edges
#define FAN_CH2_AVG_8           0x00000030  // Average 8 edges
#define FAN_CH2_PPR_M           0x0000000C  // Pulse per Revolution on
                                            // Tachometer
#define FAN_CH2_PPR_1           0x00000000  // 1 pulse per revolution
#define FAN_CH2_PPR_2           0x00000004  // 2 pulses per revolution
#define FAN_CH2_PPR_4           0x00000008  // 4 pulses per revolution
#define FAN_CH2_PPR_8           0x0000000C  // 8 pulses per revolution
#define FAN_CH2_MAN             0x00000001  // Control Type
#define FAN_CH2_HYST_S          11
#define FAN_CH2_STPER_S         8

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CMD2 register.
//
//*****************************************************************************
#define FAN_CMD2_DC_M           0x01FF0000  // PWM Duty Cycle
#define FAN_CMD2_RPM_M          0x00001FFF  // Fan Speed (in RPM)
#define FAN_CMD2_DC_S           16
#define FAN_CMD2_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CST2 register.
//
//*****************************************************************************
#define FAN_CST2_COUNT_M        0x07FF0000  // Last Read Tachometer Count
#define FAN_CST2_RPM_M          0x00001FFF  // Last Read RPM Speed Computed
#define FAN_CST2_COUNT_S        16
#define FAN_CST2_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CH3 register.
//
//*****************************************************************************
#define FAN_CH3_ASTART          0x00008000  // Auto Restart
#define FAN_CH3_ACCEL           0x00004000  // Acceleration and Deceleration
                                            // Rule
#define FAN_CH3_HYST_M          0x00003800  // Hysteresis Adjustment on PID
                                            // Adjust
#define FAN_CH3_STPER_M         0x00000700  // Start Period
#define FAN_CH3_START_M         0x000000C0  // Fast Start Rule
#define FAN_CH3_START_NOFAST    0x00000000  // No fast start
#define FAN_CH3_START_50DC      0x00000040  // 50% duty cycle
#define FAN_CH3_START_75DC      0x00000080  // 75% duty cycle
#define FAN_CH3_START_100DC     0x000000C0  // 100% duty cycle
#define FAN_CH3_AVG_M           0x00000030  // Averaging of Tachometer
#define FAN_CH3_AVG_0           0x00000000  // No averaging
#define FAN_CH3_AVG_2           0x00000010  // Average 2 edges
#define FAN_CH3_AVG_4           0x00000020  // Average 4 edges
#define FAN_CH3_AVG_8           0x00000030  // Average 8 edges
#define FAN_CH3_PPR_M           0x0000000C  // Pulse per Revolution on
                                            // Tachometer
#define FAN_CH3_PPR_1           0x00000000  // 1 pulse per revolution
#define FAN_CH3_PPR_2           0x00000004  // 2 pulses per revolution
#define FAN_CH3_PPR_4           0x00000008  // 4 pulses per revolution
#define FAN_CH3_PPR_8           0x0000000C  // 8 pulses per revolution
#define FAN_CH3_MAN             0x00000001  // Control Type
#define FAN_CH3_HYST_S          11
#define FAN_CH3_STPER_S         8

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CMD3 register.
//
//*****************************************************************************
#define FAN_CMD3_DC_M           0x01FF0000  // PWM Duty Cycle
#define FAN_CMD3_RPM_M          0x00001FFF  // Fan Speed (in RPM)
#define FAN_CMD3_DC_S           16
#define FAN_CMD3_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CST3 register.
//
//*****************************************************************************
#define FAN_CST3_COUNT_M        0x07FF0000  // Last Read Tachometer Count
#define FAN_CST3_RPM_M          0x00001FFF  // Last Read RPM Speed Computed
#define FAN_CST3_COUNT_S        16
#define FAN_CST3_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CH4 register.
//
//*****************************************************************************
#define FAN_CH4_ASTART          0x00008000  // Auto Restart
#define FAN_CH4_ACCEL           0x00004000  // Acceleration and Deceleration
                                            // Rule
#define FAN_CH4_HYST_M          0x00003800  // Hysteresis Adjustment on PID
                                            // Adjust
#define FAN_CH4_STPER_M         0x00000700  // Start Period
#define FAN_CH4_START_M         0x000000C0  // Fast Start Rule
#define FAN_CH4_START_NOFAST    0x00000000  // No fast start
#define FAN_CH4_START_50DC      0x00000040  // 50% duty cycle
#define FAN_CH4_START_75DC      0x00000080  // 75% duty cycle
#define FAN_CH4_START_100DC     0x000000C0  // 100% duty cycle
#define FAN_CH4_AVG_M           0x00000030  // Averaging of Tachometer
#define FAN_CH4_AVG_0           0x00000000  // No averaging
#define FAN_CH4_AVG_2           0x00000010  // Average 2 edges
#define FAN_CH4_AVG_4           0x00000020  // Average 4 edges
#define FAN_CH4_AVG_8           0x00000030  // Average 8 edges
#define FAN_CH4_PPR_M           0x0000000C  // Pulse per Revolution on
                                            // Tachometer
#define FAN_CH4_PPR_1           0x00000000  // 1 pulse per revolution
#define FAN_CH4_PPR_2           0x00000004  // 2 pulses per revolution
#define FAN_CH4_PPR_4           0x00000008  // 4 pulses per revolution
#define FAN_CH4_PPR_8           0x0000000C  // 8 pulses per revolution
#define FAN_CH4_MAN             0x00000001  // Control Type
#define FAN_CH4_HYST_S          11
#define FAN_CH4_STPER_S         8

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CMD4 register.
//
//*****************************************************************************
#define FAN_CMD4_DC_M           0x01FF0000  // PWM Duty Cycle
#define FAN_CMD4_RPM_M          0x00001FFF  // Fan Speed (in RPM)
#define FAN_CMD4_DC_S           16
#define FAN_CMD4_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CST4 register.
//
//*****************************************************************************
#define FAN_CST4_COUNT_M        0x07FF0000  // Last Read Tachometer Count
#define FAN_CST4_RPM_M          0x00001FFF  // Last Read RPM Speed Computed
#define FAN_CST4_COUNT_S        16
#define FAN_CST4_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CH5 register.
//
//*****************************************************************************
#define FAN_CH5_ASTART          0x00008000  // Auto Restart
#define FAN_CH5_ACCEL           0x00004000  // Acceleration and Deceleration
                                            // Rule
#define FAN_CH5_HYST_M          0x00003800  // Hysteresis Adjustment on PID
                                            // Adjust
#define FAN_CH5_STPER_M         0x00000700  // Start Period
#define FAN_CH5_START_M         0x000000C0  // Fast Start Rule
#define FAN_CH5_START_NOFAST    0x00000000  // No fast start
#define FAN_CH5_START_50DC      0x00000040  // 50% duty cycle
#define FAN_CH5_START_75DC      0x00000080  // 75% duty cycle
#define FAN_CH5_START_100DC     0x000000C0  // 100% duty cycle
#define FAN_CH5_AVG_M           0x00000030  // Averaging of Tachometer
#define FAN_CH5_AVG_0           0x00000000  // No averaging
#define FAN_CH5_AVG_2           0x00000010  // Average 2 edges
#define FAN_CH5_AVG_4           0x00000020  // Average 4 edges
#define FAN_CH5_AVG_8           0x00000030  // Average 8 edges
#define FAN_CH5_PPR_M           0x0000000C  // Pulse per Revolution on
                                            // Tachometer
#define FAN_CH5_PPR_1           0x00000000  // 1 pulse per revolution
#define FAN_CH5_PPR_2           0x00000004  // 2 pulses per revolution
#define FAN_CH5_PPR_4           0x00000008  // 4 pulses per revolution
#define FAN_CH5_PPR_8           0x0000000C  // 8 pulses per revolution
#define FAN_CH5_MAN             0x00000001  // Control Type
#define FAN_CH5_HYST_S          11
#define FAN_CH5_STPER_S         8

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CMD5 register.
//
//*****************************************************************************
#define FAN_CMD5_DC_M           0x01FF0000  // PWM Duty Cycle
#define FAN_CMD5_RPM_M          0x00001FFF  // Fan Speed (in RPM)
#define FAN_CMD5_DC_S           16
#define FAN_CMD5_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_CST5 register.
//
//*****************************************************************************
#define FAN_CST5_COUNT_M        0x07FF0000  // Last Read Tachometer Count
#define FAN_CST5_RPM_M          0x00001FFF  // Last Read RPM Speed Computed
#define FAN_CST5_COUNT_S        16
#define FAN_CST5_RPM_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_IM register.
//
//*****************************************************************************
#define FAN_IM_C5INT1IM         0x00400000  // Channel 5 Interrupt 1 Mask
#define FAN_IM_C5INT0IM         0x00200000  // Channel 5 Interrupt 0 Mask
#define FAN_IM_C5STALLIM        0x00100000  // Channel 5 Stall Interrupt Mask
#define FAN_IM_C4INT1IM         0x00040000  // Channel 4 Interrupt 1 Mask
#define FAN_IM_C4INT0IM         0x00020000  // Channel 4 Interrupt 0 Mask
#define FAN_IM_C4STALLIM        0x00010000  // Channel 4 Stall Interrupt Mask
#define FAN_IM_C3INT1IM         0x00004000  // Channel 3 Interrupt 1 Mask
#define FAN_IM_C3INT0IM         0x00002000  // Channel 3 Interrupt 0 Mask
#define FAN_IM_C3STALLIM        0x00001000  // Channel 3 Stall Interrupt Mask
#define FAN_IM_C2INT1IM         0x00000400  // Channel 2 Interrupt 1 Mask
#define FAN_IM_C2INT0IM         0x00000200  // Channel 2 Interrupt 0 Mask
#define FAN_IM_C2STALLIM        0x00000100  // Channel 2 Stall Interrupt Mask
#define FAN_IM_C1INT1IM         0x00000040  // Channel 1 Interrupt 1 Mask
#define FAN_IM_C1INT0IM         0x00000020  // Channel 1 Interrupt 0 Mask
#define FAN_IM_C1STALLIM        0x00000010  // Channel 1 Stall Interrupt Mask
#define FAN_IM_C0INT1IM         0x00000004  // Channel 0 Interrupt 1 Mask
#define FAN_IM_C0INT0IM         0x00000002  // Channel 0 Interrupt 0 Mask
#define FAN_IM_C0STALLIM        0x00000001  // Channel 0 Stall Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_RIS register.
//
//*****************************************************************************
#define FAN_RIS_C5INT1RIS       0x00400000  // Channel 5 Raw Interrupt 1 Status
#define FAN_RIS_C5INT0RIS       0x00200000  // Channel 5 Raw Interrupt 0 Status
#define FAN_RIS_C5STALLRIS      0x00100000  // Channel 5 Raw Stall Interrupt
                                            // Status
#define FAN_RIS_C4INT1RIS       0x00040000  // Channel 4 Raw Interrupt 1 Status
#define FAN_RIS_C4INT0RIS       0x00020000  // Channel 4 Raw Interrupt 0 Status
#define FAN_RIS_C4STALLRIS      0x00010000  // Channel 4 Raw Stall Interrupt
                                            // Status
#define FAN_RIS_C3INT1RIS       0x00004000  // Channel 3 Raw Interrupt 1 Status
#define FAN_RIS_C3INT0RIS       0x00002000  // Channel 3 Raw Interrupt 0 Status
#define FAN_RIS_C3STALLRIS      0x00001000  // Channel 3 Stall Raw Interrupt
                                            // Status
#define FAN_RIS_C2INT1RIS       0x00000400  // Channel 2 Raw Interrupt 1 Status
#define FAN_RIS_C2INT0RIS       0x00000200  // Channel 2 Raw Interrupt 0 Status
#define FAN_RIS_C2STALLRIS      0x00000100  // Channel 2 Stall Raw Interrupt
                                            // Status
#define FAN_RIS_C1INT1RIS       0x00000040  // Channel 1 Raw Interrupt 1 Status
#define FAN_RIS_C1INT0RIS       0x00000020  // Channel 1 Raw Interrupt 0 Status
#define FAN_RIS_C1STALLRIS      0x00000010  // Channel 1 Stall Raw Interrupt
                                            // Status
#define FAN_RIS_C0INT1RIS       0x00000004  // Channel 0 Raw Interrupt 1 Status
#define FAN_RIS_C0INT0RIS       0x00000002  // Channel 0 Raw Interrupt 0 Status
#define FAN_RIS_C0STALLRIS      0x00000001  // Channel 0 Stall Raw Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_MIS register.
//
//*****************************************************************************
#define FAN_MIS_C5INT1MIS       0x00400000  // Channel 5 Masked Interrupt 1
                                            // Status
#define FAN_MIS_C5INT0MIS       0x00200000  // Channel 5 Masked Interrupt 0
                                            // Status
#define FAN_MIS_C5STALLMIS      0x00100000  // Channel 5 Masked Stall Interrupt
                                            // Status
#define FAN_MIS_C4NT1MIS        0x00040000  // Channel 4 Masked Interrupt 1
                                            // Status
#define FAN_MIS_C4INT0MIS       0x00020000  // Channel 4 Masked Interrupt 0
                                            // Status
#define FAN_MIS_C4STALLMIS      0x00010000  // Channel 4 Masked Stall Interrupt
                                            // Status
#define FAN_MIS_C3INT1MIS       0x00004000  // Channel 3 Masked Interrupt 1
                                            // Status
#define FAN_MIS_C3INT0MIS       0x00002000  // Channel 3 Masked Interrupt 0
                                            // Status
#define FAN_MIS_C3STALLMIS      0x00001000  // Channel 3 Stall Masked Interrupt
                                            // Status
#define FAN_MIS_C2INT1MIS       0x00000400  // Channel 2 Masked Interrupt 1
                                            // Status
#define FAN_MIS_C2INT0MIS       0x00000200  // Channel 2 Masked Interrupt 0
                                            // Status
#define FAN_MIS_C2STALLMIS      0x00000100  // Channel 2 Stall Masked Interrupt
                                            // Status
#define FAN_MIS_C1INT1MIS       0x00000040  // Channel 1 Masked Interrupt 1
                                            // Status
#define FAN_MIS_C1INT0MIS       0x00000020  // Channel 1 Masked Interrupt 0
                                            // Status
#define FAN_MIS_C1STALLMIS      0x00000010  // Channel 1 Stall Masked Interrupt
                                            // Status
#define FAN_MIS_C0INT1MIS       0x00000004  // Channel 0 Masked Interrupt 1
                                            // Status
#define FAN_MIS_C0INT0MIS       0x00000002  // Channel 0 Masked Interrupt 0
                                            // Status
#define FAN_MIS_C0STALLMIS      0x00000001  // Channel 0 Stall Masked Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_IC register.
//
//*****************************************************************************
#define FAN_IC_C5INT1IC         0x00400000  // Channel 5 Interrupt 1 Clear
#define FAN_IC_C5INT0IC         0x00200000  // Channel 5 Interrupt 0 Clear
#define FAN_IC_C5STALLIC        0x00100000  // Channel 5 Stall Interrupt Clear
#define FAN_IC_C4INT1IC         0x00040000  // Channel 4 Interrupt 1 Clear
#define FAN_IC_C4INT0IC         0x00020000  // Channel 4 Interrupt 0 Clear
#define FAN_IC_C4STALLIC        0x00010000  // Channel 4 Stall Interrupt Clear
#define FAN_IC_C3INT1IC         0x00004000  // Channel 3 Interrupt 1 Clear
#define FAN_IC_C53INT0IC        0x00002000  // Channel 3 Interrupt 0 Clear
#define FAN_IC_C3STALLIC        0x00001000  // Channel 3 Stall Interrupt Clear
#define FAN_IC_C2INT1IC         0x00000400  // Channel 2 Interrupt 1 Clear
#define FAN_IC_C2INT0IC         0x00000200  // Channel 2 Interrupt 0 Clear
#define FAN_IC_C2STALLIC        0x00000100  // Channel 2 Stall Interrupt Clear
#define FAN_IC_C1INT1IC         0x00000040  // Channel 1 Interrupt 1 Clear
#define FAN_IC_C1INT0IC         0x00000020  // Channel 1 Interrupt 0 Clear
#define FAN_IC_C1STALLIC        0x00000010  // Channel 1 Stall Interrupt Clear
#define FAN_IC_C0INT1IC         0x00000004  // Channel 0 Interrupt 1 Clear
#define FAN_IC_C0INT0IC         0x00000002  // Channel 0 Interrupt 0 Clear
#define FAN_IC_C0STALLIC        0x00000001  // Channel 0 Stall Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the FAN_O_PP register.
//
//*****************************************************************************
#define FAN_PP_CHAN_M           0x0000000F  // Channel Count
#define FAN_PP_CHAN_S           0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EESIZE register.
//
//*****************************************************************************
#define EEPROM_EESIZE_BLKCNT_M  0x07FF0000  // Number of 16-Word Blocks
#define EEPROM_EESIZE_WORDCNT_M 0x0000FFFF  // Number of 32-Bit Words
#define EEPROM_EESIZE_BLKCNT_S  16
#define EEPROM_EESIZE_WORDCNT_S 0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEBLOCK register.
//
//*****************************************************************************
#define EEPROM_EEBLOCK_BLOCK_M  0x0000FFFF  // Current Block
#define EEPROM_EEBLOCK_BLOCK_S  0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEOFFSET
// register.
//
//*****************************************************************************
#define EEPROM_EEOFFSET_OFFSET_M \
                                0x0000000F  // Current Address Offset
#define EEPROM_EEOFFSET_OFFSET_S \
                                0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EERDWR register.
//
//*****************************************************************************
#define EEPROM_EERDWR_VALUE_M   0xFFFFFFFF  // EEPROM Read or Write Data
#define EEPROM_EERDWR_VALUE_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EERDWRINC
// register.
//
//*****************************************************************************
#define EEPROM_EERDWRINC_VALUE_M \
                                0xFFFFFFFF  // EEPROM Read or Write Data with
                                            // Increment
#define EEPROM_EERDWRINC_VALUE_S \
                                0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEDONE register.
//
//*****************************************************************************
#define EEPROM_EEDONE_INVPL     0x00000100  // Invalid Program Voltage Level
#define EEPROM_EEDONE_WRBUSY    0x00000020  // Write Busy
#define EEPROM_EEDONE_NOPERM    0x00000010  // Write Without Permission
#define EEPROM_EEDONE_WKCOPY    0x00000008  // Working on a Copy
#define EEPROM_EEDONE_WKERASE   0x00000004  // Working on an Erase
#define EEPROM_EEDONE_WORKING   0x00000001  // EEPROM Working

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EESUPP register.
//
//*****************************************************************************
#define EEPROM_EESUPP_PRETRY    0x00000008  // Programming Must Be Retried
#define EEPROM_EESUPP_ERETRY    0x00000004  // Erase Must Be Retried
#define EEPROM_EESUPP_EREQ      0x00000002  // Erase Required
#define EEPROM_EESUPP_START     0x00000001  // Start Erase

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEUNLOCK
// register.
//
//*****************************************************************************
#define EEPROM_EEUNLOCK_UNLOCK_M \
                                0xFFFFFFFF  // EEPROM Unlock

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEPROT register.
//
//*****************************************************************************
#define EEPROM_EEPROT_ACC       0x00000008  // Access Control
#define EEPROM_EEPROT_PROT_M    0x00000007  // Protection Control
#define EEPROM_EEPROT_PROT_RWNPW \
                                0x00000000  // This setting is the default. If
                                            // there is no password, the block
                                            // is not protected and is readable
                                            // and writable
#define EEPROM_EEPROT_PROT_RWPW 0x00000001  // If there is a password, the
                                            // block is readable or writable
                                            // only when unlocked
#define EEPROM_EEPROT_PROT_RONPW \
                                0x00000002  // If there is no password, the
                                            // block is readable, not writable

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEPASS0 register.
//
//*****************************************************************************
#define EEPROM_EEPASS0_PASS_M   0xFFFFFFFF  // Password
#define EEPROM_EEPASS0_PASS_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEPASS1 register.
//
//*****************************************************************************
#define EEPROM_EEPASS1_PASS_M   0xFFFFFFFF  // Password
#define EEPROM_EEPASS1_PASS_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEPASS2 register.
//
//*****************************************************************************
#define EEPROM_EEPASS2_PASS_M   0xFFFFFFFF  // Password
#define EEPROM_EEPASS2_PASS_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEINT register.
//
//*****************************************************************************
#define EEPROM_EEINT_INT        0x00000001  // Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEHIDE register.
//
//*****************************************************************************
#define EEPROM_EEHIDE_HN_M      0xFFFFFFFE  // Hide Block

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_EEDBGME register.
//
//*****************************************************************************
#define EEPROM_EEDBGME_KEY_M    0xFFFF0000  // Erase Key
#define EEPROM_EEDBGME_ME       0x00000001  // Mass Erase
#define EEPROM_EEDBGME_KEY_S    16

//*****************************************************************************
//
// The following are defines for the bit fields in the EEPROM_PP register.
//
//*****************************************************************************
#define EEPROM_PP_SIZE_M        0x0000001F  // EEPROM Size
#define EEPROM_PP_SIZE_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_CTL register.
//
//*****************************************************************************
#define PECI_CTL_OFFSET_M       0xFFFF0000  // Offset Value
#define PECI_CTL_CRETRY_M       0x00007000  // Number of Controller Retries
#define PECI_CTL_BYERR          0x00000800  // Timing Negotiation Error Bypass
#define PECI_CTL_RXINV          0x00000400  // Invert PECI RX Input
#define PECI_CTL_M1D1EN         0x00000008  // Enable Microprocessor 1 and
                                            // Domain 1
#define PECI_CTL_M1D0EN         0x00000004  // Enable Microprocessor 1 and
                                            // Domain 0
#define PECI_CTL_M0D1EN         0x00000002  // Enable Microprocessor 0 and
                                            // Domain 1
#define PECI_CTL_M0D0EN         0x00000001  // Enable Microprocessor 0 and
                                            // Domain 0
#define PECI_CTL_OFFSET_S       16
#define PECI_CTL_CRETRY_S       12

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_DIV register.
//
//*****************************************************************************
#define PECI_DIV_BAUD_M         0xFFFF0000  // Baud Rate
#define PECI_DIV_POLL_M         0x0000FFFF  // Counter for Inter-Poll Delay
#define PECI_DIV_BAUD_S         16
#define PECI_DIV_POLL_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M0D0C register.
//
//*****************************************************************************
#define PECI_M0D0C_HITHR_M      0xFFFF0000  // High Threshold
#define PECI_M0D0C_LOTHR_M      0x0000FFFF  // Low Threshold
#define PECI_M0D0C_HITHR_S      16
#define PECI_M0D0C_LOTHR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M0D1C register.
//
//*****************************************************************************
#define PECI_M0D1C_HITHR_M      0xFFFF0000  // High Threshold
#define PECI_M0D1C_LOTHR_M      0x0000FFFF  // Low Threshold
#define PECI_M0D1C_HITHR_S      16
#define PECI_M0D1C_LOTHR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M1D0C register.
//
//*****************************************************************************
#define PECI_M1D0C_HITHR_M      0xFFFF0000  // High Threshold
#define PECI_M1D0C_LOTHR_M      0x0000FFFF  // Low Threshold
#define PECI_M1D0C_HITHR_S      16
#define PECI_M1D0C_LOTHR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M1D1C register.
//
//*****************************************************************************
#define PECI_M1D1C_HITHR_M      0xFFFF0000  // High Threshold
#define PECI_M1D1C_LOTHR_M      0x0000FFFF  // Low Threshold
#define PECI_M1D1C_HITHR_S      16
#define PECI_M1D1C_LOTHR_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M0D0 register.
//
//*****************************************************************************
#define PECI_M0D0_MAXREAD_M     0xFFFF0000  // Highest Value Read
#define PECI_M0D0_VALUE_M       0x0000FFFF  // Last Value Read
#define PECI_M0D0_VALUE_MCRCTX  0x000081F9  // Microprocessor abort CRC error
                                            // on transmitted data
#define PECI_M0D0_VALUE_CRCRX   0x000081FA  // CRC error on received data
#define PECI_M0D0_VALUE_CRCTX   0x000081FB  // CRC error on transmitted data
#define PECI_M0D0_VALUE_NEG     0x000081FC  // Negotiation error
#define PECI_M0D0_VALUE_TO      0x000081FD  // Timeout error
#define PECI_M0D0_MAXREAD_S     16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M0D1 register.
//
//*****************************************************************************
#define PECI_M0D1_MAXREAD_M     0xFFFF0000  // Highest Value Read
#define PECI_M0D1_VALUE_M       0x0000FFFF  // Last Value Read
#define PECI_M0D1_VALUE_MCRCTX  0x000081F9  // Microprocessor abort CRC error
                                            // on transmitted data
#define PECI_M0D1_VALUE_CRCRX   0x000081FA  // CRC error on received data
#define PECI_M0D1_VALUE_CRCTX   0x000081FB  // CRC error on transmitted data
#define PECI_M0D1_VALUE_NEG     0x000081FC  // Negotiation error
#define PECI_M0D1_VALUE_TO      0x000081FD  // Timeout error
#define PECI_M0D1_MAXREAD_S     16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M1D0 register.
//
//*****************************************************************************
#define PECI_M1D0_MAXREAD_M     0xFFFF0000  // Highest Value Read
#define PECI_M1D0_VALUE_M       0x0000FFFF  // Last Value Read
#define PECI_M1D0_VALUE_MCRCTX  0x000081F9  // Microprocessor abort CRC error
                                            // on transmitted data
#define PECI_M1D0_VALUE_CRCRX   0x000081FA  // CRC error on received data
#define PECI_M1D0_VALUE_CRCTX   0x000081FB  // CRC error on transmitted data
#define PECI_M1D0_VALUE_NEG     0x000081FC  // Negotiation error
#define PECI_M1D0_VALUE_TO      0x000081FD  // Timeout error
#define PECI_M1D0_MAXREAD_S     16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_M1D1 register.
//
//*****************************************************************************
#define PECI_M1D1_MAXREAD_M     0xFFFF0000  // Highest Value Read
#define PECI_M1D1_VALUE_M       0x0000FFFF  // Last Value Read
#define PECI_M1D1_VALUE_MCRCTX  0x000081F9  // Microprocessor abort CRC error
                                            // on transmitted data
#define PECI_M1D1_VALUE_CRCRX   0x000081FA  // CRC error on received data
#define PECI_M1D1_VALUE_CRCTX   0x000081FB  // CRC error on transmitted data
#define PECI_M1D1_VALUE_NEG     0x000081FC  // Negotiation error
#define PECI_M1D1_VALUE_TO      0x000081FD  // Timeout error
#define PECI_M1D1_MAXREAD_S     16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_IM register.
//
//*****************************************************************************
#define PECI_IM_M1D1IM_M        0x00C00000  // Microprocessor 1 and Domain 1
                                            // Interrupt Mask
#define PECI_IM_M1D1IM_HIGH     0x00400000  // Interrupt when crossing above
                                            // high threshold
#define PECI_IM_M1D1IM_CROSSUP  0x00800000  // Interrupt when crossing above
                                            // either threshold
#define PECI_IM_M1D1IM_CROSSANY 0x00C00000  // Interrupt when crossing either
                                            // threshold in either direction
#define PECI_IM_M1D0IM_M        0x00300000  // Microprocessor 1 and Domain 0
                                            // Interrupt Mask
#define PECI_IM_M0D1IM_M        0x000C0000  // Microprocessor 0 and Domain 1
                                            // Interrupt Mask
#define PECI_IM_M0D0IM_M        0x00030000  // Microprocessor 0 and Domain 0
                                            // Interrupt Mask
#define PECI_IM_ACIM            0x00000004  // Advanced Command Interrupt Mask
#define PECI_IM_ERRIM           0x00000002  // Polling Error Detected Interrupt
                                            // Mask
#define PECI_IM_POLLIM          0x00000001  // Poll Completed Interrupt Mask
#define PECI_IM_M1D0IM_S        20
#define PECI_IM_M0D1IM_S        18
#define PECI_IM_M0D0IM_S        16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_RIS register.
//
//*****************************************************************************
#define PECI_RIS_M1D1RIS_M      0x00C00000  // Microprocessor 1 and Domain 1
                                            // Raw Interrupt Status
#define PECI_RIS_M1D0RIS_M      0x00300000  // Microprocessor 1 and Domain 0
                                            // Raw Interrupt Status
#define PECI_RIS_M0D1RIS_M      0x000C0000  // Microprocessor 0 and Domain 1
                                            // Raw Interrupt Status
#define PECI_RIS_M0D0RIS_M      0x00030000  // Microprocessor 0 and Domain 0
                                            // Raw Interrupt Status
#define PECI_RIS_ACRIS          0x00000004  // Advanced Command Raw Interrupt
                                            // Status
#define PECI_RIS_ERRRIS         0x00000002  // Polling Error Detected Raw
                                            // Interrupt Status
#define PECI_RIS_POLLRIS        0x00000001  // Poll Completed Raw Interrupt
                                            // Status
#define PECI_RIS_M1D1RIS_S      22
#define PECI_RIS_M1D0RIS_S      20
#define PECI_RIS_M0D1RIS_S      18
#define PECI_RIS_M0D0RIS_S      16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_MIS register.
//
//*****************************************************************************
#define PECI_MIS_M1D1MIS_M      0x00C00000  // Microprocessor 1 and Domain 1
                                            // Masked Interrupt Status
#define PECI_MIS_M1D0MIS_M      0x00300000  // Microprocessor 1 and Domain 0
                                            // Masked Interrupt Status
#define PECI_MIS_M0D1MIS_M      0x000C0000  // Microprocessor 0 and Domain 1
                                            // Masked Interrupt Status
#define PECI_MIS_M0D0MIS_M      0x00030000  // Microprocessor 0 and Domain 0
                                            // Masked Interrupt Status
#define PECI_MIS_ACMIS          0x00000004  // Advanced Command Masked
                                            // Interrupt Status
#define PECI_MIS_ERRMIS         0x00000002  // Polling Error Detected Masked
                                            // Interrupt Status
#define PECI_MIS_POLLMIS        0x00000001  // Poll Completed Masked Interrupt
                                            // Status
#define PECI_MIS_M1D1MIS_S      22
#define PECI_MIS_M1D0MIS_S      20
#define PECI_MIS_M0D1MIS_S      18
#define PECI_MIS_M0D0MIS_S      16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_IC register.
//
//*****************************************************************************
#define PECI_IC_M1D1IC_M        0x00C00000  // Microprocessor 1 and Domain 1
                                            // Interrupt Clear
#define PECI_IC_M1D0IC_M        0x00300000  // Microprocessor 1 and Domain 0
                                            // Interrupt Clear
#define PECI_IC_M0D1IC_M        0x000C0000  // Microprocessor 0 and Domain 1
                                            // Interrupt Clear
#define PECI_IC_M0D0IC_M        0x00030000  // Microprocessor 0 and Domain 0
                                            // Interrupt Clear
#define PECI_IC_ACIC            0x00000004  // Advanced Command Interrupt Clear
#define PECI_IC_ERRIC           0x00000002  // Polling Error Detected Interrupt
                                            // Clear
#define PECI_IC_POLLIC          0x00000001  // Poll Completed Interrupt Clear
#define PECI_IC_M1D1IC_S        22
#define PECI_IC_M1D0IC_S        20
#define PECI_IC_M0D1IC_S        18
#define PECI_IC_M0D0IC_S        16

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_ACADDR register.
//
//*****************************************************************************
#define PECI_ACADDR_HIDRE_M     0xFF000000  // HostID and Retry
#define PECI_ACADDR_SIZE_M      0x00FF0000  // Data Size
#define PECI_ACADDR_DOMAIN_M    0x0000FF00  // Domain Select
#define PECI_ACADDR_PROCADD_M   0x000000FF  // Processor Address
#define PECI_ACADDR_HIDRE_S     24
#define PECI_ACADDR_SIZE_S      16
#define PECI_ACADDR_DOMAIN_S    8
#define PECI_ACADDR_PROCADD_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_ACARG register.
//
//*****************************************************************************
#define PECI_ACARG_ARG3_M       0xFF000000  // Argument 3
#define PECI_ACARG_ARG2_M       0x00FF0000  // Argument 2
#define PECI_ACARG_ARG1_M       0x0000FF00  // Argument 1
#define PECI_ACARG_ARG0_M       0x000000FF  // Argument 0
#define PECI_ACARG_ARG3_S       24
#define PECI_ACARG_ARG2_S       16
#define PECI_ACARG_ARG1_S       8
#define PECI_ACARG_ARG0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_ACRDWR0 register.
//
//*****************************************************************************
#define PECI_ACRDWR0_D3_M       0xFF000000  // Data 3
#define PECI_ACRDWR0_D2_M       0x00FF0000  // Data 2
#define PECI_ACRDWR0_D1_M       0x0000FF00  // Data 1
#define PECI_ACRDWR0_D0_M       0x000000FF  // Data 0
#define PECI_ACRDWR0_D3_S       24
#define PECI_ACRDWR0_D2_S       16
#define PECI_ACRDWR0_D1_S       8
#define PECI_ACRDWR0_D0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_ACRDWR1 register.
//
//*****************************************************************************
#define PECI_ACRDWR1_D3_M       0xFF000000  // Data 3
#define PECI_ACRDWR1_D2_M       0x00FF0000  // Data 2
#define PECI_ACRDWR1_D1_M       0x0000FF00  // Data 1
#define PECI_ACRDWR1_D0_M       0x000000FF  // Data 0
#define PECI_ACRDWR1_D3_S       24
#define PECI_ACRDWR1_D2_S       16
#define PECI_ACRDWR1_D1_S       8
#define PECI_ACRDWR1_D0_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_ACCMD register.
//
//*****************************************************************************
#define PECI_ACCMD_CMD_M        0x000000FF  // Base Command
#define PECI_ACCMD_CMD_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the PECI_O_ACCODE register.
//
//*****************************************************************************
#define PECI_ACCODE_CODE_M      0xFFFFFFFF  // Signed Completion Code

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSEXC_RIS register.
//
//*****************************************************************************
#define SYSEXC_RIS_FPIXCRIS     0x00000020  // Floating-Point Inexact Exception
                                            // Raw Interrupt Status
#define SYSEXC_RIS_FPOFCRIS     0x00000010  // Floating-Point Overflow
                                            // Exception Raw Interrupt Status
#define SYSEXC_RIS_FPUFCRIS     0x00000008  // Floating-Point Underflow
                                            // Exception Raw Interrupt Status
#define SYSEXC_RIS_FPIOCRIS     0x00000004  // Floating-Point Invalid Operation
                                            // Raw Interrupt Status
#define SYSEXC_RIS_FPDZCRIS     0x00000002  // Floating-Point Divide By 0
                                            // Exception Raw Interrupt Status
#define SYSEXC_RIS_FPIDCRIS     0x00000001  // Floating-Point Input Denormal
                                            // Exception Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSEXC_IM register.
//
//*****************************************************************************
#define SYSEXC_IM_FPIXCIM       0x00000020  // Floating-Point Inexact Exception
                                            // Interrupt Mask
#define SYSEXC_IM_FPOFCIM       0x00000010  // Floating-Point Overflow
                                            // Exception Interrupt Mask
#define SYSEXC_IM_FPUFCIM       0x00000008  // Floating-Point Underflow
                                            // Exception Interrupt Mask
#define SYSEXC_IM_FPIOCIM       0x00000004  // Floating-Point Invalid Operation
                                            // Interrupt Mask
#define SYSEXC_IM_FPDZCIM       0x00000002  // Floating-Point Divide By 0
                                            // Exception Interrupt Mask
#define SYSEXC_IM_FPIDCIM       0x00000001  // Floating-Point Input Denormal
                                            // Exception Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSEXC_MIS register.
//
//*****************************************************************************
#define SYSEXC_MIS_FPIXCMIS     0x00000020  // Floating-Point Inexact Exception
                                            // Masked Interrupt Status
#define SYSEXC_MIS_FPOFCMIS     0x00000010  // Floating-Point Overflow
                                            // Exception Masked Interrupt
                                            // Status
#define SYSEXC_MIS_FPUFCMIS     0x00000008  // Floating-Point Underflow
                                            // Exception Masked Interrupt
                                            // Status
#define SYSEXC_MIS_FPIOCMIS     0x00000004  // Floating-Point Invalid Operation
                                            // Masked Interrupt Status
#define SYSEXC_MIS_FPDZCMIS     0x00000002  // Floating-Point Divide By 0
                                            // Exception Masked Interrupt
                                            // Status
#define SYSEXC_MIS_FPIDCMIS     0x00000001  // Floating-Point Input Denormal
                                            // Exception Masked Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSEXC_IC register.
//
//*****************************************************************************
#define SYSEXC_IC_FPIXCIC       0x00000020  // Floating-Point Inexact Exception
                                            // Interrupt Clear
#define SYSEXC_IC_FPOFCIC       0x00000010  // Floating-Point Overflow
                                            // Exception Interrupt Clear
#define SYSEXC_IC_FPUFCIC       0x00000008  // Floating-Point Underflow
                                            // Exception Interrupt Clear
#define SYSEXC_IC_FPIOCIC       0x00000004  // Floating-Point Invalid Operation
                                            // Interrupt Clear
#define SYSEXC_IC_FPDZCIC       0x00000002  // Floating-Point Divide By 0
                                            // Exception Interrupt Clear
#define SYSEXC_IC_FPIDCIC       0x00000001  // Floating-Point Input Denormal
                                            // Exception Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_RTCC register.
//
//*****************************************************************************
#define HIB_RTCC_M              0xFFFFFFFF  // RTC Counter
#define HIB_RTCC_S              0

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_RTCM0 register.
//
//*****************************************************************************
#define HIB_RTCM0_M             0xFFFFFFFF  // RTC Match 0
#define HIB_RTCM0_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_RTCLD register.
//
//*****************************************************************************
#define HIB_RTCLD_M             0xFFFFFFFF  // RTC Load
#define HIB_RTCLD_S             0

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_CTL register.
//
//*****************************************************************************
#define HIB_CTL_WRC             0x80000000  // Write Complete/Capable
#define HIB_CTL_OSCDRV          0x00020000  // Oscillator Drive Capability
#define HIB_CTL_OSCBYP          0x00010000  // Oscillator Bypass
#define HIB_CTL_VBATSEL_M       0x00006000  // Select for Low-Battery
                                            // Comparator
#define HIB_CTL_VBATSEL_1_9V    0x00000000  // 1.9 Volts
#define HIB_CTL_VBATSEL_2_1V    0x00002000  // 2.1 Volts (default)
#define HIB_CTL_VBATSEL_2_3V    0x00004000  // 2.3 Volts
#define HIB_CTL_VBATSEL_2_5V    0x00006000  // 2.5 Volts
#define HIB_CTL_BATCHK          0x00000400  // Check Battery Status
#define HIB_CTL_BATWKEN         0x00000200  // Wake on Low Battery
#define HIB_CTL_VDD3ON          0x00000100  // VDD Powered
#define HIB_CTL_VABORT          0x00000080  // Power Cut Abort Enable
#define HIB_CTL_CLK32EN         0x00000040  // Clocking Enable
#define HIB_CTL_PINWEN          0x00000010  // External WAKE Pin Enable
#define HIB_CTL_RTCWEN          0x00000008  // RTC Wake-up Enable
#define HIB_CTL_HIBREQ          0x00000002  // Hibernation Request
#define HIB_CTL_RTCEN           0x00000001  // RTC Timer Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_IM register.
//
//*****************************************************************************
#define HIB_IM_WC               0x00000010  // External Write Complete/Capable
                                            // Interrupt Mask
#define HIB_IM_EXTW             0x00000008  // External Wake-Up Interrupt Mask
#define HIB_IM_LOWBAT           0x00000004  // Low Battery Voltage Interrupt
                                            // Mask
#define HIB_IM_RTCALT0          0x00000001  // RTC Alert 0 Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_RIS register.
//
//*****************************************************************************
#define HIB_RIS_WC              0x00000010  // Write Complete/Capable Raw
                                            // Interrupt Status
#define HIB_RIS_EXTW            0x00000008  // External Wake-Up Raw Interrupt
                                            // Status
#define HIB_RIS_LOWBAT          0x00000004  // Low Battery Voltage Raw
                                            // Interrupt Status
#define HIB_RIS_RTCALT0         0x00000001  // RTC Alert 0 Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_MIS register.
//
//*****************************************************************************
#define HIB_MIS_WC              0x00000010  // Write Complete/Capable Masked
                                            // Interrupt Status
#define HIB_MIS_EXTW            0x00000008  // External Wake-Up Masked
                                            // Interrupt Status
#define HIB_MIS_LOWBAT          0x00000004  // Low Battery Voltage Masked
                                            // Interrupt Status
#define HIB_MIS_RTCALT0         0x00000001  // RTC Alert 0 Masked Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_IC register.
//
//*****************************************************************************
#define HIB_IC_WC               0x00000010  // Write Complete/Capable Masked
                                            // Interrupt Clear
#define HIB_IC_EXTW             0x00000008  // External Wake-Up Masked
                                            // Interrupt Clear
#define HIB_IC_LOWBAT           0x00000004  // Low Battery Voltage Masked
                                            // Interrupt Clear
#define HIB_IC_RTCALT0          0x00000001  // RTC Alert0 Masked Interrupt
                                            // Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_RTCT register.
//
//*****************************************************************************
#define HIB_RTCT_TRIM_M         0x0000FFFF  // RTC Trim Value
#define HIB_RTCT_TRIM_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_RTCSS register.
//
//*****************************************************************************
#define HIB_RTCSS_RTCSSM_M      0x7FFF0000  // RTC Sub Seconds Match
#define HIB_RTCSS_RTCSSC_M      0x00007FFF  // RTC Sub Seconds Count
#define HIB_RTCSS_RTCSSM_S      16
#define HIB_RTCSS_RTCSSC_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the HIB_DATA register.
//
//*****************************************************************************
#define HIB_DATA_RTD_M          0xFFFFFFFF  // Hibernation Module NV Data
#define HIB_DATA_RTD_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FMA register.
//
//*****************************************************************************
#define FLASH_FMA_OFFSET_M      0x0003FFFF  // Address Offset
#define FLASH_FMA_OFFSET_S      0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FMD register.
//
//*****************************************************************************
#define FLASH_FMD_DATA_M        0xFFFFFFFF  // Data Value
#define FLASH_FMD_DATA_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FMC register.
//
//*****************************************************************************
#define FLASH_FMC_WRKEY         0xA4420000  // FLASH write key
#define FLASH_FMC_COMT          0x00000008  // Commit Register Value
#define FLASH_FMC_MERASE        0x00000004  // Mass Erase Flash Memory
#define FLASH_FMC_ERASE         0x00000002  // Erase a Page of Flash Memory
#define FLASH_FMC_WRITE         0x00000001  // Write a Word into Flash Memory

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FCRIS register.
//
//*****************************************************************************
#define FLASH_FCRIS_PROGRIS     0x00002000  // PROGVER Raw Interrupt Status
#define FLASH_FCRIS_ERRIS       0x00000800  // ERVER Raw Interrupt Status
#define FLASH_FCRIS_INVDRIS     0x00000400  // Invalid Data Raw Interrupt
                                            // Status
#define FLASH_FCRIS_VOLTRIS     0x00000200  // VOLTSTAT Raw Interrupt Status
#define FLASH_FCRIS_ERIS        0x00000004  // EEPROM Raw Interrupt Status
#define FLASH_FCRIS_PRIS        0x00000002  // Programming Raw Interrupt Status
#define FLASH_FCRIS_ARIS        0x00000001  // Access Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FCIM register.
//
//*****************************************************************************
#define FLASH_FCIM_PROGMASK     0x00002000  // PROGVER Interrupt Mask
#define FLASH_FCIM_ERMASK       0x00000800  // ERVER Interrupt Mask
#define FLASH_FCIM_INVDMASK     0x00000400  // Invalid Data Interrupt Mask
#define FLASH_FCIM_VOLTMASK     0x00000200  // VOLT Interrupt Mask
#define FLASH_FCIM_EMASK        0x00000004  // EEPROM Interrupt Mask
#define FLASH_FCIM_PMASK        0x00000002  // Programming Interrupt Mask
#define FLASH_FCIM_AMASK        0x00000001  // Access Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FCMISC register.
//
//*****************************************************************************
#define FLASH_FCMISC_PROGMISC   0x00002000  // PROGVER Masked Interrupt Status
                                            // and Clear
#define FLASH_FCMISC_ERMISC     0x00000800  // ERVER Masked Interrupt Status
                                            // and Clear
#define FLASH_FCMISC_INVDMISC   0x00000400  // Invalid Data Masked Interrupt
                                            // Status and Clear
#define FLASH_FCMISC_VOLTMISC   0x00000200  // VOLT Masked Interrupt Status and
                                            // Clear
#define FLASH_FCMISC_EMISC      0x00000004  // EEPROM Masked Interrupt Status
                                            // and Clear
#define FLASH_FCMISC_PMISC      0x00000002  // Programming Masked Interrupt
                                            // Status and Clear
#define FLASH_FCMISC_AMISC      0x00000001  // Access Masked Interrupt Status
                                            // and Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FMC2 register.
//
//*****************************************************************************
#define FLASH_FMC2_WRBUF        0x00000001  // Buffered Flash Memory Write

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FWBVAL register.
//
//*****************************************************************************
#define FLASH_FWBVAL_FWB_M      0xFFFFFFFF  // Flash Memory Write Buffer

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FWBN register.
//
//*****************************************************************************
#define FLASH_FWBN_DATA_M       0xFFFFFFFF  // Data

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_FSIZE register.
//
//*****************************************************************************
#define FLASH_FSIZE_SIZE_M      0x0000FFFF  // Flash Size
#define FLASH_FSIZE_SIZE_8KB    0x00000003  // 8 KB of Flash
#define FLASH_FSIZE_SIZE_16KB   0x00000007  // 16 KB of Flash
#define FLASH_FSIZE_SIZE_32KB   0x0000000F  // 32 KB of Flash
#define FLASH_FSIZE_SIZE_64KB   0x0000001F  // 64 KB of Flash
#define FLASH_FSIZE_SIZE_96KB   0x0000002F  // 96 KB of Flash
#define FLASH_FSIZE_SIZE_128KB  0x0000003F  // 128 KB of Flash
#define FLASH_FSIZE_SIZE_192KB  0x0000005F  // 192 KB of Flash
#define FLASH_FSIZE_SIZE_256KB  0x0000007F  // 256 KB of Flash

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_SSIZE register.
//
//*****************************************************************************
#define FLASH_SSIZE_SIZE_M      0x0000FFFF  // SRAM Size
#define FLASH_SSIZE_SIZE_2KB    0x00000007  // 2 KB of SRAM
#define FLASH_SSIZE_SIZE_4KB    0x0000000F  // 4 KB of SRAM
#define FLASH_SSIZE_SIZE_6KB    0x00000017  // 6 KB of SRAM
#define FLASH_SSIZE_SIZE_8KB    0x0000001F  // 8 KB of SRAM
#define FLASH_SSIZE_SIZE_12KB   0x0000002F  // 12 KB of SRAM
#define FLASH_SSIZE_SIZE_16KB   0x0000003F  // 16 KB of SRAM
#define FLASH_SSIZE_SIZE_20KB   0x0000004F  // 20 KB of SRAM
#define FLASH_SSIZE_SIZE_24KB   0x0000005F  // 24 KB of SRAM
#define FLASH_SSIZE_SIZE_32KB   0x0000007F  // 32 KB of SRAM

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_ROMSWMAP register.
//
//*****************************************************************************
#define FLASH_ROMSWMAP_SAFERTOS 0x00000001  // SafeRTOS Present

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_RMCTL register.
//
//*****************************************************************************
#define FLASH_RMCTL_BA          0x00000001  // Boot Alias

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_BOOTCFG register.
//
//*****************************************************************************
#define FLASH_BOOTCFG_NW        0x80000000  // Not Written
#define FLASH_BOOTCFG_PORT_M    0x0000E000  // Boot GPIO Port
#define FLASH_BOOTCFG_PORT_A    0x00000000  // Port A
#define FLASH_BOOTCFG_PORT_B    0x00002000  // Port B
#define FLASH_BOOTCFG_PORT_C    0x00004000  // Port C
#define FLASH_BOOTCFG_PORT_D    0x00006000  // Port D
#define FLASH_BOOTCFG_PORT_E    0x00008000  // Port E
#define FLASH_BOOTCFG_PORT_F    0x0000A000  // Port F
#define FLASH_BOOTCFG_PORT_G    0x0000C000  // Port G
#define FLASH_BOOTCFG_PORT_H    0x0000E000  // Port H
#define FLASH_BOOTCFG_PIN_M     0x00001C00  // Boot GPIO Pin
#define FLASH_BOOTCFG_PIN_0     0x00000000  // Pin 0
#define FLASH_BOOTCFG_PIN_1     0x00000400  // Pin 1
#define FLASH_BOOTCFG_PIN_2     0x00000800  // Pin 2
#define FLASH_BOOTCFG_PIN_3     0x00000C00  // Pin 3
#define FLASH_BOOTCFG_PIN_4     0x00001000  // Pin 4
#define FLASH_BOOTCFG_PIN_5     0x00001400  // Pin 5
#define FLASH_BOOTCFG_PIN_6     0x00001800  // Pin 6
#define FLASH_BOOTCFG_PIN_7     0x00001C00  // Pin 7
#define FLASH_BOOTCFG_POL       0x00000200  // Boot GPIO Polarity
#define FLASH_BOOTCFG_EN        0x00000100  // Boot GPIO Enable
#define FLASH_BOOTCFG_DBG1      0x00000002  // Debug Control 1
#define FLASH_BOOTCFG_DBG0      0x00000001  // Debug Control 0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_USERREG0 register.
//
//*****************************************************************************
#define FLASH_USERREG0_DATA_M   0xFFFFFFFF  // User Data
#define FLASH_USERREG0_DATA_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_USERREG1 register.
//
//*****************************************************************************
#define FLASH_USERREG1_DATA_M   0xFFFFFFFF  // User Data
#define FLASH_USERREG1_DATA_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_USERREG2 register.
//
//*****************************************************************************
#define FLASH_USERREG2_DATA_M   0xFFFFFFFF  // User Data
#define FLASH_USERREG2_DATA_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the FLASH_USERREG3 register.
//
//*****************************************************************************
#define FLASH_USERREG3_DATA_M   0xFFFFFFFF  // User Data
#define FLASH_USERREG3_DATA_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DID0 register.
//
//*****************************************************************************
#define SYSCTL_DID0_VER_M       0x70000000  // DID0 Version
#define SYSCTL_DID0_VER_1       0x10000000  // Second version of the DID0
                                            // register format
#define SYSCTL_DID0_CLASS_M     0x00FF0000  // Device Class
#define SYSCTL_DID0_CLASS_BLIZZARD \
                                0x00050000  // Stellaris(R) Blizzard-class
                                            // microcontrollers
#define SYSCTL_DID0_MAJ_M       0x0000FF00  // Major Revision
#define SYSCTL_DID0_MAJ_REVA    0x00000000  // Revision A (initial device)
#define SYSCTL_DID0_MAJ_REVB    0x00000100  // Revision B (first base layer
                                            // revision)
#define SYSCTL_DID0_MAJ_REVC    0x00000200  // Revision C (second base layer
                                            // revision)
#define SYSCTL_DID0_MIN_M       0x000000FF  // Minor Revision
#define SYSCTL_DID0_MIN_0       0x00000000  // Initial device, or a major
                                            // revision update
#define SYSCTL_DID0_MIN_1       0x00000001  // First metal layer change
#define SYSCTL_DID0_MIN_2       0x00000002  // Second metal layer change

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DID1 register.
//
//*****************************************************************************
#define SYSCTL_DID1_VER_M       0xF0000000  // DID1 Version
#define SYSCTL_DID1_VER_0       0x00000000  // Initial DID1 register format
                                            // definition, indicating a
                                            // Stellaris LM3Snnn device
#define SYSCTL_DID1_VER_1       0x10000000  // Second version of the DID1
                                            // register format
#define SYSCTL_DID1_FAM_M       0x0F000000  // Family
#define SYSCTL_DID1_FAM_STELLARIS \
                                0x00000000  // Stellaris family of
                                            // microcontollers, that is, all
                                            // devices with external part
                                            // numbers starting with LM3S
#define SYSCTL_DID1_PRTNO_M     0x00FF0000  // Part Number
#define SYSCTL_DID1_PRTNO_LM4FS1GH5BB \
                                0x00EA0000  // LM4FS1GH5BB
#define SYSCTL_DID1_PINCNT_M    0x0000E000  // Package Pin Count
#define SYSCTL_DID1_PINCNT_28   0x00000000  // 28-pin package
#define SYSCTL_DID1_PINCNT_48   0x00002000  // 48-pin package
#define SYSCTL_DID1_PINCNT_100  0x00004000  // 100-pin package
#define SYSCTL_DID1_PINCNT_64   0x00006000  // 64-pin package
#define SYSCTL_DID1_PINCNT_144  0x00008000  // 144-pin package
#define SYSCTL_DID1_PINCNT_157  0x0000A000  // 157-pin package
#define SYSCTL_DID1_TEMP_M      0x000000E0  // Temperature Range
#define SYSCTL_DID1_TEMP_C      0x00000000  // Commercial temperature range (0C
                                            // to 70C)
#define SYSCTL_DID1_TEMP_I      0x00000020  // Industrial temperature range
                                            // (-40C to 85C)
#define SYSCTL_DID1_TEMP_E      0x00000040  // Extended temperature range (-40C
                                            // to 105C)
#define SYSCTL_DID1_PKG_M       0x00000018  // Package Type
#define SYSCTL_DID1_PKG_SOIC    0x00000000  // SOIC package
#define SYSCTL_DID1_PKG_QFP     0x00000008  // LQFP package
#define SYSCTL_DID1_PKG_BGA     0x00000010  // BGA package
#define SYSCTL_DID1_ROHS        0x00000004  // RoHS-Compliance
#define SYSCTL_DID1_QUAL_M      0x00000003  // Qualification Status
#define SYSCTL_DID1_QUAL_ES     0x00000000  // Engineering Sample (unqualified)
#define SYSCTL_DID1_QUAL_PP     0x00000001  // Pilot Production (unqualified)
#define SYSCTL_DID1_QUAL_FQ     0x00000002  // Fully Qualified

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC0 register.
//
//*****************************************************************************
#define SYSCTL_DC0_SRAMSZ_M     0xFFFF0000  // SRAM Size
#define SYSCTL_DC0_SRAMSZ_2KB   0x00070000  // 2 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_4KB   0x000F0000  // 4 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_6KB   0x00170000  // 6 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_8KB   0x001F0000  // 8 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_12KB  0x002F0000  // 12 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_16KB  0x003F0000  // 16 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_20KB  0x004F0000  // 20 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_24KB  0x005F0000  // 24 KB of SRAM
#define SYSCTL_DC0_SRAMSZ_32KB  0x007F0000  // 32 KB of SRAM
#define SYSCTL_DC0_FLASHSZ_M    0x0000FFFF  // Flash Size
#define SYSCTL_DC0_FLASHSZ_8KB  0x00000003  // 8 KB of Flash
#define SYSCTL_DC0_FLASHSZ_16KB 0x00000007  // 16 KB of Flash
#define SYSCTL_DC0_FLASHSZ_32KB 0x0000000F  // 32 KB of Flash
#define SYSCTL_DC0_FLASHSZ_64KB 0x0000001F  // 64 KB of Flash
#define SYSCTL_DC0_FLASHSZ_96KB 0x0000002F  // 96 KB of Flash
#define SYSCTL_DC0_FLASHSZ_128K 0x0000003F  // 128 KB of Flash
#define SYSCTL_DC0_FLASHSZ_192K 0x0000005F  // 192 KB of Flash
#define SYSCTL_DC0_FLASHSZ_256K 0x0000007F  // 256 KB of Flash
#define SYSCTL_DC0_SRAMSZ_S     16          // SRAM size shift
#define SYSCTL_DC0_FLASHSZ_S    0           // Flash size shift

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC1 register.
//
//*****************************************************************************
#define SYSCTL_DC1_WDT1         0x10000000  // Watchdog Timer1 Present
#define SYSCTL_DC1_CAN1         0x02000000  // CAN Module 1 Present
#define SYSCTL_DC1_CAN0         0x01000000  // CAN Module 0 Present
#define SYSCTL_DC1_PWM1         0x00200000  // PWM Module 1 Present
#define SYSCTL_DC1_PWM0         0x00100000  // PWM Module 0 Present
#define SYSCTL_DC1_ADC1         0x00020000  // ADC Module 1 Present
#define SYSCTL_DC1_ADC0         0x00010000  // ADC Module 0 Present
#define SYSCTL_DC1_MINSYSDIV_M  0x0000F000  // System Clock Divider
#define SYSCTL_DC1_MINSYSDIV_100 \
                                0x00001000  // Divide VCO (400MHZ) by 5 minimum
#define SYSCTL_DC1_MINSYSDIV_66 0x00002000  // Divide VCO (400MHZ) by 2*2 + 2 =
                                            // 6 minimum
#define SYSCTL_DC1_MINSYSDIV_50 0x00003000  // Specifies a 50-MHz CPU clock
                                            // with a PLL divider of 4
#define SYSCTL_DC1_MINSYSDIV_40 0x00004000  // Specifies a 40-MHz CPU clock
                                            // with a PLL divider of 5
#define SYSCTL_DC1_MINSYSDIV_25 0x00007000  // Specifies a 25-MHz clock with a
                                            // PLL divider of 8
#define SYSCTL_DC1_MINSYSDIV_20 0x00009000  // Specifies a 20-MHz clock with a
                                            // PLL divider of 10
#define SYSCTL_DC1_ADC1SPD_M    0x00000C00  // Max ADC1 Speed
#define SYSCTL_DC1_ADC1SPD_125K 0x00000000  // 125K samples/second
#define SYSCTL_DC1_ADC1SPD_250K 0x00000400  // 250K samples/second
#define SYSCTL_DC1_ADC1SPD_500K 0x00000800  // 500K samples/second
#define SYSCTL_DC1_ADC1SPD_1M   0x00000C00  // 1M samples/second
#define SYSCTL_DC1_ADC0SPD_M    0x00000300  // Max ADC0 Speed
#define SYSCTL_DC1_ADC0SPD_125K 0x00000000  // 125K samples/second
#define SYSCTL_DC1_ADC0SPD_250K 0x00000100  // 250K samples/second
#define SYSCTL_DC1_ADC0SPD_500K 0x00000200  // 500K samples/second
#define SYSCTL_DC1_ADC0SPD_1M   0x00000300  // 1M samples/second
#define SYSCTL_DC1_MPU          0x00000080  // MPU Present
#define SYSCTL_DC1_HIB          0x00000040  // Hibernation Module Present
#define SYSCTL_DC1_TEMP         0x00000020  // Temp Sensor Present
#define SYSCTL_DC1_PLL          0x00000010  // PLL Present
#define SYSCTL_DC1_WDT0         0x00000008  // Watchdog Timer 0 Present
#define SYSCTL_DC1_SWO          0x00000004  // SWO Trace Port Present
#define SYSCTL_DC1_SWD          0x00000002  // SWD Present
#define SYSCTL_DC1_JTAG         0x00000001  // JTAG Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC2 register.
//
//*****************************************************************************
#define SYSCTL_DC2_EPI0         0x40000000  // EPI Module 0 Present
#define SYSCTL_DC2_I2S0         0x10000000  // I2S Module 0 Present
#define SYSCTL_DC2_COMP2        0x04000000  // Analog Comparator 2 Present
#define SYSCTL_DC2_COMP1        0x02000000  // Analog Comparator 1 Present
#define SYSCTL_DC2_COMP0        0x01000000  // Analog Comparator 0 Present
#define SYSCTL_DC2_TIMER3       0x00080000  // Timer Module 3 Present
#define SYSCTL_DC2_TIMER2       0x00040000  // Timer Module 2 Present
#define SYSCTL_DC2_TIMER1       0x00020000  // Timer Module 1 Present
#define SYSCTL_DC2_TIMER0       0x00010000  // Timer Module 0 Present
#define SYSCTL_DC2_I2C1HS       0x00008000  // I2C Module 1 Speed
#define SYSCTL_DC2_I2C1         0x00004000  // I2C Module 1 Present
#define SYSCTL_DC2_I2C0HS       0x00002000  // I2C Module 0 Speed
#define SYSCTL_DC2_I2C0         0x00001000  // I2C Module 0 Present
#define SYSCTL_DC2_QEI1         0x00000200  // QEI Module 1 Present
#define SYSCTL_DC2_QEI0         0x00000100  // QEI Module 0 Present
#define SYSCTL_DC2_SSI1         0x00000020  // SSI Module 1 Present
#define SYSCTL_DC2_SSI0         0x00000010  // SSI Module 0 Present
#define SYSCTL_DC2_UART2        0x00000004  // UART Module 2 Present
#define SYSCTL_DC2_UART1        0x00000002  // UART Module 1 Present
#define SYSCTL_DC2_UART0        0x00000001  // UART Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC3 register.
//
//*****************************************************************************
#define SYSCTL_DC3_32KHZ        0x80000000  // 32KHz Input Clock Available
#define SYSCTL_DC3_CCP5         0x20000000  // CCP5 Pin Present
#define SYSCTL_DC3_CCP4         0x10000000  // CCP4 Pin Present
#define SYSCTL_DC3_CCP3         0x08000000  // CCP3 Pin Present
#define SYSCTL_DC3_CCP2         0x04000000  // CCP2 Pin Present
#define SYSCTL_DC3_CCP1         0x02000000  // CCP1 Pin Present
#define SYSCTL_DC3_CCP0         0x01000000  // CCP0 Pin Present
#define SYSCTL_DC3_ADC0AIN7     0x00800000  // ADC Module 0 AIN7 Pin Present
#define SYSCTL_DC3_ADC0AIN6     0x00400000  // ADC Module 0 AIN6 Pin Present
#define SYSCTL_DC3_ADC0AIN5     0x00200000  // ADC Module 0 AIN5 Pin Present
#define SYSCTL_DC3_ADC0AIN4     0x00100000  // ADC Module 0 AIN4 Pin Present
#define SYSCTL_DC3_ADC0AIN3     0x00080000  // ADC Module 0 AIN3 Pin Present
#define SYSCTL_DC3_ADC0AIN2     0x00040000  // ADC Module 0 AIN2 Pin Present
#define SYSCTL_DC3_ADC0AIN1     0x00020000  // ADC Module 0 AIN1 Pin Present
#define SYSCTL_DC3_ADC0AIN0     0x00010000  // ADC Module 0 AIN0 Pin Present
#define SYSCTL_DC3_PWMFAULT     0x00008000  // PWM Fault Pin Present
#define SYSCTL_DC3_C2O          0x00004000  // C2o Pin Present
#define SYSCTL_DC3_C2PLUS       0x00002000  // C2+ Pin Present
#define SYSCTL_DC3_C2MINUS      0x00001000  // C2- Pin Present
#define SYSCTL_DC3_C1O          0x00000800  // C1o Pin Present
#define SYSCTL_DC3_C1PLUS       0x00000400  // C1+ Pin Present
#define SYSCTL_DC3_C1MINUS      0x00000200  // C1- Pin Present
#define SYSCTL_DC3_C0O          0x00000100  // C0o Pin Present
#define SYSCTL_DC3_C0PLUS       0x00000080  // C0+ Pin Present
#define SYSCTL_DC3_C0MINUS      0x00000040  // C0- Pin Present
#define SYSCTL_DC3_PWM5         0x00000020  // PWM5 Pin Present
#define SYSCTL_DC3_PWM4         0x00000010  // PWM4 Pin Present
#define SYSCTL_DC3_PWM3         0x00000008  // PWM3 Pin Present
#define SYSCTL_DC3_PWM2         0x00000004  // PWM2 Pin Present
#define SYSCTL_DC3_PWM1         0x00000002  // PWM1 Pin Present
#define SYSCTL_DC3_PWM0         0x00000001  // PWM0 Pin Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC4 register.
//
//*****************************************************************************
#define SYSCTL_DC4_EPHY0        0x40000000  // Ethernet PHY Layer 0 Present
#define SYSCTL_DC4_EMAC0        0x10000000  // Ethernet MAC Layer 0 Present
#define SYSCTL_DC4_E1588        0x01000000  // 1588 Capable
#define SYSCTL_DC4_PICAL        0x00040000  // PIOSC Calibrate
#define SYSCTL_DC4_CCP7         0x00008000  // CCP7 Pin Present
#define SYSCTL_DC4_CCP6         0x00004000  // CCP6 Pin Present
#define SYSCTL_DC4_UDMA         0x00002000  // Micro-DMA Module Present
#define SYSCTL_DC4_ROM          0x00001000  // Internal Code ROM Present
#define SYSCTL_DC4_GPIOJ        0x00000100  // GPIO Port J Present
#define SYSCTL_DC4_GPIOH        0x00000080  // GPIO Port H Present
#define SYSCTL_DC4_GPIOG        0x00000040  // GPIO Port G Present
#define SYSCTL_DC4_GPIOF        0x00000020  // GPIO Port F Present
#define SYSCTL_DC4_GPIOE        0x00000010  // GPIO Port E Present
#define SYSCTL_DC4_GPIOD        0x00000008  // GPIO Port D Present
#define SYSCTL_DC4_GPIOC        0x00000004  // GPIO Port C Present
#define SYSCTL_DC4_GPIOB        0x00000002  // GPIO Port B Present
#define SYSCTL_DC4_GPIOA        0x00000001  // GPIO Port A Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC5 register.
//
//*****************************************************************************
#define SYSCTL_DC5_PWMFAULT3    0x08000000  // PWM Fault 3 Pin Present
#define SYSCTL_DC5_PWMFAULT2    0x04000000  // PWM Fault 2 Pin Present
#define SYSCTL_DC5_PWMFAULT1    0x02000000  // PWM Fault 1 Pin Present
#define SYSCTL_DC5_PWMFAULT0    0x01000000  // PWM Fault 0 Pin Present
#define SYSCTL_DC5_PWMEFLT      0x00200000  // PWM Extended Fault Active
#define SYSCTL_DC5_PWMESYNC     0x00100000  // PWM Extended SYNC Active
#define SYSCTL_DC5_PWM7         0x00000080  // PWM7 Pin Present
#define SYSCTL_DC5_PWM6         0x00000040  // PWM6 Pin Present
#define SYSCTL_DC5_PWM5         0x00000020  // PWM5 Pin Present
#define SYSCTL_DC5_PWM4         0x00000010  // PWM4 Pin Present
#define SYSCTL_DC5_PWM3         0x00000008  // PWM3 Pin Present
#define SYSCTL_DC5_PWM2         0x00000004  // PWM2 Pin Present
#define SYSCTL_DC5_PWM1         0x00000002  // PWM1 Pin Present
#define SYSCTL_DC5_PWM0         0x00000001  // PWM0 Pin Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC6 register.
//
//*****************************************************************************
#define SYSCTL_DC6_USB0PHY      0x00000010  // USB Module 0 PHY Present
#define SYSCTL_DC6_USB0_M       0x00000003  // USB Module 0 Present
#define SYSCTL_DC6_USB0_DEV     0x00000001  // USB0 is Device Only
#define SYSCTL_DC6_USB0_HOSTDEV 0x00000002  // USB is Device or Host
#define SYSCTL_DC6_USB0_OTG     0x00000003  // USB0 is OTG

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC7 register.
//
//*****************************************************************************
#define SYSCTL_DC7_DMACH30      0x40000000  // SW
#define SYSCTL_DC7_DMACH29      0x20000000  // I2S0_TX / CAN1_TX
#define SYSCTL_DC7_DMACH28      0x10000000  // I2S0_RX / CAN1_RX
#define SYSCTL_DC7_DMACH27      0x08000000  // CAN1_TX / ADC1_SS3
#define SYSCTL_DC7_DMACH26      0x04000000  // CAN1_RX / ADC1_SS2
#define SYSCTL_DC7_DMACH25      0x02000000  // SSI1_TX / ADC1_SS1
#define SYSCTL_DC7_DMACH24      0x01000000  // SSI1_RX / ADC1_SS0
#define SYSCTL_DC7_DMACH23      0x00800000  // UART1_TX / CAN2_TX
#define SYSCTL_DC7_DMACH22      0x00400000  // UART1_RX / CAN2_RX
#define SYSCTL_DC7_DMACH21      0x00200000  // Timer1B / EPI0_WFIFO
#define SYSCTL_DC7_DMACH20      0x00100000  // Timer1A / EPI0_NBRFIFO
#define SYSCTL_DC7_DMACH19      0x00080000  // Timer0B / Timer1B
#define SYSCTL_DC7_DMACH18      0x00040000  // Timer0A / Timer1A
#define SYSCTL_DC7_DMACH17      0x00020000  // ADC0_SS3
#define SYSCTL_DC7_DMACH16      0x00010000  // ADC0_SS2
#define SYSCTL_DC7_DMACH15      0x00008000  // ADC0_SS1 / Timer2B
#define SYSCTL_DC7_DMACH14      0x00004000  // ADC0_SS0 / Timer2A
#define SYSCTL_DC7_DMACH13      0x00002000  // CAN0_TX / UART2_TX
#define SYSCTL_DC7_DMACH12      0x00001000  // CAN0_RX / UART2_RX
#define SYSCTL_DC7_DMACH11      0x00000800  // SSI0_TX / SSI1_TX
#define SYSCTL_DC7_DMACH10      0x00000400  // SSI0_RX / SSI1_RX
#define SYSCTL_DC7_DMACH9       0x00000200  // UART0_TX / UART1_TX
#define SYSCTL_DC7_DMACH8       0x00000100  // UART0_RX / UART1_RX
#define SYSCTL_DC7_DMACH7       0x00000080  // ETH_TX / Timer2B
#define SYSCTL_DC7_DMACH6       0x00000040  // ETH_RX / Timer2A
#define SYSCTL_DC7_DMACH5       0x00000020  // USB_EP3_TX / Timer2B
#define SYSCTL_DC7_DMACH4       0x00000010  // USB_EP3_RX / Timer2A
#define SYSCTL_DC7_DMACH3       0x00000008  // USB_EP2_TX / Timer3B
#define SYSCTL_DC7_DMACH2       0x00000004  // USB_EP2_RX / Timer3A
#define SYSCTL_DC7_DMACH1       0x00000002  // USB_EP1_TX / UART2_TX
#define SYSCTL_DC7_DMACH0       0x00000001  // USB_EP1_RX / UART2_RX

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC8 register.
//
//*****************************************************************************
#define SYSCTL_DC8_ADC1AIN15    0x80000000  // ADC Module 1 AIN15 Pin Present
#define SYSCTL_DC8_ADC1AIN14    0x40000000  // ADC Module 1 AIN14 Pin Present
#define SYSCTL_DC8_ADC1AIN13    0x20000000  // ADC Module 1 AIN13 Pin Present
#define SYSCTL_DC8_ADC1AIN12    0x10000000  // ADC Module 1 AIN12 Pin Present
#define SYSCTL_DC8_ADC1AIN11    0x08000000  // ADC Module 1 AIN11 Pin Present
#define SYSCTL_DC8_ADC1AIN10    0x04000000  // ADC Module 1 AIN10 Pin Present
#define SYSCTL_DC8_ADC1AIN9     0x02000000  // ADC Module 1 AIN9 Pin Present
#define SYSCTL_DC8_ADC1AIN8     0x01000000  // ADC Module 1 AIN8 Pin Present
#define SYSCTL_DC8_ADC1AIN7     0x00800000  // ADC Module 1 AIN7 Pin Present
#define SYSCTL_DC8_ADC1AIN6     0x00400000  // ADC Module 1 AIN6 Pin Present
#define SYSCTL_DC8_ADC1AIN5     0x00200000  // ADC Module 1 AIN5 Pin Present
#define SYSCTL_DC8_ADC1AIN4     0x00100000  // ADC Module 1 AIN4 Pin Present
#define SYSCTL_DC8_ADC1AIN3     0x00080000  // ADC Module 1 AIN3 Pin Present
#define SYSCTL_DC8_ADC1AIN2     0x00040000  // ADC Module 1 AIN2 Pin Present
#define SYSCTL_DC8_ADC1AIN1     0x00020000  // ADC Module 1 AIN1 Pin Present
#define SYSCTL_DC8_ADC1AIN0     0x00010000  // ADC Module 1 AIN0 Pin Present
#define SYSCTL_DC8_ADC0AIN15    0x00008000  // ADC Module 0 AIN15 Pin Present
#define SYSCTL_DC8_ADC0AIN14    0x00004000  // ADC Module 0 AIN14 Pin Present
#define SYSCTL_DC8_ADC0AIN13    0x00002000  // ADC Module 0 AIN13 Pin Present
#define SYSCTL_DC8_ADC0AIN12    0x00001000  // ADC Module 0 AIN12 Pin Present
#define SYSCTL_DC8_ADC0AIN11    0x00000800  // ADC Module 0 AIN11 Pin Present
#define SYSCTL_DC8_ADC0AIN10    0x00000400  // ADC Module 0 AIN10 Pin Present
#define SYSCTL_DC8_ADC0AIN9     0x00000200  // ADC Module 0 AIN9 Pin Present
#define SYSCTL_DC8_ADC0AIN8     0x00000100  // ADC Module 0 AIN8 Pin Present
#define SYSCTL_DC8_ADC0AIN7     0x00000080  // ADC Module 0 AIN7 Pin Present
#define SYSCTL_DC8_ADC0AIN6     0x00000040  // ADC Module 0 AIN6 Pin Present
#define SYSCTL_DC8_ADC0AIN5     0x00000020  // ADC Module 0 AIN5 Pin Present
#define SYSCTL_DC8_ADC0AIN4     0x00000010  // ADC Module 0 AIN4 Pin Present
#define SYSCTL_DC8_ADC0AIN3     0x00000008  // ADC Module 0 AIN3 Pin Present
#define SYSCTL_DC8_ADC0AIN2     0x00000004  // ADC Module 0 AIN2 Pin Present
#define SYSCTL_DC8_ADC0AIN1     0x00000002  // ADC Module 0 AIN1 Pin Present
#define SYSCTL_DC8_ADC0AIN0     0x00000001  // ADC Module 0 AIN0 Pin Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PBORCTL register.
//
//*****************************************************************************
#define SYSCTL_PBORCTL_BORIOR   0x00000002  // BOR Interrupt or Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRCR0 register.
//
//*****************************************************************************
#define SYSCTL_SRCR0_WDT1       0x10000000  // WDT1 Reset Control
#define SYSCTL_SRCR0_ADC1       0x00020000  // ADC1 Reset Control
#define SYSCTL_SRCR0_ADC0       0x00010000  // ADC0 Reset Control
#define SYSCTL_SRCR0_HIB        0x00000040  // HIB Reset Control
#define SYSCTL_SRCR0_WDT0       0x00000008  // WDT0 Reset Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRCR1 register.
//
//*****************************************************************************
#define SYSCTL_SRCR1_COMP2      0x04000000  // Analog Comp 2 Reset Control
#define SYSCTL_SRCR1_COMP1      0x02000000  // Analog Comp 1 Reset Control
#define SYSCTL_SRCR1_COMP0      0x01000000  // Analog Comp 0 Reset Control
#define SYSCTL_SRCR1_TIMER3     0x00080000  // Timer 3 Reset Control
#define SYSCTL_SRCR1_TIMER2     0x00040000  // Timer 2 Reset Control
#define SYSCTL_SRCR1_TIMER1     0x00020000  // Timer 1 Reset Control
#define SYSCTL_SRCR1_TIMER0     0x00010000  // Timer 0 Reset Control
#define SYSCTL_SRCR1_I2C1       0x00004000  // I2C1 Reset Control
#define SYSCTL_SRCR1_I2C0       0x00001000  // I2C0 Reset Control
#define SYSCTL_SRCR1_SSI1       0x00000020  // SSI1 Reset Control
#define SYSCTL_SRCR1_SSI0       0x00000010  // SSI0 Reset Control
#define SYSCTL_SRCR1_UART2      0x00000004  // UART2 Reset Control
#define SYSCTL_SRCR1_UART1      0x00000002  // UART1 Reset Control
#define SYSCTL_SRCR1_UART0      0x00000001  // UART0 Reset Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRCR2 register.
//
//*****************************************************************************
#define SYSCTL_SRCR2_UDMA       0x00002000  // Micro-DMA Reset Control
#define SYSCTL_SRCR2_GPIOJ      0x00000100  // Port J Reset Control
#define SYSCTL_SRCR2_GPIOH      0x00000080  // Port H Reset Control
#define SYSCTL_SRCR2_GPIOG      0x00000040  // Port G Reset Control
#define SYSCTL_SRCR2_GPIOF      0x00000020  // Port F Reset Control
#define SYSCTL_SRCR2_GPIOE      0x00000010  // Port E Reset Control
#define SYSCTL_SRCR2_GPIOD      0x00000008  // Port D Reset Control
#define SYSCTL_SRCR2_GPIOC      0x00000004  // Port C Reset Control
#define SYSCTL_SRCR2_GPIOB      0x00000002  // Port B Reset Control
#define SYSCTL_SRCR2_GPIOA      0x00000001  // Port A Reset Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RIS register.
//
//*****************************************************************************
#define SYSCTL_RIS_MOSCPUPRIS   0x00000100  // MOSC Power Up Raw Interrupt
                                            // Status
#define SYSCTL_RIS_PLLLRIS      0x00000040  // PLL Lock Raw Interrupt Status
#define SYSCTL_RIS_MOFRIS       0x00000008  // Main Oscillator Fault Raw
                                            // Interrupt Status
#define SYSCTL_RIS_BORRIS       0x00000002  // Brown-Out Reset Raw Interrupt
                                            // Status

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_IMC register.
//
//*****************************************************************************
#define SYSCTL_IMC_MOSCPUPIM    0x00000100  // MOSC Power Up Interrupt Mask
#define SYSCTL_IMC_PLLLIM       0x00000040  // PLL Lock Interrupt Mask
#define SYSCTL_IMC_MOFIM        0x00000008  // Main Oscillator Fault Interrupt
                                            // Mask
#define SYSCTL_IMC_BORIM        0x00000002  // Brown-Out Reset Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_MISC register.
//
//*****************************************************************************
#define SYSCTL_MISC_MOSCPUPMIS  0x00000100  // MOSC Power Up Masked Interrupt
                                            // Status
#define SYSCTL_MISC_PLLLMIS     0x00000040  // PLL Lock Masked Interrupt Status
#define SYSCTL_MISC_MOFMIS      0x00000008  // Main Oscillator Fault Masked
                                            // Interrupt Status
#define SYSCTL_MISC_BORMIS      0x00000002  // BOR Masked Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RESC register.
//
//*****************************************************************************
#define SYSCTL_RESC_MOSCFAIL    0x00010000  // MOSC Failure Reset
#define SYSCTL_RESC_WDT1        0x00000020  // Watchdog Timer 1 Reset
#define SYSCTL_RESC_SW          0x00000010  // Software Reset
#define SYSCTL_RESC_WDT0        0x00000008  // Watchdog Timer 0 Reset
#define SYSCTL_RESC_BOR         0x00000004  // Brown-Out Reset
#define SYSCTL_RESC_POR         0x00000002  // Power-On Reset
#define SYSCTL_RESC_EXT         0x00000001  // External Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCC register.
//
//*****************************************************************************
#define SYSCTL_RCC_ACG          0x08000000  // Auto Clock Gating
#define SYSCTL_RCC_SYSDIV_M     0x07800000  // System Clock Divisor
#define SYSCTL_RCC_USESYSDIV    0x00400000  // Enable System Clock Divider
#define SYSCTL_RCC_PWRDN        0x00002000  // PLL Power Down
#define SYSCTL_RCC_BYPASS       0x00000800  // PLL Bypass
#define SYSCTL_RCC_XTAL_M       0x000007C0  // Crystal Value
#define SYSCTL_RCC_XTAL_4MHZ    0x00000180  // 4 MHz
#define SYSCTL_RCC_XTAL_4_09MHZ 0x000001C0  // 4.096 MHz
#define SYSCTL_RCC_XTAL_4_91MHZ 0x00000200  // 4.9152 MHz
#define SYSCTL_RCC_XTAL_5MHZ    0x00000240  // 5 MHz
#define SYSCTL_RCC_XTAL_5_12MHZ 0x00000280  // 5.12 MHz
#define SYSCTL_RCC_XTAL_6MHZ    0x000002C0  // 6 MHz
#define SYSCTL_RCC_XTAL_6_14MHZ 0x00000300  // 6.144 MHz
#define SYSCTL_RCC_XTAL_7_37MHZ 0x00000340  // 7.3728 MHz
#define SYSCTL_RCC_XTAL_8MHZ    0x00000380  // 8 MHz
#define SYSCTL_RCC_XTAL_8_19MHZ 0x000003C0  // 8.192 MHz
#define SYSCTL_RCC_XTAL_10MHZ   0x00000400  // 10 MHz
#define SYSCTL_RCC_XTAL_12MHZ   0x00000440  // 12 MHz
#define SYSCTL_RCC_XTAL_12_2MHZ 0x00000480  // 12.288 MHz
#define SYSCTL_RCC_XTAL_13_5MHZ 0x000004C0  // 13.56 MHz
#define SYSCTL_RCC_XTAL_14_3MHZ 0x00000500  // 14.31818 MHz
#define SYSCTL_RCC_XTAL_16MHZ   0x00000540  // 16 MHz
#define SYSCTL_RCC_XTAL_16_3MHZ 0x00000580  // 16.384 MHz
#define SYSCTL_RCC_XTAL_18MHZ   0x000005C0  // 18.0 MHz
#define SYSCTL_RCC_XTAL_20MHZ   0x00000600  // 20.0 MHz
#define SYSCTL_RCC_XTAL_24MHZ   0x00000640  // 24.0 MHz
#define SYSCTL_RCC_XTAL_25MHZ   0x00000680  // 25.0 MHz
#define SYSCTL_RCC_OSCSRC_M     0x00000030  // Oscillator Source
#define SYSCTL_RCC_OSCSRC_MAIN  0x00000000  // MOSC
#define SYSCTL_RCC_OSCSRC_INT   0x00000010  // IOSC
#define SYSCTL_RCC_OSCSRC_INT4  0x00000020  // IOSC/4
#define SYSCTL_RCC_OSCSRC_30    0x00000030  // 30 kHz
#define SYSCTL_RCC_IOSCDIS      0x00000002  // Internal Oscillator Disable
#define SYSCTL_RCC_MOSCDIS      0x00000001  // Main Oscillator Disable
#define SYSCTL_RCC_SYSDIV_S     23

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_GPIOHBCTL
// register.
//
//*****************************************************************************
#define SYSCTL_GPIOHBCTL_PORTJ  0x00000100  // Port J Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTH  0x00000080  // Port H Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTG  0x00000040  // Port G Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTF  0x00000020  // Port F Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTE  0x00000010  // Port E Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTD  0x00000008  // Port D Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTC  0x00000004  // Port C Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTB  0x00000002  // Port B Advanced High-Performance
                                            // Bus
#define SYSCTL_GPIOHBCTL_PORTA  0x00000001  // Port A Advanced High-Performance
                                            // Bus

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCC2 register.
//
//*****************************************************************************
#define SYSCTL_RCC2_USERCC2     0x80000000  // Use RCC2
#define SYSCTL_RCC2_DIV400      0x40000000  // Divide PLL as 400 MHz vs. 200
                                            // MHz
#define SYSCTL_RCC2_SYSDIV2_M   0x1F800000  // System Clock Divisor 2
#define SYSCTL_RCC2_SYSDIV2_2   0x00800000  // System clock /2
#define SYSCTL_RCC2_SYSDIV2_3   0x01000000  // System clock /3
#define SYSCTL_RCC2_SYSDIV2_4   0x01800000  // System clock /4
#define SYSCTL_RCC2_SYSDIV2_5   0x02000000  // System clock /5
#define SYSCTL_RCC2_SYSDIV2_6   0x02800000  // System clock /6
#define SYSCTL_RCC2_SYSDIV2_7   0x03000000  // System clock /7
#define SYSCTL_RCC2_SYSDIV2_8   0x03800000  // System clock /8
#define SYSCTL_RCC2_SYSDIV2_9   0x04000000  // System clock /9
#define SYSCTL_RCC2_SYSDIV2_10  0x04800000  // System clock /10
#define SYSCTL_RCC2_SYSDIV2_11  0x05000000  // System clock /11
#define SYSCTL_RCC2_SYSDIV2_12  0x05800000  // System clock /12
#define SYSCTL_RCC2_SYSDIV2_13  0x06000000  // System clock /13
#define SYSCTL_RCC2_SYSDIV2_14  0x06800000  // System clock /14
#define SYSCTL_RCC2_SYSDIV2_15  0x07000000  // System clock /15
#define SYSCTL_RCC2_SYSDIV2_16  0x07800000  // System clock /16
#define SYSCTL_RCC2_SYSDIV2_17  0x08000000  // System clock /17
#define SYSCTL_RCC2_SYSDIV2_18  0x08800000  // System clock /18
#define SYSCTL_RCC2_SYSDIV2_19  0x09000000  // System clock /19
#define SYSCTL_RCC2_SYSDIV2_20  0x09800000  // System clock /20
#define SYSCTL_RCC2_SYSDIV2_21  0x0A000000  // System clock /21
#define SYSCTL_RCC2_SYSDIV2_22  0x0A800000  // System clock /22
#define SYSCTL_RCC2_SYSDIV2_23  0x0B000000  // System clock /23
#define SYSCTL_RCC2_SYSDIV2_24  0x0B800000  // System clock /24
#define SYSCTL_RCC2_SYSDIV2_25  0x0C000000  // System clock /25
#define SYSCTL_RCC2_SYSDIV2_26  0x0C800000  // System clock /26
#define SYSCTL_RCC2_SYSDIV2_27  0x0D000000  // System clock /27
#define SYSCTL_RCC2_SYSDIV2_28  0x0D800000  // System clock /28
#define SYSCTL_RCC2_SYSDIV2_29  0x0E000000  // System clock /29
#define SYSCTL_RCC2_SYSDIV2_30  0x0E800000  // System clock /30
#define SYSCTL_RCC2_SYSDIV2_31  0x0F000000  // System clock /31
#define SYSCTL_RCC2_SYSDIV2_32  0x0F800000  // System clock /32
#define SYSCTL_RCC2_SYSDIV2_33  0x10000000  // System clock /33
#define SYSCTL_RCC2_SYSDIV2_34  0x10800000  // System clock /34
#define SYSCTL_RCC2_SYSDIV2_35  0x11000000  // System clock /35
#define SYSCTL_RCC2_SYSDIV2_36  0x11800000  // System clock /36
#define SYSCTL_RCC2_SYSDIV2_37  0x12000000  // System clock /37
#define SYSCTL_RCC2_SYSDIV2_38  0x12800000  // System clock /38
#define SYSCTL_RCC2_SYSDIV2_39  0x13000000  // System clock /39
#define SYSCTL_RCC2_SYSDIV2_40  0x13800000  // System clock /40
#define SYSCTL_RCC2_SYSDIV2_41  0x14000000  // System clock /41
#define SYSCTL_RCC2_SYSDIV2_42  0x14800000  // System clock /42
#define SYSCTL_RCC2_SYSDIV2_43  0x15000000  // System clock /43
#define SYSCTL_RCC2_SYSDIV2_44  0x15800000  // System clock /44
#define SYSCTL_RCC2_SYSDIV2_45  0x16000000  // System clock /45
#define SYSCTL_RCC2_SYSDIV2_46  0x16800000  // System clock /46
#define SYSCTL_RCC2_SYSDIV2_47  0x17000000  // System clock /47
#define SYSCTL_RCC2_SYSDIV2_48  0x17800000  // System clock /48
#define SYSCTL_RCC2_SYSDIV2_49  0x18000000  // System clock /49
#define SYSCTL_RCC2_SYSDIV2_50  0x18800000  // System clock /50
#define SYSCTL_RCC2_SYSDIV2_51  0x19000000  // System clock /51
#define SYSCTL_RCC2_SYSDIV2_52  0x19800000  // System clock /52
#define SYSCTL_RCC2_SYSDIV2_53  0x1A000000  // System clock /53
#define SYSCTL_RCC2_SYSDIV2_54  0x1A800000  // System clock /54
#define SYSCTL_RCC2_SYSDIV2_55  0x1B000000  // System clock /55
#define SYSCTL_RCC2_SYSDIV2_56  0x1B800000  // System clock /56
#define SYSCTL_RCC2_SYSDIV2_57  0x1C000000  // System clock /57
#define SYSCTL_RCC2_SYSDIV2_58  0x1C800000  // System clock /58
#define SYSCTL_RCC2_SYSDIV2_59  0x1D000000  // System clock /59
#define SYSCTL_RCC2_SYSDIV2_60  0x1D800000  // System clock /60
#define SYSCTL_RCC2_SYSDIV2_61  0x1E000000  // System clock /61
#define SYSCTL_RCC2_SYSDIV2_62  0x1E800000  // System clock /62
#define SYSCTL_RCC2_SYSDIV2_63  0x1F000000  // System clock /63
#define SYSCTL_RCC2_SYSDIV2_64  0x1F800000  // System clock /64
#define SYSCTL_RCC2_SYSDIV2LSB  0x00400000  // Additional LSB for SYSDIV2
#define SYSCTL_RCC2_PWRDN2      0x00002000  // Power-Down PLL 2
#define SYSCTL_RCC2_BYPASS2     0x00000800  // PLL Bypass 2
#define SYSCTL_RCC2_OSCSRC2_M   0x00000070  // Oscillator Source 2
#define SYSCTL_RCC2_OSCSRC2_MO  0x00000000  // MOSC
#define SYSCTL_RCC2_OSCSRC2_IO  0x00000010  // PIOSC
#define SYSCTL_RCC2_OSCSRC2_IO4 0x00000020  // PIOSC/4
#define SYSCTL_RCC2_OSCSRC2_30  0x00000030  // 30 kHz
#define SYSCTL_RCC2_OSCSRC2_32  0x00000070  // 32.768 kHz
#define SYSCTL_RCC2_SYSDIV2_S   23

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_MOSCCTL register.
//
//*****************************************************************************
#define SYSCTL_MOSCCTL_NOXTAL   0x00000004  // No Crystal Connected
#define SYSCTL_MOSCCTL_MOSCIM   0x00000002  // MOSC Failure Action
#define SYSCTL_MOSCCTL_CVAL     0x00000001  // Clock Validation for MOSC

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGC0 register.
//
//*****************************************************************************
#define SYSCTL_RCGC0_WDT1       0x10000000  // WDT1 Clock Gating Control
#define SYSCTL_RCGC0_ADC1       0x00020000  // ADC1 Clock Gating Control
#define SYSCTL_RCGC0_ADC0       0x00010000  // ADC0 Clock Gating Control
#define SYSCTL_RCGC0_ADC1SPD_M  0x00000C00  // ADC1 Sample Speed
#define SYSCTL_RCGC0_ADC1SPD_125K \
                                0x00000000  // 125K samples/second
#define SYSCTL_RCGC0_ADC1SPD_250K \
                                0x00000400  // 250K samples/second
#define SYSCTL_RCGC0_ADC1SPD_500K \
                                0x00000800  // 500K samples/second
#define SYSCTL_RCGC0_ADC1SPD_1M 0x00000C00  // 1M samples/second
#define SYSCTL_RCGC0_ADC0SPD_M  0x00000300  // ADC0 Sample Speed
#define SYSCTL_RCGC0_ADC0SPD_125K \
                                0x00000000  // 125K samples/second
#define SYSCTL_RCGC0_ADC0SPD_250K \
                                0x00000100  // 250K samples/second
#define SYSCTL_RCGC0_ADC0SPD_500K \
                                0x00000200  // 500K samples/second
#define SYSCTL_RCGC0_ADC0SPD_1M 0x00000300  // 1M samples/second
#define SYSCTL_RCGC0_HIB        0x00000040  // HIB Clock Gating Control
#define SYSCTL_RCGC0_WDT0       0x00000008  // WDT0 Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGC1 register.
//
//*****************************************************************************
#define SYSCTL_RCGC1_COMP2      0x04000000  // Analog Comparator 2 Clock Gating
#define SYSCTL_RCGC1_COMP1      0x02000000  // Analog Comparator 1 Clock Gating
#define SYSCTL_RCGC1_COMP0      0x01000000  // Analog Comparator 0 Clock Gating
#define SYSCTL_RCGC1_TIMER3     0x00080000  // Timer 3 Clock Gating Control
#define SYSCTL_RCGC1_TIMER2     0x00040000  // Timer 2 Clock Gating Control
#define SYSCTL_RCGC1_TIMER1     0x00020000  // Timer 1 Clock Gating Control
#define SYSCTL_RCGC1_TIMER0     0x00010000  // Timer 0 Clock Gating Control
#define SYSCTL_RCGC1_I2C1       0x00004000  // I2C1 Clock Gating Control
#define SYSCTL_RCGC1_I2C0       0x00001000  // I2C0 Clock Gating Control
#define SYSCTL_RCGC1_SSI1       0x00000020  // SSI1 Clock Gating Control
#define SYSCTL_RCGC1_SSI0       0x00000010  // SSI0 Clock Gating Control
#define SYSCTL_RCGC1_UART2      0x00000004  // UART2 Clock Gating Control
#define SYSCTL_RCGC1_UART1      0x00000002  // UART1 Clock Gating Control
#define SYSCTL_RCGC1_UART0      0x00000001  // UART0 Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGC2 register.
//
//*****************************************************************************
#define SYSCTL_RCGC2_UDMA       0x00002000  // Micro-DMA Clock Gating Control
#define SYSCTL_RCGC2_GPIOJ      0x00000100  // Port J Clock Gating Control
#define SYSCTL_RCGC2_GPIOH      0x00000080  // Port H Clock Gating Control
#define SYSCTL_RCGC2_GPIOG      0x00000040  // Port G Clock Gating Control
#define SYSCTL_RCGC2_GPIOF      0x00000020  // Port F Clock Gating Control
#define SYSCTL_RCGC2_GPIOE      0x00000010  // Port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOD      0x00000008  // Port D Clock Gating Control
#define SYSCTL_RCGC2_GPIOC      0x00000004  // Port C Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // Port B Clock Gating Control
#define SYSCTL_RCGC2_GPIOA      0x00000001  // Port A Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGC0 register.
//
//*****************************************************************************
#define SYSCTL_SCGC0_WDT1       0x10000000  // WDT1 Clock Gating Control
#define SYSCTL_SCGC0_ADC1       0x00020000  // ADC1 Clock Gating Control
#define SYSCTL_SCGC0_ADC0       0x00010000  // ADC0 Clock Gating Control
#define SYSCTL_SCGC0_HIB        0x00000040  // HIB Clock Gating Control
#define SYSCTL_SCGC0_WDT0       0x00000008  // WDT0 Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGC1 register.
//
//*****************************************************************************
#define SYSCTL_SCGC1_COMP2      0x04000000  // Analog Comparator 2 Clock Gating
#define SYSCTL_SCGC1_COMP1      0x02000000  // Analog Comparator 1 Clock Gating
#define SYSCTL_SCGC1_COMP0      0x01000000  // Analog Comparator 0 Clock Gating
#define SYSCTL_SCGC1_TIMER3     0x00080000  // Timer 3 Clock Gating Control
#define SYSCTL_SCGC1_TIMER2     0x00040000  // Timer 2 Clock Gating Control
#define SYSCTL_SCGC1_TIMER1     0x00020000  // Timer 1 Clock Gating Control
#define SYSCTL_SCGC1_TIMER0     0x00010000  // Timer 0 Clock Gating Control
#define SYSCTL_SCGC1_I2C1       0x00004000  // I2C1 Clock Gating Control
#define SYSCTL_SCGC1_I2C0       0x00001000  // I2C0 Clock Gating Control
#define SYSCTL_SCGC1_SSI1       0x00000020  // SSI1 Clock Gating Control
#define SYSCTL_SCGC1_SSI0       0x00000010  // SSI0 Clock Gating Control
#define SYSCTL_SCGC1_UART2      0x00000004  // UART2 Clock Gating Control
#define SYSCTL_SCGC1_UART1      0x00000002  // UART1 Clock Gating Control
#define SYSCTL_SCGC1_UART0      0x00000001  // UART0 Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGC2 register.
//
//*****************************************************************************
#define SYSCTL_SCGC2_UDMA       0x00002000  // Micro-DMA Clock Gating Control
#define SYSCTL_SCGC2_GPIOJ      0x00000100  // Port J Clock Gating Control
#define SYSCTL_SCGC2_GPIOH      0x00000080  // Port H Clock Gating Control
#define SYSCTL_SCGC2_GPIOG      0x00000040  // Port G Clock Gating Control
#define SYSCTL_SCGC2_GPIOF      0x00000020  // Port F Clock Gating Control
#define SYSCTL_SCGC2_GPIOE      0x00000010  // Port E Clock Gating Control
#define SYSCTL_SCGC2_GPIOD      0x00000008  // Port D Clock Gating Control
#define SYSCTL_SCGC2_GPIOC      0x00000004  // Port C Clock Gating Control
#define SYSCTL_SCGC2_GPIOB      0x00000002  // Port B Clock Gating Control
#define SYSCTL_SCGC2_GPIOA      0x00000001  // Port A Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGC0 register.
//
//*****************************************************************************
#define SYSCTL_DCGC0_WDT1       0x10000000  // WDT1 Clock Gating Control
#define SYSCTL_DCGC0_ADC1       0x00020000  // ADC1 Clock Gating Control
#define SYSCTL_DCGC0_ADC0       0x00010000  // ADC0 Clock Gating Control
#define SYSCTL_DCGC0_HIB        0x00000040  // HIB Clock Gating Control
#define SYSCTL_DCGC0_WDT0       0x00000008  // WDT0 Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGC1 register.
//
//*****************************************************************************
#define SYSCTL_DCGC1_COMP2      0x04000000  // Analog Comparator 2 Clock Gating
#define SYSCTL_DCGC1_COMP1      0x02000000  // Analog Comparator 1 Clock Gating
#define SYSCTL_DCGC1_COMP0      0x01000000  // Analog Comparator 0 Clock Gating
#define SYSCTL_DCGC1_TIMER3     0x00080000  // Timer 3 Clock Gating Control
#define SYSCTL_DCGC1_TIMER2     0x00040000  // Timer 2 Clock Gating Control
#define SYSCTL_DCGC1_TIMER1     0x00020000  // Timer 1 Clock Gating Control
#define SYSCTL_DCGC1_TIMER0     0x00010000  // Timer 0 Clock Gating Control
#define SYSCTL_DCGC1_I2C1       0x00004000  // I2C1 Clock Gating Control
#define SYSCTL_DCGC1_I2C0       0x00001000  // I2C0 Clock Gating Control
#define SYSCTL_DCGC1_SSI1       0x00000020  // SSI1 Clock Gating Control
#define SYSCTL_DCGC1_SSI0       0x00000010  // SSI0 Clock Gating Control
#define SYSCTL_DCGC1_UART2      0x00000004  // UART2 Clock Gating Control
#define SYSCTL_DCGC1_UART1      0x00000002  // UART1 Clock Gating Control
#define SYSCTL_DCGC1_UART0      0x00000001  // UART0 Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGC2 register.
//
//*****************************************************************************
#define SYSCTL_DCGC2_UDMA       0x00002000  // Micro-DMA Clock Gating Control
#define SYSCTL_DCGC2_GPIOJ      0x00000100  // Port J Clock Gating Control
#define SYSCTL_DCGC2_GPIOH      0x00000080  // Port H Clock Gating Control
#define SYSCTL_DCGC2_GPIOG      0x00000040  // Port G Clock Gating Control
#define SYSCTL_DCGC2_GPIOF      0x00000020  // Port F Clock Gating Control
#define SYSCTL_DCGC2_GPIOE      0x00000010  // Port E Clock Gating Control
#define SYSCTL_DCGC2_GPIOD      0x00000008  // Port D Clock Gating Control
#define SYSCTL_DCGC2_GPIOC      0x00000004  // Port C Clock Gating Control
#define SYSCTL_DCGC2_GPIOB      0x00000002  // Port B Clock Gating Control
#define SYSCTL_DCGC2_GPIOA      0x00000001  // Port A Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DSLPCLKCFG
// register.
//
//*****************************************************************************
#define SYSCTL_DSLPCLKCFG_D_M   0x1F800000  // Divider Field Override
#define SYSCTL_DSLPCLKCFG_D_1   0x00000000  // System clock /1
#define SYSCTL_DSLPCLKCFG_D_2   0x00800000  // System clock /2
#define SYSCTL_DSLPCLKCFG_D_3   0x01000000  // System clock /3
#define SYSCTL_DSLPCLKCFG_D_4   0x01800000  // System clock /4
#define SYSCTL_DSLPCLKCFG_D_64  0x1F800000  // System clock /64
#define SYSCTL_DSLPCLKCFG_O_M   0x00000070  // Clock Source
#define SYSCTL_DSLPCLKCFG_O_IGN 0x00000000  // MOSC
#define SYSCTL_DSLPCLKCFG_O_IO  0x00000010  // PIOSC
#define SYSCTL_DSLPCLKCFG_O_30  0x00000030  // 30 kHz
#define SYSCTL_DSLPCLKCFG_O_32  0x00000070  // 32.768 kHz

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SYSPROP register.
//
//*****************************************************************************
#define SYSCTL_SYSPROP_FPU      0x00000001  // FPU Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PIOSCCAL
// register.
//
//*****************************************************************************
#define SYSCTL_PIOSCCAL_UTEN    0x80000000  // Use User Trim Value
#define SYSCTL_PIOSCCAL_CAL     0x00000200  // Start Calibration
#define SYSCTL_PIOSCCAL_UPDATE  0x00000100  // Update Trim
#define SYSCTL_PIOSCCAL_UT_M    0x0000007F  // User Trim Value
#define SYSCTL_PIOSCCAL_UT_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PIOSCSTAT
// register.
//
//*****************************************************************************
#define SYSCTL_PIOSCSTAT_DT_M   0x007F0000  // Default Trim Value
#define SYSCTL_PIOSCSTAT_CR_M   0x00000300  // Calibration Result
#define SYSCTL_PIOSCSTAT_CRNONE 0x00000000  // Calibration has not been
                                            // attempted
#define SYSCTL_PIOSCSTAT_CRPASS 0x00000100  // The last calibration operation
                                            // completed to meet 1% accuracy
#define SYSCTL_PIOSCSTAT_CRFAIL 0x00000200  // The last calibration operation
                                            // failed to meet 1% accuracy
#define SYSCTL_PIOSCSTAT_CT_M   0x0000007F  // Calibration Trim Value
#define SYSCTL_PIOSCSTAT_DT_S   16
#define SYSCTL_PIOSCSTAT_CT_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PLLFREQ0
// register.
//
//*****************************************************************************
#define SYSCTL_PLLFREQ0_MFRAC_M 0x000FFC00  // PLL M Fractional Value
#define SYSCTL_PLLFREQ0_MINT_M  0x000003FF  // PLL M Integer Value
#define SYSCTL_PLLFREQ0_MFRAC_S 10
#define SYSCTL_PLLFREQ0_MINT_S  0

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PLLFREQ1
// register.
//
//*****************************************************************************
#define SYSCTL_PLLFREQ1_Q_M     0x00001F00  // PLL Q Value
#define SYSCTL_PLLFREQ1_N_M     0x0000001F  // PLL N Value
#define SYSCTL_PLLFREQ1_Q_S     8
#define SYSCTL_PLLFREQ1_N_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PLLSTAT register.
//
//*****************************************************************************
#define SYSCTL_PLLSTAT_LOCK     0x00000001  // PLL Lock

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DC9 register.
//
//*****************************************************************************
#define SYSCTL_DC9_ADC1DC7      0x00800000  // ADC1 DC7 Present
#define SYSCTL_DC9_ADC1DC6      0x00400000  // ADC1 DC6 Present
#define SYSCTL_DC9_ADC1DC5      0x00200000  // ADC1 DC5 Present
#define SYSCTL_DC9_ADC1DC4      0x00100000  // ADC1 DC4 Present
#define SYSCTL_DC9_ADC1DC3      0x00080000  // ADC1 DC3 Present
#define SYSCTL_DC9_ADC1DC2      0x00040000  // ADC1 DC2 Present
#define SYSCTL_DC9_ADC1DC1      0x00020000  // ADC1 DC1 Present
#define SYSCTL_DC9_ADC1DC0      0x00010000  // ADC1 DC0 Present
#define SYSCTL_DC9_ADC0DC7      0x00000080  // ADC0 DC7 Present
#define SYSCTL_DC9_ADC0DC6      0x00000040  // ADC0 DC6 Present
#define SYSCTL_DC9_ADC0DC5      0x00000020  // ADC0 DC5 Present
#define SYSCTL_DC9_ADC0DC4      0x00000010  // ADC0 DC4 Present
#define SYSCTL_DC9_ADC0DC3      0x00000008  // ADC0 DC3 Present
#define SYSCTL_DC9_ADC0DC2      0x00000004  // ADC0 DC2 Present
#define SYSCTL_DC9_ADC0DC1      0x00000002  // ADC0 DC1 Present
#define SYSCTL_DC9_ADC0DC0      0x00000001  // ADC0 DC0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_NVMSTAT register.
//
//*****************************************************************************
#define SYSCTL_NVMSTAT_FWB      0x00000001  // 32 Word Flash Write Buffer
                                            // Active

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPWD register.
//
//*****************************************************************************
#define SYSCTL_PPWD_P1          0x00000002  // Watchdog Timer 1 Present
#define SYSCTL_PPWD_P0          0x00000001  // Watchdog Timer 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPTIMER register.
//
//*****************************************************************************
#define SYSCTL_PPTIMER_P5       0x00000020  // Timer 5 Present
#define SYSCTL_PPTIMER_P4       0x00000010  // Timer 4 Present
#define SYSCTL_PPTIMER_P3       0x00000008  // Timer 3 Present
#define SYSCTL_PPTIMER_P2       0x00000004  // Timer 2 Present
#define SYSCTL_PPTIMER_P1       0x00000002  // Timer 1 Present
#define SYSCTL_PPTIMER_P0       0x00000001  // Timer 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPGPIO register.
//
//*****************************************************************************
#define SYSCTL_PPGPIO_P14       0x00004000  // GPIO Port Q Present
#define SYSCTL_PPGPIO_P13       0x00002000  // GPIO Port P Present
#define SYSCTL_PPGPIO_P12       0x00001000  // GPIO Port N Present
#define SYSCTL_PPGPIO_P11       0x00000800  // GPIO Port M Present
#define SYSCTL_PPGPIO_P10       0x00000400  // GPIO Port L Present
#define SYSCTL_PPGPIO_P9        0x00000200  // GPIO Port K Present
#define SYSCTL_PPGPIO_P8        0x00000100  // GPIO Port J Present
#define SYSCTL_PPGPIO_P7        0x00000080  // GPIO Port H Present
#define SYSCTL_PPGPIO_P6        0x00000040  // GPIO Port G Present
#define SYSCTL_PPGPIO_P5        0x00000020  // GPIO Port F Present
#define SYSCTL_PPGPIO_P4        0x00000010  // GPIO Port E Present
#define SYSCTL_PPGPIO_P3        0x00000008  // GPIO Port D Present
#define SYSCTL_PPGPIO_P2        0x00000004  // GPIO Port C Present
#define SYSCTL_PPGPIO_P1        0x00000002  // GPIO Port B Present
#define SYSCTL_PPGPIO_P0        0x00000001  // GPIO Port A Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPDMA register.
//
//*****************************************************************************
#define SYSCTL_PPDMA_P0         0x00000001  // uDMA Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPHIB register.
//
//*****************************************************************************
#define SYSCTL_PPHIB_P0         0x00000001  // Hibernation Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPUART register.
//
//*****************************************************************************
#define SYSCTL_PPUART_P7        0x00000080  // UART Module 7 Present
#define SYSCTL_PPUART_P6        0x00000040  // UART Module 6 Present
#define SYSCTL_PPUART_P5        0x00000020  // UART Module 5 Present
#define SYSCTL_PPUART_P4        0x00000010  // UART Module 4 Present
#define SYSCTL_PPUART_P3        0x00000008  // UART Module 3 Present
#define SYSCTL_PPUART_P2        0x00000004  // UART Module 2 Present
#define SYSCTL_PPUART_P1        0x00000002  // UART Module 1 Present
#define SYSCTL_PPUART_P0        0x00000001  // UART Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPSSI register.
//
//*****************************************************************************
#define SYSCTL_PPSSI_P3         0x00000008  // SSI Module 3 Present
#define SYSCTL_PPSSI_P2         0x00000004  // SSI Module 2 Present
#define SYSCTL_PPSSI_P1         0x00000002  // SSI Module 1 Present
#define SYSCTL_PPSSI_P0         0x00000001  // SSI Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPI2C register.
//
//*****************************************************************************
#define SYSCTL_PPI2C_P5         0x00000020  // I2C Module 5 Present
#define SYSCTL_PPI2C_P4         0x00000010  // I2C Module 4 Present
#define SYSCTL_PPI2C_P3         0x00000008  // I2C Module 3 Present
#define SYSCTL_PPI2C_P2         0x00000004  // I2C Module 2 Present
#define SYSCTL_PPI2C_P1         0x00000002  // I2C Module 1 Present
#define SYSCTL_PPI2C_P0         0x00000001  // I2C Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPUSB register.
//
//*****************************************************************************
#define SYSCTL_PPUSB_P0         0x00000001  // USB Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPCAN register.
//
//*****************************************************************************
#define SYSCTL_PPCAN_P1         0x00000002  // CAN Module 1 Present
#define SYSCTL_PPCAN_P0         0x00000001  // CAN Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPADC register.
//
//*****************************************************************************
#define SYSCTL_PPADC_P1         0x00000002  // ADC Module 1 Present
#define SYSCTL_PPADC_P0         0x00000001  // ADC Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPACMP register.
//
//*****************************************************************************
#define SYSCTL_PPACMP_P0        0x00000001  // Analog Comparator Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPPWM register.
//
//*****************************************************************************
#define SYSCTL_PPPWM_P1         0x00000002  // PWM Module 1 Present
#define SYSCTL_PPPWM_P0         0x00000001  // PWM Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPQEI register.
//
//*****************************************************************************
#define SYSCTL_PPQEI_P1         0x00000002  // QEI Module 1 Present
#define SYSCTL_PPQEI_P0         0x00000001  // QEI Module 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPLPC register.
//
//*****************************************************************************
#define SYSCTL_PPLPC_P0         0x00000001  // LPC Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPPECI register.
//
//*****************************************************************************
#define SYSCTL_PPPECI_P0        0x00000001  // PECI Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPFAN register.
//
//*****************************************************************************
#define SYSCTL_PPFAN_P0         0x00000001  // FAN Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPEEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_PPEEPROM_P0      0x00000001  // EEPROM Module Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PPWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_PPWTIMER_P5      0x00000020  // Wide Timer 5 Present
#define SYSCTL_PPWTIMER_P4      0x00000010  // Wide Timer 4 Present
#define SYSCTL_PPWTIMER_P3      0x00000008  // Wide Timer 3 Present
#define SYSCTL_PPWTIMER_P2      0x00000004  // Wide Timer 2 Present
#define SYSCTL_PPWTIMER_P1      0x00000002  // Wide Timer 1 Present
#define SYSCTL_PPWTIMER_P0      0x00000001  // Wide Timer 0 Present

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRWD register.
//
//*****************************************************************************
#define SYSCTL_SRWD_R1          0x00000002  // Watchdog Timer 1 Software Reset
#define SYSCTL_SRWD_R0          0x00000001  // Watchdog Timer 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRTIMER register.
//
//*****************************************************************************
#define SYSCTL_SRTIMER_R5       0x00000020  // Timer 5 Software Reset
#define SYSCTL_SRTIMER_R4       0x00000010  // Timer 4 Software Reset
#define SYSCTL_SRTIMER_R3       0x00000008  // Timer 3 Software Reset
#define SYSCTL_SRTIMER_R2       0x00000004  // Timer 2 Software Reset
#define SYSCTL_SRTIMER_R1       0x00000002  // Timer 1 Software Reset
#define SYSCTL_SRTIMER_R0       0x00000001  // Timer 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRGPIO register.
//
//*****************************************************************************
#define SYSCTL_SRGPIO_R14       0x00004000  // GPIO Port Q Software Reset
#define SYSCTL_SRGPIO_R13       0x00002000  // GPIO Port P Software Reset
#define SYSCTL_SRGPIO_R12       0x00001000  // GPIO Port N Software Reset
#define SYSCTL_SRGPIO_R11       0x00000800  // GPIO Port M Software Reset
#define SYSCTL_SRGPIO_R10       0x00000400  // GPIO Port L Software Reset
#define SYSCTL_SRGPIO_R9        0x00000200  // GPIO Port K Software Reset
#define SYSCTL_SRGPIO_R8        0x00000100  // GPIO Port J Software Reset
#define SYSCTL_SRGPIO_R7        0x00000080  // GPIO Port H Software Reset
#define SYSCTL_SRGPIO_R6        0x00000040  // GPIO Port G Software Reset
#define SYSCTL_SRGPIO_R5        0x00000020  // GPIO Port F Software Reset
#define SYSCTL_SRGPIO_R4        0x00000010  // GPIO Port E Software Reset
#define SYSCTL_SRGPIO_R3        0x00000008  // GPIO Port D Software Reset
#define SYSCTL_SRGPIO_R2        0x00000004  // GPIO Port C Software Reset
#define SYSCTL_SRGPIO_R1        0x00000002  // GPIO Port B Software Reset
#define SYSCTL_SRGPIO_R0        0x00000001  // GPIO Port A Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRDMA register.
//
//*****************************************************************************
#define SYSCTL_SRDMA_R0         0x00000001  // uDMA Module Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRHIB register.
//
//*****************************************************************************
#define SYSCTL_SRHIB_R0         0x00000001  // Hibernation Module Software
                                            // Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRUART register.
//
//*****************************************************************************
#define SYSCTL_SRUART_R7        0x00000080  // UART Module 7 Software Reset
#define SYSCTL_SRUART_R6        0x00000040  // UART Module 6 Software Reset
#define SYSCTL_SRUART_R5        0x00000020  // UART Module 5 Software Reset
#define SYSCTL_SRUART_R4        0x00000010  // UART Module 4 Software Reset
#define SYSCTL_SRUART_R3        0x00000008  // UART Module 3 Software Reset
#define SYSCTL_SRUART_R2        0x00000004  // UART Module 2 Software Reset
#define SYSCTL_SRUART_R1        0x00000002  // UART Module 1 Software Reset
#define SYSCTL_SRUART_R0        0x00000001  // UART Module 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRSSI register.
//
//*****************************************************************************
#define SYSCTL_SRSSI_R3         0x00000008  // SSI Module 3 Software Reset
#define SYSCTL_SRSSI_R2         0x00000004  // SSI Module 2 Software Reset
#define SYSCTL_SRSSI_R1         0x00000002  // SSI Module 1 Software Reset
#define SYSCTL_SRSSI_R0         0x00000001  // SSI Module 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRI2C register.
//
//*****************************************************************************
#define SYSCTL_SRI2C_R5         0x00000020  // I2C Module 5 Software Reset
#define SYSCTL_SRI2C_R4         0x00000010  // I2C Module 4 Software Reset
#define SYSCTL_SRI2C_R3         0x00000008  // I2C Module 3 Software Reset
#define SYSCTL_SRI2C_R2         0x00000004  // I2C Module 2 Software Reset
#define SYSCTL_SRI2C_R1         0x00000002  // I2C Module 1 Software Reset
#define SYSCTL_SRI2C_R0         0x00000001  // I2C Module 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRADC register.
//
//*****************************************************************************
#define SYSCTL_SRADC_R1         0x00000002  // ADC Module 1 Software Reset
#define SYSCTL_SRADC_R0         0x00000001  // ADC Module 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRACMP register.
//
//*****************************************************************************
#define SYSCTL_SRACMP_R0        0x00000001  // Analog Comparator Module 0
                                            // Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRLPC register.
//
//*****************************************************************************
#define SYSCTL_SRLPC_R0         0x00000001  // LPC Module Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRPECI register.
//
//*****************************************************************************
#define SYSCTL_SRPECI_R0        0x00000001  // PECI Module Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRFAN register.
//
//*****************************************************************************
#define SYSCTL_SRFAN_R0         0x00000001  // FAN Module Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SREEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_SREEPROM_R0      0x00000001  // EEPROM Module Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SRWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_SRWTIMER_R5      0x00000020  // Wide Timer 5 Software Reset
#define SYSCTL_SRWTIMER_R4      0x00000010  // Wide Timer 4 Software Reset
#define SYSCTL_SRWTIMER_R3      0x00000008  // Wide Timer 3 Software Reset
#define SYSCTL_SRWTIMER_R2      0x00000004  // Wide Timer 2 Software Reset
#define SYSCTL_SRWTIMER_R1      0x00000002  // Wide Timer 1 Software Reset
#define SYSCTL_SRWTIMER_R0      0x00000001  // Wide Timer 0 Software Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCWD register.
//
//*****************************************************************************
#define SYSCTL_RCGCWD_R1        0x00000002  // Watchdog Timer 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCWD_R0        0x00000001  // Watchdog Timer 0 Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCTIMER_R5     0x00000020  // Timer 5 Run Mode Clock Gating
                                            // Control
#define SYSCTL_RCGCTIMER_R4     0x00000010  // Timer 4 Run Mode Clock Gating
                                            // Control
#define SYSCTL_RCGCTIMER_R3     0x00000008  // Timer 3 Run Mode Clock Gating
                                            // Control
#define SYSCTL_RCGCTIMER_R2     0x00000004  // Timer 2 Run Mode Clock Gating
                                            // Control
#define SYSCTL_RCGCTIMER_R1     0x00000002  // Timer 1 Run Mode Clock Gating
                                            // Control
#define SYSCTL_RCGCTIMER_R0     0x00000001  // Timer 0 Run Mode Clock Gating
                                            // Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCGPIO
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCGPIO_R14     0x00004000  // GPIO Port Q Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R13     0x00002000  // GPIO Port P Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R12     0x00001000  // GPIO Port N Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R11     0x00000800  // GPIO Port M Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R10     0x00000400  // GPIO Port L Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R9      0x00000200  // GPIO Port K Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R8      0x00000100  // GPIO Port J Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R7      0x00000080  // GPIO Port H Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R6      0x00000040  // GPIO Port G Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R5      0x00000020  // GPIO Port F Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R4      0x00000010  // GPIO Port E Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R3      0x00000008  // GPIO Port D Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R2      0x00000004  // GPIO Port C Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R1      0x00000002  // GPIO Port B Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R0      0x00000001  // GPIO Port A Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCDMA register.
//
//*****************************************************************************
#define SYSCTL_RCGCDMA_R0       0x00000001  // uDMA Module Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCHIB register.
//
//*****************************************************************************
#define SYSCTL_RCGCHIB_R0       0x00000001  // Hibernation Module Run Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCUART
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCUART_R7      0x00000080  // UART Module 7 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R6      0x00000040  // UART Module 6 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R5      0x00000020  // UART Module 5 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R4      0x00000010  // UART Module 4 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R3      0x00000008  // UART Module 3 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R2      0x00000004  // UART Module 2 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R1      0x00000002  // UART Module 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCUART_R0      0x00000001  // UART Module 0 Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCSSI register.
//
//*****************************************************************************
#define SYSCTL_RCGCSSI_R3       0x00000008  // SSI Module 3 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCSSI_R2       0x00000004  // SSI Module 2 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCSSI_R1       0x00000002  // SSI Module 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCSSI_R0       0x00000001  // SSI Module 0 Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCI2C register.
//
//*****************************************************************************
#define SYSCTL_RCGCI2C_R5       0x00000020  // I2C Module 5 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCI2C_R4       0x00000010  // I2C Module 4 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCI2C_R3       0x00000008  // I2C Module 3 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCI2C_R2       0x00000004  // I2C Module 2 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCI2C_R1       0x00000002  // I2C Module 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCI2C_R0       0x00000001  // I2C Module 0 Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCADC register.
//
//*****************************************************************************
#define SYSCTL_RCGCADC_R1       0x00000002  // ADC Module 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCADC_R0       0x00000001  // ADC Module 0 Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCACMP
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCACMP_R0      0x00000001  // Analog Comparator Module 0 Run
                                            // Mode Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCLPC register.
//
//*****************************************************************************
#define SYSCTL_RCGCLPC_R0       0x00000001  // LPC Module Run Mode Clock Gating
                                            // Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCPECI
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCPECI_R0      0x00000001  // PECI Module Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCFAN register.
//
//*****************************************************************************
#define SYSCTL_RCGCFAN_R0       0x00000001  // FAN Module Run Mode Clock Gating
                                            // Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCEEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCEEPROM_R0    0x00000001  // EEPROM Module Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_RCGCWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_RCGCWTIMER_R5    0x00000020  // Wide Timer 5 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCWTIMER_R4    0x00000010  // Wide Timer 4 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCWTIMER_R3    0x00000008  // Wide Timer 3 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCWTIMER_R2    0x00000004  // Wide Timer 2 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCWTIMER_R1    0x00000002  // Wide Timer 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCWTIMER_R0    0x00000001  // Wide Timer 0 Run Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCWD register.
//
//*****************************************************************************
#define SYSCTL_SCGCWD_S1        0x00000002  // Watchdog Timer 1 Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_SCGCWD_S0        0x00000001  // Watchdog Timer 0 Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCTIMER_S5     0x00000020  // Timer 5 Sleep Mode Clock Gating
                                            // Control
#define SYSCTL_SCGCTIMER_S4     0x00000010  // Timer 4 Sleep Mode Clock Gating
                                            // Control
#define SYSCTL_SCGCTIMER_S3     0x00000008  // Timer 3 Sleep Mode Clock Gating
                                            // Control
#define SYSCTL_SCGCTIMER_S2     0x00000004  // Timer 2 Sleep Mode Clock Gating
                                            // Control
#define SYSCTL_SCGCTIMER_S1     0x00000002  // Timer 1 Sleep Mode Clock Gating
                                            // Control
#define SYSCTL_SCGCTIMER_S0     0x00000001  // Timer 0 Sleep Mode Clock Gating
                                            // Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCGPIO
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCGPIO_S14     0x00004000  // GPIO Port Q Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S13     0x00002000  // GPIO Port P Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S12     0x00001000  // GPIO Port N Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S11     0x00000800  // GPIO Port M Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S10     0x00000400  // GPIO Port L Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S9      0x00000200  // GPIO Port K Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S8      0x00000100  // GPIO Port J Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S7      0x00000080  // GPIO Port H Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S6      0x00000040  // GPIO Port G Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S5      0x00000020  // GPIO Port F Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S4      0x00000010  // GPIO Port E Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S3      0x00000008  // GPIO Port D Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S2      0x00000004  // GPIO Port C Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S1      0x00000002  // GPIO Port B Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCGPIO_S0      0x00000001  // GPIO Port A Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCDMA register.
//
//*****************************************************************************
#define SYSCTL_SCGCDMA_S0       0x00000001  // uDMA Module Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCHIB register.
//
//*****************************************************************************
#define SYSCTL_SCGCHIB_S0       0x00000001  // Hibernation Module Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCUART
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCUART_S7      0x00000080  // UART Module 7 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S6      0x00000040  // UART Module 6 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S5      0x00000020  // UART Module 5 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S4      0x00000010  // UART Module 4 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S3      0x00000008  // UART Module 3 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S2      0x00000004  // UART Module 2 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S1      0x00000002  // UART Module 1 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCUART_S0      0x00000001  // UART Module 0 Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCSSI register.
//
//*****************************************************************************
#define SYSCTL_SCGCSSI_S3       0x00000008  // SSI Module 3 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCSSI_S2       0x00000004  // SSI Module 2 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCSSI_S1       0x00000002  // SSI Module 1 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCSSI_S0       0x00000001  // SSI Module 0 Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCI2C register.
//
//*****************************************************************************
#define SYSCTL_SCGCI2C_S5       0x00000020  // I2C Module 5 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCI2C_S4       0x00000010  // I2C Module 4 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCI2C_S3       0x00000008  // I2C Module 3 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCI2C_S2       0x00000004  // I2C Module 2 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCI2C_S1       0x00000002  // I2C Module 1 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCI2C_S0       0x00000001  // I2C Module 0 Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCADC register.
//
//*****************************************************************************
#define SYSCTL_SCGCADC_S1       0x00000002  // ADC Module 1 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCADC_S0       0x00000001  // ADC Module 0 Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCACMP
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCACMP_S0      0x00000001  // Analog Comparator Module 0 Sleep
                                            // Mode Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCLPC register.
//
//*****************************************************************************
#define SYSCTL_SCGCLPC_S0       0x00000001  // LPC Module Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCPECI
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCPECI_S0      0x00000001  // PECI Module Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCFAN register.
//
//*****************************************************************************
#define SYSCTL_SCGCFAN_S0       0x00000001  // FAN Module Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCEEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCEEPROM_S0    0x00000001  // EEPROM Module Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_SCGCWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_SCGCWTIMER_S5    0x00000020  // Wide Timer 5 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCWTIMER_S4    0x00000010  // Wide Timer 4 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCWTIMER_S3    0x00000008  // Wide Timer 3 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCWTIMER_S2    0x00000004  // Wide Timer 2 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCWTIMER_S1    0x00000002  // Wide Timer 1 Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_SCGCWTIMER_S0    0x00000001  // Wide Timer 0 Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCWD register.
//
//*****************************************************************************
#define SYSCTL_DCGCWD_D1        0x00000002  // Watchdog Timer 1 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCWD_D0        0x00000001  // Watchdog Timer 0 Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCTIMER_D5     0x00000020  // Timer 5 Deep-Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_DCGCTIMER_D4     0x00000010  // Timer 4 Deep-Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_DCGCTIMER_D3     0x00000008  // Timer 3 Deep-Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_DCGCTIMER_D2     0x00000004  // Timer 2 Deep-Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_DCGCTIMER_D1     0x00000002  // Timer 1 Deep-Sleep Mode Clock
                                            // Gating Control
#define SYSCTL_DCGCTIMER_D0     0x00000001  // Timer 0 Deep-Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCGPIO
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCGPIO_D14     0x00004000  // GPIO Port Q Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D13     0x00002000  // GPIO Port P Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D12     0x00001000  // GPIO Port N Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D11     0x00000800  // GPIO Port M Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D10     0x00000400  // GPIO Port L Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D9      0x00000200  // GPIO Port K Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D8      0x00000100  // GPIO Port J Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D7      0x00000080  // GPIO Port H Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D6      0x00000040  // GPIO Port G Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D5      0x00000020  // GPIO Port F Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D4      0x00000010  // GPIO Port E Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D3      0x00000008  // GPIO Port D Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D2      0x00000004  // GPIO Port C Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D1      0x00000002  // GPIO Port B Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCGPIO_D0      0x00000001  // GPIO Port A Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCDMA register.
//
//*****************************************************************************
#define SYSCTL_DCGCDMA_D0       0x00000001  // uDMA Module Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCHIB register.
//
//*****************************************************************************
#define SYSCTL_DCGCHIB_D0       0x00000001  // Hibernation Module Deep-Sleep
                                            // Mode Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCUART
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCUART_D7      0x00000080  // UART Module 7 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D6      0x00000040  // UART Module 6 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D5      0x00000020  // UART Module 5 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D4      0x00000010  // UART Module 4 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D3      0x00000008  // UART Module 3 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D2      0x00000004  // UART Module 2 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D1      0x00000002  // UART Module 1 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCUART_D0      0x00000001  // UART Module 0 Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCSSI register.
//
//*****************************************************************************
#define SYSCTL_DCGCSSI_D3       0x00000008  // SSI Module 3 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCSSI_D2       0x00000004  // SSI Module 2 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCSSI_D1       0x00000002  // SSI Module 1 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCSSI_D0       0x00000001  // SSI Module 0 Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCI2C register.
//
//*****************************************************************************
#define SYSCTL_DCGCI2C_D5       0x00000020  // I2C Module 5 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCI2C_D4       0x00000010  // I2C Module 4 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCI2C_D3       0x00000008  // I2C Module 3 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCI2C_D2       0x00000004  // I2C Module 2 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCI2C_D1       0x00000002  // I2C Module 1 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCI2C_D0       0x00000001  // I2C Module 0 Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCADC register.
//
//*****************************************************************************
#define SYSCTL_DCGCADC_D1       0x00000002  // ADC Module 1 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCADC_D0       0x00000001  // ADC Module 0 Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCACMP
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCACMP_D0      0x00000001  // Analog Comparator Module 0
                                            // Deep-Sleep Mode Clock Gating
                                            // Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCLPC register.
//
//*****************************************************************************
#define SYSCTL_DCGCLPC_D0       0x00000001  // LPC Module Deep-Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCPECI
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCPECI_D0      0x00000001  // PECI Module Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCFAN register.
//
//*****************************************************************************
#define SYSCTL_DCGCFAN_D0       0x00000001  // FAN Module Deep-Sleep Mode Clock
                                            // Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCEEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCEEPROM_D0    0x00000001  // EEPROM Module Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_DCGCWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_DCGCWTIMER_D5    0x00000020  // Wide Timer 5 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCWTIMER_D4    0x00000010  // Wide Timer 4 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCWTIMER_D3    0x00000008  // Wide Timer 3 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCWTIMER_D2    0x00000004  // Wide Timer 2 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCWTIMER_D1    0x00000002  // Wide Timer 1 Deep-Sleep Mode
                                            // Clock Gating Control
#define SYSCTL_DCGCWTIMER_D0    0x00000001  // Wide Timer 0 Deep-Sleep Mode
                                            // Clock Gating Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCWD register.
//
//*****************************************************************************
#define SYSCTL_PCWD_P1          0x00000002  // Watchdog Timer 1 Power Control
#define SYSCTL_PCWD_P0          0x00000001  // Watchdog Timer 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCTIMER register.
//
//*****************************************************************************
#define SYSCTL_PCTIMER_P5       0x00000020  // Timer 5 Power Control
#define SYSCTL_PCTIMER_P4       0x00000010  // Timer 4 Power Control
#define SYSCTL_PCTIMER_P3       0x00000008  // Timer 3 Power Control
#define SYSCTL_PCTIMER_P2       0x00000004  // Timer 2 Power Control
#define SYSCTL_PCTIMER_P1       0x00000002  // Timer 1 Power Control
#define SYSCTL_PCTIMER_P0       0x00000001  // Timer 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCGPIO register.
//
//*****************************************************************************
#define SYSCTL_PCGPIO_P14       0x00004000  // GPIO Port Q Power Control
#define SYSCTL_PCGPIO_P13       0x00002000  // GPIO Port P Power Control
#define SYSCTL_PCGPIO_P12       0x00001000  // GPIO Port N Power Control
#define SYSCTL_PCGPIO_P11       0x00000800  // GPIO Port M Power Control
#define SYSCTL_PCGPIO_P10       0x00000400  // GPIO Port L Power Control
#define SYSCTL_PCGPIO_P9        0x00000200  // GPIO Port K Power Control
#define SYSCTL_PCGPIO_P8        0x00000100  // GPIO Port J Power Control
#define SYSCTL_PCGPIO_P7        0x00000080  // GPIO Port H Power Control
#define SYSCTL_PCGPIO_P6        0x00000040  // GPIO Port G Power Control
#define SYSCTL_PCGPIO_P5        0x00000020  // GPIO Port F Power Control
#define SYSCTL_PCGPIO_P4        0x00000010  // GPIO Port E Power Control
#define SYSCTL_PCGPIO_P3        0x00000008  // GPIO Port D Power Control
#define SYSCTL_PCGPIO_P2        0x00000004  // GPIO Port C Power Control
#define SYSCTL_PCGPIO_P1        0x00000002  // GPIO Port B Power Control
#define SYSCTL_PCGPIO_P0        0x00000001  // GPIO Port A Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCDMA register.
//
//*****************************************************************************
#define SYSCTL_PCDMA_P0         0x00000001  // uDMA Module Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCHIB register.
//
//*****************************************************************************
#define SYSCTL_PCHIB_P0         0x00000001  // Hibernation Module Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCUART register.
//
//*****************************************************************************
#define SYSCTL_PCUART_P7        0x00000080  // UART Module 7 Power Control
#define SYSCTL_PCUART_P6        0x00000040  // UART Module 6 Power Control
#define SYSCTL_PCUART_P5        0x00000020  // UART Module 5 Power Control
#define SYSCTL_PCUART_P4        0x00000010  // UART Module 4 Power Control
#define SYSCTL_PCUART_P3        0x00000008  // UART Module 3 Power Control
#define SYSCTL_PCUART_P2        0x00000004  // UART Module 2 Power Control
#define SYSCTL_PCUART_P1        0x00000002  // UART Module 1 Power Control
#define SYSCTL_PCUART_P0        0x00000001  // UART Module 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCSSI register.
//
//*****************************************************************************
#define SYSCTL_PCSSI_P3         0x00000008  // SSI Module 3 Power Control
#define SYSCTL_PCSSI_P2         0x00000004  // SSI Module 2 Power Control
#define SYSCTL_PCSSI_P1         0x00000002  // SSI Module 1 Power Control
#define SYSCTL_PCSSI_P0         0x00000001  // SSI Module 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCI2C register.
//
//*****************************************************************************
#define SYSCTL_PCI2C_P5         0x00000020  // I2C Module 5 Power Control
#define SYSCTL_PCI2C_P4         0x00000010  // I2C Module 4 Power Control
#define SYSCTL_PCI2C_P3         0x00000008  // I2C Module 3 Power Control
#define SYSCTL_PCI2C_P2         0x00000004  // I2C Module 2 Power Control
#define SYSCTL_PCI2C_P1         0x00000002  // I2C Module 1 Power Control
#define SYSCTL_PCI2C_P0         0x00000001  // I2C Module 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCADC register.
//
//*****************************************************************************
#define SYSCTL_PCADC_P1         0x00000002  // ADC Module 1 Power Control
#define SYSCTL_PCADC_P0         0x00000001  // ADC Module 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCACMP register.
//
//*****************************************************************************
#define SYSCTL_PCACMP_P0        0x00000001  // Analog Comparator Module 0 Power
                                            // Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCLPC register.
//
//*****************************************************************************
#define SYSCTL_PCLPC_P0         0x00000001  // LPC Module Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCPECI register.
//
//*****************************************************************************
#define SYSCTL_PCPECI_P0        0x00000001  // PECI Module Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCFAN register.
//
//*****************************************************************************
#define SYSCTL_PCFAN_P0         0x00000001  // FAN Module Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCEEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_PCEEPROM_P0      0x00000001  // EEPROM Module Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PCWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_PCWTIMER_P5      0x00000020  // Wide Timer 5 Power Control
#define SYSCTL_PCWTIMER_P4      0x00000010  // Wide Timer 4 Power Control
#define SYSCTL_PCWTIMER_P3      0x00000008  // Wide Timer 3 Power Control
#define SYSCTL_PCWTIMER_P2      0x00000004  // Wide Timer 2 Power Control
#define SYSCTL_PCWTIMER_P1      0x00000002  // Wide Timer 1 Power Control
#define SYSCTL_PCWTIMER_P0      0x00000001  // Wide Timer 0 Power Control

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRWD register.
//
//*****************************************************************************
#define SYSCTL_PRWD_R1          0x00000002  // Watchdog Timer 1 Peripheral
                                            // Ready
#define SYSCTL_PRWD_R0          0x00000001  // Watchdog Timer 0 Peripheral
                                            // Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRTIMER register.
//
//*****************************************************************************
#define SYSCTL_PRTIMER_R5       0x00000020  // Timer 5 Peripheral Ready
#define SYSCTL_PRTIMER_R4       0x00000010  // Timer 4 Peripheral Ready
#define SYSCTL_PRTIMER_R3       0x00000008  // Timer 3 Peripheral Ready
#define SYSCTL_PRTIMER_R2       0x00000004  // Timer 2 Peripheral Ready
#define SYSCTL_PRTIMER_R1       0x00000002  // Timer 1 Peripheral Ready
#define SYSCTL_PRTIMER_R0       0x00000001  // Timer 0 Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRGPIO register.
//
//*****************************************************************************
#define SYSCTL_PRGPIO_R14       0x00004000  // GPIO Port Q Peripheral Ready
#define SYSCTL_PRGPIO_R13       0x00002000  // GPIO Port P Peripheral Ready
#define SYSCTL_PRGPIO_R12       0x00001000  // GPIO Port N Peripheral Ready
#define SYSCTL_PRGPIO_R11       0x00000800  // GPIO Port M Peripheral Ready
#define SYSCTL_PRGPIO_R10       0x00000400  // GPIO Port L Peripheral Ready
#define SYSCTL_PRGPIO_R9        0x00000200  // GPIO Port K Peripheral Ready
#define SYSCTL_PRGPIO_R8        0x00000100  // GPIO Port J Peripheral Ready
#define SYSCTL_PRGPIO_R7        0x00000080  // GPIO Port H Peripheral Ready
#define SYSCTL_PRGPIO_R6        0x00000040  // GPIO Port G Peripheral Ready
#define SYSCTL_PRGPIO_R5        0x00000020  // GPIO Port F Peripheral Ready
#define SYSCTL_PRGPIO_R4        0x00000010  // GPIO Port E Peripheral Ready
#define SYSCTL_PRGPIO_R3        0x00000008  // GPIO Port D Peripheral Ready
#define SYSCTL_PRGPIO_R2        0x00000004  // GPIO Port C Peripheral Ready
#define SYSCTL_PRGPIO_R1        0x00000002  // GPIO Port B Peripheral Ready
#define SYSCTL_PRGPIO_R0        0x00000001  // GPIO Port A Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRDMA register.
//
//*****************************************************************************
#define SYSCTL_PRDMA_R0         0x00000001  // uDMA Module Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRHIB register.
//
//*****************************************************************************
#define SYSCTL_PRHIB_R0         0x00000001  // Hibernation Module Peripheral
                                            // Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRUART register.
//
//*****************************************************************************
#define SYSCTL_PRUART_R7        0x00000080  // UART Module 7 Peripheral Ready
#define SYSCTL_PRUART_R6        0x00000040  // UART Module 6 Peripheral Ready
#define SYSCTL_PRUART_R5        0x00000020  // UART Module 5 Peripheral Ready
#define SYSCTL_PRUART_R4        0x00000010  // UART Module 4 Peripheral Ready
#define SYSCTL_PRUART_R3        0x00000008  // UART Module 3 Peripheral Ready
#define SYSCTL_PRUART_R2        0x00000004  // UART Module 2 Peripheral Ready
#define SYSCTL_PRUART_R1        0x00000002  // UART Module 1 Peripheral Ready
#define SYSCTL_PRUART_R0        0x00000001  // UART Module 0 Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRSSI register.
//
//*****************************************************************************
#define SYSCTL_PRSSI_R3         0x00000008  // SSI Module 3 Peripheral Ready
#define SYSCTL_PRSSI_R2         0x00000004  // SSI Module 2 Peripheral Ready
#define SYSCTL_PRSSI_R1         0x00000002  // SSI Module 1 Peripheral Ready
#define SYSCTL_PRSSI_R0         0x00000001  // SSI Module 0 Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRI2C register.
//
//*****************************************************************************
#define SYSCTL_PRI2C_R5         0x00000020  // I2C Module 5 Peripheral Ready
#define SYSCTL_PRI2C_R4         0x00000010  // I2C Module 4 Peripheral Ready
#define SYSCTL_PRI2C_R3         0x00000008  // I2C Module 3 Peripheral Ready
#define SYSCTL_PRI2C_R2         0x00000004  // I2C Module 2 Peripheral Ready
#define SYSCTL_PRI2C_R1         0x00000002  // I2C Module 1 Peripheral Ready
#define SYSCTL_PRI2C_R0         0x00000001  // I2C Module 0 Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRADC register.
//
//*****************************************************************************
#define SYSCTL_PRADC_R1         0x00000002  // ADC Module 1 Peripheral Ready
#define SYSCTL_PRADC_R0         0x00000001  // ADC Module 0 Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRACMP register.
//
//*****************************************************************************
#define SYSCTL_PRACMP_R0        0x00000001  // Analog Comparator Module 0
                                            // Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRLPC register.
//
//*****************************************************************************
#define SYSCTL_PRLPC_R0         0x00000001  // LPC Module Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRPECI register.
//
//*****************************************************************************
#define SYSCTL_PRPECI_R0        0x00000001  // PECI Module Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRFAN register.
//
//*****************************************************************************
#define SYSCTL_PRFAN_R0         0x00000001  // FAN Module Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PREEPROM
// register.
//
//*****************************************************************************
#define SYSCTL_PREEPROM_R0      0x00000001  // EEPROM Module Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the SYSCTL_PRWTIMER
// register.
//
//*****************************************************************************
#define SYSCTL_PRWTIMER_R5      0x00000020  // Wide Timer 5 Peripheral Ready
#define SYSCTL_PRWTIMER_R4      0x00000010  // Wide Timer 4 Peripheral Ready
#define SYSCTL_PRWTIMER_R3      0x00000008  // Wide Timer 3 Peripheral Ready
#define SYSCTL_PRWTIMER_R2      0x00000004  // Wide Timer 2 Peripheral Ready
#define SYSCTL_PRWTIMER_R1      0x00000002  // Wide Timer 1 Peripheral Ready
#define SYSCTL_PRWTIMER_R0      0x00000001  // Wide Timer 0 Peripheral Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_STAT register.
//
//*****************************************************************************
#define UDMA_STAT_DMACHANS_M    0x001F0000  // Available uDMA Channels Minus 1
#define UDMA_STAT_STATE_M       0x000000F0  // Control State Machine Status
#define UDMA_STAT_STATE_IDLE    0x00000000  // Idle
#define UDMA_STAT_STATE_RD_CTRL 0x00000010  // Reading channel controller data
#define UDMA_STAT_STATE_RD_SRCENDP \
                                0x00000020  // Reading source end pointer
#define UDMA_STAT_STATE_RD_DSTENDP \
                                0x00000030  // Reading destination end pointer
#define UDMA_STAT_STATE_RD_SRCDAT \
                                0x00000040  // Reading source data
#define UDMA_STAT_STATE_WR_DSTDAT \
                                0x00000050  // Writing destination data
#define UDMA_STAT_STATE_WAIT    0x00000060  // Waiting for uDMA request to
                                            // clear
#define UDMA_STAT_STATE_WR_CTRL 0x00000070  // Writing channel controller data
#define UDMA_STAT_STATE_STALL   0x00000080  // Stalled
#define UDMA_STAT_STATE_DONE    0x00000090  // Done
#define UDMA_STAT_STATE_UNDEF   0x000000A0  // Undefined
#define UDMA_STAT_MASTEN        0x00000001  // Master Enable Status
#define UDMA_STAT_DMACHANS_S    16

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CFG register.
//
//*****************************************************************************
#define UDMA_CFG_MASTEN         0x00000001  // Controller Master Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CTLBASE register.
//
//*****************************************************************************
#define UDMA_CTLBASE_ADDR_M     0xFFFFFC00  // Channel Control Base Address
#define UDMA_CTLBASE_ADDR_S     10

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ALTBASE register.
//
//*****************************************************************************
#define UDMA_ALTBASE_ADDR_M     0xFFFFFFFF  // Alternate Channel Address
                                            // Pointer
#define UDMA_ALTBASE_ADDR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_WAITSTAT register.
//
//*****************************************************************************
#define UDMA_WAITSTAT_WAITREQ_M 0xFFFFFFFF  // Channel [n] Wait Status

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_SWREQ register.
//
//*****************************************************************************
#define UDMA_SWREQ_M            0xFFFFFFFF  // Channel [n] Software Request

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_USEBURSTSET
// register.
//
//*****************************************************************************
#define UDMA_USEBURSTSET_SET_M  0xFFFFFFFF  // Channel [n] Useburst Set

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_USEBURSTCLR
// register.
//
//*****************************************************************************
#define UDMA_USEBURSTCLR_CLR_M  0xFFFFFFFF  // Channel [n] Useburst Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_REQMASKSET
// register.
//
//*****************************************************************************
#define UDMA_REQMASKSET_SET_M   0xFFFFFFFF  // Channel [n] Request Mask Set

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_REQMASKCLR
// register.
//
//*****************************************************************************
#define UDMA_REQMASKCLR_CLR_M   0xFFFFFFFF  // Channel [n] Request Mask Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ENASET register.
//
//*****************************************************************************
#define UDMA_ENASET_SET_M       0xFFFFFFFF  // Channel [n] Enable Set

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ENACLR register.
//
//*****************************************************************************
#define UDMA_ENACLR_CLR_M       0xFFFFFFFF  // Clear Channel [n] Enable Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ALTSET register.
//
//*****************************************************************************
#define UDMA_ALTSET_SET_M       0xFFFFFFFF  // Channel [n] Alternate Set

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ALTCLR register.
//
//*****************************************************************************
#define UDMA_ALTCLR_CLR_M       0xFFFFFFFF  // Channel [n] Alternate Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_PRIOSET register.
//
//*****************************************************************************
#define UDMA_PRIOSET_SET_M      0xFFFFFFFF  // Channel [n] Priority Set

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_PRIOCLR register.
//
//*****************************************************************************
#define UDMA_PRIOCLR_CLR_M      0xFFFFFFFF  // Channel [n] Priority Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ERRCLR register.
//
//*****************************************************************************
#define UDMA_ERRCLR_ERRCLR      0x00000001  // uDMA Bus Error Status

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHASGN register.
//
//*****************************************************************************
#define UDMA_CHASGN_M           0xFFFFFFFF  // Channel [n] Assignment Select
#define UDMA_CHASGN_PRIMARY     0x00000000  // Use the primary channel
                                            // assignment
#define UDMA_CHASGN_SECONDARY   0x00000001  // Use the secondary channel
                                            // assignment

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHIS register.
//
//*****************************************************************************
#define UDMA_CHIS_M             0xFFFFFFFF  // Channel [n] Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP0 register.
//
//*****************************************************************************
#define UDMA_CHMAP0_CH7SEL_M    0xF0000000  // uDMA Channel 7 Source Select
#define UDMA_CHMAP0_CH6SEL_M    0x0F000000  // uDMA Channel 6 Source Select
#define UDMA_CHMAP0_CH5SEL_M    0x00F00000  // uDMA Channel 5 Source Select
#define UDMA_CHMAP0_CH4SEL_M    0x000F0000  // uDMA Channel 4 Source Select
#define UDMA_CHMAP0_CH3SEL_M    0x0000F000  // uDMA Channel 3 Source Select
#define UDMA_CHMAP0_CH2SEL_M    0x00000F00  // uDMA Channel 2 Source Select
#define UDMA_CHMAP0_CH1SEL_M    0x000000F0  // uDMA Channel 1 Source Select
#define UDMA_CHMAP0_CH0SEL_M    0x0000000F  // uDMA Channel 0 Source Select
#define UDMA_CHMAP0_CH7SEL_S    28
#define UDMA_CHMAP0_CH6SEL_S    24
#define UDMA_CHMAP0_CH5SEL_S    20
#define UDMA_CHMAP0_CH4SEL_S    16
#define UDMA_CHMAP0_CH3SEL_S    12
#define UDMA_CHMAP0_CH2SEL_S    8
#define UDMA_CHMAP0_CH1SEL_S    4
#define UDMA_CHMAP0_CH0SEL_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP1 register.
//
//*****************************************************************************
#define UDMA_CHMAP1_CH15SEL_M   0xF0000000  // uDMA Channel 15 Source Select
#define UDMA_CHMAP1_CH14SEL_M   0x0F000000  // uDMA Channel 14 Source Select
#define UDMA_CHMAP1_CH13SEL_M   0x00F00000  // uDMA Channel 13 Source Select
#define UDMA_CHMAP1_CH12SEL_M   0x000F0000  // uDMA Channel 12 Source Select
#define UDMA_CHMAP1_CH11SEL_M   0x0000F000  // uDMA Channel 11 Source Select
#define UDMA_CHMAP1_CH10SEL_M   0x00000F00  // uDMA Channel 10 Source Select
#define UDMA_CHMAP1_CH9SEL_M    0x000000F0  // uDMA Channel 9 Source Select
#define UDMA_CHMAP1_CH8SEL_M    0x0000000F  // uDMA Channel 8 Source Select
#define UDMA_CHMAP1_CH15SEL_S   28
#define UDMA_CHMAP1_CH14SEL_S   24
#define UDMA_CHMAP1_CH13SEL_S   20
#define UDMA_CHMAP1_CH12SEL_S   16
#define UDMA_CHMAP1_CH11SEL_S   12
#define UDMA_CHMAP1_CH10SEL_S   8
#define UDMA_CHMAP1_CH9SEL_S    4
#define UDMA_CHMAP1_CH8SEL_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP2 register.
//
//*****************************************************************************
#define UDMA_CHMAP2_CH23SEL_M   0xF0000000  // uDMA Channel 23 Source Select
#define UDMA_CHMAP2_CH22SEL_M   0x0F000000  // uDMA Channel 22 Source Select
#define UDMA_CHMAP2_CH21SEL_M   0x00F00000  // uDMA Channel 21 Source Select
#define UDMA_CHMAP2_CH20SEL_M   0x000F0000  // uDMA Channel 20 Source Select
#define UDMA_CHMAP2_CH19SEL_M   0x0000F000  // uDMA Channel 19 Source Select
#define UDMA_CHMAP2_CH18SEL_M   0x00000F00  // uDMA Channel 18 Source Select
#define UDMA_CHMAP2_CH17SEL_M   0x000000F0  // uDMA Channel 17 Source Select
#define UDMA_CHMAP2_CH16SEL_M   0x0000000F  // uDMA Channel 16 Source Select
#define UDMA_CHMAP2_CH23SEL_S   28
#define UDMA_CHMAP2_CH22SEL_S   24
#define UDMA_CHMAP2_CH21SEL_S   20
#define UDMA_CHMAP2_CH20SEL_S   16
#define UDMA_CHMAP2_CH19SEL_S   12
#define UDMA_CHMAP2_CH18SEL_S   8
#define UDMA_CHMAP2_CH17SEL_S   4
#define UDMA_CHMAP2_CH16SEL_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP3 register.
//
//*****************************************************************************
#define UDMA_CHMAP3_CH31SEL_M   0xF0000000  // uDMA Channel 31 Source Select
#define UDMA_CHMAP3_CH30SEL_M   0x0F000000  // uDMA Channel 30 Source Select
#define UDMA_CHMAP3_CH29SEL_M   0x00F00000  // uDMA Channel 29 Source Select
#define UDMA_CHMAP3_CH28SEL_M   0x000F0000  // uDMA Channel 28 Source Select
#define UDMA_CHMAP3_CH27SEL_M   0x0000F000  // uDMA Channel 27 Source Select
#define UDMA_CHMAP3_CH26SEL_M   0x00000F00  // uDMA Channel 26 Source Select
#define UDMA_CHMAP3_CH25SEL_M   0x000000F0  // uDMA Channel 25 Source Select
#define UDMA_CHMAP3_CH24SEL_M   0x0000000F  // uDMA Channel 24 Source Select
#define UDMA_CHMAP3_CH31SEL_S   28
#define UDMA_CHMAP3_CH30SEL_S   24
#define UDMA_CHMAP3_CH29SEL_S   20
#define UDMA_CHMAP3_CH28SEL_S   16
#define UDMA_CHMAP3_CH27SEL_S   12
#define UDMA_CHMAP3_CH26SEL_S   8
#define UDMA_CHMAP3_CH25SEL_S   4
#define UDMA_CHMAP3_CH24SEL_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_SRCENDP register.
//
//*****************************************************************************
#define UDMA_SRCENDP_ADDR_M     0xFFFFFFFF  // Source Address End Pointer
#define UDMA_SRCENDP_ADDR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_DSTENDP register.
//
//*****************************************************************************
#define UDMA_DSTENDP_ADDR_M     0xFFFFFFFF  // Destination Address End Pointer
#define UDMA_DSTENDP_ADDR_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHCTL register.
//
//*****************************************************************************
#define UDMA_CHCTL_DSTINC_M     0xC0000000  // Destination Address Increment
#define UDMA_CHCTL_DSTINC_8     0x00000000  // Byte
#define UDMA_CHCTL_DSTINC_16    0x40000000  // Half-word
#define UDMA_CHCTL_DSTINC_32    0x80000000  // Word
#define UDMA_CHCTL_DSTINC_NONE  0xC0000000  // No increment
#define UDMA_CHCTL_DSTSIZE_M    0x30000000  // Destination Data Size
#define UDMA_CHCTL_DSTSIZE_8    0x00000000  // Byte
#define UDMA_CHCTL_DSTSIZE_16   0x10000000  // Half-word
#define UDMA_CHCTL_DSTSIZE_32   0x20000000  // Word
#define UDMA_CHCTL_SRCINC_M     0x0C000000  // Source Address Increment
#define UDMA_CHCTL_SRCINC_8     0x00000000  // Byte
#define UDMA_CHCTL_SRCINC_16    0x04000000  // Half-word
#define UDMA_CHCTL_SRCINC_32    0x08000000  // Word
#define UDMA_CHCTL_SRCINC_NONE  0x0C000000  // No increment
#define UDMA_CHCTL_SRCSIZE_M    0x03000000  // Source Data Size
#define UDMA_CHCTL_SRCSIZE_8    0x00000000  // Byte
#define UDMA_CHCTL_SRCSIZE_16   0x01000000  // Half-word
#define UDMA_CHCTL_SRCSIZE_32   0x02000000  // Word
#define UDMA_CHCTL_ARBSIZE_M    0x0003C000  // Arbitration Size
#define UDMA_CHCTL_ARBSIZE_1    0x00000000  // 1 Transfer
#define UDMA_CHCTL_ARBSIZE_2    0x00004000  // 2 Transfers
#define UDMA_CHCTL_ARBSIZE_4    0x00008000  // 4 Transfers
#define UDMA_CHCTL_ARBSIZE_8    0x0000C000  // 8 Transfers
#define UDMA_CHCTL_ARBSIZE_16   0x00010000  // 16 Transfers
#define UDMA_CHCTL_ARBSIZE_32   0x00014000  // 32 Transfers
#define UDMA_CHCTL_ARBSIZE_64   0x00018000  // 64 Transfers
#define UDMA_CHCTL_ARBSIZE_128  0x0001C000  // 128 Transfers
#define UDMA_CHCTL_ARBSIZE_256  0x00020000  // 256 Transfers
#define UDMA_CHCTL_ARBSIZE_512  0x00024000  // 512 Transfers
#define UDMA_CHCTL_ARBSIZE_1024 0x00028000  // 1024 Transfers
#define UDMA_CHCTL_XFERSIZE_M   0x00003FF0  // Transfer Size (minus 1)
#define UDMA_CHCTL_NXTUSEBURST  0x00000008  // Next Useburst
#define UDMA_CHCTL_XFERMODE_M   0x00000007  // uDMA Transfer Mode
#define UDMA_CHCTL_XFERMODE_STOP \
                                0x00000000  // Stop
#define UDMA_CHCTL_XFERMODE_BASIC \
                                0x00000001  // Basic
#define UDMA_CHCTL_XFERMODE_AUTO \
                                0x00000002  // Auto-Request
#define UDMA_CHCTL_XFERMODE_PINGPONG \
                                0x00000003  // Ping-Pong
#define UDMA_CHCTL_XFERMODE_MEM_SG \
                                0x00000004  // Memory Scatter-Gather
#define UDMA_CHCTL_XFERMODE_MEM_SGA \
                                0x00000005  // Alternate Memory Scatter-Gather
#define UDMA_CHCTL_XFERMODE_PER_SG \
                                0x00000006  // Peripheral Scatter-Gather
#define UDMA_CHCTL_XFERMODE_PER_SGA \
                                0x00000007  // Alternate Peripheral
                                            // Scatter-Gather
#define UDMA_CHCTL_XFERSIZE_S   4

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_INT_TYPE register.
//
//*****************************************************************************
#define NVIC_INT_TYPE_LINES_M   0x0000001F  // Number of interrupt lines (x32)
#define NVIC_INT_TYPE_LINES_S   0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ACTLR register.
//
//*****************************************************************************
#define NVIC_ACTLR_DISOOFP      0x00000200  // Disable Out-Of-Order Floating
                                            // Point
#define NVIC_ACTLR_DISFPCA      0x00000100  // Disable CONTROL
#define NVIC_ACTLR_DISFOLD      0x00000004  // Disable IT Folding
#define NVIC_ACTLR_DISWBUF      0x00000002  // Disable Write Buffer
#define NVIC_ACTLR_DISMCYC      0x00000001  // Disable Interrupts of Multiple
                                            // Cycle Instructions

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ST_CTRL register.
//
//*****************************************************************************
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count Flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt Enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ST_RELOAD register.
//
//*****************************************************************************
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Reload Value
#define NVIC_ST_RELOAD_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ST_CURRENT
// register.
//
//*****************************************************************************
#define NVIC_ST_CURRENT_M       0x00FFFFFF  // Current Value
#define NVIC_ST_CURRENT_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ST_CAL register.
//
//*****************************************************************************
#define NVIC_ST_CAL_NOREF       0x80000000  // No reference clock
#define NVIC_ST_CAL_SKEW        0x40000000  // Clock skew
#define NVIC_ST_CAL_ONEMS_M     0x00FFFFFF  // 1ms reference value
#define NVIC_ST_CAL_ONEMS_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_EN0 register.
//
//*****************************************************************************
#define NVIC_EN0_INT_M          0xFFFFFFFF  // Interrupt Enable
#define NVIC_EN0_INT0           0x00000001  // Interrupt 0 enable
#define NVIC_EN0_INT1           0x00000002  // Interrupt 1 enable
#define NVIC_EN0_INT2           0x00000004  // Interrupt 2 enable
#define NVIC_EN0_INT3           0x00000008  // Interrupt 3 enable
#define NVIC_EN0_INT4           0x00000010  // Interrupt 4 enable
#define NVIC_EN0_INT5           0x00000020  // Interrupt 5 enable
#define NVIC_EN0_INT6           0x00000040  // Interrupt 6 enable
#define NVIC_EN0_INT7           0x00000080  // Interrupt 7 enable
#define NVIC_EN0_INT8           0x00000100  // Interrupt 8 enable
#define NVIC_EN0_INT9           0x00000200  // Interrupt 9 enable
#define NVIC_EN0_INT10          0x00000400  // Interrupt 10 enable
#define NVIC_EN0_INT11          0x00000800  // Interrupt 11 enable
#define NVIC_EN0_INT12          0x00001000  // Interrupt 12 enable
#define NVIC_EN0_INT13          0x00002000  // Interrupt 13 enable
#define NVIC_EN0_INT14          0x00004000  // Interrupt 14 enable
#define NVIC_EN0_INT15          0x00008000  // Interrupt 15 enable
#define NVIC_EN0_INT16          0x00010000  // Interrupt 16 enable
#define NVIC_EN0_INT17          0x00020000  // Interrupt 17 enable
#define NVIC_EN0_INT18          0x00040000  // Interrupt 18 enable
#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define NVIC_EN0_INT20          0x00100000  // Interrupt 20 enable
#define NVIC_EN0_INT21          0x00200000  // Interrupt 21 enable
#define NVIC_EN0_INT22          0x00400000  // Interrupt 22 enable
#define NVIC_EN0_INT23          0x00800000  // Interrupt 23 enable
#define NVIC_EN0_INT24          0x01000000  // Interrupt 24 enable
#define NVIC_EN0_INT25          0x02000000  // Interrupt 25 enable
#define NVIC_EN0_INT26          0x04000000  // Interrupt 26 enable
#define NVIC_EN0_INT27          0x08000000  // Interrupt 27 enable
#define NVIC_EN0_INT28          0x10000000  // Interrupt 28 enable
#define NVIC_EN0_INT29          0x20000000  // Interrupt 29 enable
#define NVIC_EN0_INT30          0x40000000  // Interrupt 30 enable
#define NVIC_EN0_INT31          0x80000000  // Interrupt 31 enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_EN1 register.
//
//*****************************************************************************
#define NVIC_EN1_INT_M          0xFFFFFFFF  // Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_EN2 register.
//
//*****************************************************************************
#define NVIC_EN2_INT_M          0xFFFFFFFF  // Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_EN3 register.
//
//*****************************************************************************
#define NVIC_EN3_INT_M          0xFFFFFFFF  // Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_EN4 register.
//
//*****************************************************************************
#define NVIC_EN4_INT_M          0x000007FF  // Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DIS0 register.
//
//*****************************************************************************
#define NVIC_DIS0_INT_M         0xFFFFFFFF  // Interrupt Disable
#define NVIC_DIS0_INT0          0x00000001  // Interrupt 0 disable
#define NVIC_DIS0_INT1          0x00000002  // Interrupt 1 disable
#define NVIC_DIS0_INT2          0x00000004  // Interrupt 2 disable
#define NVIC_DIS0_INT3          0x00000008  // Interrupt 3 disable
#define NVIC_DIS0_INT4          0x00000010  // Interrupt 4 disable
#define NVIC_DIS0_INT5          0x00000020  // Interrupt 5 disable
#define NVIC_DIS0_INT6          0x00000040  // Interrupt 6 disable
#define NVIC_DIS0_INT7          0x00000080  // Interrupt 7 disable
#define NVIC_DIS0_INT8          0x00000100  // Interrupt 8 disable
#define NVIC_DIS0_INT9          0x00000200  // Interrupt 9 disable
#define NVIC_DIS0_INT10         0x00000400  // Interrupt 10 disable
#define NVIC_DIS0_INT11         0x00000800  // Interrupt 11 disable
#define NVIC_DIS0_INT12         0x00001000  // Interrupt 12 disable
#define NVIC_DIS0_INT13         0x00002000  // Interrupt 13 disable
#define NVIC_DIS0_INT14         0x00004000  // Interrupt 14 disable
#define NVIC_DIS0_INT15         0x00008000  // Interrupt 15 disable
#define NVIC_DIS0_INT16         0x00010000  // Interrupt 16 disable
#define NVIC_DIS0_INT17         0x00020000  // Interrupt 17 disable
#define NVIC_DIS0_INT18         0x00040000  // Interrupt 18 disable
#define NVIC_DIS0_INT19         0x00080000  // Interrupt 19 disable
#define NVIC_DIS0_INT20         0x00100000  // Interrupt 20 disable
#define NVIC_DIS0_INT21         0x00200000  // Interrupt 21 disable
#define NVIC_DIS0_INT22         0x00400000  // Interrupt 22 disable
#define NVIC_DIS0_INT23         0x00800000  // Interrupt 23 disable
#define NVIC_DIS0_INT24         0x01000000  // Interrupt 24 disable
#define NVIC_DIS0_INT25         0x02000000  // Interrupt 25 disable
#define NVIC_DIS0_INT26         0x04000000  // Interrupt 26 disable
#define NVIC_DIS0_INT27         0x08000000  // Interrupt 27 disable
#define NVIC_DIS0_INT28         0x10000000  // Interrupt 28 disable
#define NVIC_DIS0_INT29         0x20000000  // Interrupt 29 disable
#define NVIC_DIS0_INT30         0x40000000  // Interrupt 30 disable
#define NVIC_DIS0_INT31         0x80000000  // Interrupt 31 disable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DIS1 register.
//
//*****************************************************************************
#define NVIC_DIS1_INT_M         0xFFFFFFFF  // Interrupt Disable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DIS2 register.
//
//*****************************************************************************
#define NVIC_DIS2_INT_M         0xFFFFFFFF  // Interrupt Disable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DIS3 register.
//
//*****************************************************************************
#define NVIC_DIS3_INT_M         0xFFFFFFFF  // Interrupt Disable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DIS4 register.
//
//*****************************************************************************
#define NVIC_DIS4_INT_M         0x000007FF  // Interrupt Disable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PEND0 register.
//
//*****************************************************************************
#define NVIC_PEND0_INT_M        0xFFFFFFFF  // Interrupt Set Pending
#define NVIC_PEND0_INT0         0x00000001  // Interrupt 0 pend
#define NVIC_PEND0_INT1         0x00000002  // Interrupt 1 pend
#define NVIC_PEND0_INT2         0x00000004  // Interrupt 2 pend
#define NVIC_PEND0_INT3         0x00000008  // Interrupt 3 pend
#define NVIC_PEND0_INT4         0x00000010  // Interrupt 4 pend
#define NVIC_PEND0_INT5         0x00000020  // Interrupt 5 pend
#define NVIC_PEND0_INT6         0x00000040  // Interrupt 6 pend
#define NVIC_PEND0_INT7         0x00000080  // Interrupt 7 pend
#define NVIC_PEND0_INT8         0x00000100  // Interrupt 8 pend
#define NVIC_PEND0_INT9         0x00000200  // Interrupt 9 pend
#define NVIC_PEND0_INT10        0x00000400  // Interrupt 10 pend
#define NVIC_PEND0_INT11        0x00000800  // Interrupt 11 pend
#define NVIC_PEND0_INT12        0x00001000  // Interrupt 12 pend
#define NVIC_PEND0_INT13        0x00002000  // Interrupt 13 pend
#define NVIC_PEND0_INT14        0x00004000  // Interrupt 14 pend
#define NVIC_PEND0_INT15        0x00008000  // Interrupt 15 pend
#define NVIC_PEND0_INT16        0x00010000  // Interrupt 16 pend
#define NVIC_PEND0_INT17        0x00020000  // Interrupt 17 pend
#define NVIC_PEND0_INT18        0x00040000  // Interrupt 18 pend
#define NVIC_PEND0_INT19        0x00080000  // Interrupt 19 pend
#define NVIC_PEND0_INT20        0x00100000  // Interrupt 20 pend
#define NVIC_PEND0_INT21        0x00200000  // Interrupt 21 pend
#define NVIC_PEND0_INT22        0x00400000  // Interrupt 22 pend
#define NVIC_PEND0_INT23        0x00800000  // Interrupt 23 pend
#define NVIC_PEND0_INT24        0x01000000  // Interrupt 24 pend
#define NVIC_PEND0_INT25        0x02000000  // Interrupt 25 pend
#define NVIC_PEND0_INT26        0x04000000  // Interrupt 26 pend
#define NVIC_PEND0_INT27        0x08000000  // Interrupt 27 pend
#define NVIC_PEND0_INT28        0x10000000  // Interrupt 28 pend
#define NVIC_PEND0_INT29        0x20000000  // Interrupt 29 pend
#define NVIC_PEND0_INT30        0x40000000  // Interrupt 30 pend
#define NVIC_PEND0_INT31        0x80000000  // Interrupt 31 pend

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PEND1 register.
//
//*****************************************************************************
#define NVIC_PEND1_INT_M        0xFFFFFFFF  // Interrupt Set Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PEND2 register.
//
//*****************************************************************************
#define NVIC_PEND2_INT_M        0xFFFFFFFF  // Interrupt Set Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PEND3 register.
//
//*****************************************************************************
#define NVIC_PEND3_INT_M        0xFFFFFFFF  // Interrupt Set Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PEND4 register.
//
//*****************************************************************************
#define NVIC_PEND4_INT_M        0x000007FF  // Interrupt Set Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_UNPEND0 register.
//
//*****************************************************************************
#define NVIC_UNPEND0_INT_M      0xFFFFFFFF  // Interrupt Clear Pending
#define NVIC_UNPEND0_INT0       0x00000001  // Interrupt 0 unpend
#define NVIC_UNPEND0_INT1       0x00000002  // Interrupt 1 unpend
#define NVIC_UNPEND0_INT2       0x00000004  // Interrupt 2 unpend
#define NVIC_UNPEND0_INT3       0x00000008  // Interrupt 3 unpend
#define NVIC_UNPEND0_INT4       0x00000010  // Interrupt 4 unpend
#define NVIC_UNPEND0_INT5       0x00000020  // Interrupt 5 unpend
#define NVIC_UNPEND0_INT6       0x00000040  // Interrupt 6 unpend
#define NVIC_UNPEND0_INT7       0x00000080  // Interrupt 7 unpend
#define NVIC_UNPEND0_INT8       0x00000100  // Interrupt 8 unpend
#define NVIC_UNPEND0_INT9       0x00000200  // Interrupt 9 unpend
#define NVIC_UNPEND0_INT10      0x00000400  // Interrupt 10 unpend
#define NVIC_UNPEND0_INT11      0x00000800  // Interrupt 11 unpend
#define NVIC_UNPEND0_INT12      0x00001000  // Interrupt 12 unpend
#define NVIC_UNPEND0_INT13      0x00002000  // Interrupt 13 unpend
#define NVIC_UNPEND0_INT14      0x00004000  // Interrupt 14 unpend
#define NVIC_UNPEND0_INT15      0x00008000  // Interrupt 15 unpend
#define NVIC_UNPEND0_INT16      0x00010000  // Interrupt 16 unpend
#define NVIC_UNPEND0_INT17      0x00020000  // Interrupt 17 unpend
#define NVIC_UNPEND0_INT18      0x00040000  // Interrupt 18 unpend
#define NVIC_UNPEND0_INT19      0x00080000  // Interrupt 19 unpend
#define NVIC_UNPEND0_INT20      0x00100000  // Interrupt 20 unpend
#define NVIC_UNPEND0_INT21      0x00200000  // Interrupt 21 unpend
#define NVIC_UNPEND0_INT22      0x00400000  // Interrupt 22 unpend
#define NVIC_UNPEND0_INT23      0x00800000  // Interrupt 23 unpend
#define NVIC_UNPEND0_INT24      0x01000000  // Interrupt 24 unpend
#define NVIC_UNPEND0_INT25      0x02000000  // Interrupt 25 unpend
#define NVIC_UNPEND0_INT26      0x04000000  // Interrupt 26 unpend
#define NVIC_UNPEND0_INT27      0x08000000  // Interrupt 27 unpend
#define NVIC_UNPEND0_INT28      0x10000000  // Interrupt 28 unpend
#define NVIC_UNPEND0_INT29      0x20000000  // Interrupt 29 unpend
#define NVIC_UNPEND0_INT30      0x40000000  // Interrupt 30 unpend
#define NVIC_UNPEND0_INT31      0x80000000  // Interrupt 31 unpend

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_UNPEND1 register.
//
//*****************************************************************************
#define NVIC_UNPEND1_INT_M      0xFFFFFFFF  // Interrupt Clear Pending
#define NVIC_UNPEND1_INT32      0x00000001  // Interrupt 32 unpend
#define NVIC_UNPEND1_INT33      0x00000002  // Interrupt 33 unpend
#define NVIC_UNPEND1_INT34      0x00000004  // Interrupt 34 unpend
#define NVIC_UNPEND1_INT35      0x00000008  // Interrupt 35 unpend
#define NVIC_UNPEND1_INT36      0x00000010  // Interrupt 36 unpend
#define NVIC_UNPEND1_INT37      0x00000020  // Interrupt 37 unpend
#define NVIC_UNPEND1_INT38      0x00000040  // Interrupt 38 unpend
#define NVIC_UNPEND1_INT39      0x00000080  // Interrupt 39 unpend
#define NVIC_UNPEND1_INT40      0x00000100  // Interrupt 40 unpend
#define NVIC_UNPEND1_INT41      0x00000200  // Interrupt 41 unpend
#define NVIC_UNPEND1_INT42      0x00000400  // Interrupt 42 unpend
#define NVIC_UNPEND1_INT43      0x00000800  // Interrupt 43 unpend
#define NVIC_UNPEND1_INT44      0x00001000  // Interrupt 44 unpend
#define NVIC_UNPEND1_INT45      0x00002000  // Interrupt 45 unpend
#define NVIC_UNPEND1_INT46      0x00004000  // Interrupt 46 unpend
#define NVIC_UNPEND1_INT47      0x00008000  // Interrupt 47 unpend
#define NVIC_UNPEND1_INT48      0x00010000  // Interrupt 48 unpend
#define NVIC_UNPEND1_INT49      0x00020000  // Interrupt 49 unpend
#define NVIC_UNPEND1_INT50      0x00040000  // Interrupt 50 unpend
#define NVIC_UNPEND1_INT51      0x00080000  // Interrupt 51 unpend
#define NVIC_UNPEND1_INT52      0x00100000  // Interrupt 52 unpend
#define NVIC_UNPEND1_INT53      0x00200000  // Interrupt 53 unpend
#define NVIC_UNPEND1_INT54      0x00400000  // Interrupt 54 unpend
#define NVIC_UNPEND1_INT55      0x00800000  // Interrupt 55 unpend

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_UNPEND2 register.
//
//*****************************************************************************
#define NVIC_UNPEND2_INT_M      0xFFFFFFFF  // Interrupt Clear Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_UNPEND3 register.
//
//*****************************************************************************
#define NVIC_UNPEND3_INT_M      0xFFFFFFFF  // Interrupt Clear Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_UNPEND4 register.
//
//*****************************************************************************
#define NVIC_UNPEND4_INT_M      0x000007FF  // Interrupt Clear Pending

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ACTIVE0 register.
//
//*****************************************************************************
#define NVIC_ACTIVE0_INT_M      0xFFFFFFFF  // Interrupt Active
#define NVIC_ACTIVE0_INT0       0x00000001  // Interrupt 0 active
#define NVIC_ACTIVE0_INT1       0x00000002  // Interrupt 1 active
#define NVIC_ACTIVE0_INT2       0x00000004  // Interrupt 2 active
#define NVIC_ACTIVE0_INT3       0x00000008  // Interrupt 3 active
#define NVIC_ACTIVE0_INT4       0x00000010  // Interrupt 4 active
#define NVIC_ACTIVE0_INT5       0x00000020  // Interrupt 5 active
#define NVIC_ACTIVE0_INT6       0x00000040  // Interrupt 6 active
#define NVIC_ACTIVE0_INT7       0x00000080  // Interrupt 7 active
#define NVIC_ACTIVE0_INT8       0x00000100  // Interrupt 8 active
#define NVIC_ACTIVE0_INT9       0x00000200  // Interrupt 9 active
#define NVIC_ACTIVE0_INT10      0x00000400  // Interrupt 10 active
#define NVIC_ACTIVE0_INT11      0x00000800  // Interrupt 11 active
#define NVIC_ACTIVE0_INT12      0x00001000  // Interrupt 12 active
#define NVIC_ACTIVE0_INT13      0x00002000  // Interrupt 13 active
#define NVIC_ACTIVE0_INT14      0x00004000  // Interrupt 14 active
#define NVIC_ACTIVE0_INT15      0x00008000  // Interrupt 15 active
#define NVIC_ACTIVE0_INT16      0x00010000  // Interrupt 16 active
#define NVIC_ACTIVE0_INT17      0x00020000  // Interrupt 17 active
#define NVIC_ACTIVE0_INT18      0x00040000  // Interrupt 18 active
#define NVIC_ACTIVE0_INT19      0x00080000  // Interrupt 19 active
#define NVIC_ACTIVE0_INT20      0x00100000  // Interrupt 20 active
#define NVIC_ACTIVE0_INT21      0x00200000  // Interrupt 21 active
#define NVIC_ACTIVE0_INT22      0x00400000  // Interrupt 22 active
#define NVIC_ACTIVE0_INT23      0x00800000  // Interrupt 23 active
#define NVIC_ACTIVE0_INT24      0x01000000  // Interrupt 24 active
#define NVIC_ACTIVE0_INT25      0x02000000  // Interrupt 25 active
#define NVIC_ACTIVE0_INT26      0x04000000  // Interrupt 26 active
#define NVIC_ACTIVE0_INT27      0x08000000  // Interrupt 27 active
#define NVIC_ACTIVE0_INT28      0x10000000  // Interrupt 28 active
#define NVIC_ACTIVE0_INT29      0x20000000  // Interrupt 29 active
#define NVIC_ACTIVE0_INT30      0x40000000  // Interrupt 30 active
#define NVIC_ACTIVE0_INT31      0x80000000  // Interrupt 31 active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ACTIVE1 register.
//
//*****************************************************************************
#define NVIC_ACTIVE1_INT_M      0xFFFFFFFF  // Interrupt Active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ACTIVE2 register.
//
//*****************************************************************************
#define NVIC_ACTIVE2_INT_M      0xFFFFFFFF  // Interrupt Active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ACTIVE3 register.
//
//*****************************************************************************
#define NVIC_ACTIVE3_INT_M      0xFFFFFFFF  // Interrupt Active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_ACTIVE4 register.
//
//*****************************************************************************
#define NVIC_ACTIVE4_INT_M      0x000007FF  // Interrupt Active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI0 register.
//
//*****************************************************************************
#define NVIC_PRI0_INT3_M        0xE0000000  // Interrupt 3 Priority Mask
#define NVIC_PRI0_INT2_M        0x00E00000  // Interrupt 2 Priority Mask
#define NVIC_PRI0_INT1_M        0x0000E000  // Interrupt 1 Priority Mask
#define NVIC_PRI0_INT0_M        0x000000E0  // Interrupt 0 Priority Mask
#define NVIC_PRI0_INT3_S        29
#define NVIC_PRI0_INT2_S        21
#define NVIC_PRI0_INT1_S        13
#define NVIC_PRI0_INT0_S        5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI1 register.
//
//*****************************************************************************
#define NVIC_PRI1_INT7_M        0xE0000000  // Interrupt 7 Priority Mask
#define NVIC_PRI1_INT6_M        0x00E00000  // Interrupt 6 Priority Mask
#define NVIC_PRI1_INT5_M        0x0000E000  // Interrupt 5 Priority Mask
#define NVIC_PRI1_INT4_M        0x000000E0  // Interrupt 4 Priority Mask
#define NVIC_PRI1_INT7_S        29
#define NVIC_PRI1_INT6_S        21
#define NVIC_PRI1_INT5_S        13
#define NVIC_PRI1_INT4_S        5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI2 register.
//
//*****************************************************************************
#define NVIC_PRI2_INT11_M       0xE0000000  // Interrupt 11 Priority Mask
#define NVIC_PRI2_INT10_M       0x00E00000  // Interrupt 10 Priority Mask
#define NVIC_PRI2_INT9_M        0x0000E000  // Interrupt 9 Priority Mask
#define NVIC_PRI2_INT8_M        0x000000E0  // Interrupt 8 Priority Mask
#define NVIC_PRI2_INT11_S       29
#define NVIC_PRI2_INT10_S       21
#define NVIC_PRI2_INT9_S        13
#define NVIC_PRI2_INT8_S        5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI3 register.
//
//*****************************************************************************
#define NVIC_PRI3_INT15_M       0xE0000000  // Interrupt 15 Priority Mask
#define NVIC_PRI3_INT14_M       0x00E00000  // Interrupt 14 Priority Mask
#define NVIC_PRI3_INT13_M       0x0000E000  // Interrupt 13 Priority Mask
#define NVIC_PRI3_INT12_M       0x000000E0  // Interrupt 12 Priority Mask
#define NVIC_PRI3_INT15_S       29
#define NVIC_PRI3_INT14_S       21
#define NVIC_PRI3_INT13_S       13
#define NVIC_PRI3_INT12_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI4 register.
//
//*****************************************************************************
#define NVIC_PRI4_INT19_M       0xE0000000  // Interrupt 19 Priority Mask
#define NVIC_PRI4_INT18_M       0x00E00000  // Interrupt 18 Priority Mask
#define NVIC_PRI4_INT17_M       0x0000E000  // Interrupt 17 Priority Mask
#define NVIC_PRI4_INT16_M       0x000000E0  // Interrupt 16 Priority Mask
#define NVIC_PRI4_INT19_S       29
#define NVIC_PRI4_INT18_S       21
#define NVIC_PRI4_INT17_S       13
#define NVIC_PRI4_INT16_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI5 register.
//
//*****************************************************************************
#define NVIC_PRI5_INT23_M       0xE0000000  // Interrupt 23 Priority Mask
#define NVIC_PRI5_INT22_M       0x00E00000  // Interrupt 22 Priority Mask
#define NVIC_PRI5_INT21_M       0x0000E000  // Interrupt 21 Priority Mask
#define NVIC_PRI5_INT20_M       0x000000E0  // Interrupt 20 Priority Mask
#define NVIC_PRI5_INT23_S       29
#define NVIC_PRI5_INT22_S       21
#define NVIC_PRI5_INT21_S       13
#define NVIC_PRI5_INT20_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI6 register.
//
//*****************************************************************************
#define NVIC_PRI6_INT27_M       0xE0000000  // Interrupt 27 Priority Mask
#define NVIC_PRI6_INT26_M       0x00E00000  // Interrupt 26 Priority Mask
#define NVIC_PRI6_INT25_M       0x0000E000  // Interrupt 25 Priority Mask
#define NVIC_PRI6_INT24_M       0x000000E0  // Interrupt 24 Priority Mask
#define NVIC_PRI6_INT27_S       29
#define NVIC_PRI6_INT26_S       21
#define NVIC_PRI6_INT25_S       13
#define NVIC_PRI6_INT24_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI7 register.
//
//*****************************************************************************
#define NVIC_PRI7_INT31_M       0xE0000000  // Interrupt 31 Priority Mask
#define NVIC_PRI7_INT30_M       0x00E00000  // Interrupt 30 Priority Mask
#define NVIC_PRI7_INT29_M       0x0000E000  // Interrupt 29 Priority Mask
#define NVIC_PRI7_INT28_M       0x000000E0  // Interrupt 28 Priority Mask
#define NVIC_PRI7_INT31_S       29
#define NVIC_PRI7_INT30_S       21
#define NVIC_PRI7_INT29_S       13
#define NVIC_PRI7_INT28_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI8 register.
//
//*****************************************************************************
#define NVIC_PRI8_INT35_M       0xE0000000  // Interrupt 35 Priority Mask
#define NVIC_PRI8_INT34_M       0x00E00000  // Interrupt 34 Priority Mask
#define NVIC_PRI8_INT33_M       0x0000E000  // Interrupt 33 Priority Mask
#define NVIC_PRI8_INT32_M       0x000000E0  // Interrupt 32 Priority Mask
#define NVIC_PRI8_INT35_S       29
#define NVIC_PRI8_INT34_S       21
#define NVIC_PRI8_INT33_S       13
#define NVIC_PRI8_INT32_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI9 register.
//
//*****************************************************************************
#define NVIC_PRI9_INT39_M       0xE0000000  // Interrupt 39 Priority Mask
#define NVIC_PRI9_INT38_M       0x00E00000  // Interrupt 38 Priority Mask
#define NVIC_PRI9_INT37_M       0x0000E000  // Interrupt 37 Priority Mask
#define NVIC_PRI9_INT36_M       0x000000E0  // Interrupt 36 Priority Mask
#define NVIC_PRI9_INT39_S       29
#define NVIC_PRI9_INT38_S       21
#define NVIC_PRI9_INT37_S       13
#define NVIC_PRI9_INT36_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI10 register.
//
//*****************************************************************************
#define NVIC_PRI10_INT43_M      0xE0000000  // Interrupt 43 Priority Mask
#define NVIC_PRI10_INT42_M      0x00E00000  // Interrupt 42 Priority Mask
#define NVIC_PRI10_INT41_M      0x0000E000  // Interrupt 41 Priority Mask
#define NVIC_PRI10_INT40_M      0x000000E0  // Interrupt 40 Priority Mask
#define NVIC_PRI10_INT43_S      29
#define NVIC_PRI10_INT42_S      21
#define NVIC_PRI10_INT41_S      13
#define NVIC_PRI10_INT40_S      5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI11 register.
//
//*****************************************************************************
#define NVIC_PRI11_INT47_M      0xE0000000  // Interrupt 47 Priority Mask
#define NVIC_PRI11_INT46_M      0x00E00000  // Interrupt 46 Priority Mask
#define NVIC_PRI11_INT45_M      0x0000E000  // Interrupt 45 Priority Mask
#define NVIC_PRI11_INT44_M      0x000000E0  // Interrupt 44 Priority Mask
#define NVIC_PRI11_INT47_S      29
#define NVIC_PRI11_INT46_S      21
#define NVIC_PRI11_INT45_S      13
#define NVIC_PRI11_INT44_S      5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI12 register.
//
//*****************************************************************************
#define NVIC_PRI12_INT51_M      0xE0000000  // Interrupt 51 Priority Mask
#define NVIC_PRI12_INT50_M      0x00E00000  // Interrupt 50 Priority Mask
#define NVIC_PRI12_INT49_M      0x0000E000  // Interrupt 49 Priority Mask
#define NVIC_PRI12_INT48_M      0x000000E0  // Interrupt 48 Priority Mask
#define NVIC_PRI12_INT51_S      29
#define NVIC_PRI12_INT50_S      21
#define NVIC_PRI12_INT49_S      13
#define NVIC_PRI12_INT48_S      5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI13 register.
//
//*****************************************************************************
#define NVIC_PRI13_INT55_M      0xE0000000  // Interrupt 55 Priority Mask
#define NVIC_PRI13_INT54_M      0x00E00000  // Interrupt 54 Priority Mask
#define NVIC_PRI13_INT53_M      0x0000E000  // Interrupt 53 Priority Mask
#define NVIC_PRI13_INT52_M      0x000000E0  // Interrupt 52 Priority Mask
#define NVIC_PRI13_INT55_S      29
#define NVIC_PRI13_INT54_S      21
#define NVIC_PRI13_INT53_S      13
#define NVIC_PRI13_INT52_S      5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI14 register.
//
//*****************************************************************************
#define NVIC_PRI14_INTD_M       0xE0000000  // Interrupt 59 Priority Mask
#define NVIC_PRI14_INTC_M       0x00E00000  // Interrupt 58 Priority Mask
#define NVIC_PRI14_INTB_M       0x0000E000  // Interrupt 57 Priority Mask
#define NVIC_PRI14_INTA_M       0x000000E0  // Interrupt 56 Priority Mask
#define NVIC_PRI14_INTD_S       29
#define NVIC_PRI14_INTC_S       21
#define NVIC_PRI14_INTB_S       13
#define NVIC_PRI14_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI15 register.
//
//*****************************************************************************
#define NVIC_PRI15_INTD_M       0xE0000000  // Interrupt 63 Priority Mask
#define NVIC_PRI15_INTC_M       0x00E00000  // Interrupt 62 Priority Mask
#define NVIC_PRI15_INTB_M       0x0000E000  // Interrupt 61 Priority Mask
#define NVIC_PRI15_INTA_M       0x000000E0  // Interrupt 60 Priority Mask
#define NVIC_PRI15_INTD_S       29
#define NVIC_PRI15_INTC_S       21
#define NVIC_PRI15_INTB_S       13
#define NVIC_PRI15_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI16 register.
//
//*****************************************************************************
#define NVIC_PRI16_INTD_M       0xE0000000  // Interrupt 67 Priority Mask
#define NVIC_PRI16_INTC_M       0x00E00000  // Interrupt 66 Priority Mask
#define NVIC_PRI16_INTB_M       0x0000E000  // Interrupt 65 Priority Mask
#define NVIC_PRI16_INTA_M       0x000000E0  // Interrupt 64 Priority Mask
#define NVIC_PRI16_INTD_S       29
#define NVIC_PRI16_INTC_S       21
#define NVIC_PRI16_INTB_S       13
#define NVIC_PRI16_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI17 register.
//
//*****************************************************************************
#define NVIC_PRI17_INTD_M       0xE0000000  // Interrupt 71 Priority Mask
#define NVIC_PRI17_INTC_M       0x00E00000  // Interrupt 70 Priority Mask
#define NVIC_PRI17_INTB_M       0x0000E000  // Interrupt 69 Priority Mask
#define NVIC_PRI17_INTA_M       0x000000E0  // Interrupt 68 Priority Mask
#define NVIC_PRI17_INTD_S       29
#define NVIC_PRI17_INTC_S       21
#define NVIC_PRI17_INTB_S       13
#define NVIC_PRI17_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI18 register.
//
//*****************************************************************************
#define NVIC_PRI18_INTD_M       0xE0000000  // Interrupt 75 Priority Mask
#define NVIC_PRI18_INTC_M       0x00E00000  // Interrupt 74 Priority Mask
#define NVIC_PRI18_INTB_M       0x0000E000  // Interrupt 73 Priority Mask
#define NVIC_PRI18_INTA_M       0x000000E0  // Interrupt 72 Priority Mask
#define NVIC_PRI18_INTD_S       29
#define NVIC_PRI18_INTC_S       21
#define NVIC_PRI18_INTB_S       13
#define NVIC_PRI18_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI19 register.
//
//*****************************************************************************
#define NVIC_PRI19_INTD_M       0xE0000000  // Interrupt 79 Priority Mask
#define NVIC_PRI19_INTC_M       0x00E00000  // Interrupt 78 Priority Mask
#define NVIC_PRI19_INTB_M       0x0000E000  // Interrupt 77 Priority Mask
#define NVIC_PRI19_INTA_M       0x000000E0  // Interrupt 76 Priority Mask
#define NVIC_PRI19_INTD_S       29
#define NVIC_PRI19_INTC_S       21
#define NVIC_PRI19_INTB_S       13
#define NVIC_PRI19_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI20 register.
//
//*****************************************************************************
#define NVIC_PRI20_INTD_M       0xE0000000  // Interrupt 83 Priority Mask
#define NVIC_PRI20_INTC_M       0x00E00000  // Interrupt 82 Priority Mask
#define NVIC_PRI20_INTB_M       0x0000E000  // Interrupt 81 Priority Mask
#define NVIC_PRI20_INTA_M       0x000000E0  // Interrupt 80 Priority Mask
#define NVIC_PRI20_INTD_S       29
#define NVIC_PRI20_INTC_S       21
#define NVIC_PRI20_INTB_S       13
#define NVIC_PRI20_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI21 register.
//
//*****************************************************************************
#define NVIC_PRI21_INTD_M       0xE0000000  // Interrupt 87 Priority Mask
#define NVIC_PRI21_INTC_M       0x00E00000  // Interrupt 86 Priority Mask
#define NVIC_PRI21_INTB_M       0x0000E000  // Interrupt 85 Priority Mask
#define NVIC_PRI21_INTA_M       0x000000E0  // Interrupt 84 Priority Mask
#define NVIC_PRI21_INTD_S       29
#define NVIC_PRI21_INTC_S       21
#define NVIC_PRI21_INTB_S       13
#define NVIC_PRI21_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI22 register.
//
//*****************************************************************************
#define NVIC_PRI22_INTD_M       0xE0000000  // Interrupt 91 Priority Mask
#define NVIC_PRI22_INTC_M       0x00E00000  // Interrupt 90 Priority Mask
#define NVIC_PRI22_INTB_M       0x0000E000  // Interrupt 89 Priority Mask
#define NVIC_PRI22_INTA_M       0x000000E0  // Interrupt 88 Priority Mask
#define NVIC_PRI22_INTD_S       29
#define NVIC_PRI22_INTC_S       21
#define NVIC_PRI22_INTB_S       13
#define NVIC_PRI22_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI23 register.
//
//*****************************************************************************
#define NVIC_PRI23_INTD_M       0xE0000000  // Interrupt 95 Priority Mask
#define NVIC_PRI23_INTC_M       0x00E00000  // Interrupt 94 Priority Mask
#define NVIC_PRI23_INTB_M       0x0000E000  // Interrupt 93 Priority Mask
#define NVIC_PRI23_INTA_M       0x000000E0  // Interrupt 92 Priority Mask
#define NVIC_PRI23_INTD_S       29
#define NVIC_PRI23_INTC_S       21
#define NVIC_PRI23_INTB_S       13
#define NVIC_PRI23_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI24 register.
//
//*****************************************************************************
#define NVIC_PRI24_INTD_M       0xE0000000  // Interrupt 99 Priority Mask
#define NVIC_PRI24_INTC_M       0x00E00000  // Interrupt 98 Priority Mask
#define NVIC_PRI24_INTB_M       0x0000E000  // Interrupt 97 Priority Mask
#define NVIC_PRI24_INTA_M       0x000000E0  // Interrupt 96 Priority Mask
#define NVIC_PRI24_INTD_S       29
#define NVIC_PRI24_INTC_S       21
#define NVIC_PRI24_INTB_S       13
#define NVIC_PRI24_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI25 register.
//
//*****************************************************************************
#define NVIC_PRI25_INTD_M       0xE0000000  // Interrupt 103 Priority Mask
#define NVIC_PRI25_INTC_M       0x00E00000  // Interrupt 102 Priority Mask
#define NVIC_PRI25_INTB_M       0x0000E000  // Interrupt 101 Priority Mask
#define NVIC_PRI25_INTA_M       0x000000E0  // Interrupt 100 Priority Mask
#define NVIC_PRI25_INTD_S       29
#define NVIC_PRI25_INTC_S       21
#define NVIC_PRI25_INTB_S       13
#define NVIC_PRI25_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI26 register.
//
//*****************************************************************************
#define NVIC_PRI26_INTD_M       0xE0000000  // Interrupt 107 Priority Mask
#define NVIC_PRI26_INTC_M       0x00E00000  // Interrupt 106 Priority Mask
#define NVIC_PRI26_INTB_M       0x0000E000  // Interrupt 105 Priority Mask
#define NVIC_PRI26_INTA_M       0x000000E0  // Interrupt 104 Priority Mask
#define NVIC_PRI26_INTD_S       29
#define NVIC_PRI26_INTC_S       21
#define NVIC_PRI26_INTB_S       13
#define NVIC_PRI26_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI27 register.
//
//*****************************************************************************
#define NVIC_PRI27_INTD_M       0xE0000000  // Interrupt 111 Priority Mask
#define NVIC_PRI27_INTC_M       0x00E00000  // Interrupt 110 Priority Mask
#define NVIC_PRI27_INTB_M       0x0000E000  // Interrupt 109 Priority Mask
#define NVIC_PRI27_INTA_M       0x000000E0  // Interrupt 108 Priority Mask
#define NVIC_PRI27_INTD_S       29
#define NVIC_PRI27_INTC_S       21
#define NVIC_PRI27_INTB_S       13
#define NVIC_PRI27_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI28 register.
//
//*****************************************************************************
#define NVIC_PRI28_INTD_M       0xE0000000  // Interrupt 115 Priority Mask
#define NVIC_PRI28_INTC_M       0x00E00000  // Interrupt 114 Priority Mask
#define NVIC_PRI28_INTB_M       0x0000E000  // Interrupt 113 Priority Mask
#define NVIC_PRI28_INTA_M       0x000000E0  // Interrupt 112 Priority Mask
#define NVIC_PRI28_INTD_S       29
#define NVIC_PRI28_INTC_S       21
#define NVIC_PRI28_INTB_S       13
#define NVIC_PRI28_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI29 register.
//
//*****************************************************************************
#define NVIC_PRI29_INTD_M       0xE0000000  // Interrupt 119 Priority Mask
#define NVIC_PRI29_INTC_M       0x00E00000  // Interrupt 118 Priority Mask
#define NVIC_PRI29_INTB_M       0x0000E000  // Interrupt 117 Priority Mask
#define NVIC_PRI29_INTA_M       0x000000E0  // Interrupt 116 Priority Mask
#define NVIC_PRI29_INTD_S       29
#define NVIC_PRI29_INTC_S       21
#define NVIC_PRI29_INTB_S       13
#define NVIC_PRI29_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI30 register.
//
//*****************************************************************************
#define NVIC_PRI30_INTD_M       0xE0000000  // Interrupt 123 Priority Mask
#define NVIC_PRI30_INTC_M       0x00E00000  // Interrupt 122 Priority Mask
#define NVIC_PRI30_INTB_M       0x0000E000  // Interrupt 121 Priority Mask
#define NVIC_PRI30_INTA_M       0x000000E0  // Interrupt 120 Priority Mask
#define NVIC_PRI30_INTD_S       29
#define NVIC_PRI30_INTC_S       21
#define NVIC_PRI30_INTB_S       13
#define NVIC_PRI30_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI31 register.
//
//*****************************************************************************
#define NVIC_PRI31_INTD_M       0xE0000000  // Interrupt 127 Priority Mask
#define NVIC_PRI31_INTC_M       0x00E00000  // Interrupt 126 Priority Mask
#define NVIC_PRI31_INTB_M       0x0000E000  // Interrupt 125 Priority Mask
#define NVIC_PRI31_INTA_M       0x000000E0  // Interrupt 124 Priority Mask
#define NVIC_PRI31_INTD_S       29
#define NVIC_PRI31_INTC_S       21
#define NVIC_PRI31_INTB_S       13
#define NVIC_PRI31_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI32 register.
//
//*****************************************************************************
#define NVIC_PRI32_INTD_M       0xE0000000  // Interrupt 131 Priority Mask
#define NVIC_PRI32_INTC_M       0x00E00000  // Interrupt 130 Priority Mask
#define NVIC_PRI32_INTB_M       0x0000E000  // Interrupt 129 Priority Mask
#define NVIC_PRI32_INTA_M       0x000000E0  // Interrupt 128 Priority Mask
#define NVIC_PRI32_INTD_S       29
#define NVIC_PRI32_INTC_S       21
#define NVIC_PRI32_INTB_S       13
#define NVIC_PRI32_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI33 register.
//
//*****************************************************************************
#define NVIC_PRI33_INTD_M       0xE0000000  // Interrupt Priority for Interrupt
                                            // [4n+3]
#define NVIC_PRI33_INTC_M       0x00E00000  // Interrupt Priority for Interrupt
                                            // [4n+2]
#define NVIC_PRI33_INTB_M       0x0000E000  // Interrupt Priority for Interrupt
                                            // [4n+1]
#define NVIC_PRI33_INTA_M       0x000000E0  // Interrupt Priority for Interrupt
                                            // [4n]
#define NVIC_PRI33_INTD_S       29
#define NVIC_PRI33_INTC_S       21
#define NVIC_PRI33_INTB_S       13
#define NVIC_PRI33_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_PRI34 register.
//
//*****************************************************************************
#define NVIC_PRI34_INTD_M       0xE0000000  // Interrupt Priority for Interrupt
                                            // [4n+3]
#define NVIC_PRI34_INTC_M       0x00E00000  // Interrupt Priority for Interrupt
                                            // [4n+2]
#define NVIC_PRI34_INTB_M       0x0000E000  // Interrupt Priority for Interrupt
                                            // [4n+1]
#define NVIC_PRI34_INTA_M       0x000000E0  // Interrupt Priority for Interrupt
                                            // [4n]
#define NVIC_PRI34_INTD_S       29
#define NVIC_PRI34_INTC_S       21
#define NVIC_PRI34_INTB_S       13
#define NVIC_PRI34_INTA_S       5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_CPUID register.
//
//*****************************************************************************
#define NVIC_CPUID_IMP_M        0xFF000000  // Implementer Code
#define NVIC_CPUID_IMP_ARM      0x41000000  // ARM
#define NVIC_CPUID_VAR_M        0x00F00000  // Variant Number
#define NVIC_CPUID_CON_M        0x000F0000  // Constant
#define NVIC_CPUID_PARTNO_M     0x0000FFF0  // Part Number
#define NVIC_CPUID_PARTNO_CM4   0x0000C240  // Cortex-M4 processor
#define NVIC_CPUID_REV_M        0x0000000F  // Revision Number

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_INT_CTRL register.
//
//*****************************************************************************
#define NVIC_INT_CTRL_NMI_SET   0x80000000  // NMI Set Pending
#define NVIC_INT_CTRL_PEND_SV   0x10000000  // PendSV Set Pending
#define NVIC_INT_CTRL_UNPEND_SV 0x08000000  // PendSV Clear Pending
#define NVIC_INT_CTRL_PENDSTSET 0x04000000  // SysTick Set Pending
#define NVIC_INT_CTRL_PENDSTCLR 0x02000000  // SysTick Clear Pending
#define NVIC_INT_CTRL_ISR_PRE   0x00800000  // Debug Interrupt Handling
#define NVIC_INT_CTRL_ISR_PEND  0x00400000  // Interrupt Pending
#define NVIC_INT_CTRL_VEC_PEN_M 0x000FF000  // Interrupt Pending Vector Number
#define NVIC_INT_CTRL_VEC_PEN_NMI \
                                0x00002000  // NMI
#define NVIC_INT_CTRL_VEC_PEN_HARD \
                                0x00003000  // Hard fault
#define NVIC_INT_CTRL_VEC_PEN_MEM \
                                0x00004000  // Memory management fault
#define NVIC_INT_CTRL_VEC_PEN_BUS \
                                0x00005000  // Bus fault
#define NVIC_INT_CTRL_VEC_PEN_USG \
                                0x00006000  // Usage fault
#define NVIC_INT_CTRL_VEC_PEN_SVC \
                                0x0000B000  // SVCall
#define NVIC_INT_CTRL_VEC_PEN_PNDSV \
                                0x0000E000  // PendSV
#define NVIC_INT_CTRL_VEC_PEN_TICK \
                                0x0000F000  // SysTick
#define NVIC_INT_CTRL_RET_BASE  0x00000800  // Return to Base
#define NVIC_INT_CTRL_VEC_ACT_M 0x000000FF  // Interrupt Pending Vector Number
#define NVIC_INT_CTRL_VEC_ACT_S 0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_VTABLE register.
//
//*****************************************************************************
#define NVIC_VTABLE_BASE        0x20000000  // Vector Table Base
#define NVIC_VTABLE_OFFSET_M    0x1FFFFC00  // Vector Table Offset
#define NVIC_VTABLE_OFFSET_S    10

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_APINT register.
//
//*****************************************************************************
#define NVIC_APINT_VECTKEY_M    0xFFFF0000  // Register Key
#define NVIC_APINT_VECTKEY      0x05FA0000  // Vector key
#define NVIC_APINT_ENDIANESS    0x00008000  // Data Endianess
#define NVIC_APINT_PRIGROUP_M   0x00000700  // Interrupt Priority Grouping
#define NVIC_APINT_PRIGROUP_7_1 0x00000000  // Priority group 7.1 split
#define NVIC_APINT_PRIGROUP_6_2 0x00000100  // Priority group 6.2 split
#define NVIC_APINT_PRIGROUP_5_3 0x00000200  // Priority group 5.3 split
#define NVIC_APINT_PRIGROUP_4_4 0x00000300  // Priority group 4.4 split
#define NVIC_APINT_PRIGROUP_3_5 0x00000400  // Priority group 3.5 split
#define NVIC_APINT_PRIGROUP_2_6 0x00000500  // Priority group 2.6 split
#define NVIC_APINT_PRIGROUP_1_7 0x00000600  // Priority group 1.7 split
#define NVIC_APINT_PRIGROUP_0_8 0x00000700  // Priority group 0.8 split
#define NVIC_APINT_SYSRESETREQ  0x00000004  // System Reset Request
#define NVIC_APINT_VECT_CLR_ACT 0x00000002  // Clear Active NMI / Fault
#define NVIC_APINT_VECT_RESET   0x00000001  // System Reset

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_SYS_CTRL register.
//
//*****************************************************************************
#define NVIC_SYS_CTRL_SEVONPEND 0x00000010  // Wake Up on Pending
#define NVIC_SYS_CTRL_SLEEPDEEP 0x00000004  // Deep Sleep Enable
#define NVIC_SYS_CTRL_SLEEPEXIT 0x00000002  // Sleep on ISR Exit

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_CFG_CTRL register.
//
//*****************************************************************************
#define NVIC_CFG_CTRL_STKALIGN  0x00000200  // Stack Alignment on Exception
                                            // Entry
#define NVIC_CFG_CTRL_BFHFNMIGN 0x00000100  // Ignore Bus Fault in NMI and
                                            // Fault
#define NVIC_CFG_CTRL_DIV0      0x00000010  // Trap on Divide by 0
#define NVIC_CFG_CTRL_UNALIGNED 0x00000008  // Trap on Unaligned Access
#define NVIC_CFG_CTRL_MAIN_PEND 0x00000002  // Allow Main Interrupt Trigger
#define NVIC_CFG_CTRL_BASE_THR  0x00000001  // Thread State Control

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_SYS_PRI1 register.
//
//*****************************************************************************
#define NVIC_SYS_PRI1_USAGE_M   0x00E00000  // Usage Fault Priority
#define NVIC_SYS_PRI1_BUS_M     0x0000E000  // Bus Fault Priority
#define NVIC_SYS_PRI1_MEM_M     0x000000E0  // Memory Management Fault Priority
#define NVIC_SYS_PRI1_USAGE_S   21
#define NVIC_SYS_PRI1_BUS_S     13
#define NVIC_SYS_PRI1_MEM_S     5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_SYS_PRI2 register.
//
//*****************************************************************************
#define NVIC_SYS_PRI2_SVC_M     0xE0000000  // SVCall Priority
#define NVIC_SYS_PRI2_SVC_S     29

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_SYS_PRI3 register.
//
//*****************************************************************************
#define NVIC_SYS_PRI3_TICK_M    0xE0000000  // SysTick Exception Priority
#define NVIC_SYS_PRI3_PENDSV_M  0x00E00000  // PendSV Priority
#define NVIC_SYS_PRI3_DEBUG_M   0x000000E0  // Debug Priority
#define NVIC_SYS_PRI3_TICK_S    29
#define NVIC_SYS_PRI3_PENDSV_S  21
#define NVIC_SYS_PRI3_DEBUG_S   5

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_SYS_HND_CTRL
// register.
//
//*****************************************************************************
#define NVIC_SYS_HND_CTRL_USAGE 0x00040000  // Usage Fault Enable
#define NVIC_SYS_HND_CTRL_BUS   0x00020000  // Bus Fault Enable
#define NVIC_SYS_HND_CTRL_MEM   0x00010000  // Memory Management Fault Enable
#define NVIC_SYS_HND_CTRL_SVC   0x00008000  // SVC Call Pending
#define NVIC_SYS_HND_CTRL_BUSP  0x00004000  // Bus Fault Pending
#define NVIC_SYS_HND_CTRL_MEMP  0x00002000  // Memory Management Fault Pending
#define NVIC_SYS_HND_CTRL_USAGEP \
                                0x00001000  // Usage Fault Pending
#define NVIC_SYS_HND_CTRL_TICK  0x00000800  // SysTick Exception Active
#define NVIC_SYS_HND_CTRL_PNDSV 0x00000400  // PendSV Exception Active
#define NVIC_SYS_HND_CTRL_MON   0x00000100  // Debug Monitor Active
#define NVIC_SYS_HND_CTRL_SVCA  0x00000080  // SVC Call Active
#define NVIC_SYS_HND_CTRL_USGA  0x00000008  // Usage Fault Active
#define NVIC_SYS_HND_CTRL_BUSA  0x00000002  // Bus Fault Active
#define NVIC_SYS_HND_CTRL_MEMA  0x00000001  // Memory Management Fault Active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_FAULT_STAT
// register.
//
//*****************************************************************************
#define NVIC_FAULT_STAT_DIV0    0x02000000  // Divide-by-Zero Usage Fault
#define NVIC_FAULT_STAT_UNALIGN 0x01000000  // Unaligned Access Usage Fault
#define NVIC_FAULT_STAT_NOCP    0x00080000  // No Coprocessor Usage Fault
#define NVIC_FAULT_STAT_INVPC   0x00040000  // Invalid PC Load Usage Fault
#define NVIC_FAULT_STAT_INVSTAT 0x00020000  // Invalid State Usage Fault
#define NVIC_FAULT_STAT_UNDEF   0x00010000  // Undefined Instruction Usage
                                            // Fault
#define NVIC_FAULT_STAT_BFARV   0x00008000  // Bus Fault Address Register Valid
#define NVIC_FAULT_STAT_BLSPERR 0x00002000  // Bus Fault on Floating-Point Lazy
                                            // State Preservation
#define NVIC_FAULT_STAT_BSTKE   0x00001000  // Stack Bus Fault
#define NVIC_FAULT_STAT_BUSTKE  0x00000800  // Unstack Bus Fault
#define NVIC_FAULT_STAT_IMPRE   0x00000400  // Imprecise Data Bus Error
#define NVIC_FAULT_STAT_PRECISE 0x00000200  // Precise Data Bus Error
#define NVIC_FAULT_STAT_IBUS    0x00000100  // Instruction Bus Error
#define NVIC_FAULT_STAT_MMARV   0x00000080  // Memory Management Fault Address
                                            // Register Valid
#define NVIC_FAULT_STAT_MLSPERR 0x00000020  // Memory Management Fault on
                                            // Floating-Point Lazy State
                                            // Preservation
#define NVIC_FAULT_STAT_MSTKE   0x00000010  // Stack Access Violation
#define NVIC_FAULT_STAT_MUSTKE  0x00000008  // Unstack Access Violation
#define NVIC_FAULT_STAT_DERR    0x00000002  // Data Access Violation
#define NVIC_FAULT_STAT_IERR    0x00000001  // Instruction Access Violation

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_HFAULT_STAT
// register.
//
//*****************************************************************************
#define NVIC_HFAULT_STAT_DBG    0x80000000  // Debug Event
#define NVIC_HFAULT_STAT_FORCED 0x40000000  // Forced Hard Fault
#define NVIC_HFAULT_STAT_VECT   0x00000002  // Vector Table Read Fault

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DEBUG_STAT
// register.
//
//*****************************************************************************
#define NVIC_DEBUG_STAT_EXTRNL  0x00000010  // EDBGRQ asserted
#define NVIC_DEBUG_STAT_VCATCH  0x00000008  // Vector catch
#define NVIC_DEBUG_STAT_DWTTRAP 0x00000004  // DWT match
#define NVIC_DEBUG_STAT_BKPT    0x00000002  // Breakpoint instruction
#define NVIC_DEBUG_STAT_HALTED  0x00000001  // Halt request

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MM_ADDR register.
//
//*****************************************************************************
#define NVIC_MM_ADDR_M          0xFFFFFFFF  // Fault Address
#define NVIC_MM_ADDR_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_FAULT_ADDR
// register.
//
//*****************************************************************************
#define NVIC_FAULT_ADDR_M       0xFFFFFFFF  // Fault Address
#define NVIC_FAULT_ADDR_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_CPAC register.
//
//*****************************************************************************
#define NVIC_CPAC_CP11_M        0x00C00000  // CP11 Coprocessor Access
                                            // Privilege
#define NVIC_CPAC_CP11_DIS      0x00000000  // Access Denied
#define NVIC_CPAC_CP11_PRIV     0x00400000  // Privileged Access Only
#define NVIC_CPAC_CP11_FULL     0x00C00000  // Full Access
#define NVIC_CPAC_CP10_M        0x00300000  // CP10 Coprocessor Access
                                            // Privilege
#define NVIC_CPAC_CP10_DIS      0x00000000  // Access Denied
#define NVIC_CPAC_CP10_PRIV     0x00100000  // Privileged Access Only
#define NVIC_CPAC_CP10_FULL     0x00300000  // Full Access

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_TYPE register.
//
//*****************************************************************************
#define NVIC_MPU_TYPE_IREGION_M 0x00FF0000  // Number of I Regions
#define NVIC_MPU_TYPE_DREGION_M 0x0000FF00  // Number of D Regions
#define NVIC_MPU_TYPE_SEPARATE  0x00000001  // Separate or Unified MPU
#define NVIC_MPU_TYPE_IREGION_S 16
#define NVIC_MPU_TYPE_DREGION_S 8

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_CTRL register.
//
//*****************************************************************************
#define NVIC_MPU_CTRL_PRIVDEFEN 0x00000004  // MPU Default Region
#define NVIC_MPU_CTRL_HFNMIENA  0x00000002  // MPU Enabled During Faults
#define NVIC_MPU_CTRL_ENABLE    0x00000001  // MPU Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_NUMBER
// register.
//
//*****************************************************************************
#define NVIC_MPU_NUMBER_M       0x00000007  // MPU Region to Access
#define NVIC_MPU_NUMBER_S       0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_BASE register.
//
//*****************************************************************************
#define NVIC_MPU_BASE_ADDR_M    0xFFFFFFE0  // Base Address Mask
#define NVIC_MPU_BASE_VALID     0x00000010  // Region Number Valid
#define NVIC_MPU_BASE_REGION_M  0x00000007  // Region Number
#define NVIC_MPU_BASE_ADDR_S    5
#define NVIC_MPU_BASE_REGION_S  0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_ATTR register.
//
//*****************************************************************************
#define NVIC_MPU_ATTR_XN        0x10000000  // Instruction Access Disable
#define NVIC_MPU_ATTR_AP_M      0x07000000  // Access Privilege
#define NVIC_MPU_ATTR_TEX_M     0x00380000  // Type Extension Mask
#define NVIC_MPU_ATTR_SHAREABLE 0x00040000  // Shareable
#define NVIC_MPU_ATTR_CACHEABLE 0x00020000  // Cacheable
#define NVIC_MPU_ATTR_BUFFRABLE 0x00010000  // Bufferable
#define NVIC_MPU_ATTR_SRD_M     0x0000FF00  // Subregion Disable Bits
#define NVIC_MPU_ATTR_SIZE_M    0x0000003E  // Region Size Mask
#define NVIC_MPU_ATTR_ENABLE    0x00000001  // Region Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_BASE1 register.
//
//*****************************************************************************
#define NVIC_MPU_BASE1_ADDR_M   0xFFFFFFE0  // Base Address Mask
#define NVIC_MPU_BASE1_VALID    0x00000010  // Region Number Valid
#define NVIC_MPU_BASE1_REGION_M 0x00000007  // Region Number
#define NVIC_MPU_BASE1_ADDR_S   5
#define NVIC_MPU_BASE1_REGION_S 0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_ATTR1 register.
//
//*****************************************************************************
#define NVIC_MPU_ATTR1_XN       0x10000000  // Instruction Access Disable
#define NVIC_MPU_ATTR1_AP_M     0x07000000  // Access Privilege
#define NVIC_MPU_ATTR1_TEX_M    0x00380000  // Type Extension Mask
#define NVIC_MPU_ATTR1_SHAREABLE \
                                0x00040000  // Shareable
#define NVIC_MPU_ATTR1_CACHEABLE \
                                0x00020000  // Cacheable
#define NVIC_MPU_ATTR1_BUFFRABLE \
                                0x00010000  // Bufferable
#define NVIC_MPU_ATTR1_SRD_M    0x0000FF00  // Subregion Disable Bits
#define NVIC_MPU_ATTR1_SIZE_M   0x0000003E  // Region Size Mask
#define NVIC_MPU_ATTR1_ENABLE   0x00000001  // Region Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_BASE2 register.
//
//*****************************************************************************
#define NVIC_MPU_BASE2_ADDR_M   0xFFFFFFE0  // Base Address Mask
#define NVIC_MPU_BASE2_VALID    0x00000010  // Region Number Valid
#define NVIC_MPU_BASE2_REGION_M 0x00000007  // Region Number
#define NVIC_MPU_BASE2_ADDR_S   5
#define NVIC_MPU_BASE2_REGION_S 0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_ATTR2 register.
//
//*****************************************************************************
#define NVIC_MPU_ATTR2_XN       0x10000000  // Instruction Access Disable
#define NVIC_MPU_ATTR2_AP_M     0x07000000  // Access Privilege
#define NVIC_MPU_ATTR2_TEX_M    0x00380000  // Type Extension Mask
#define NVIC_MPU_ATTR2_SHAREABLE \
                                0x00040000  // Shareable
#define NVIC_MPU_ATTR2_CACHEABLE \
                                0x00020000  // Cacheable
#define NVIC_MPU_ATTR2_BUFFRABLE \
                                0x00010000  // Bufferable
#define NVIC_MPU_ATTR2_SRD_M    0x0000FF00  // Subregion Disable Bits
#define NVIC_MPU_ATTR2_SIZE_M   0x0000003E  // Region Size Mask
#define NVIC_MPU_ATTR2_ENABLE   0x00000001  // Region Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_BASE3 register.
//
//*****************************************************************************
#define NVIC_MPU_BASE3_ADDR_M   0xFFFFFFE0  // Base Address Mask
#define NVIC_MPU_BASE3_VALID    0x00000010  // Region Number Valid
#define NVIC_MPU_BASE3_REGION_M 0x00000007  // Region Number
#define NVIC_MPU_BASE3_ADDR_S   5
#define NVIC_MPU_BASE3_REGION_S 0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_MPU_ATTR3 register.
//
//*****************************************************************************
#define NVIC_MPU_ATTR3_XN       0x10000000  // Instruction Access Disable
#define NVIC_MPU_ATTR3_AP_M     0x07000000  // Access Privilege
#define NVIC_MPU_ATTR3_TEX_M    0x00380000  // Type Extension Mask
#define NVIC_MPU_ATTR3_SHAREABLE \
                                0x00040000  // Shareable
#define NVIC_MPU_ATTR3_CACHEABLE \
                                0x00020000  // Cacheable
#define NVIC_MPU_ATTR3_BUFFRABLE \
                                0x00010000  // Bufferable
#define NVIC_MPU_ATTR3_SRD_M    0x0000FF00  // Subregion Disable Bits
#define NVIC_MPU_ATTR3_SIZE_M   0x0000003E  // Region Size Mask
#define NVIC_MPU_ATTR3_ENABLE   0x00000001  // Region Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DBG_CTRL register.
//
//*****************************************************************************
#define NVIC_DBG_CTRL_DBGKEY_M  0xFFFF0000  // Debug key mask
#define NVIC_DBG_CTRL_DBGKEY    0xA05F0000  // Debug key
#define NVIC_DBG_CTRL_S_RESET_ST \
                                0x02000000  // Core has reset since last read
#define NVIC_DBG_CTRL_S_RETIRE_ST \
                                0x01000000  // Core has executed insruction
                                            // since last read
#define NVIC_DBG_CTRL_S_LOCKUP  0x00080000  // Core is locked up
#define NVIC_DBG_CTRL_S_SLEEP   0x00040000  // Core is sleeping
#define NVIC_DBG_CTRL_S_HALT    0x00020000  // Core status on halt
#define NVIC_DBG_CTRL_S_REGRDY  0x00010000  // Register read/write available
#define NVIC_DBG_CTRL_C_SNAPSTALL \
                                0x00000020  // Breaks a stalled load/store
#define NVIC_DBG_CTRL_C_MASKINT 0x00000008  // Mask interrupts when stepping
#define NVIC_DBG_CTRL_C_STEP    0x00000004  // Step the core
#define NVIC_DBG_CTRL_C_HALT    0x00000002  // Halt the core
#define NVIC_DBG_CTRL_C_DEBUGEN 0x00000001  // Enable debug

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DBG_XFER register.
//
//*****************************************************************************
#define NVIC_DBG_XFER_REG_WNR   0x00010000  // Write or not read
#define NVIC_DBG_XFER_REG_SEL_M 0x0000001F  // Register
#define NVIC_DBG_XFER_REG_R0    0x00000000  // Register R0
#define NVIC_DBG_XFER_REG_R1    0x00000001  // Register R1
#define NVIC_DBG_XFER_REG_R2    0x00000002  // Register R2
#define NVIC_DBG_XFER_REG_R3    0x00000003  // Register R3
#define NVIC_DBG_XFER_REG_R4    0x00000004  // Register R4
#define NVIC_DBG_XFER_REG_R5    0x00000005  // Register R5
#define NVIC_DBG_XFER_REG_R6    0x00000006  // Register R6
#define NVIC_DBG_XFER_REG_R7    0x00000007  // Register R7
#define NVIC_DBG_XFER_REG_R8    0x00000008  // Register R8
#define NVIC_DBG_XFER_REG_R9    0x00000009  // Register R9
#define NVIC_DBG_XFER_REG_R10   0x0000000A  // Register R10
#define NVIC_DBG_XFER_REG_R11   0x0000000B  // Register R11
#define NVIC_DBG_XFER_REG_R12   0x0000000C  // Register R12
#define NVIC_DBG_XFER_REG_R13   0x0000000D  // Register R13
#define NVIC_DBG_XFER_REG_R14   0x0000000E  // Register R14
#define NVIC_DBG_XFER_REG_R15   0x0000000F  // Register R15
#define NVIC_DBG_XFER_REG_FLAGS 0x00000010  // xPSR/Flags register
#define NVIC_DBG_XFER_REG_MSP   0x00000011  // Main SP
#define NVIC_DBG_XFER_REG_PSP   0x00000012  // Process SP
#define NVIC_DBG_XFER_REG_DSP   0x00000013  // Deep SP
#define NVIC_DBG_XFER_REG_CFBP  0x00000014  // Control/Fault/BasePri/PriMask

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DBG_DATA register.
//
//*****************************************************************************
#define NVIC_DBG_DATA_M         0xFFFFFFFF  // Data temporary cache
#define NVIC_DBG_DATA_S         0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_DBG_INT register.
//
//*****************************************************************************
#define NVIC_DBG_INT_HARDERR    0x00000400  // Debug trap on hard fault
#define NVIC_DBG_INT_INTERR     0x00000200  // Debug trap on interrupt errors
#define NVIC_DBG_INT_BUSERR     0x00000100  // Debug trap on bus error
#define NVIC_DBG_INT_STATERR    0x00000080  // Debug trap on usage fault state
#define NVIC_DBG_INT_CHKERR     0x00000040  // Debug trap on usage fault check
#define NVIC_DBG_INT_NOCPERR    0x00000020  // Debug trap on coprocessor error
#define NVIC_DBG_INT_MMERR      0x00000010  // Debug trap on mem manage fault
#define NVIC_DBG_INT_RESET      0x00000008  // Core reset status
#define NVIC_DBG_INT_RSTPENDCLR 0x00000004  // Clear pending core reset
#define NVIC_DBG_INT_RSTPENDING 0x00000002  // Core reset is pending
#define NVIC_DBG_INT_RSTVCATCH  0x00000001  // Reset vector catch

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_SW_TRIG register.
//
//*****************************************************************************
#define NVIC_SW_TRIG_INTID_M    0x000000FF  // Interrupt ID
#define NVIC_SW_TRIG_INTID_S    0

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_FPCC register.
//
//*****************************************************************************
#define NVIC_FPCC_ASPEN         0x80000000  // Automatic State Preservation
                                            // Enable
#define NVIC_FPCC_LSPEN         0x40000000  // Lazy State Preservation Enable
#define NVIC_FPCC_MONRDY        0x00000100  // Monitor Ready
#define NVIC_FPCC_BFRDY         0x00000040  // Bus Fault Ready
#define NVIC_FPCC_MMRDY         0x00000020  // Memory Management Fault Ready
#define NVIC_FPCC_HFRDY         0x00000010  // Hard Fault Ready
#define NVIC_FPCC_THREAD        0x00000008  // Thread Mode
#define NVIC_FPCC_USER          0x00000002  // User Privilege Level
#define NVIC_FPCC_LSPACT        0x00000001  // Lazy State Preservation Active

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_FPCA register.
//
//*****************************************************************************
#define NVIC_FPCA_ADDRESS_M     0xFFFFFFF8  // Address
#define NVIC_FPCA_ADDRESS_S     3

//*****************************************************************************
//
// The following are defines for the bit fields in the NVIC_FPDSC register.
//
//*****************************************************************************
#define NVIC_FPDSC_AHP          0x04000000  // AHP Bit Default
#define NVIC_FPDSC_DN           0x02000000  // DN Bit Default
#define NVIC_FPDSC_FZ           0x01000000  // FZ Bit Default
#define NVIC_FPDSC_RMODE_M      0x00C00000  // RMODE Bit Default
#define NVIC_FPDSC_RMODE_RN     0x00000000  // Round to Nearest (RN) mode
#define NVIC_FPDSC_RMODE_RP     0x00400000  // Round towards Plus Infinity (RP)
                                            // mode
#define NVIC_FPDSC_RMODE_RM     0x00800000  // Round towards Minus Infinity
                                            // (RM) mode
#define NVIC_FPDSC_RMODE_RZ     0x00C00000  // Round towards Zero (RZ) mode

//*****************************************************************************
//
// The following definitions are deprecated.
//
//*****************************************************************************
#ifndef DEPRECATED

//*****************************************************************************
//
// Deprecated defines for the bit fields in the FAN_O_STS register.
//
//*****************************************************************************
#define FAN_STS_ST4_S           8
#define FAN_STS_ST3_S           6
#define FAN_STS_ST2_S           4
#define FAN_STS_ST1_S           2

//*****************************************************************************
//
// Deprecated defines for the EEPROM register offsets.
//
//*****************************************************************************
#define EEPROM_EEPROMPP_R       (*((volatile unsigned long *)0x400AFFC0))

//*****************************************************************************
//
// Deprecated defines for the bit fields in the EEPROM_EEPROMPP register.
//
//*****************************************************************************
#define EEPROM_EEPROMPP_SIZE_M  0x0000001F  // EEPROM Size
#define EEPROM_EEPROMPP_SIZE_S  0

#endif

#endif // __LM4FS1GH5BB_H__
