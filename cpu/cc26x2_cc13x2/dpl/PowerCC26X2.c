/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== PowerCC26X2_nortos.c ========
 */

#include <stdio.h>
#include <stdlib.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>

#include <inc/hw_types.h>
#include <driverlib/prcm.h>
#include <driverlib/osc.h>
#include <driverlib/cpu.h>
#include <driverlib/sys_ctrl.h>
#include <driverlib/vims.h>

extern PowerCC26X2_ModuleState PowerCC26X2_module;

static uintptr_t PowerCC26X2_swiKey;

/*
 *  ======== PowerCC26XX_standbyPolicy ========
 */
void PowerCC26XX_standbyPolicy()
{
    bool justIdle = true;
    uint32_t constraints;
    uint32_t ticks, time;
    uintptr_t key;

    /* disable interrupts */
    key = HwiP_disable();

    /*
     * Check if the Power policy has been disabled since we last checked.
     * Since we're in this policy function already, the policy must have
     * been enabled (with a valid policyFxn) when we were called, but
     * could have been disbled to short-circuit this function.
     * SemaphoreP_post() does this purposely (see comments in there).
     */
    if (!PowerCC26X2_module.enablePolicy) {
        HwiP_restore(key);

        return;
    }

    /* check operating conditions, optimally choose DCDC versus GLDO */
    SysCtrl_DCDC_VoltageConditionalControl();

    /* query the declared constraints */
    constraints = Power_getConstraintMask();

    /* do quick check to see if only WFI allowed; if yes, do it now */
    if ((constraints &
        ((1 << PowerCC26XX_DISALLOW_STANDBY) |
            (1 << PowerCC26XX_DISALLOW_IDLE))) ==
        ((1 << PowerCC26XX_DISALLOW_STANDBY) |
            (1 << PowerCC26XX_DISALLOW_IDLE))) {
        PRCMSleep();
    }
    /*
     *  check if any sleep modes are allowed for automatic activation
     */
    else {
        /* check if we are allowed to go to standby */
        if ((constraints & (1 << PowerCC26XX_DISALLOW_STANDBY)) == 0) {
            /*
             * Check how many ticks until the next scheduled wakeup.  A value of
             * zero indicates a wakeup will occur as the current Clock tick
             * period expires; a very large value indicates a very large number
             * of Clock tick periods will occur before the next scheduled
             * wakeup.
             */
            ticks = ClockP_getTicksUntilInterrupt();

            /* convert ticks to usec */
            time = ticks * ClockP_tickPeriod;

            /* check if can go to STANDBY */
            if (time > Power_getTransitionLatency(PowerCC26XX_STANDBY,
                Power_TOTAL)) {

                /* schedule the wakeup event */
                ticks -= PowerCC26X2_WAKEDELAYSTANDBY / ClockP_tickPeriod;
                ClockP_setTimeout(ClockP_handle((ClockP_Struct *)
                    &PowerCC26X2_module.clockObj), ticks);
                ClockP_start(ClockP_handle((ClockP_Struct *)
                    &PowerCC26X2_module.clockObj));

                /* go to standby mode */
                Power_sleep(PowerCC26XX_STANDBY);
                ClockP_stop(ClockP_handle((ClockP_Struct *)
                    &PowerCC26X2_module.clockObj));
                justIdle = false;
            }
        }

        /* idle if allowed */
        if (justIdle) {

            /*
             * Power off the CPU domain; VIMS will power down if SYSBUS is
             * powered down, and SYSBUS will power down if there are no
             * dependencies
             * NOTE: if radio driver is active it must force SYSBUS enable to
             * allow access to the bus and SRAM
             */
            if ((constraints & (1 << PowerCC26XX_DISALLOW_IDLE)) == 0) {
                uint32_t modeVIMS;
                /* 1. Get the current VIMS mode */
                do {
                    modeVIMS = VIMSModeGet(VIMS_BASE);
                } while (modeVIMS == VIMS_MODE_CHANGING);

                /* 2. Configure flash to remain on in IDLE or not and keep
                 *    VIMS powered on if it is configured as GPRAM
                 * 3. Always keep cache retention ON in IDLE
                 * 4. Turn off the CPU power domain
                 * 5. Ensure any possible outstanding AON writes complete
                 * 6. Enter IDLE
                 */
                if ((constraints & (1 << PowerCC26XX_NEED_FLASH_IN_IDLE)) ||
                    (modeVIMS == VIMS_MODE_DISABLED)) {
                    SysCtrlIdle(VIMS_ON_BUS_ON_MODE);
                }
                else {
                    SysCtrlIdle(VIMS_ON_CPU_ON_MODE);
                }

                /* 7. Make sure MCU and AON are in sync after wakeup */
                SysCtrlAonUpdate();
            }
            else {
                PRCMSleep();
            }
        }
    }

    /* re-enable interrupts */
    HwiP_restore(key);
}

/*
 *  ======== PowerCC26XX_schedulerDisable ========
 */
void PowerCC26XX_schedulerDisable()
{
    PowerCC26X2_swiKey = SwiP_disable();
}

/*
 *  ======== PowerCC26XX_schedulerRestore ========
 */
void PowerCC26XX_schedulerRestore()
{
    SwiP_restore(PowerCC26X2_swiKey);
}
