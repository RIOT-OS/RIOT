/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file   NoRTOS.h
 *
 *  @brief  NoRTOS framework module
 *
 *  The NoRTOS header file should be included in an application as follows:
 *  @code
 *  #include <NoRTOS.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  The NoRTOS framework module controls various aspects of the application's
 *  behavior with respect to the TI Driver interfaces that are used by the
 *  application.
 *
 *  Some TI Drivers make use of an internal Clock module that offers timing
 *  services to the drivers, based on a Clock "tick" which operates at a
 *  frequency based on a configured period.  This period can be configured
 *  per application requirements through the NoRTOS module, although the
 *  default setting might be required by certain TI Drivers that assume a
 *  certain Clock tick frequency.
 *
 *  Some TI Drivers make use of an internal Swi (Software interrupt) module
 *  that offers a scheduling paradigm that lies between an application's
 *  'main' thread and hardware interrupts.  The internal Swi implementation
 *  utilizes a software-triggered-only hardware interrupt for achieving this
 *  mid-level scheduling paradigm.  This hardware interrupt can be configured
 *  per application requirements through the NoRTOS module, although the
 *  default setting is likely what should be used.
 *
 *  Some TI Drivers will "suspend" the application's operation until a certain
 *  event occurs.  When an application is running in the NoRTOS framework
 *  there are no other "threads" to run when the 'main' thread becomes
 *  suspended, which in essence implies that the application has entered an
 *  'idle' mode.  An 'idle callback' function is called when this 'idle' mode
 *  has been entered.  This callback function can be configured per the
 *  application requirements through the NoRTOS module, although the default
 *  setting is likely what should be used.
 *
 *  # Usage #
 *
 *  The NoRTOS module contains the following APIs:
 *    - NoRTOS_getConfig(): Retrieve the current NoRTOS configuration values.
 *    - NoRTOS_setConfig(): Set NoRTOS configuration values.
 *    - NoRTOS_start(): Enable NoRTOS system operation (required).
 *
 *  ### NoRTOS Framework Configuration #
 *
 *  The NoRTOS framework utilizes a few settings that the application should
 *  be able to control so as to accommodate the unique needs of the
 *  application or the system upon which the application runs.  It is expected
 *  that the default values of these settings will suffice for the majority of
 *  applications using the NoRTOS framework.  These default values should be
 *  of no concern to most applications since the aspects they control are
 *  internal to the TI Drivers' operation, but system requirements or other
 *  code that is being integrated with the application could require different
 *  values.  The NoRTOS configuration functions offer this capability to the
 *  application.  Please refer to the documentation for the NoRTOS_Config
 *  structure for details on each configuration element.
 *
 *  ### Starting the NoRTOS framework operation #
 *
 *  Realtime systems often require precise timing when enabling certain
 *  aspects of the system.  If some part of the system is enabled too early
 *  then other parts of the system may not operate correctly.  Certain system
 *  elements need to be setup and initialized before other elements will
 *  operate correctly.
 *
 *  The NoRTOS_start() API allows the application to control when the system
 *  as a whole should be "started".  NoRTOS_start() *must* be called for
 *  the system to start, and should be called after all TI Driver and
 *  peripheral initialization has been performed.
 *
 *  ### Example usage #
 *
 *  @code
 *  #include <NoRTOS.h>

 *  int main(int argc, char *argv[])
 *  {
 *      NoRTOS_Config cfg;
 *
 *      // Get current values of all configuration settings
 *      NoRTOS_getConfig(&cfg);
 *
 *      // Change config settings we want to change while leaving other
 *      // settings at their default values ...
 *
 *      // Change system "tick" frequency to 10,000 Hz
 *      cfg.clockTickPeriod = 100;
 *
 *      // Change interrupt used for Swi scheduling to 11 (SVCall)
 *      cfg.swiIntNum = 11;
 *
 *      // Affect the changes
 *      NoRTOS_setConfig(&cfg);
 *
 *      `perform board and driver initialization`;
 *
 *      // Start NoRTOS
 *      NoRTOS_start();
 *
 *      // Call mainThread function
 *      mainThread(NULL);
 *  }
 *  @endcode
 *
 *******************************************************************************
 */

/*!
 *  @brief  NoRTOS framework global configuration
 *
 *  The NoRTOS_Config structure contains a set of values utilized by the
 *  NoRTOS framework.
 *
 *  The NoRTOS_getConfig() API can be used to retrieve the current settings.
 *  When changing one or more of these settings, a NoRTOS_Config structure
 *  should first be populated with the current settings by calling
 *  NoRTOS_getConfig(), after which the particular setting(s) that needs to be
 *  changed should be set and given to the NoRTOS framework by calling
 *  NoRTOS_setConfig().
 */
typedef struct _NoRTOS_Config {
    /*! Function that is called when a TI Driver "suspends" its operation */
    void (*idleCallback)(void);

    /*! Period of the internal Clock module's periodic "tick" (microsecs) */
    uint32_t clockTickPeriod;

    /*! Hardware interrupt posted by software to achieve the Swi (Software
        interrupt) scheduling paradigm */
    int swiIntNum;
} NoRTOS_Config;

/*!
 *  @brief  Function to retrieve current NoRTOS configuration values
 *
 *  @param  cfg    Pointer to a NoRTOS_Config structure in which to store
 *                 the current configuration values
 */
void NoRTOS_getConfig(NoRTOS_Config *cfg);

/*!
 *  @brief  Function to set or modify NoRTOS configuration values
 *
 *  @param  cfg    Pointer to a NoRTOS_Config structure from which NoRTOS
 *                 configuration values are set
 */
void NoRTOS_setConfig(NoRTOS_Config *cfg);

/*
 *  @brief  Function to call for enabling NoRTOS system operation
 */
void NoRTOS_start(void);
