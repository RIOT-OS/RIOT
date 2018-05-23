/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */
/* As of now the implementation only supports the atmega256rfr2 */
#ifdef MODULE_AT86RFR2

#include "board.h"
#include "avr/interrupt.h"
#include "sched.h"

#include "at86rf2xx_registers.h"
#include "at86rf2xx_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef RXTX_LED_ENABLE
    #define RXTX_LED_ENABLE (1)
#endif

#if RXTX_LED_ENABLE
    #define RXTX_LED_INIT (LED_PORT_DDR |= LED0_MASK)
    #define RXTX_LED_ON   LED0_ON
    #define RXTX_LED_OFF  LED0_OFF
#endif

#define DEBUG_ATRFR2_PINS (0)
#ifdef DEBUG_ATRFR2_PINS
    #define DEBUG_ATRFR2_PORT_DDR     (DDRF)
    #define DEBUG_ATRFR2_PORT         (PORTF)
    #define DEBUG_ATRFR2_PIN_TX_START (1 << PORTF7)
    #define DEBUG_ATRFR2_PIN_TX_END   (1 << PORTF6)
    #define DEBUG_ATRFR2_PIN_RX_END   (1 << PORTF5)
#endif

/* netdev_t to at86rfr2_dev for Interrupt handling */
netdev_t *at86rfr2_dev = 0;

/* Counting Retries */
int8_t retries = 0;

void enable_rxtx_led(void)
{

#if RXTX_LED_ENABLE
    RXTX_LED_INIT;
#endif

#if DEBUG_ATRFR2_PINS
    /*initialize Debug Pins */
    /* Port Pin as Output */
    DEBUG_ATRFR2_PORT_DDR |=  (DEBUG_ATRFR2_PIN_TX_START
                               | DEBUG_ATRFR2_PIN_TX_END | DEBUG_ATRFR2_PIN_RX_END);
    /* Pin Low */
    DEBUG_ATRFR2_PORT     &= ~(DEBUG_ATRFR2_PIN_TX_START
                               | DEBUG_ATRFR2_PIN_TX_END | DEBUG_ATRFR2_PIN_RX_END);
#endif
}

void _exit_isr_yield(void)
{
    if (sched_context_switch_request) {
        thread_yield();
        thread_yield_isr();
    }
    __exit_isr();
}

/**
 * \brief ISR for transceiver's receive end interrupt
 *
 *  Is triggered when valid data is received. FCS check passed.
 *  Save IRQ status and inform upper layer of data reception.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_RX_END_vect, ISR_BLOCK){
    __enter_isr();

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT |= DEBUG_ATRFR2_PIN_RX_END;
#endif

    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_END 0x%x\n", status);

    ((at86rf2xx_t *)at86rfr2_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__RX_END;
    /* Call upper layer to process received data */
    at86rfr2_dev->event_callback(at86rfr2_dev, NETDEV_EVENT_ISR);

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT &= ~DEBUG_ATRFR2_PIN_RX_END;
#endif

    _exit_isr_yield();
}

/**
 * \brief  Transceiver Frame Address Match, indicates incoming frame
 *
 *  Is triggered when Frame with valid Address is received.
 *  Can be used to wake up MCU from sleep, etc.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_XAH_AMI_vect, ISR_BLOCK){
    __enter_isr();
    DEBUG("TRX24_XAH_AMI\n");
    ((at86rf2xx_t *)at86rfr2_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__AMI;

    _exit_isr_yield();
}

/**
 * \brief ISR for transceiver's transmit end interrupt
 *
 *  Is triggered when data or when acknowledge frames where send.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_TX_END_vect, ISR_BLOCK){
    __enter_isr();

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT |= DEBUG_ATRFR2_PIN_TX_END;
#endif
    /* This is a short light pulse*/
#if RXTX_LED_ENABLE
    RXTX_LED_ON;
#endif

    at86rf2xx_t *dev = (at86rf2xx_t *) at86rfr2_dev;
    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_TX_END 0x%x\n", status);

    /* only inform upper layer when a transmission was done,
     * not for sending acknowledge frames if data was received. */
    if (status != AT86RF2XX_STATE_RX_AACK_ON) {
        dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__TX_END;

        /* Only radios with the XAH_CTRL_2 register support frame retry reporting
         * but with TX_START and TX_END retries can also be counted */
        dev->tx_retries = retries;
        retries = -1;

        /* Call upper layer to process if data was send succesfull */
        at86rfr2_dev->event_callback(at86rfr2_dev, NETDEV_EVENT_ISR);
    }

#if RXTX_LED_ENABLE
    RXTX_LED_OFF;
#endif

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT &= ~DEBUG_ATRFR2_PIN_TX_END;
#endif

    _exit_isr_yield();
}

/**
 * \brief ISR for transceiver's TX_START interrupt
 *
 * In procedure TX_ARET the TRX24_TX_START interrupt is issued separately for every
 * frame transmission and frame retransmission.
 * Indicates the frame start of a transmitted acknowledge frame in procedure RX_AACK.
 *
 * Flow Diagram Manual p. 52 / 63
 */
ISR(TRX24_TX_START_vect){
    /* __enter_isr(); is not neccessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT |= DEBUG_ATRFR2_PIN_TX_START;
#endif

    /* This is a short light pulse*/
#if RXTX_LED_ENABLE
    RXTX_LED_ON;
#endif

    retries++;

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT &= ~DEBUG_ATRFR2_PIN_TX_START;
#endif

#if RXTX_LED_ENABLE
    RXTX_LED_OFF;
#endif
}

#endif /* MODULE_AT86RFR2 */
