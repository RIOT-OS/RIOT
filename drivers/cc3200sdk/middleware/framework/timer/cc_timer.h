//*****************************************************************************
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

/*******************************************************************************
 *  cc_timer.h - API and other data constructs of timer framework.
 *******************************************************************************
 */
#ifndef _CC_TIMER_H_
#define _CC_TIMER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "hw_timer32.h"

#define MAX_HWT_PLUG  2    /* Max # of HW timers managed by timer Framework */
#define MAX_SW_TIMER  6    /* Maximum # of SW Timers at run time in system  */


struct cc_timer_cfg {
        
        u32      source;      /* REALTIMER or MONOTONIC */
        cc_cb_fn timeout_cb;
        cc_hndl  cb_param;
};

struct cc_timer_setup {
        sys_irq_enbl enbl_irqc;
        sys_irq_dsbl dsbl_irqc;        
};

/*
  Create a software timer context.
  
  returns a valid handle on success otherwise NULL
*/
cc_hndl cc_timer_create(struct cc_timer_cfg *cfg);

/* 
   Kick start or activate a timer that was already created. 

   Note: A timer can't be marked periodic with an absolute time. In other words,
   flags can't be enabled for both periodicity and absolute time.

   returns 0 on success otherwise -1.
*/
#define OPT_TIMER_ONE_SHOT	0x00
#define OPT_TIMER_PERIODIC  0x01
#define OPT_TIME_ABS_VALUE  0x02

i32 cc_timer_start(cc_hndl hndl, struct u64_time *time_u64, u32 options);

/* 
   Stop an active timer.

   returns 0 on success otherwise -1.
*/
i32 cc_timer_stop(cc_hndl hndl);


/*
  Deletes an inactive timer

  returns 0 on success otherwise -1.
*/
i32 cc_timer_delete(cc_hndl hndl);

/* 
   Initialize SW Timer Framework.

   Returns 0 on success otherwise -1.
*/
i32 cc_timer_module_init(struct cc_timer_setup *timer_setup);

/*
  Register context, source and operations of the HW Timer (hwt).

  Returns 0 on success otherwise -1.
*/
i32 cc_timer_register_hwt_ops(u32 source, cc_hndl hwt_hndl, 
                              struct hw_timer_ops *hwt_ops);


#endif //_CC_TIMER_H_

