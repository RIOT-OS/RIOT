/*
 * Copyright (C) 2016 Leon George, Florent-Valéry Coen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdio.h>
#include <string.h>

#include "cc26x0_prcm.h"

#include "cc26x0_rfc.h"

#define BLE_ADV_STR "this is not a riot\n"

void isr_rfc_cmd_ack(void)
{
    printf("cmd_ack %lx\n", RFC_DBELL->RFACKIFG);
    RFC_DBELL->RFACKIFG = 0;
}

void isr_rfc_hw(void)
{
    uint32_t flags = RFC_DBELL->RFHWIFG;
    printf("hw %lx\n", flags);
    RFC_DBELL->RFHWIFG = ~flags;
}

void isr_rfc_cpe0(void)
{
    uint32_t flags = RFC_DBELL->RFCPEIFG & (~RFC_DBELL->RFCPEISL);
    printf("cpe0 %lx\n", flags);
    RFC_DBELL->RFCPEIFG = ~flags;
}

void isr_rfc_cpe1(void)
{
    uint32_t flags = RFC_DBELL->RFCPEIFG & RFC_DBELL->RFCPEISL;
    printf("cpe1 %lx\n", flags);
    RFC_DBELL->RFCPEIFG = ~flags;
}

/* TODO that's shit design */
static void run_command_ptr(radio_op_command_t *rop)
{
    printf("RFC_DBELL->CMDSTA before %lx\n", RFC_DBELL->CMDSTA);
    RFC_DBELL->CMDR = (uintptr_t) rop;
    printf("CMDR %lx\n", RFC_DBELL->CMDR);
    while (!(RFC_DBELL->CMDSTA & 0xFF)) ;
    printf("RFC_DBELL->CMDSTA %lx\n", RFC_DBELL->CMDSTA);
    do {
        //printf("rop->op.status %x\n", rop->op.status);
    } while (rop->status < R_OP_STATUS_SKIPPED);
    printf("rop->op.status %x\n", rop->status);
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

void rfc_ping_test(void)
{
    uint8_t buf[sizeof(radio_setup_t) + 3];
    radio_setup_t *rs = (radio_setup_t *)((uintptr_t)(buf + 3) & (0xFFFFFFFC));
    memset(rs, 0, sizeof(rs));

    rop->op.commandNo = CMDR_CMDID_PING;

    run_command_ptr(&rs->op);
}

void rfc_setup_ble(void)
{
    uint8_t buf[sizeof(radio_setup_t) + 3];
    radio_setup_t *rs = (radio_setup_t *)((uintptr_t)(buf + 3) & (0xFFFFFFFC));
    memset(rs, 0, sizeof(rs));

    run_command_ptr(&rs->op);
}

void rfc_beacon(void)
{
    uint8_t buf[sizeof(ble_rop_cmd_t) + 3];
    ble_rop_cmd_t *rop = (ble_rop_cmd_t *)((uintptr_t)(buf + 3) & (0xFFFFFFFC));

    //rop->op.commandNo = CMDR_CMDID_BLE_ADV_SCAN;
    rop->op.commandNo = CMDR_CMDID_PING;
    rop->op.status = 0;
    rop->op.pNextOp = 0;
    rop->op.startTrigger = 0;
    rop->op.condition = 0;

    printf("cmdno %x\n", rop->op.commandNo);
    for (int i = 0; i < sizeof(*rop); i++)
        printf("%.2x", ((uint8_t *) rop)[i]);
    printf("\n");

    run_command_ptr(&rop->op);
}

void rfc_prepare(void)
{
    PRCM->RFCCLKG = 1;

    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) ;

    /* RFC POWER DOMAIN */
    PRCM->PDCTL0 |= PDCTL0_RFC_ON;
    PRCM->PDCTL1 |= PDCTL1_RFC_ON;
    while (!(PRCM->PDSTAT0 & PDSTAT0_RFC_ON)) ;
    while (!(PRCM->PDSTAT1 & PDSTAT1_RFC_ON)) ;

    //RFC_PWR->PWMCLKEN |= RFC_PWR_PWMCLKEN_CPE;
    //RFC_PWR->PWMCLKEN |= RFC_PWR_PWMCLKEN_CPERAM;
    RFC_PWR->PWMCLKEN |= 0x7FF;
    printf("RFC_PWR->PWMCLKEN %lx\n", RFC_PWR->PWMCLKEN);

    /* RFC IRQ */
    rfc_irq_enable();
}