/*

Copyright (c) 2010 - 2025, Nordic Semiconductor ASA All rights reserved.

SPDX-License-Identifier: BSD-3-Clause

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of Nordic Semiconductor ASA nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef NRF52840_NAME_CHANGE_H
#define NRF52840_NAME_CHANGE_H

/*lint ++flb "Enter library region */

/* This file is given to prevent your SW from not compiling with the updates made to nrf52840.h and 
 * nrf52840_bitfields.h. The macros defined in this file were available previously. Do not use these
 * macros on purpose. Use the ones defined in nrf52810.h and nrf52810_bitfields.h instead.
 */

/* IRQ */
/* Some IRQns have changed name. */
#ifndef POWER_CLOCK_IRQn
    #define POWER_CLOCK_IRQn                                CLOCK_POWER_IRQn
#endif
#ifndef UARTE0_UART0_IRQn
    #define UARTE0_UART0_IRQn                               UART0_UARTE0_IRQn
#endif
#ifndef SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn
    #define SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn          SPI0_SPIM0_SPIS0_TWI0_TWIM0_TWIS0_IRQn
#endif
#ifndef SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn
    #define SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn          SPI1_SPIM1_SPIS1_TWI1_TWIM1_TWIS1_IRQn
#endif
#ifndef CCM_AAR_IRQn
    #define CCM_AAR_IRQn                                    AAR_CCM_IRQn
#endif
#ifndef SPIM2_SPIS2_SPI2_IRQn
    #define SPIM2_SPIS2_SPI2_IRQn                           SPI2_SPIM2_SPIS2_IRQn
#endif
#ifndef SWI0_EGU0_IRQn
    #define SWI0_EGU0_IRQn                                  EGU0_SWI0_IRQn
#endif
#ifndef SWI1_EGU1_IRQn
    #define SWI1_EGU1_IRQn                                  EGU1_SWI1_IRQn
#endif
#ifndef SWI2_EGU2_IRQn
    #define SWI2_EGU2_IRQn                                  EGU2_SWI2_IRQn
#endif
#ifndef SWI3_EGU3_IRQn
    #define SWI3_EGU3_IRQn                                  EGU3_SWI3_IRQn
#endif
#ifndef SWI4_EGU4_IRQn
    #define SWI4_EGU4_IRQn                                  EGU4_SWI4_IRQn
#endif
#ifndef SWI5_EGU5_IRQn
    #define SWI5_EGU5_IRQn                                  EGU5_SWI5_IRQn
#endif

/* Some IRQHandlers have changed name. */
#ifndef POWER_CLOCK_IRQHandler
    #define POWER_CLOCK_IRQHandler                                CLOCK_POWER_IRQHandler
#endif
#ifndef UARTE0_UART0_IRQHandler
    #define UARTE0_UART0_IRQHandler                               UART0_UARTE0_IRQHandler
#endif
#ifndef SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
    #define SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler          SPI0_SPIM0_SPIS0_TWI0_TWIM0_TWIS0_IRQHandler
#endif
#ifndef SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
    #define SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler          SPI1_SPIM1_SPIS1_TWI1_TWIM1_TWIS1_IRQHandler
#endif
#ifndef CCM_AAR_IRQHandler
    #define CCM_AAR_IRQHandler                                    AAR_CCM_IRQHandler
#endif
#ifndef SPIM2_SPIS2_SPI2_IRQHandler
    #define SPIM2_SPIS2_SPI2_IRQHandler                           SPI2_SPIM2_SPIS2_IRQHandler
#endif
#ifndef SWI0_EGU0_IRQHandler
    #define SWI0_EGU0_IRQHandler                                  EGU0_SWI0_IRQHandler
#endif
#ifndef SWI1_EGU1_IRQHandler
    #define SWI1_EGU1_IRQHandler                                  EGU1_SWI1_IRQHandler
#endif
#ifndef SWI2_EGU2_IRQHandler
    #define SWI2_EGU2_IRQHandler                                  EGU2_SWI2_IRQHandler
#endif
#ifndef SWI3_EGU3_IRQHandler
    #define SWI3_EGU3_IRQHandler                                  EGU3_SWI3_IRQHandler
#endif
#ifndef SWI4_EGU4_IRQHandler
    #define SWI4_EGU4_IRQHandler                                  EGU4_SWI4_IRQHandler
#endif
#ifndef SWI5_EGU5_IRQHandler
    #define SWI5_EGU5_IRQHandler                                  EGU5_SWI5_IRQHandler
#endif

 
/* CC_HOST_RGF */
/* Enumerated Value Debug was renamed to DebugEnable in register HOST_IOT. */
#define  CC_HOST_RGF_HOST_IOT_LCS_LCS_Debug  CC_HOST_RGF_HOST_IOT_LCS_LCS_DebugEnable
 
 /*lint --flb "Leave library region" */

#endif /* NRF52840_NAME_CHANGE_H */
