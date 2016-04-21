/*
 * Copyright (C) 2016 Leon George, Florent-Valéry Coen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "cc26x0_prcm.h"
#include "cc26x0_rfc.h"

#define BLE_ADV_STR "this is not a riot\n"

void isr_rfc_cmd_ack(void)
{
    /*ROP ack = op submitted, DIR or IMM ack = op executed*/
    printf("Command acknowledged. CMDSTA: 0x%" PRIu32 " \n", RFC_DBELL->CMDSTA);
    RFC_DBELL->RFACKIFG = 0;
}

void isr_rfc_hw(void)
{
    uint32_t flags = RFC_DBELL->RFHWIFG;
    printf("hw 0x%" PRIx32 "\n", flags);
    RFC_DBELL->RFHWIFG = ~flags;
}

void isr_rfc_cpe0(void)
{
    uint32_t flags = RFC_DBELL->RFCPEIFG & (~RFC_DBELL->RFCPEISL);
    printf("cpe0 0x%" PRIx32 "\n", flags);
    RFC_DBELL->RFCPEIFG = ~flags;
}

void isr_rfc_cpe1(void)
{
    uint32_t flags = RFC_DBELL->RFCPEIFG & RFC_DBELL->RFCPEISL;
    printf("cpe1 0x%" PRIx32 "\n", flags);
    RFC_DBELL->RFCPEIFG = ~flags;
}

void rfc_irq_enable(void)
{
    NVIC_EnableIRQ(RF_CMD_ACK_IRQN);
    NVIC_EnableIRQ(RF_CPE0_IRQN);
    NVIC_EnableIRQ(RF_CPE1_IRQN);
    NVIC_EnableIRQ(RF_HW_IRQN);
}

void rfc_irq_disable(void)
{
    NVIC_DisableIRQ(RF_CMD_ACK_IRQN);
    NVIC_DisableIRQ(RF_CPE0_IRQN);
    NVIC_DisableIRQ(RF_CPE1_IRQN);
    NVIC_DisableIRQ(RF_HW_IRQN);
}

uint32_t rfc_send_cmd(void *ropCmd)
{
    RFC_DBELL->CMDR = (uint32_t) ropCmd;

    /* wait for cmd ack (rop cmd was submitted successfully) */
    while (RFC_DBELL->RFACKIFG << 31);
    while (!RFC_DBELL->CMDSTA);

    return RFC_DBELL->CMDSTA;
}

uint16_t rfc_wait_cmd_done(radio_op_command_t *command)
{
    uint32_t timeout_cnt = 0;
    /* wait for cmd execution. condition on rop status doesn't work by itself (too fast?). */
    do {
        if (++timeout_cnt > 500000) {
            command->status = R_OP_STATUS_DONE_TIMEOUT;
            break;
        }
    } while (command->status < R_OP_STATUS_SKIPPED);

    return command->status;
}

void rfc_setup_ble(void)
{
    radio_setup_cmd_t rs;
    memset(&rs, 0, sizeof(rs));

    rfc_send_cmd(&rs.ropCmd);
}

void rfc_beacon(void)
{
    ble_rop_cmd_t rop;
    memset(&rop, 0, sizeof(rop));

    //rop->op.commandNo = CMDR_CMDID_BLE_ADV_SCAN;
    rop.ropCmd.commandNo = CMDR_CMDID_PING;

    rfc_send_cmd(&rop.ropCmd);
}

void rfc_prepare(void)
{
    /* RFC POWER DOMAIN CLOCK GATE */
    PRCM->RFCCLKG = 1;

    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) ;

    /* RFC POWER DOMAIN */
    PRCM->PDCTL0 |= PDCTL0_RFC_ON;
    PRCM->PDCTL1 |= PDCTL1_RFC_ON;
    while (!(PRCM->PDSTAT0 & PDSTAT0_RFC_ON)) ;
    while (!(PRCM->PDSTAT1 & PDSTAT1_RFC_ON)) ;

    /* RFC CLOCK */
    //RFC_PWR->PWMCLKEN |= RFC_PWR_PWMCLKEN_CPE;
    //RFC_PWR->PWMCLKEN |= RFC_PWR_PWMCLKEN_CPERAM;
    RFC_PWR->PWMCLKEN |= 0x7FF;
    printf("RFC_PWR->PWMCLKEN %lx\n", RFC_PWR->PWMCLKEN);

    /* RFC IRQ */
    rfc_irq_enable();
}
