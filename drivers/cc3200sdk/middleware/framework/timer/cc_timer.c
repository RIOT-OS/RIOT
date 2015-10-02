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


#include "cc_timer.h"

#define U32NS_U16MS(nsec)  (nsec / 1000000)
#define U16MS_U32NS(msec)  (msec * 1000000)

/* Model the HW Timer information */
struct hwt_info {

        struct hw_timer_ops  *ops;       /* Methods to operate HW Timer */
        cc_hndl               hndl;      /* Reference  to real HW Timer */

        struct sw_timer      *used_list; // change to work_list? 
        bool                  hw_64bits;
        
        u32                   source;    /* Source that drives HW Timer */
};

static struct hwt_info hwt_objs[MAX_HWT_PLUG];

static sys_irq_enbl enbl_irqc;
static sys_irq_dsbl dsbl_irqc; 

/*-----------------------------------------------------------------------------
 * Utility functions to access either 32bits or 64bits HW Timer on platform
 *---------------------------------------------------------------------------*/
static struct hwt_info *get_hwt(u32 source)
{
        if((HW_REALTIME_CLK == source) ||
           (HW_MONOTONE_CTR == source)) {
                return hwt_objs + source;
        }

        return NULL;
}

static i32 hwt_start(struct hwt_info *hwt, struct u64_val *expires)
{
        struct hw_timer_ops *hwt_ops = hwt->ops;

        return  hwt->hw_64bits? 
                hwt_ops->start64(hwt->hndl, expires, HW_TIMER_MONOTONE) :
                hwt_ops->start32(hwt->hndl, expires->lo_32, HW_TIMER_MONOTONE);
            
}

static i32 hwt_update(struct hwt_info *hwt, struct u64_val *expires)
{
        struct hw_timer_ops *hwt_ops = hwt->ops;

        return  hwt->hw_64bits?
                hwt_ops->update_exp64(hwt->hndl, expires) :
                hwt_ops->update_exp32(hwt->hndl, expires->lo_32);
}

static i32 hwt_stop(struct hwt_info *hwt)
{
        struct hw_timer_ops *hwt_ops = hwt->ops;

        return hwt_ops->stop(hwt->hndl);
}

static i32 hwt_is_running(struct hwt_info *hwt)
{
        struct hw_timer_ops *hwt_ops = hwt->ops;

        return hwt_ops->is_running(hwt->hndl);
}

static i32 hwt_get_current(struct hwt_info *hwt, struct u64_val *current)
{
        struct hw_timer_ops *hwt_ops = hwt->ops;
        i32 rv = -1;
        
        if(hwt->hw_64bits)
                rv = hwt_ops->get_current64(hwt->hndl, current);
        else {
                rv = hwt_ops->get_current32(hwt->hndl, &current->lo_32);
                current->hi_32 = hwt_ops->get_rollovers(hwt->hndl);
        }

        return rv;
}

static i32 hwt_get_frequency(struct hwt_info *hwt)
{
        struct hw_timer_ops *hwt_ops = hwt->ops;

        return hwt_ops->get_frequency(hwt->hndl);
}

static inline bool hwt_is_64bits(struct hwt_info *hwt)
{
        return hwt->hw_64bits;
}

static inline bool hwt_has_abs_time(struct hwt_info *hwt)
{
        return (HW_REALTIME_CLK == hwt->source)? true : false;
}

/*-----------------------------------------------------------------------------
 * SW Timer structure and utility functions
 *----------------------------------------------------------------------------*/
struct sw_timer {

#define SW_TIMER_PERIODIC   0x00000001
#define SW_TIMER_HW_SCHED   0x00000002
#define SW_TIMER_ALLOCATE   0x00000004
#define SW_TIMER_ACTIVATE   0x00000008

        u32                   flags;       /* Flags to manage SW Timer state */

        struct u64_val        hwt_expires; /* Expiry value: unit is HW count */
        struct u64_val        sw_interval; /* SWT Timeout interval: HW count */ 
        
        cc_cb_fn              timeout_cb;  /* Invoke @ timeout: usr callback */
        cc_hndl               cb_param;

        struct hwt_info      *hwt_obj;     /* Associated Real-world HW Timer */

        struct sw_timer      *next;
};

static inline bool is_periodic(struct sw_timer *swt)
{
        return (swt->flags & SW_TIMER_PERIODIC)? true : false;
}

static inline void set_periodic(struct sw_timer *swt, bool periodic)
{
        if(periodic)
                swt->flags |=  SW_TIMER_PERIODIC;
        else
                swt->flags &= ~SW_TIMER_PERIODIC;
}

static inline void set_activate(struct sw_timer *swt, bool activate)
{
        if(activate)
                swt->flags |=  SW_TIMER_ACTIVATE;
        else
                swt->flags &= ~SW_TIMER_ACTIVATE;
}

static inline bool is_scheduled(struct sw_timer *swt)
{
        return (swt->flags & SW_TIMER_HW_SCHED)? true : false;
}

static inline void set_scheduled(struct sw_timer *swt, bool sched)
{
        if(sched) 
                swt->flags |=  SW_TIMER_HW_SCHED;
        else
                swt->flags &= ~SW_TIMER_HW_SCHED;
}

static inline void set_alloc_status(struct sw_timer *swt, bool alloc)
{
        if(alloc)
                swt->flags = SW_TIMER_ALLOCATE;
        else
                swt->flags = 0;
}

static inline bool is_alloc_only(struct sw_timer *swt)
{
        return (swt->flags == SW_TIMER_ALLOCATE)? true : false;
}

static inline bool has_started(struct sw_timer *swt)
{
        return !is_alloc_only(swt);
}

/*-----------------------------------------------------------------------------
 * 64bit Math utility functions
 *----------------------------------------------------------------------------*/

/* Returns: 1 for val1 > val2; 0 for val1 = val2; -1 for val1 < val2 */
static i32 cmp_u64(struct u64_val *u64_val1, struct u64_val *u64_val2)
{
        return util_u64_data_cmp(u64_val1, u64_val2);
}

#define NSEC_VAL_FOR_1SEC   1000000000

static i32 calc_ticks_interval_u32(u64 addendum, struct u64_val *sw_interval)
{
        u64 tmp64 = addendum + sw_interval->lo_32;

        if(0xFFFFFFFF < tmp64)
                return -1;  /* Exceeds 32bit value */
        
        sw_interval->lo_32 = tmp64 & 0xFFFFFFFF;
        sw_interval->hi_32 = 0;

        return 0;
}

static i32 calc_ticks_interval_u64(u64 addendum, struct u64_val *sw_interval)
{
        u64 tmp64 = addendum + sw_interval->lo_32;
        
        sw_interval->lo_32 =  tmp64 & 0xFFFFFFFF;
        sw_interval->hi_32 = (tmp64 >> 32) & 0xFFFFFFFF;
        
        return 0;
}

static 
i32 calc_ticks_interval(struct hwt_info *hwt, struct u64_time *time_u64, 
                        struct u64_val *sw_interval)
                       
{
        u64 tmp64 = 0;
        u32 hwt_freq = hwt_get_frequency(hwt);
        
        if(time_u64->nsec > (NSEC_VAL_FOR_1SEC - 1))
                return -1; /* Value exceeds a sec */

        tmp64 = ((u64)time_u64->nsec) * hwt_freq;
        sw_interval->lo_32 = tmp64 / NSEC_VAL_FOR_1SEC;

        return  hwt_is_64bits(hwt)? 
                calc_ticks_interval_u64(((u64)time_u64->secs) * hwt_freq,
                                        sw_interval) :
                calc_ticks_interval_u32(((u64)time_u64->secs) * hwt_freq,
                                        sw_interval);
}

static void calc_hwt_expiry_secs(struct hwt_info *hwt,
                                 struct u64_val  *hwt_current,
                                 struct u64_val  *sw_interval,
                                 struct u64_val  *hwt_expires)
{
        struct u64_time time_expiry = {0, 0};

        util_u32_nsec_add(hwt_current->lo_32,
                          sw_interval->lo_32,
                          &time_expiry);

        hwt_expires->lo_32  = time_expiry.nsec;
        hwt_expires->hi_32  = time_expiry.secs;

        hwt_expires->hi_32 += hwt_current->hi_32 + sw_interval->hi_32;
        
        return;
}

static void calc_hwt_expiry_tick(struct hwt_info *hwt,
                                 struct u64_val  *hwt_current,
                                 struct u64_val  *sw_interval,
                                 struct u64_val  *hwt_expires)
{
        util_u64_data_add(hwt_current, sw_interval, hwt_expires);

        return;
}

static void calc_hwt_expiry(struct hwt_info *hwt,
                            struct u64_val  *hwt_current, 
                            struct u64_val  *sw_interval,
                            struct u64_val  *hwt_expires)
{
        hwt_has_abs_time(hwt)?
                calc_hwt_expiry_secs(hwt, hwt_current, sw_interval,
                                     hwt_expires):
                calc_hwt_expiry_tick(hwt, hwt_current, sw_interval,
                                     hwt_expires);
        
        return;
}

/*-----------------------------------------------------------------------------
 * Core SW Timer servics
 *----------------------------------------------------------------------------*/
static void insert_ordered_expiry(struct sw_timer *elem, struct sw_timer **list)
{
        struct sw_timer *node = *list, *prev = NULL;

        elem->next = NULL;  /* Remove stale reference / info, if any */

        if(NULL == node) {  /* First user request for the  HW timer */
                *list = elem;
                goto sort_insert_elem_exit1;
        }

        while(node) {        /* There is atleast one active request */
                if(0 > cmp_u64(&elem->hwt_expires, &node->hwt_expires)) {
                        /* 'elem' expires earlier than this 'node'  */
                        elem->next = node; 
                        
                        if(NULL == prev) {
                                *list  = elem; /* New first element */
                                set_scheduled(node, false);
                        }
                        else
                                prev->next = elem; /* Intermediary  */

                        /* 'elem' has been placed in 'list', quit.. */
                        goto sort_insert_elem_exit1;
                }

                prev = node;
                node = node->next;
        }

        if(NULL == node) 
                prev->next = elem; /* Farthest in time for schedule */

 sort_insert_elem_exit1:
        return;
}

static i32 sched_timer_if_new(struct sw_timer *head)
{
        i32 rv = 0;
        struct hwt_info *hwt = head->hwt_obj;

        if(true == is_scheduled(head)) 
                goto sched_timer_if_new_exit;
        
        /* If HW timer running, then update it othewise start it */        
        rv = hwt_is_running(hwt)?
                hwt_update(hwt, &head->hwt_expires) : 
                hwt_start(hwt, &head->hwt_expires);
        
        if(0 == rv)
                set_scheduled(head, true);
        
 sched_timer_if_new_exit:
        return rv;
}

static i32 setup_timer_exec(struct sw_timer *swt, struct u64_val *hwt_current,
                            bool has_expval) 
{
        struct hwt_info *hwt = swt->hwt_obj;
        struct u64_val u64_current = {0, 0};

        if(NULL == hwt_current) {
                /* No ref provided for current value, so read it from HWT */
                hwt_current = &u64_current;

                /* Read HWT (current) counter value only if it is running */
                if((hwt_is_running(hwt) || hwt_has_abs_time(hwt)) && 
                   (-1 == hwt_get_current(hwt, hwt_current)))
                                return -1;
        }

        if(false == has_expval)
                calc_hwt_expiry(hwt, hwt_current, &swt->sw_interval,
                                &swt->hwt_expires);
        
        insert_ordered_expiry(swt, &hwt->used_list);

        sched_timer_if_new(hwt->used_list);

        set_activate(swt, true);

        return 0;
}

static i32 estbl_timer_interval(struct sw_timer *swt, struct u64_time *time_u64)
{
        struct hwt_info *hwt = swt->hwt_obj;

        if(hwt_has_abs_time(hwt)) {
                /* Work with user info, if HWT supports real-time */
                swt->sw_interval.hi_32 = time_u64->secs;
                swt->sw_interval.lo_32 = time_u64->nsec;
                return 0;
        }
        
        /* Convert time specified by user into ticks for HW Timer */
        return calc_ticks_interval(hwt, time_u64, &swt->sw_interval);
}

static i32 timer_start(struct sw_timer *swt, struct u64_time *time_u64,
                       u32 options)
{
        bool has_expval = false;

        /* Check if SWT has been already activated, if yes can't use */ 
        if(false == is_alloc_only(swt))
                return -1; /* Not only allocated but started as well */
        
        if(options & OPT_TIME_ABS_VALUE) {
                /* Absolute expiry time has been provided, so use it */
                swt->hwt_expires.hi_32 = time_u64->secs;
                swt->hwt_expires.lo_32 = time_u64->nsec;
                has_expval = true;
        } else {
                /* Need to establish (expiry) interval for this SWT  */
                if(-1 == estbl_timer_interval(swt, time_u64))
                        return -1;
        }

        set_periodic(swt, options & OPT_TIMER_PERIODIC? true : false);

        return setup_timer_exec(swt, NULL, has_expval);
}

static bool has_valid_opts(struct hwt_info *hwt, u32 options)
{
        /* Implementtion: A periodic timer can't work with absolute time */
        if((OPT_TIMER_PERIODIC & options) && (OPT_TIME_ABS_VALUE & options))
                return false; 

        /* A request for absolute time has to be supported in timer, HWT */
        if((options & OPT_TIME_ABS_VALUE) && !hwt_has_abs_time(hwt))
                return false;

        return true;
}

i32 cc_timer_start(cc_hndl hndl, struct u64_time *time_u64, u32 options)
{
        struct sw_timer *swt = (struct sw_timer*) hndl;
        struct u64_time time64_ref = {0, 0}; /* init */
        u32 intr_mask;
        i32 rv = -1;
        
        if((NULL == swt) || 
           (false == has_valid_opts(swt->hwt_obj, options)))
                goto cc_timer_start_exit1;
        
#define IS_LRT_TIMER(hwt) hwt_has_abs_time(hwt) /* LRT:: Low Resolution Timer */
        
        time64_ref.secs = time_u64->secs;
        time64_ref.nsec = time_u64->nsec;

        if(IS_LRT_TIMER(swt->hwt_obj)) {
                /* Reducing precision to millisec to avoid floating point ops */
                time64_ref.nsec = U16MS_U32NS(U32NS_U16MS(time_u64->nsec));
        }

        intr_mask = dsbl_irqc();
        rv = timer_start(swt, &time64_ref, options);
        enbl_irqc(intr_mask);

cc_timer_start_exit1:
        
        return rv;
} 

static i32 remove_elem(struct sw_timer *elem, struct sw_timer **list)
{
        struct sw_timer *node = *list, *prev = NULL;
        i32 rv = -1;

        if(NULL == node)
                return rv;

        while(node) {
                if(elem == node) {
                        if(NULL == prev)
                                *list      = node->next;
                        else
                                prev->next = node->next; 
                        
                        elem->next = NULL; /* Reset */
                        rv = 0;
                        break;  /* Exit while loop */
                }
                
                prev = node;
                node = node->next;
        }

        return rv;
}

static i32 timer_stop(struct sw_timer *swt)
{
        struct hwt_info *hwt = swt->hwt_obj;

        if(0 != remove_elem(swt, &hwt->used_list))
                return -1; /* Did not find SWT in 'used_list' */

        /* Clear all run-time flags */
        set_scheduled(swt, false);
        set_periodic(swt, false);
        set_activate(swt, false);

        if(NULL != hwt->used_list)
                sched_timer_if_new(hwt->used_list);
        else
                hwt_stop(hwt); /* No pending request, stop HW */

        return 0;
}

i32 cc_timer_stop(cc_hndl hndl)
{
        struct sw_timer *swt = (struct sw_timer*) hndl;
        u32 intr_mask;
        i32 rv = -1;

        intr_mask = dsbl_irqc();
        if(swt && has_started(swt)) {
                rv = timer_stop(swt);
        }
        enbl_irqc(intr_mask);

        return rv;
}

/* Called in the interrupt context of the HWT */
static void handle_expired_swt(struct sw_timer *swt)
{
        set_scheduled(swt, false);

        /* Invoke and service user's callback routine */
        if(swt->timeout_cb)
                swt->timeout_cb(swt->cb_param);
        
        /* Period timer: Set-up next expiry iteration */
        if(true == is_periodic(swt)) {
                struct u64_val *p;
                struct u64_val hwt_current;
                u32 intr_mask = dsbl_irqc();

                p = &swt->hwt_expires;
                hwt_current.hi_32 = p->hi_32;
                hwt_current.lo_32 = p->lo_32;

                setup_timer_exec(swt, &hwt_current,
                                 false /* Do ExpVal */);
                enbl_irqc(intr_mask);
        } else {
                /* Timer is not required to re-start. */
                set_activate(swt, false);
        }

        return;
}

/* Called in the interrupt context of the HWT */
static struct sw_timer *isolate_expired_swt_list(struct hwt_info *hwt)
{
        struct sw_timer *elem = NULL, *head = NULL, *tail = NULL;
        struct u64_val hwt_current = {0, 0};

        if(0 != hwt_get_current(hwt, &hwt_current))
                return NULL;

        /*** Watch out for assignment in the 'while' statement ***/
        while(elem = hwt->used_list) {

                if(0 > cmp_u64(&hwt_current, &elem->hwt_expires)) {
                        /* SWT is yet to reach expiry, sched it  */
                        sched_timer_if_new(elem);
                        break;
                }

                /* Remove an expired SWT 'elem' from 'used_list' */
                remove_elem(elem, &hwt->used_list);

                if(NULL == head) head = elem;

                if(tail)   tail->next = elem;

                tail = elem;
        }

        return head; /* Head of list of expired SW-Timers (SWTs) */
}

/* To be invoked by HAL and in the interrupt context of CPU */
static void timer_isr(cc_hndl cb_param)
{
        struct hwt_info *hwt = (struct hwt_info*) cb_param;
        struct sw_timer *swt_list = NULL;
        u32 intr_mask = 0;

        if(NULL == hwt)
                return;

        /* Lockout higher priority or nested IRQ(s) */
        intr_mask = dsbl_irqc();
        swt_list = isolate_expired_swt_list(hwt);
        enbl_irqc(intr_mask);

        while(swt_list) {
                /* Run thru list of expireed SWT(s) */
                struct sw_timer *swt = swt_list;
                swt_list = swt_list->next;
                handle_expired_swt(swt);
        }

        intr_mask = dsbl_irqc();
        if(NULL == hwt->used_list)
                /* No active SW-Timer, so stop HWT */
                hwt_stop(hwt);
        enbl_irqc(intr_mask);        

        return;
}

/*-----------------------------------------------------------------------------
 * SW Timer Service Initialization
 *----------------------------------------------------------------------------*/
static struct sw_timer  sw_timers[MAX_SW_TIMER];
static struct sw_timer *free_list = NULL;

cc_hndl cc_timer_create(struct cc_timer_cfg *cfg)
{
        struct sw_timer *swt = NULL;
        struct hwt_info *hwt = NULL;
        u32 intr_mask;        

        hwt = get_hwt(cfg->source);
        if((NULL == hwt) || (NULL == hwt->ops))
                goto cc_timer_create_exit1;

        intr_mask = dsbl_irqc();  /* Disable interrupts */

        swt = free_list;
        if(NULL == swt)
                goto cc_timer_create_exit2;

        free_list = swt->next;
        swt->next = NULL;

        set_alloc_status(swt, true);
        swt->hwt_obj    = hwt;
        swt->timeout_cb = cfg->timeout_cb;
        swt->cb_param   = cfg->cb_param;

 cc_timer_create_exit2:
        enbl_irqc(intr_mask);     /* Enable  interrupts */

 cc_timer_create_exit1:

        return (cc_hndl) swt;
}

i32 cc_timer_delete(cc_hndl hndl)
{
        struct sw_timer *swt = (struct sw_timer*) hndl;
        u32 intr_mask;
        i32 rv = -1;

        intr_mask = dsbl_irqc();

        if((NULL == swt)                  ||
           (false == is_alloc_only(swt)))
                /* Timer is started; stop it first */
                goto cc_timer_delete_exit1;

        set_alloc_status(swt, false);
        memset(swt, 0, sizeof(swt));
        swt->hwt_obj = NULL;
        swt->next    = NULL;

        swt->next = free_list;
        free_list = swt;
        rv        = 0;

 cc_timer_delete_exit1:
        enbl_irqc(intr_mask);

        return rv;
}

static i32 sw_timers_init(void)
{
        i32 i = 0;

        memset(sw_timers, 0, sizeof(sw_timers));

        for(i = 0; i < MAX_SW_TIMER; i++) {
                struct sw_timer *swt = sw_timers + i;
                swt->next = free_list;
                free_list = swt;
        }

        return 0;
}

static i32 hw_timers_init(void)
{
        i32 i = 0;
        i32 max_val = MAX_HWT_PLUG - 1;
        
        if((HW_REALTIME_CLK > max_val) || (HW_MONOTONE_CTR > max_val))
                return -1;
        
        for(i = 0; i < MAX_HWT_PLUG; i++) {
                struct hwt_info *hwt = hwt_objs + i;

                hwt->ops       = NULL;
                hwt->hndl      = NULL;
                hwt->used_list = NULL;
                hwt->hw_64bits = false;
                hwt->source    = i;
        }
        
        return 0;
}

i32 cc_timer_module_init(struct cc_timer_setup *timer_setup)
{
        if((NULL == timer_setup->enbl_irqc) || 
           (NULL == timer_setup->dsbl_irqc)) {
                return -1;
        }

        enbl_irqc = timer_setup->enbl_irqc;
        dsbl_irqc = timer_setup->dsbl_irqc;

        return sw_timers_init() || hw_timers_init();
}

i32 cc_timer_register_hwt_ops(u32 source,  cc_hndl hwt_hndl, 
                              struct hw_timer_ops *hwt_ops)
{
        struct hwt_info *hwt = get_hwt(source);
        bool op64;
        
        if((NULL == hwt) || (NULL != hwt->ops) || (NULL == hwt_hndl)) 
                return -1; /* HWT ops already initialized */

#define HAS_HWT_OPS(f1, f2, f3, f4)                                     \
        (hwt_ops->f1 && hwt_ops->f2 && hwt_ops->f3 && hwt_ops->f4)?  true : false
        
        op64 = HAS_HWT_OPS(start64, update_exp64, get_remaining64,
                           get_current64);

        /* Set of ops specific to either 32bit or 64bit must be available */
        if(!(HAS_HWT_OPS(start32, update_exp32, get_remaining32, get_current32) ^ 
             op64))
                return -1;
        
        /* Set of ops common to both 32bit or 64bit must be availalbe */
        if(!HAS_HWT_OPS(stop, is_running, get_rollovers, get_frequency) ||
           !hwt_ops->register_cb)
                return -1;

        /* Install callback function in HW Timer */
        if(-1 == hwt_ops->register_cb(hwt_hndl, timer_isr, hwt))
                return -1;

        hwt->ops       = hwt_ops;
        hwt->hndl      = hwt_hndl;
        hwt->hw_64bits = op64;

        return 0;
}
