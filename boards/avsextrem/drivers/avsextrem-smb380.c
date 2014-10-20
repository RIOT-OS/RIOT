/*
 * msba2acc-smb380.c - implementation of the Driver for the SMB380 acceleration
 * sensor on the AVSEXTREM board.
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @file
 * @internal
 * @brief       SMB380 acceleration sensor driver for the LPC2387 on the
 *              AVSEXTREM board.
 *
 * @author      Marco Ziegert <ziegert@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note        $Id:  msba2acc-smb380.c 3854 2010-01-18 15:27:01Z zkasmi $
 */


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "lpc23xx.h"   /* LPC23XX/24xx Peripheral Registers */
#include "cpu.h"
#include "lpm.h"
#include "VIC.h"
#include "kernel.h"
#include "ssp0-board.h"
#include "smb380-board.h"
#include "hwtimer.h"
#include "sched.h"
#include "msg.h"
#include "irq.h"

#include "gpioint.h"
#include "math.h"
#include "lpc2387.h"


kernel_pid_t simple_pid = KERNEL_PID_UNDEF;
int16_t simple_buffer[4];

volatile int16_t *ringBuff_X = NULL;
volatile int16_t *ringBuff_Y = NULL;
volatile int16_t *ringBuff_Z = NULL;
volatile int16_t *ringBuff_T = NULL;
uint16_t readPointerPos[SMB380_RING_BUFF_MAX_THREADS];
kernel_pid_t PointerList[SMB380_RING_BUFF_MAX_THREADS];
static msg_t wakeupmessage;

/*
 * pointer to a user-defined function which is called during a writepointer
 * action
 */
uint8_t (*smb380function)(int16_t *);
uint16_t sampleRateSMB380;  // condition if range-check should be done
bool dynRange = false;
uint8_t counter_Decreasing = 0;

volatile uint16_t interruptTicksSMB380;

typedef struct {
    u_int writePointerPos;  //Writepointer position
    /*
     * check value for updated range settings (only needed for multiplication
     * in Float-mode
     */
    uint8_t countRange;
    uint8_t range;      //current range
} settingsSMB380;

settingsSMB380 settings;

// measuring temperature dependent internal sample rate of SMB380
static volatile uint32_t tickStart = 0;
static volatile uint32_t tickLastSample = 0;
static volatile uint32_t tickCurrentSamples = 0;

uint8_t initRingReadPointerforCurrentThread(void);
uint8_t getRingReadPointerforCurrentThread(void);
void wakeUpRegisteredProcesses(void);
uint8_t smb380emptyfunction(int16_t *);
static void SMB380_extIntHandler(void);

extern unsigned long ktimer_now(void);

float SMB380_getSampleRatio(void)
{
    return ((1.0 / ((float)(tickLastSample - tickStart) / tickCurrentSamples)) *
            100000);
}

uint8_t smb380emptyfunction(int16_t *value)
{
    return 1;
}

uint8_t SMB380_HystereseFunctionSample(int16_t *value)
{
    static int16_t x = 0, y = 0, z = 0;
    static uint8_t counter = 0;
    int16_t delta = abs(value[0] - x) + abs(value[1] - y) + abs(value[2] - z);

    if (delta < 40) { //TODO: delta and counter are constant values, change it!
        counter++;
    }
    else {
        counter = 0;
    }

    x = value[0];
    y = value[1];
    z = value[2];

    if (smb380_mode == SMB380_THRESHOLD) {
        SMB380_disableAnyMotionLimit();
        SMB380_enableNewDataInt();
        smb380_mode = SMB380_CONTINOUS;
        printf("Threshold: x=%i, y=%i, z=%i\n\r", value[0], value[1], value[2]);
    }

    if ((counter == 100) && (smb380_mode == SMB380_POLL)) {
        //MZ SMB380_disableNewDataInt();
        //SMB380_setAnyMotionLimit(100,0);
        SMB380_enableAnyMotionLimit();
        smb380_mode = SMB380_THRESHOLD;
        counter = 0;
    }
    else if (counter == 100) {
        SMB380_disableNewDataInt();
        //SMB380_setAnyMotionLimit(100,0);
        SMB380_enableAnyMotionLimit();
        smb380_mode = SMB380_FALSEALERT;
        counter = 0;
        return 0;
    }


    return 1;
}

static void SMB380_simple_interrupthandler(void)
{
    lpm_awake();

    SMB380_getAcceleration(SMB380_X_AXIS, NULL, &simple_buffer[0]);
    SMB380_getAcceleration(SMB380_Y_AXIS, NULL, &simple_buffer[1]);
    SMB380_getAcceleration(SMB380_Z_AXIS, NULL, &simple_buffer[2]);
    simple_buffer[3] = SMB380_getTemperature();

    if (interruptTicksSMB380 >= sampleRateSMB380 - 1) {
        interruptTicksSMB380 = 0;
        wakeupmessage.type = MSG_TYPE_SMB380_WAKEUP;
        msg_try_send(&wakeupmessage, simple_pid);
    }
    else {
        interruptTicksSMB380++;
    }

    return;
}

// enables simple Interrupt driven Mode
uint8_t SMB380_init_simple(uint16_t samplerate, uint8_t bandwidth, uint8_t
                           range)
{
    SSP0Init();
    interruptTicksSMB380 = 0;
    simple_pid = sched_active_pid;
    gpioint_set(0, BIT1, GPIOINT_RISING_EDGE, &SMB380_simple_interrupthandler);
    SMB380_softReset();
    hwtimer_wait(HWTIMER_TICKS(100000));
    SMB380_disableUpperLimit();
    SMB380_disableLowerLimit();
    SMB380_setSampleRate(samplerate);
    SMB380_setBandWidth(bandwidth);
    SMB380_setRange(range);

    return 0;
}

uint8_t SMB380_init(uint8_t (*func)(int16_t *))
{
    SSP0Init();

#if SMB380_EXTINT_MODE

    if (gpioint_set(0, BIT1, GPIOINT_RISING_EDGE, &SMB380_extIntHandler));

#endif

    interruptTicksSMB380 = 0;

    if (func != NULL) {
        smb380function = func;
    }
    else {
        smb380function = smb380emptyfunction;
    }

    //smb380function = SMB380_HystereseFunctionSample;  //placeholder

    SMB380_softReset();
    hwtimer_wait(HWTIMER_TICKS(100000));
    SMB380_disableUpperLimit();
    SMB380_disableLowerLimit();

    smb380_mode = SMB380_POLL;
    SMB380_setSampleRate(SMB380_SAMPLE_RATE_MAX);  //set output to 3000 Hz

    settings.writePointerPos = 0;
    settings.range = 0;
    settings.countRange = 0;

    for (int i = 0; i < SMB380_RING_BUFF_MAX_THREADS; i++) {
        readPointerPos[i] = 0;
        PointerList[i] = 0;
    }

    ringBuff_X = (int16_t *)malloc(SMB380_RING_BUFF_SIZE * sizeof(int16_t));
    ringBuff_Y = (int16_t *)malloc(SMB380_RING_BUFF_SIZE * sizeof(int16_t));
    ringBuff_Z = (int16_t *)malloc(SMB380_RING_BUFF_SIZE * sizeof(int16_t));
    ringBuff_T = (int16_t *)malloc(SMB380_RING_BUFF_SIZE * sizeof(int16_t));

    if ((ringBuff_X == NULL) | (ringBuff_Y == NULL) | (ringBuff_Z == NULL) |
        (ringBuff_T == NULL)) {
        if (ringBuff_X != NULL) {
            free((int16_t *)ringBuff_X);
        }

        if (ringBuff_Y != NULL) {
            free((int16_t *)ringBuff_Y);
        }

        if (ringBuff_Z != NULL) {
            free((int16_t *)ringBuff_Z);
        }

        if (ringBuff_T != NULL) {
            free((int16_t *)ringBuff_T);
        }

        return 0;
    }

    return 1;
}

static void SMB380_extIntHandler(void)
{
    int16_t accInt[4];

    lpm_awake(); //initializes clock

    SMB380_getAcceleration(SMB380_X_AXIS, NULL, &accInt[0]);
    SMB380_getAcceleration(SMB380_Y_AXIS, NULL, &accInt[1]);
    SMB380_getAcceleration(SMB380_Z_AXIS, NULL, &accInt[2]);
    accInt[3] = SMB380_getTemperature();


    writeRingBuff(accInt);

    //  printf("SMB380 acc x,y,z: [%i|%i|%i|%2.3f]\r\n", accInt[0], accInt[1],
    //         accInt[2], acc[3]);
    //  printf("SMB380 acc x,y,z: [%2.3f|%2.3f|%2.3f|%2.3f]\r\n\n\n", acc[0],
    //         acc[1], acc[2], acc[3]);
    //  printf("Nach Interrupt Reset:\n");
    //  SMB380_ShowMemory();
}

void SMB380_setSampleRate(uint16_t rate)
{
    if (rate > 0 && rate <= SMB380_SAMPLE_RATE_MAX) {
        sampleRateSMB380 = SMB380_SAMPLE_RATE_MAX / rate;
    }
    else {
        sampleRateSMB380 = 1;
    }
}

uint16_t SMB380_getSampleRate(void)
{
    return SMB380_SAMPLE_RATE_MAX / sampleRateSMB380;
}

uint8_t SMB380_Prepare(void)
{
    return SSP0Prepare(SMB380_ACC, 16, 1, 1, 8000);
}

uint8_t SMB380_Unprepare(void)
{
    return SSP0Unprepare(SMB380_ACC);
}

// return the pointerNo related with the current thread
uint8_t getRingReadPointerforCurrentThread(void)
{
    uint8_t pointerNo = 0;

    while ((pointerNo < SMB380_RING_BUFF_MAX_THREADS) &&
           (PointerList[pointerNo] != sched_active_pid)) {
        pointerNo++;
    }

    return pointerNo;
}

uint8_t initRingReadPointerforCurrentThread(void)
{
    //TODO: make it Threadsafe
    uint8_t pointerNo = 0;

    while ((pointerNo < SMB380_RING_BUFF_MAX_THREADS) &&
           (PointerList[pointerNo] > 0)) {
        pointerNo++;
    }

    if (pointerNo == SMB380_RING_BUFF_MAX_THREADS) {
        return 0;
    }
    else {
        PointerList[pointerNo] = sched_active_pid;
        readPointerPos[pointerNo] = settings.writePointerPos;
        return 1;
    }
}

void freeRingReadPointer(void)
{
    //Should be Threadsafe
    uint8_t pointerNo = getRingReadPointerforCurrentThread();

    if (pointerNo != SMB380_RING_BUFF_MAX_THREADS) {
        PointerList[pointerNo] = 0;
    }
}

void actualizeRingReadPointer(void)
{
    uint8_t pointerNo = getRingReadPointerforCurrentThread();

    if (pointerNo != SMB380_RING_BUFF_MAX_THREADS) {
        readPointerPos[pointerNo] = settings.writePointerPos;
    }
}

//TODO: more read-pointer
uint8_t readRingBuff(int16_t *value)
{

    uint8_t pointerNo = getRingReadPointerforCurrentThread();

    /*
     * If thread is not known to read on the ringbuffer, try adding him to the
     * list of known threads, otherwise exit with error
     */

    if (pointerNo == SMB380_RING_BUFF_MAX_THREADS) {
        if (!initRingReadPointerforCurrentThread()) {
            //printf("%sNo Readpointer left, maximum of %u is reached!\n\r",
            //       SMB380_DEBUG_MESSAGE, SMB380_RING_BUFF_MAX_THREADS);
            return 0;
        }
        else {
            pointerNo = getRingReadPointerforCurrentThread();
        }
    }

    if (readPointerPos[pointerNo] == settings.writePointerPos) {
        value[0] = 0;
        value[1] = 0;
        value[2] = 0;
        value[3] = 0;
        return 0;
    }

    value[0] = ringBuff_X[readPointerPos[pointerNo]];
    value[1] = ringBuff_Y[readPointerPos[pointerNo]];
    value[2] = ringBuff_Z[readPointerPos[pointerNo]];
    value[3] = ringBuff_T[readPointerPos[pointerNo]];
    readPointerPos[pointerNo] += 1;

    if (readPointerPos[pointerNo] == SMB380_RING_BUFF_SIZE) {
        readPointerPos[pointerNo] = 0;
    }

    return 1;
}

//TODO: more read-pointer
uint8_t writeRingBuff(int16_t *value)
{
    if (smb380_mode == SMB380_FALSEALERT)   {
        smb380_mode = SMB380_THRESHOLD;
        return 0;
    }

    if ((interruptTicksSMB380 >= sampleRateSMB380 - 1) ||
        (smb380_mode == SMB380_THRESHOLD)) {
        interruptTicksSMB380 = 0;

        /* measuring temperature dependent internal sample rate of SMB380 */
        if (smb380_mode == SMB380_CONTINOUS) {
            tickLastSample = hwtimer_now();
            tickCurrentSamples++;
        }

        ringBuff_X[settings.writePointerPos] = value[0];
        ringBuff_Y[settings.writePointerPos] = value[1];
        ringBuff_Z[settings.writePointerPos] = value[2];
        ringBuff_T[settings.writePointerPos] = value[3];

        /* check for increasing range if dynRange is true */
        if (dynRange) {
            smb380function = checkRange;
        }
        else {
            smb380function = smb380emptyfunction;
        }

        smb380function(value);

        //printf("Werte: x=%i, y=%i, z=%i\n\r",value[0], value[1], value[2]);
        //vgpio_toggle(&gpio_led_green);

        settings.writePointerPos += 1;

        if (settings.writePointerPos == SMB380_RING_BUFF_SIZE) {
            settings.writePointerPos = 0;
        }

        wakeUpRegisteredProcesses();
        return 1;
    }
    else {
        interruptTicksSMB380++;
        return 0;
    }
}

void SMB380_activateDynRangeSet(uint8_t activate)
{
    switch (activate) {
        case 0:
            dynRange = false ;
            break;

        case 1:
            dynRange = true ;
            break;
    }
}

/*
 * checking for in-/decreasing range
 */
uint8_t checkRange(int16_t *value)
{
    bool increased = false;

    // printf("CurrentValues:  %i %i %i \r\n", xyzTMP[0], xyzTMP[1], xyzTMP[2]);

    // determination of max and min value
    int16_t maxValue = value[0]; // max(xValue, max(yValue, zValue));
    int16_t minValue = value[0]; // min(xValue, min(yValue, zValue));

    for (int i = 1; i < 3; i++) {
        if (value[i] > maxValue) {
            maxValue = value[i];
        }

        if (value[i] < minValue) {
            minValue = value[i];
        }
    }

    int8_t isRange = SMB380_getRange();

    //increase range immediately
    switch (isRange) {
        case 2:
            if ((maxValue > 1800) || (minValue < -1800)) {
                SMB380_setRange(SMB380_RANGE_4G);
                increased = true;
                printf("=== Set range from 2 g to 4 g ===\r\n");
            }

            break;

        case 4:
            if ((maxValue > 3800) || (minValue < -3800)) {
                SMB380_setRange(SMB380_RANGE_8G);
                increased = true;
                printf("=== Set range from 4 g to 8 g ===\r\n");
            }

            break;

        case 8:
            break;
    }

    /* check for decreasing range */
    if (increased) {
        counter_Decreasing = 0;
    }
    else {
        //decrease range after 10 consecutive relevant values
        switch (isRange) {
            case 2:
                break;

            case 4:
                if ((maxValue < 2000) && (minValue > -2000)) {
                    if (counter_Decreasing >= 9) {
                        SMB380_setRange(SMB380_RANGE_2G);
                        printf("=== Set range from 4 g to 2 g ===\r\n");
                        counter_Decreasing = 0;
                    }
                    else {
                        counter_Decreasing++;
                    }
                }
                else {
                    counter_Decreasing = 0;
                }

                break;

            case 8:
                if ((maxValue < 4000) && (minValue > -4000)) {
                    if (counter_Decreasing >= 9) {
                        SMB380_setRange(SMB380_RANGE_4G);
                        printf("=== Set range from 8 g to 4 g ===\r\n");
                        counter_Decreasing = 0;
                    }
                    else {
                        counter_Decreasing++;
                    }
                }
                else {
                    counter_Decreasing = 0;
                }

                break;
        }
    }

    return 0;
}


void wakeUpRegisteredProcesses(void)
{
    uint8_t pointerNo = 0;
    //wake up waiting processes
    wakeupmessage.type = MSG_TYPE_SMB380_WAKEUP;

    while ((pointerNo < SMB380_RING_BUFF_MAX_THREADS) &&
           (PointerList[pointerNo] > 0)) {
        msg_try_send(&wakeupmessage, PointerList[pointerNo]);
        pointerNo++;
    }
}

int16_t SMB380_getTemperature(void)
{
    int16_t t = 0;

    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_TEMP, 0, SMB380_READ_REGISTER);

    //  t = (SMB380_ssp_read() & 0xFF) / 2.0 + SMB380_TEMP_OFFSET;
    t = (SMB380_ssp_read() & 0xFF);
    t = (t >> 1) + SMB380_TEMP_OFFSET;
    SMB380_Unprepare();
    restoreIRQ(cpsr);

    return t;
}

void SMB380_getAcceleration(unsigned char axis, int16_t *pAbs, int16_t *pMg)
{
    unsigned short ur;

    if (!settings.countRange && (pMg != NULL)) {
        settings.countRange = 1;
        settings.range = SMB380_getRange();
    }

    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();

    switch (axis) {
        case SMB380_X_AXIS:
            SMB380_ssp_write(SMB380_ACC_X_MSB, 0, SMB380_READ_REGISTER);
            SMB380_ssp_write(SMB380_ACC_X_LSB_NEWDATA, 0, SMB380_READ_REGISTER);
            break;

        case SMB380_Y_AXIS:
            SMB380_ssp_write(SMB380_ACC_Y_MSB, 0, SMB380_READ_REGISTER);
            SMB380_ssp_write(SMB380_ACC_Y_LSB_NEWDATA, 0, SMB380_READ_REGISTER);
            break;

        default:
            SMB380_ssp_write(SMB380_ACC_Z_MSB, 0, SMB380_READ_REGISTER);
            SMB380_ssp_write(SMB380_ACC_Z_LSB_NEWDATA, 0, SMB380_READ_REGISTER);
    }

    ur = (SMB380_ssp_read() & SMB380_ACC_MSB_MASK) << 2;
    ur |= (SMB380_ssp_read() & SMB380_ACC_LSB_MASK) >> 6;
    SMB380_Unprepare();
    restoreIRQ(cpsr);

    if (pAbs != NULL) {
        if (ur & BIT9) {  //ur<0
            *pAbs = ur | 0xFC00;
        }
        else {
            *pAbs = ur & 0x03FF;
        }
    }

    if (pMg != NULL) {
        if (ur & BIT9) {  //ur<0
            *pMg = -(((settings.range * (512 - (ur & 0x1FF))) * 2000) / 1024);
        }
        else {
            *pMg = ((settings.range * ur) * 2000) / 1024;
        }
    }

}

unsigned char SMB380_getChipID(void)
{
    unsigned char ur = 0;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CHIP_ID, 0, 0);
    ur = (unsigned char)(SMB380_ssp_read() & SMB380_CHIP_ID_MASK);
    restoreIRQ(cpsr);
    return ur;
}

void SMB380_setWakeUpPause(unsigned char duration)
{
    unsigned char utemp;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL4, 0, SMB380_READ_REGISTER);
    utemp = SMB380_ssp_read();
    utemp &= ~SMB380_CONTROL4_WAKEUP_PAUSE_MASK;
    utemp |= (duration & (0x3) << 1);//+1;
    SMB380_ssp_write(SMB380_CONTROL4, utemp, SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

unsigned char SMB380_getWakeUpPause(void)
{
    unsigned char up;

    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL4, 0, SMB380_READ_REGISTER);
    up = (unsigned char)SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
    up &= SMB380_CONTROL4_WAKEUP_PAUSE_MASK;
    up = up >> 1;

    return up;
}

void SMB380_setBandWidth(unsigned char bandWidth)
{
    if ((bandWidth == SMB380_BAND_WIDTH_100HZ) ||
            (bandWidth == SMB380_BAND_WIDTH_1500HZ) ||
            (bandWidth == SMB380_BAND_WIDTH_190HZ) ||
            (bandWidth == SMB380_BAND_WIDTH_25HZ) ||
            (bandWidth == SMB380_BAND_WIDTH_375HZ) ||
            (bandWidth == SMB380_BAND_WIDTH_50HZ) ||
            (bandWidth == SMB380_BAND_WIDTH_750HZ)) {
        unsigned long cpsr = disableIRQ();
        SMB380_Prepare();
        SMB380_ssp_write(SMB380_CONTROL3, 0, SMB380_READ_REGISTER);
        unsigned char utemp = SMB380_ssp_read();
        utemp &= ~SMB380_CONTROL3_BANDWITH_MASK;
        utemp |= (bandWidth & 0x7);
        SMB380_ssp_write(SMB380_CONTROL3, utemp, SMB380_WRITE_REGISTER);
        SMB380_ssp_read();
        SMB380_Unprepare();
        restoreIRQ(cpsr);
    }
}

void SMB380_setRange(unsigned char range)
{
    if (range != 0x3) {
        unsigned long cpsr = disableIRQ();
        SMB380_Prepare();
        SMB380_ssp_write(SMB380_CONTROL3, 0, SMB380_READ_REGISTER);
        unsigned char utemp = (unsigned char)SMB380_ssp_read();
        utemp &= ~SMB380_CONTROL3_RANGE_MASK;
        utemp |= (range & 0x3) << 3;
        SMB380_ssp_write(SMB380_CONTROL3, utemp, SMB380_WRITE_REGISTER);
        SMB380_ssp_read();
        SMB380_Unprepare();
        restoreIRQ(cpsr);
        settings.countRange = 0;
    }

}

unsigned char SMB380_getRange(void)
{
    unsigned char ur;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL3, 0, SMB380_READ_REGISTER);
    ur = (SMB380_ssp_read() & SMB380_CONTROL3_RANGE_MASK) >> 3;
    SMB380_Unprepare();
    restoreIRQ(cpsr);

    switch (ur) {
        case SMB380_RANGE_2G:
            return 2;

        case SMB380_RANGE_4G:
            return 4;

        case SMB380_RANGE_8G:
            return 8;

        default:
            return 4;
    }
}

unsigned char SMB380_getBandWidth(void)
{
    unsigned char uBand;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL3, 0, SMB380_READ_REGISTER);
    uBand = SMB380_ssp_read() & SMB380_CONTROL3_BANDWITH_MASK;
    SMB380_Unprepare();
    restoreIRQ(cpsr);
    return uBand;
}

int16_t SMB380_getBandWidthAbs(void)
{
    unsigned char uBand;
    uBand = SMB380_getBandWidth();

    switch (uBand) {
        case SMB380_BAND_WIDTH_25HZ:
            return 25;

        case SMB380_BAND_WIDTH_50HZ:
            return 50;

        case SMB380_BAND_WIDTH_100HZ:
            return 100;

        case SMB380_BAND_WIDTH_190HZ:
            return 190;

        case SMB380_BAND_WIDTH_375HZ:
            return 375;

        case SMB380_BAND_WIDTH_750HZ:
            return 750;

        case SMB380_BAND_WIDTH_1500HZ:
            return 1500;

        default:
            return uBand;
    }
}

void SMB380_softReset(void)
{
    unsigned char ur;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL1, SMB380_CONTROL1_SOFT_RESET_MASK,
                     SMB380_WRITE_REGISTER);
    ur = ((unsigned char)SMB380_ssp_read()) & SMB380_CONTROL1_SOFT_RESET_MASK;
    SMB380_Unprepare();
    restoreIRQ(cpsr);
    ur = ur >> 1;
}

void SMB380_setCustomerReg(unsigned char data)
{
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CUST1, data, SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

unsigned char SMB380_getCustomerReg(void)
{
    unsigned uReg = 0;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CUST1, 0, SMB380_READ_REGISTER);
    uReg = (unsigned char)SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
    return uReg;
}

// Selftest
void SMB380_Selftest_1(void)
{
    unsigned char uReg = 0;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_LG_THRES, 6, SMB380_WRITE_REGISTER);
    //SSP0Init();
    SMB380_ssp_read();
    SMB380_ssp_write(SMB380_LG_DUR, 0, SMB380_WRITE_REGISTER);
    //SSP0Init();
    SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL1, 0, SMB380_READ_REGISTER);

    uReg = (unsigned char)SMB380_ssp_read();
    uReg &= ~SMB380_CONTROL1_SELF_TEST_1_MASK;
    uReg |= 0x01 << 3;
    SMB380_ssp_write(SMB380_CONTROL1, uReg, SMB380_WRITE_REGISTER);
    //  SSP0Init();
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_ShowMemory(void)
{
    uint8_t bitMask[16];
    printf("SMB380 Speicher\n\r");

    for (unsigned char regAd = 0x16; regAd > 0; regAd--) {
        unsigned long cpsr = disableIRQ();
        SMB380_Prepare();
        SMB380_ssp_write(regAd - 1, 0, SMB380_READ_REGISTER);
        uint16_t uReg = SMB380_ssp_read();
        SMB380_Unprepare();
        restoreIRQ(cpsr);
        printf("Register: = %X: 0x%X = ", regAd - 1, uReg);

        for (int pos = 0; pos < 16; pos++) { //uReg != 0)
            bitMask[15 - pos] = uReg % 2;

            uReg = uReg / 2;
        }

        for (int pos = 8; pos < 16; pos++) { //uReg != 0)
            printf("%d", bitMask[pos]);

            if ((pos % 4) == 0) {
                printf(" ");
            }
        }

        printf("\n");
    }
}

void SMB380_setUpperLimit(void)
{
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_HG_THRES, 128, SMB380_WRITE_REGISTER); //1g
    SMB380_ssp_read();
    SMB380_ssp_write(SMB380_HG_DUR, 0, SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_enableUpperLimit(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL2, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2, uReg | SMB380_CONTROL2_ENABLE_HG_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_disableUpperLimit(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL2, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2, uReg & ~SMB380_CONTROL2_ENABLE_HG_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_enableLowerLimit(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL2, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2, uReg | SMB380_CONTROL2_ENABLE_LG_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_disableLowerLimit(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL2, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2, uReg & ~SMB380_CONTROL2_ENABLE_LG_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

/* @param gvaluefloat - value is in mg
 * @param gvalueint - value in range of 0 to 512
 * Choose one of them, set the other to zero
 */
uint8_t SMB380_setAnyMotionLimit(uint16_t mg, uint16_t gvalueint)
{

    uint8_t threshold = 0;

    if (mg != 0) {
        threshold = mg / (15.6 * (SMB380_getRange() / 2));
    }
    else if (gvalueint != 0) {
        /* Scaling for different gRanges is not needed */
        threshold = ceil(((gvalueint * 2000) / 512.0) / 15.6);
    }
    else {
        return 0;
    }

    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    /* 0,3g = 300 / 15,6mg = 19 */
    SMB380_ssp_write(SMB380_ANY_MOTION_THRES, threshold, SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    //Set duration at this point
    SMB380_ssp_write(SMB380_ANY_MOTION_DUR_HYST, 0, SMB380_READ_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
    return 1;
}

void SMB380_enableAnyMotionLimit(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL4, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL4, uReg | SMB380_CONTROL4_ENABLE_ADV_INT_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2, uReg | SMB380_CONTROL2_ANY_MOTION_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_disableAnyMotionLimit(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL2, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL2,
                     uReg & ~SMB380_CONTROL2_ANY_MOTION_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL4, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL4,
                     uReg & ~SMB380_CONTROL4_ENABLE_ADV_INT_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_enableNewDataInt(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    /*
     * prevent deep sleep, reason: 400 µs wake-up time is to long for 3kHz
     * interrupts
     */
    SETBIT(lpm_prevent_sleep, LPM_PREVENT_SLEEP_ACCSENSOR);
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL4, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL4, uReg | SMB380_CONTROL4_NEW_DATA_INT_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    // measuring temperature dependent internal sample rate of SMB380
    tickStart = hwtimer_now();
    tickCurrentSamples = 0;
    restoreIRQ(cpsr);
}

void SMB380_disableNewDataInt(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL4, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL4, uReg & ~SMB380_CONTROL4_NEW_DATA_INT_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    /*
     * enable deep sleep, reason: 400 µs wake-up time was to long for 3kHz
     * interrupts
     */
    CLRBIT(lpm_prevent_sleep, LPM_PREVENT_SLEEP_ACCSENSOR);
    restoreIRQ(cpsr);
}

void SMB380_resetInterruptFlags(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL1, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL1, uReg | SMB380_CONTROL1_RESET_INT_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_enableEEPROM(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL1, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL1, uReg | SMB380_CONTROL1_EE_W_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

void SMB380_disableEEPROM(void)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();
    SMB380_ssp_write(SMB380_CONTROL1, 0, SMB380_READ_REGISTER);
    uReg = SMB380_ssp_read();
    SMB380_ssp_write(SMB380_CONTROL1, uReg & ~SMB380_CONTROL1_EE_W_MASK,
                     SMB380_WRITE_REGISTER);
    SMB380_ssp_read();
    SMB380_Unprepare();
    restoreIRQ(cpsr);
}

/*
 * Return offsets from offset registers,
 * remove xyz afterwards because it is useless
 */
unsigned char SMB380_readOffset(uint16_t *offset)
{
    if (sizeof(offset) < 3) {
        return false;
    }

    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();

    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_X, 0, SMB380_READ_REGISTER);
    uReg = ((SMB380_ssp_read() & SMB380_ACC_LSB_MASK) >> 6);
    SMB380_ssp_write(SMB380_OFFSET_MSB_X, 0, SMB380_READ_REGISTER);
    uReg |= (SMB380_ssp_read() & SMB380_ACC_MSB_MASK) << 2;


    offset[0] = uReg;
    printf("Offset X: %u ", uReg);

    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Y, 0, SMB380_READ_REGISTER);
    uReg = (SMB380_ssp_read() & SMB380_ACC_LSB_MASK) >> 6;
    SMB380_ssp_write(SMB380_OFFSET_MSB_Y, 0, SMB380_READ_REGISTER);
    uReg |= (SMB380_ssp_read() & SMB380_ACC_MSB_MASK) << 2;

    offset[1] = uReg;
    printf("Offset Y: %u ", uReg);

    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Z, 0, SMB380_READ_REGISTER);
    uReg = (SMB380_ssp_read() & SMB380_ACC_LSB_MASK) >> 6;
    SMB380_ssp_write(SMB380_OFFSET_MSB_Z, 0, SMB380_READ_REGISTER);
    uReg |= (SMB380_ssp_read() & SMB380_ACC_MSB_MASK) << 2;

    offset[2] = uReg;
    printf("Offset Z: %u \r\n", uReg);

    SMB380_Unprepare();
    restoreIRQ(cpsr);

    return true;
}

unsigned char SMB380_readOffsetTemp(uint16_t *offset)
{
    unsigned short uReg;
    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();

    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_T, 0, SMB380_READ_REGISTER);
    uReg = ((SMB380_ssp_read() & SMB380_ACC_LSB_MASK) >> 6);
    SMB380_ssp_write(SMB380_OFFSET_MSB_T, 0, SMB380_READ_REGISTER);
    uReg |= (SMB380_ssp_read() & SMB380_ACC_MSB_MASK) << 2;


    offset[0] = uReg;
    printf("Offset T: %u ", uReg);

    SMB380_Unprepare();
    restoreIRQ(cpsr);

    return true;
}

/*
 * EEPROM = 1 if data has to be written in EEPROM and not only in image
 */
void SMB380_writeOffset(uint16_t *offset, uint8_t EEPROM)
{
    printf("Writing Offset X: %u Y: %u Z: %u\r\n", offset[0], offset[1], offset[2]);

    if (sizeof(offset) >= 3) {
        uint16_t eeoffset = 0;

        if (EEPROM) {
            //create offset if saving to EEPROM is needed
            eeoffset = SMB380_EEPROM_OFFSET;
        }

        unsigned short uReg;
        unsigned long cpsr = disableIRQ();
        SMB380_Prepare();

        //x-Axis
        uReg = (offset[0] & 0x03) << 6; //get both LSB Bits
        //write them to image or eeprom
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_X, 0, SMB380_READ_REGISTER);
        uReg |= (SMB380_ssp_read() & 0x3F); //saves gain from same register
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_X + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        uReg = (offset[0] & 0x3FC) >> 2; //get MSB Bits
        SMB380_ssp_write(SMB380_OFFSET_MSB_X + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        //y-Axis
        uReg = (offset[1] & 0x03) << 6; //get both LSB Bits
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Y, 0, SMB380_READ_REGISTER);
        uReg |= (SMB380_ssp_read() & 0x3F); //saves gain from same register
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Y + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        uReg = (offset[1] & 0x3FC) >> 2; //get MSB Bits
        SMB380_ssp_write(SMB380_OFFSET_MSB_Y + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        //z-Axis
        uReg = (offset[2] & 0x03) << 6; //get both LSB Bits
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Z, 0,
                         SMB380_READ_REGISTER); //write them to image or eeprom
        uReg |= (SMB380_ssp_read() & 0x3F); //saves gain from same register
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Z + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        uReg = (offset[2] & 0x3FC) >> 2; //get MSB Bits
        SMB380_ssp_write(SMB380_OFFSET_MSB_Z + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        SMB380_Unprepare();
        restoreIRQ(cpsr);

    }
}

void SMB380_writeOffsetTemp(uint16_t *offset, uint8_t EEPROM)
{
    printf("Writing Offset Temp: %u\r\n", offset[0]);

    if (sizeof(offset) >= 1) {
        uint16_t eeoffset = 0;

        if (EEPROM) {
            //create offset if saving to EEPROM is needed
            eeoffset = SMB380_EEPROM_OFFSET;
        }

        unsigned short uReg;
        unsigned long cpsr = disableIRQ();
        SMB380_Prepare();

        //T-Axis
        uReg = (offset[0] & 0x03) << 6; //get both LSB Bits
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_T, 0,
                         SMB380_READ_REGISTER); //write them to image or eeprom
        uReg |= (SMB380_ssp_read() & 0x3F); //saves gain from same register
        SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_T + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        uReg = (offset[0] & 0x3FC) >> 2; //get MSB Bits
        SMB380_ssp_write(SMB380_OFFSET_MSB_T + eeoffset, (uint8_t)uReg,
                         SMB380_WRITE_REGISTER); //write them to image or eeprom
        SMB380_ssp_read();
        hwtimer_wait(HWTIMER_TICKS(50000));

        SMB380_Unprepare();
        restoreIRQ(cpsr);
    }
}

unsigned char SMB380_readGain(uint16_t *gain)
{
    if (sizeof(gain) < 3) {
        return false;
    }

    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();

    //x-gain
    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_X, 0, SMB380_READ_REGISTER);
    gain[0] = (SMB380_ssp_read() & SMB380_OFFSET_GAIN_MASK);
    //y-gain
    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Y, 0, SMB380_READ_REGISTER);
    gain[1] = (SMB380_ssp_read() & SMB380_OFFSET_GAIN_MASK);
    //z-gain
    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_Z, 0, SMB380_READ_REGISTER);
    gain[2] = (SMB380_ssp_read() & SMB380_OFFSET_GAIN_MASK);

    SMB380_Unprepare();
    restoreIRQ(cpsr);

    return true;
}

unsigned char SMB380_readGainTemp(uint16_t *gain)
{
    if (sizeof(gain) < 1) {
        return false;
    }

    unsigned long cpsr = disableIRQ();
    SMB380_Prepare();

    //T-gain
    SMB380_ssp_write(SMB380_OFFSET_LSB_GAIN_T, 0, SMB380_READ_REGISTER);
    gain[0] = (SMB380_ssp_read() & SMB380_OFFSET_GAIN_MASK);

    SMB380_Unprepare();
    restoreIRQ(cpsr);

    return true;
}
